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

#include "objectmanager.h"
#include "oms/objects.pb.h"
#include "script/scriptengine.h"
#include <angelscript.h>
#include <boost/algorithm/string.hpp>

#include <cassert>
#include <stdexcept>

namespace oms {

ObjectManager::ObjectManager()
 : m_modeClient(false)
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;
}

ObjectManager::~ObjectManager()
{
    clear();
}

void ObjectManager::setScriptEngine(std::shared_ptr<script::ScriptEngine> engine)
{
    m_scriptEngine = engine;
    int r;
    m_scriptEngine->engine()->RegisterInterface("IObject");
    r = m_scriptEngine->engine()->RegisterGlobalFunction("IObject@ gameObjectCreate(string,string)", asMETHOD(ObjectManager, createObjectRet), asCALL_THISCALL_ASGLOBAL, this);
    assert(r >= 0);
    r = m_scriptEngine->engine()->RegisterGlobalFunction("void gameObjectRemove(int)", asMETHODPR(ObjectManager, removeObject, (uint32_t), void), asCALL_THISCALL_ASGLOBAL, this);
    assert(r >= 0);
    r = m_scriptEngine->engine()->RegisterGlobalFunction("IObject@ gameObjectFind(int)", asMETHODPR(ObjectManager, asFindObject, (GameObject::id_t), asIScriptObject*), asCALL_THISCALL_ASGLOBAL, this);
    assert(r >= 0);
    r = m_scriptEngine->engine()->RegisterGlobalFunction("IObject@ gameObjectFind(string)", asMETHODPR(ObjectManager, asFindObject, (const std::string&), asIScriptObject*), asCALL_THISCALL_ASGLOBAL, this);
    assert(r >= 0);
    r = m_scriptEngine->engine()->RegisterGlobalFunction("bool gameObjectExists(string)", asMETHODPR(ObjectManager, asObjectExists, (const std::string&), bool), asCALL_THISCALL_ASGLOBAL, this);
    assert(r >= 0);
    r = m_scriptEngine->engine()->RegisterGlobalFunction("bool gameObjectExists(int)", asMETHODPR(ObjectManager, asObjectExists, (GameObject::id_t), bool), asCALL_THISCALL_ASGLOBAL, this);
    assert(r >= 0);
    r = m_scriptEngine->engine()->RegisterGlobalFunction("bool gameObjectRegister(string,string,string,uint32)",
                                                         asMETHODPR(ObjectManager,
                                                                    registerObject,
                                                                    ( const std::string&, const std::string&, const std::string&, uint32_t ), bool
                                                                    ), asCALL_THISCALL_ASGLOBAL, this);
    assert(r >= 0);
    r = m_scriptEngine->engine()->RegisterGlobalFunction("bool gameObjectRegister(string,string,string,string,uint32)",
                                                         asMETHODPR(ObjectManager,
                                                                    registerObject,
                                                                    ( const std::string&, const std::string&, const std::string&,
                                                                      const std::string&, uint32_t ), bool
                                                                    ), asCALL_THISCALL_ASGLOBAL, this);
    assert(r >= 0);
}


std::shared_ptr< GameObject > ObjectManager::createObject(const std::string& typeName, const std::string& name)
{
    return createObject(typeName, name, true, 0);
}

std::shared_ptr< GameObject > ObjectManager::createObject(const std::string& typeName, const std::string& name, bool autoId, GameObject::id_t id)
{
    bool saveState = asGetActiveContext() != nullptr;
    if(saveState)
        asGetActiveContext()->PushState();
    assert(m_registeredTypes.find(typeName) != m_registeredTypes.end());
    auto objectInfo = m_registeredTypes[typeName];
    auto engine = m_scriptEngine->engine();
    auto typeId = engine->GetModule("campaign")->GetTypeIdByDecl(objectInfo.className.c_str());
    assert(typeId >= 0);
    auto type = engine->GetObjectTypeById(typeId);
    assert(type != nullptr);
    std::string decl((objectInfo.className+" @"+objectInfo.className+"()"));
    nDebugL(2) << "Factory declaration: " << decl << std::endl;
    auto factory = type->GetFactoryByDecl(decl.c_str());
    assert(factory != nullptr);
    auto ctx = asGetActiveContext();
    if(!ctx)
        ctx = m_scriptEngine->context().get();
    ctx->Prepare(factory);
    ctx->Execute();
    auto scriptObjPtr = script::shared_addref(*(asIScriptObject**)ctx->GetAddressOfReturnValue());
    assert(scriptObjPtr.get() != nullptr);
    std::shared_ptr<GameObject> newObject;
    if(autoId)
        newObject = std::make_shared<GameObject>(scriptObjPtr, objectInfo, name);
    else
        newObject = std::make_shared<GameObject>(id, scriptObjPtr, objectInfo, name);
    for(size_t i=0; i<scriptObjPtr->GetPropertyCount(); i++) {
        if(scriptObjPtr->GetPropertyName(i) != std::string("m_objectId"))
            continue;
        assert(scriptObjPtr->GetPropertyTypeId(i) == asTYPEID_UINT32);
        auto objectId = reinterpret_cast<uint32_t*>(scriptObjPtr->GetAddressOfProperty(i));
        *objectId = newObject->id();
        break;
    }
    newObject->setScriptEngine(m_scriptEngine);
    m_newObjects.insert(newObject);
    if(saveState)
        asGetActiveContext()->PopState();
    m_objectsById[newObject->id()] = newObject;
    return newObject;
}

asIScriptObject* ObjectManager::createObjectRet(const std::string& type, const std::string& name)
{
    auto obj = createObject(type, name)->getScriptObject().get();
    obj->AddRef();
    return obj;
}

void ObjectManager::removeObject(GameObject::id_t id)
{
    auto it = m_objectsById.find(id);
    if(it != m_objectsById.end()) {
        // if this method was not called from AS script,
        // we do not need to store the state, obviously.
        bool asFunction = asGetActiveContext() != nullptr;
        if(asFunction) asGetActiveContext()->PushState();
        it->second->_onRemove();
        if(asFunction) asGetActiveContext()->PopState();
        m_objectsById.erase(it);
        m_removedIds.insert(id);
    }
}

void ObjectManager::removeObject(const std::shared_ptr< GameObject >& obj)
{
    if(obj.get()) {
        removeObject(obj->id());
    }
}

bool ObjectManager::registerObject(const std::string& name, const std::string& className, const std::string& syncProperties, uint32_t flags)
{
    return registerObject(name, className, className, syncProperties, flags);
}

bool ObjectManager::registerObject(const std::string& name, const std::string& serverClass, const std::string& clientClass, const std::string& syncProperties, uint32_t flags)
{
    assert(asGetActiveContext() != nullptr);
    assert(m_registeredTypes.find(name) == m_registeredTypes.end());
    auto props = m_scriptEngine->serializer()->getPropertyList(syncProperties);
    nDebug << serverClass << " " << clientClass << " " << m_modeClient << std::endl;
    auto scriptClass = m_modeClient? clientClass : serverClass;
    GameObject::scriptObjectInfo_t obj = {name, scriptClass, props, flags};
    auto typeId = m_scriptEngine->engine()->GetModule("campaign")->GetTypeIdByDecl(scriptClass.c_str());
    if(typeId < 0) {
        logError() << "Cannot find script class '" << serverClass << "' for registering type '" << name << "'" << std::endl;
        return false;
    }
    // TODO: check if all specified properties can be serialized
//     auto objectType = m_scriptEngine->engine()->GetObjectTypeById(typeId);
//     if(!m_scriptEngine->serializer().isSerializableType(objectType, props)) {
//         return false;
//     }
    m_registeredTypes[name] = obj;
    nDebug << "Registered game objectl type " << name << " with class " << scriptClass << std::endl;
    return true;
}

void ObjectManager::clear()
{
    m_alteredObjects.clear();
    m_newObjects.clear();
    m_objectsById.clear();
}

std::shared_ptr< GameObject > ObjectManager::findObject(const std::string& name) const
{
    for(auto it = m_objectsById.begin(); it != m_objectsById.end(); it++) {
        if(it->second->name() == name) {
            return it->second;
        }
    }
    throw std::out_of_range(std::string("Object with specified name not found"));
}

std::shared_ptr< GameObject > ObjectManager::findObject(GameObject::id_t id) const
{
    return m_objectsById.at(id);
}

void ObjectManager::addUpdatedObject(const std::shared_ptr< GameObject >& obj)
{
    m_alteredObjects.insert(obj);
}

void ObjectManager::flushUpdateCache()
{
    m_alteredObjects.clear();
    m_newObjects.clear();
    m_removedIds.clear();
}

void ObjectManager::serializeChanges(omsproto::GameObjectSet* object_set) const
{
    for(const auto& obj: m_newObjects) {
        obj->serialize(object_set->add_objects(), true);
    }
    for(const auto& obj: m_alteredObjects) {
        // Ff the object was newly created before, the changes are
        // already transmitted by the "new object" block, so we don't
        // need to re-serialize
        if(m_newObjects.find(obj) == m_newObjects.end()) {
            obj->serialize(object_set->add_objects(), false);
        }
    }
}

void ObjectManager::serializeFull(omsproto::GameObjectSet* object_set) const
{
    for(const auto& it: m_objectsById) {
        it.second->serialize(object_set->add_objects(), true);
    }
}

void ObjectManager::deserialize(const omsproto::GameObjectSet* object_set)
{
    for(const auto& object: object_set->objects()) {
        const auto& it = m_objectsById.find(object.id());
        if(it == m_objectsById.end()) {
            // todo: get object type info
            std::string objectTypeName(object.data().object_type());
            auto objInfo = m_registeredTypes[objectTypeName];
            auto newObject = createObject(objectTypeName, object.name(), true, object.id());
            newObject->deserialize(&object);
        }
        else {
            m_objectsById[object.id()]->deserialize(&object);
        }
    }
    flushUpdateCache();
}

asIScriptObject* ObjectManager::asFindObject(const std::string& name)
{
    assert(asGetActiveContext());
    asGetActiveContext()->PushState();
    try {
        auto object = findObject(name);
        asGetActiveContext()->PopState();
        auto scriptObj = object->getScriptObject().get();
        scriptObj->AddRef();
        return scriptObj;
    } catch(std::out_of_range) {
        asGetActiveContext()->PopState();
        return nullptr;
    }
}

asIScriptObject* ObjectManager::asFindObject(GameObject::id_t id)
{
    assert(asGetActiveContext());
    asGetActiveContext()->PushState();
    try {
        auto object = findObject(id);
        asGetActiveContext()->PopState();
        auto scriptObj = object->getScriptObject().get();
        scriptObj->AddRef();
        return scriptObj;
    } catch(std::out_of_range) {
        asGetActiveContext()->PopState();
        return nullptr;
    }
}

bool ObjectManager::asObjectExists(const std::string& name)
{
    try {
        auto object = findObject(name);
        return true;
    } catch(std::out_of_range) {
        return false;
    }
}

bool ObjectManager::asObjectExists(GameObject::id_t id)
{
    try {
        auto object = findObject(id);
        return true;
    } catch(std::out_of_range) {
        return false;
    }
}

void ObjectManager::step(float dt)
{
    for(auto it = m_objectsById.begin(); it != m_objectsById.end();) {
        auto obj = it->second;
        // Advance iterator before executing step(),
        // otherwise the iterator becomes invalid if
        // the object is removed in the current step.
        // -> SIGSEGV
        it++;
        obj->step(dt);
    }
}


}


