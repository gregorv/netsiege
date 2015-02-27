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

#include "gameobject.h"
#include "oms/objects.pb.h"
#include "script/scriptengine.h"
#include "script/serializer.h"
#include "debug/ndebug.h"

#include <cassert>
#include <angelscript.h>

namespace oms {

GameObject::id_t GameObject::nextId = 0;

GameObject::GameObject(std::shared_ptr<asIScriptObject> scriptObject, const GameObject::scriptObjectInfo_t& info, const std::string& name)
: GameObject(nextId++, scriptObject, info, name)
{
}

GameObject::GameObject(id_t id, std::shared_ptr< asIScriptObject > scriptObject, const GameObject::scriptObjectInfo_t& info, const std::string& name)
: m_id(id), m_name(name), m_scriptObject(scriptObject), m_scriptInfo(info)
{
    nDebugVerbose << "GameObject(" << m_id << ") " << m_name << " created!" << std::endl;
}

GameObject::~GameObject()
{
    _onRemove();
    nDebugVerbose << "GameObject(" << m_id << ") " << m_name << " destroyed!" << std::endl;
}

void GameObject::setScriptEngine(std::shared_ptr< script::ScriptEngine > engine)
{
    if(engine.get() == 0) {
        logWarning() << "GameObject::setScriptEngineAndCreateObject() called, but engine is not set!" << std::endl;
        return;
    }
    assert(!m_engine.get());
    assert(m_scriptObject);
    m_engine = engine;

    auto type = m_scriptObject->GetObjectType();
    auto ctx = m_engine->context();
    auto function = type->GetMethodByDecl("void init()");
    if(!function) {
        return; // it is no error if init() is not specified!
    }
    ctx->Prepare(function);
    ctx->SetObject(m_scriptObject.get());
    auto ret = ctx->Execute();
    if(ret != asEXECUTION_FINISHED) {
        logError() << "Calling method 'int' of Game Object " << m_name
                   << " (class " << m_scriptInfo.className << ") failed!" << std::endl;
        m_engine->PrintException(ctx);
    }
}

void GameObject::step(float dt)
{
    assert(m_engine.get());
    assert(m_scriptObject.get());

    auto type = m_scriptObject->GetObjectType();
    auto ctx = m_engine->context();
    auto function = type->GetMethodByDecl("void step(float)");
    if(!function) {
        return; // it is no error if step() is not specified!
    }
    ctx->Prepare(function);
    ctx->SetObject(m_scriptObject.get());
    ctx->SetArgFloat(0, dt);
    auto ret = ctx->Execute();
    if(ret != asEXECUTION_FINISHED) {
        logError() << "Calling method 'step' of Game Object " << m_name
                   << " (class " << m_scriptInfo.className << ") failed!" << std::endl;
        m_engine->PrintException(ctx);
    }
}

void GameObject::_onRemove()
{
    if(m_scriptObject.get()) {
        auto type = m_scriptObject->GetObjectType();
        auto ctx = m_engine->context();
        auto function = type->GetMethodByDecl("void cleanup()");
        if(!function) {
            return; // it is no error if cleanup() is not specified!
        }
        ctx->Prepare(function);
        ctx->SetObject(m_scriptObject.get());
        auto ret = ctx->Execute();
        if(ret != asEXECUTION_FINISHED) {
            logError() << "Calling method 'step' of Game Object " << m_name
                       << " (class " << m_scriptInfo.className << ") failed!" << std::endl;
            m_engine->PrintException(ctx);
        }
        m_scriptObject.reset();
        m_engine.reset();
    }
}

void GameObject::serialize(omsproto::GameObject* object, bool forceFullSerialize) const
{
    assert(object);
    object->set_id(m_id);
    if(forceFullSerialize) {
        object->set_name(m_name);
    }
    object->set_is_visible(m_isVisible);
    object->clear_position();
    for(int8_t i=0; i<16; i++) {
        object->add_position((&m_positionMatrix[0][0])[i]);
    }
    auto data = object->mutable_data();
    data->set_object_type(m_scriptInfo.typeName);
    m_engine->serializer()->serializeObject(data, m_scriptObject, m_scriptInfo.syncProperties);
}

void GameObject::deserialize(const omsproto::GameObject* object)
{
    assert(object);
    for(int8_t i=0; i<16; i++) {
        (&m_positionMatrix[0][0])[i] = object->position().Get(i);
    }
    m_isVisible = object->is_visible();
    assert(m_engine.get() != nullptr);
    m_engine->serializer()->deserializeObject(object->data(), m_scriptObject);
}

void GameObject::setPositionMatrix(const Ogre::Matrix4& pos)
{
    m_positionMatrix = pos;
}

}
