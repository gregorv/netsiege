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
    objectManager.createObject(0, std::string("Foo_1"));
    QCOMPARE(objectManager.newObjects().size(), 1ul);
    QCOMPARE(objectManager.objects().size(), 1ul);

    objectManager.createObject(0, std::string("Foo_2"));
    QCOMPARE(objectManager.newObjects().size(), 2ul);
    QCOMPARE(objectManager.objects().size(), 2ul);

    objectManager.flushUpdateCache();
    objectManager.createObject(0, std::string("Foo_3"));
    QCOMPARE(objectManager.newObjects().size(), 1ul);
    QCOMPARE(objectManager.objects().size(), 3ul);

    objectManager.flushUpdateCache();
    QCOMPARE(objectManager.newObjects().size(), 0ul);
    QCOMPARE(objectManager.updatedObjects().size(), 0ul);
    QCOMPARE(objectManager.objects().size(), 3ul);
    QCOMPARE(objectManager.removedIds().size(), 0ul);
}
void omstest::testRemoval()
{
    ObjectManager objectManager;
    GameObject::id_t id = objectManager.createObject(0, std::string("Foo_1"))->id();
    objectManager.createObject(0, std::string("Foo_2"));
    objectManager.createObject(0, std::string("Foo_3"));
    objectManager.flushUpdateCache();
    QCOMPARE(objectManager.newObjects().size(), 0ul);
    QCOMPARE(objectManager.updatedObjects().size(), 0ul);
    QCOMPARE(objectManager.objects().size(), 3ul);
    QCOMPARE(objectManager.removedIds().size(), 0ul);

    objectManager.removeObject(objectManager.findObject(std::string("Foo_3")));
    QCOMPARE(objectManager.removedIds().size(), 1ul);
    QCOMPARE(objectManager.objects().size(), 2ul);

    objectManager.removeObject(id);
    QCOMPARE(objectManager.removedIds().size(), 2ul);
    QCOMPARE(objectManager.objects().size(), 1ul);

    objectManager.flushUpdateCache();
    QCOMPARE(objectManager.removedIds().size(), 0ul);
    QCOMPARE(objectManager.objects().size(), 1ul);

    objectManager.removeObject(75);
    try {
        objectManager.removeObject(objectManager.findObject(std::string("Bar")));
        QFAIL("Searching non-existant object did not raise exception!");
    }
    catch(std::exception& e) {
    }
    QCOMPARE(objectManager.removedIds().size(), 0ul);
    QCOMPARE(objectManager.objects().size(), 1ul);
}

void omstest::testAccess()
{
    ObjectManager objectManager;
    GameObject::id_t id = objectManager.createObject(0, std::string("Foo_1"))->id();
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
    sourceMgr.createObject(0, std::string("Foo_1"));
    sourceMgr.createObject(0, std::string("Foo_2"));
    sourceMgr.createObject(0, std::string("Foo_3"));
    sourceMgr.createObject(0, std::string("Foo_4"));
    sourceMgr.createObject(0, std::string("Foo_5"));

    omsproto::GameObjectSet sourceSet, targetSet;
    sourceMgr.serializeChanges(&sourceSet);
    targetSet.ParseFromString(sourceSet.SerializeAsString());
    targetMgr.deserialize(&targetSet);

    QCOMPARE(targetMgr.objects().size(), sourceMgr.objects().size());
}

// #include "oms/tests/omstest.moc"
