/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2014  <copyright holder> <email>
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

#include "netsynctest.h"

#include <QtTest>

QTEST_MAIN(netsynctest);

void netsynctest::initTestCase()
{
    udp::endpoint serverInterface;
    boost::asio::ip::address addr(boost::asio::ip::address_v4::from_string("127.0.0.1"));
    serverInterface.address(addr);
    serverInterface.port(6375);
//     m_client = std::make_shared<network::NetworkClient>(serverInterface);
//     m_server = std::make_shared<network::NetworkClient>(serverInterface);
}

void netsynctest::cleanupTestCase()
{
}

void netsynctest::init()
{
    // Called before each testfunction is executed
}

void netsynctest::cleanup()
{
    // Called after every testfunction
}

void netsynctest::testSeqIdCalculus()
{
    using network::NetworkServer;
    using network::UdpConnection;
    QCOMPARE(network::isSeqIdMoreRecent(0, 1), false);
    QCOMPARE(network::isSeqIdMoreRecent(1, 0), true);
    QCOMPARE(network::isSeqIdMoreRecent(network::MAX_SEQ_ID, 0), false);
    QCOMPARE(network::isSeqIdMoreRecent(0, network::MAX_SEQ_ID), true);

    QCOMPARE(network::seqIdDifference(0, 5), static_cast<network::seq_id_t>(5));
    QCOMPARE(network::seqIdDifference(5, 0), static_cast<network::seq_id_t>(5));
    QCOMPARE(network::seqIdDifference(network::MAX_SEQ_ID-2, 5), static_cast<network::seq_id_t>(8));
    QCOMPARE(network::seqIdDifference(5, network::MAX_SEQ_ID-2), static_cast<network::seq_id_t>(8));

    network::ack_mask_t ackMask = 0;
    network::seq_id_t remoteSeqId = 100;
    ackMask |= 1 << 0; // 99
    ackMask |= 1 << 1; // 98
    ackMask |= 1 << 5; // 94
    ackMask |= 1 << 7; // 92
    QCOMPARE(network::isSeqIdAcknowledged(ackMask, remoteSeqId, 100), true);
    QCOMPARE(network::isSeqIdAcknowledged(ackMask, remoteSeqId, 99), true);
    QCOMPARE(network::isSeqIdAcknowledged(ackMask, remoteSeqId, 98), true);
    QCOMPARE(network::isSeqIdAcknowledged(ackMask, remoteSeqId, 97), false);
    QCOMPARE(network::isSeqIdAcknowledged(ackMask, remoteSeqId, 96), false);
    QCOMPARE(network::isSeqIdAcknowledged(ackMask, remoteSeqId, 95), false);
    QCOMPARE(network::isSeqIdAcknowledged(ackMask, remoteSeqId, 94), true);
    QCOMPARE(network::isSeqIdAcknowledged(ackMask, remoteSeqId, 93), false);
    QCOMPARE(network::isSeqIdAcknowledged(ackMask, remoteSeqId, 92), true);
    QCOMPARE(network::isSeqIdAcknowledged(ackMask, remoteSeqId, 91), false);
}


// #include "netsynctest.moc"
