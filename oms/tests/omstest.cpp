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

#include "omstest.h"

#include "oms/objects.pb.h"

#include <QtTest>
#include <sstream>

using namespace oms;

QTEST_MAIN(omstest);

void omstest::initTestCase()
{
}

void omstest::cleanupTestCase()
{
    // Called after the last testfunction was executed
}

void omstest::init()
{
    // Called before each testfunction is executed
}

void omstest::cleanup()
{
}

void omstest::testInsertion()
{
    ObjectManager objectManager;
    objectManager.createObject(std::string("Foo_1"));
    QCOMPARE(objectManager.getNewObjects().size(), 1ul);
    QCOMPARE(objectManager.getObjects().size(), 1ul);

    objectManager.createObject(std::string("Foo_2"));
    QCOMPARE(objectManager.getNewObjects().size(), 2ul);
    QCOMPARE(objectManager.getObjects().size(), 2ul);

    objectManager.flushUpdateCache();
    objectManager.createObject(std::string("Foo_3"));
    QCOMPARE(objectManager.getNewObjects().size(), 1ul);
    QCOMPARE(objectManager.getObjects().size(), 3ul);

    objectManager.flushUpdateCache();
    QCOMPARE(objectManager.getNewObjects().size(), 0ul);
    QCOMPARE(objectManager.getUpdatedObjects().size(), 0ul);
    QCOMPARE(objectManager.getObjects().size(), 3ul);
    QCOMPARE(objectManager.getRemovedIds().size(), 0ul);
}
void omstest::testRemoval()
{
    ObjectManager objectManager;
    GameObject::id_t id = objectManager.createObject(std::string("Foo_1"))->getId();
    objectManager.createObject(std::string("Foo_2"));
    objectManager.createObject(std::string("Foo_3"));
    objectManager.flushUpdateCache();
    QCOMPARE(objectManager.getNewObjects().size(), 0ul);
    QCOMPARE(objectManager.getUpdatedObjects().size(), 0ul);
    QCOMPARE(objectManager.getObjects().size(), 3ul);
    QCOMPARE(objectManager.getRemovedIds().size(), 0ul);

    objectManager.removeObject(objectManager.findObject(std::string("Foo_3")));
    QCOMPARE(objectManager.getRemovedIds().size(), 1ul);
    QCOMPARE(objectManager.getObjects().size(), 2ul);

    objectManager.removeObject(id);
    QCOMPARE(objectManager.getRemovedIds().size(), 2ul);
    QCOMPARE(objectManager.getObjects().size(), 1ul);

    objectManager.flushUpdateCache();
    QCOMPARE(objectManager.getRemovedIds().size(), 0ul);
    QCOMPARE(objectManager.getObjects().size(), 1ul);

    objectManager.removeObject(75);
    try {
        objectManager.removeObject(objectManager.findObject(std::string("Bar")));
        QFAIL("Searching non-existant object did not raise exception!");
    }
    catch(std::exception& e) {
    }
    QCOMPARE(objectManager.getRemovedIds().size(), 0ul);
    QCOMPARE(objectManager.getObjects().size(), 1ul);
}

void omstest::testAccess()
{
    ObjectManager objectManager;
    GameObject::id_t id = objectManager.createObject(std::string("Foo_1"))->getId();
    std::shared_ptr<GameObject> null(nullptr);

    try {
        auto found = objectManager.findObject(id);
        QVERIFY(found != null);
    } catch(std::exception& e) {
        QFAIL("Object expected to exist was not found!");
    }

    try {
        auto found = objectManager.findObject(9999999); // won't exist (hopefuly ;-)
        QVERIFY(found == null);
        QFAIL("Searching non-existant object did not raise exception!");
    }
    catch(std::exception& e) {
    }

    try {
        auto found = objectManager.findObject(std::string("Foo_1"));
        QVERIFY(found != null);
    } catch(std::exception& e) {
        QFAIL("Object expected to exist was not found!");
    }

    try {
        auto found = objectManager.findObject(std::string("Bar"));
        QVERIFY(found == null);
        QFAIL("Searching non-existant object did not raise exception!");
    }
    catch(std::exception& e) {
    }
}

void omstest::testSerialize()
{
    ObjectManager sourceMgr, targetMgr;
    sourceMgr.createObject(std::string("Foo_1"));
    sourceMgr.createObject(std::string("Foo_2"));
    sourceMgr.createObject(std::string("Foo_3"));
    sourceMgr.createObject(std::string("Foo_4"));
    sourceMgr.createObject(std::string("Foo_5"));

    omsproto::GameObjectSet sourceSet, targetSet;
    sourceMgr.serializeChanges(&sourceSet);
    targetSet.ParseFromString(sourceSet.SerializeAsString());
    targetMgr.deserialize(&targetSet);

    QCOMPARE(targetMgr.getObjects().size(), sourceMgr.getObjects().size());
}

// #include "oms/tests/omstest.moc"
