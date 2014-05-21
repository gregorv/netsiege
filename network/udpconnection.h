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
#include <boost/asio.hpp>

using boost::asio::ip::udp;

namespace network{

template<typename package_type_enum,
         typename protobufMsg,
         typename sender_class_t>
class UdpConnection
{
public:
    UdpConnection(udp::endpoint remoteEndpoint, sender_class_t* sender) : m_sender(sender) { }

    steady_time_point_t timeOfLastAck() const { return m_timeOfLastAck; }
    seq_id_t seqId() const { return m_seqId; }
    ack_mask_t ackMask() const { return m_ackMask; }
    seq_id_t remoteSeqId() const { return m_remoteSeqId; }

    void sendPackage(protobufMsg& msg) {
        msg.seq_id(m_seqId++);
        msg.ack_id(m_remoteSeqId);
        msg.ack_mask(m_ackMask);
        assert(msg.ByteSize() < MAX_PACKAGE_SIZE);
        package_buffer_t raw_msg;
        msg.SerializeToArray(&raw_msg.front(), raw_msg.size());
        m_sender->send(m_remoteEndpoint, raw_msg);
    }

    static bool isSeqIdMoreRecent(seq_id_t seqId1, seq_id_t seqId2)
    {
        // Inspired by
        // http://gafferongames.com/networking-for-game-programmers/reliability-and-flow-control/
        return
            (( seqId1 > seqId2 ) &&
            ( seqId1 - seqId2 <= MAX_SEQ_ID/2 ))
            ||
            (( seqId2 > seqId1 ) &&
            ( seqId2 - seqId1  > MAX_SEQ_ID/2 ));
    }

    static seq_id_t seqIdDifference(seq_id_t seqId1, seq_id_t seqId2) {
        seq_id_t diff;
        if(isSeqIdMoreRecent(seqId1, seqId2)) {
            diff = seqId1 - seqId2;
        }
        else {
            diff = seqId2 - seqId1;
        }
        return diff;
    }

protected:
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
    }

    bool isSeqIdAcknowledged(seq_id_t seq_id) {
        if(seq_id == m_remoteSeqId) {
            return true;
        }
        if(isSeqIdMoreRecent(seq_id, m_remoteSeqId)) {
            return false;
        }
        seq_id_t diff = seqIdDifference(seq_id, m_remoteSeqId);
        if(diff >= NUM_ACKS) {
            return false;
        }
        return m_ackMask & (1 << (diff-1));
    }

private:
    udp::endpoint m_remoteEndpoint;
    sender_class_t* m_sender;
    steady_time_point_t m_timeOfLastAck;
    seq_id_t m_seqId = 0;
    ack_mask_t m_ackMask = 0;
    seq_id_t m_remoteSeqId = 0;
};

}

#endif // NETSIEGE_UDPCONNECTION_H
