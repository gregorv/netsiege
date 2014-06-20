/*
 * netsiege - A multiplayer point and click adventure
 * Copyright (C) 2014 Gregor Vollmer <gregor@celement.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef NETSIEGE_UDPCONNECTION_H
#define NETSIEGE_UDPCONNECTION_H

#include "network.h"
#include "rpcpackage.h"
#include "rpcdispatcher.h"
#include "network/network.pb.h"
#include "debug/ndebug.h"
#include <boost/asio.hpp>
#include <list>

using boost::asio::ip::udp;

namespace network{

template<typename package_type_enum,
         typename recv_msg,
         typename send_msg,
         typename sender_class_t>
class UdpConnection
{
public:
    typedef std::shared_ptr<recv_msg> recv_msg_ptr;
    typedef std::shared_ptr<send_msg> send_msg_ptr;

    enum ConnectionStatus_t {
        CS_UNCONNECTED=0,
        CS_HANDSHAKE_FAILED,
        CS_CONNECTED
    };

    UdpConnection(udp::endpoint remoteEndpoint, sender_class_t* sender, RPCDispatcher* rpcDispatcher)
     : m_remoteEndpoint(remoteEndpoint), m_sender(sender), m_rpcDispatcher(rpcDispatcher) { }

    steady_time_point_t timeOfLastAck() const { return m_timeOfLastAck; }
    seq_id_t seqId() const { return m_seqId; }
    ack_mask_t ackMask() const { return m_ackMask; }
    seq_id_t remoteSeqId() const { return m_remoteSeqId; }

    void close() {
    }

    void sendPackage(send_msg_ptr& msg) {
        msg->set_protocol_id(PROTOCOL_ID);
        msg->set_seq_id(m_seqId++);
        msg->set_ack_seq_id(m_remoteSeqId);
        msg->set_ack_mask(m_ackMask);
        auto size = msg->ByteSize();
        assert(size < MAX_PACKAGE_SIZE);
        package_buffer_t raw_msg;
        msg->SerializeToArray(&raw_msg.front(), raw_msg.size());
        m_averageSizeOutgoingPackage += (size - m_averageSizeOutgoingPackage)*0.1f;
        m_sender->send(m_remoteEndpoint, raw_msg, size);
    }

    bool remoteProcedureCall(std::shared_ptr<RPCPackage> rpc)
    {
        return remoteProcedureCall(rpc->rpcId(), rpc->data(), rpc->dataSize());
    }

    bool remoteProcedureCall(const rpc_id_t& procedureId, const rpc_data_t& data, const size_t& dataSize=MAX_RPC_DATA_SIZE) {
        return remoteProcedureCall(m_nextMsgId++, procedureId, data, dataSize);
    }

    bool isSeqIdAcknowledged(seq_id_t seqId) const {
        return network::isSeqIdAcknowledged(m_ackMask, m_remoteSeqId, seqId);
    }

    ConnectionStatus_t connectionStatus() const {
        return m_connectionStatus;
    }

protected:
    bool parsePackage(const recv_msg& msg, uint16_t srcClientId=0)
    {
        refresh(msg.seq_id(), msg.ack_seq_id(), msg.ack_mask());
        if(msg.has_rpc()) {
            m_rpcDispatcher->executeReceivedCall(srcClientId, msg.rpc().rpc_id(), msg.rpc().rpc_data());
        } else {
            return false;
        }
        return true;
    }

    ConnectionStatus_t m_connectionStatus = CS_UNCONNECTED;

private:
    struct rpc_info_t {
        seq_id_t msgId;
        seq_id_t seqId;
        rpc_id_t rpcId;
        rpc_data_t data;
        size_t dataSize;
        steady_time_point_t timeOfSend;
    };

    void refresh(seq_id_t remoteSeqId, seq_id_t ack, ack_mask_t ackMask)
    {
        m_timeOfLastAck = std::chrono::steady_clock::now();
        if(isSeqIdMoreRecent(remoteSeqId, m_remoteSeqId)) {
            seq_id_t maskOffset = remoteSeqId - m_remoteSeqId;
            // handle seq id wrap
            if(remoteSeqId < m_remoteSeqId) {
                maskOffset = MAX_SEQ_ID - maskOffset;
            }
            m_ackMask <<= maskOffset;
            if(maskOffset-1 < NUM_ACKS) {
                m_ackMask |= 1<<(maskOffset-1);
            }
            m_remoteSeqId = remoteSeqId;
        }
        else {
            seq_id_t maskOffset = m_remoteSeqId - remoteSeqId;
            // handle seq id wrap
            if(remoteSeqId > m_remoteSeqId) {
                maskOffset = MAX_SEQ_ID - maskOffset;
            }
            if(maskOffset < NUM_ACKS) {
                m_ackMask |= 1<<maskOffset;
            }
        }
        processRpcAcknowledges();
    }

    bool remoteProcedureCall(const seq_id_t& messageId, const rpc_id_t& procedureId, const rpc_data_t& data, const size_t& dataSize)
    {
        auto msg = std::make_shared<send_msg>();
        auto rpc = msg->mutable_rpc();
        rpc->set_msg_seq_id(messageId);
        rpc->set_rpc_id(procedureId);
        rpc->set_rpc_data(data.data(), dataSize);
        sendPackage(msg);
        nDebugL(4) << "Sent RPC package: seqId " << msg->seq_id() << ", msgId " << messageId << ", rpcId " << procedureId << std::endl;

        rpc_info_t rpc_info {
            static_cast<seq_id_t>(rpc->msg_seq_id()),
            static_cast<seq_id_t>(msg->seq_id()),
            procedureId,
            data,
            dataSize,
            std::chrono::steady_clock::now()
        };
        m_rpcMemory.push_back(rpc_info);
        return true;
    }

    bool remoteProcedureCall(const rpc_info_t& rpc)
    {
        return remoteProcedureCall(rpc.msgId, rpc.rpcId, rpc.data, rpc.dataSize);
    }

    void processRpcAcknowledges()
    {
        using namespace std::chrono;
        auto now = steady_clock::now();
        for(auto it = m_rpcMemory.begin(); it != m_rpcMemory.end(); it++) {
            duration<float> time_span = duration_cast<duration<float>>(now - it->timeOfSend);
            if(isSeqIdAcknowledged(it->seqId)) {
                nDebugL(4) << "RPC message no." << it->msgId << " acknowledged!" << std::endl;
                it = m_rpcMemory.erase(it);
            }
            else if(time_span.count() > PACKAGE_LOSS_TIMEOUT) {
                nDebugL(4) << "RPC Package timeout, resend message no. " << it->msgId << ", RPC ID " << it->rpcId << std::endl;
                remoteProcedureCall(*it);
                it = m_rpcMemory.erase(it);
            }
        }

    }


protected:
    udp::endpoint m_remoteEndpoint;

private:
    sender_class_t* m_sender;
    steady_time_point_t m_timeOfLastAck;
    seq_id_t m_seqId = 0;
    ack_mask_t m_ackMask = 0;
    seq_id_t m_remoteSeqId = 0;
    seq_id_t m_nextMsgId = 0;
    std::list<rpc_info_t> m_rpcMemory;
    float m_averageSizeOutgoingPackage = 0.0f;
    RPCDispatcher* m_rpcDispatcher;
};

}

#endif // NETSIEGE_UDPCONNECTION_H
