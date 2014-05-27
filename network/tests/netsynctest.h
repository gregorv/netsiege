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

#ifndef NETSYNCTEST_H
#define NETSYNCTEST_H

#include <QObject>
#include "network/udpconnection.h"
#include "network/network.pb.h"
#include "network/networkclient.h"
#include "network/networkserver.h"


class netsynctest : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();

    void init();
    void cleanup();

    void testSeqIdCalculus();

private:
    std::shared_ptr<network::NetworkServer> m_server;
    std::shared_ptr<network::NetworkClient> m_client;

};

#endif // NETSYNCTEST_H
