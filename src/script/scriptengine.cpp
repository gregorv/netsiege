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

#include "scriptengine.h"
#include "scriptfilemanager.h"
#include "angelscript.h"
#include "scriptbuilder/scriptbuilder.h"
#include "scriptstdstring/scriptstdstring.h"
#include "scriptmath/scriptmath.h"
#include "scriptany/scriptany.h"
#include "scriptarray/scriptarray.h"
#include "scripthandle/scripthandle.h"
#include "weakref/weakref.h"
#include "network/rpcpackage.h"
#include "debug/ndebug.h"
#include "serializer.h"
#include <OgreMatrix3.h>
#include <OgreMatrix4.h>
#include <OgreVector2.h>
#include <OgreVector3.h>
#include <OgreVector4.h>

using namespace script;

void debug(uint32_t verbosity, const std::string &msg)
{
#ifndef PERMANENTLY_DISABLE_DEBUG_OUTPUT
    if(!ndebug::g_disableDebugOutput) {
        auto ctx = asGetActiveContext();
        int lineNo = 0;
        const char* secName = nullptr;
        lineNo = ctx->GetLineNumber(0, 0, &secName);
        if(ndebug::g_coloredOutput) {
            std::cerr << "\x1b[35;1m" << secName << ":" << lineNo << ": \x1b[0m" << msg << std::endl;
        } else {
            std::cerr << secName << ":" << lineNo << ": " << msg << std::endl;
        }
    }
#endif
}

void debug(const std::string &msg)
{
#ifndef PERMANENTLY_DISABLE_DEBUG_OUTPUT
    debug(1, msg);
#endif
}


void MessageCallback(const asSMessageInfo *msg, void *param)
{
    const char *type = "ERR ";
    if( msg->type == asMSGTYPE_WARNING )
            type = "WARN";
    else if( msg->type == asMSGTYPE_INFORMATION )
            type = "INFO";

    logError() << "AngelScript " << " (" <<  msg->section << ":" << msg->row << ") " << type << ": " << msg->message << std::endl;
}

int IncludeCallback(const char *include, const char *from, CScriptBuilder *builder, void *userParam)
{
    nDebug << "AS IncludeCallback " << include << ", " << from << std::endl;
//     auto scriptEngine = reinterpret_cast<ScriptEngine*>(userParam);
    auto resource = ScriptFileManager::getSingleton().load(Ogre::String(include), Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    auto code = resource->code();
    builder->AddSectionFromMemory(include, code.c_str(), code.length());
    return 0;
}

ScriptEngine::ScriptEngine()
 : m_engine(asCreateScriptEngine(ANGELSCRIPT_VERSION), ASRefRelease<asIScriptEngine>),
 m_context(shared(m_engine->CreateContext())),
 m_scriptBuilder(std::unique_ptr<CScriptBuilder>(new CScriptBuilder)),
 m_serializer(std::unique_ptr<Serializer>(new Serializer(this)))
{
    int r = m_engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);
    assert(r >= 0);
    RegisterScriptAny(m_engine.get());
    RegisterScriptArray(m_engine.get(), true);
    RegisterStdString(m_engine.get());
//     RegisterStdStringUtils(m_engine.get());
    RegisterScriptMath(m_engine.get());
    RegisterScriptHandle(m_engine.get());
    RegisterScriptWeakRef(m_engine.get());
//     registerOgreMath();
    r = m_engine->RegisterGlobalFunction("void debug(const string &in)", asFUNCTIONPR(debug, (const std::string&), void), asCALL_CDECL); assert( r >= 0 );
    r = m_engine->RegisterGlobalFunction("void debug(uint, const string &in)", asFUNCTIONPR(debug, (uint32_t, const std::string&), void), asCALL_CDECL); assert( r >= 0 );
    m_scriptBuilder->SetIncludeCallback(IncludeCallback, this);
}

ScriptEngine::~ScriptEngine()
{
}

void ScriptEngine::registerObjectSystem()
{

}

void ScriptEngine::registerOgreMath()
{
    int r;
//     r = m_engine->RegisterObjectType("mat3", sizeof(Ogre::Matrix3), asOBJ_VALUE | asOBJ_POD); assert( r >= 0 );
//     r = m_engine->RegisterObjectMethod("mat3", "float determinant()", asMETHOD(Ogre::Matrix3,Determinant), asCALL_THISCALL); assert( r >= 0 );
//     r = m_engine->RegisterObjectMethod("mat3", "vec3 getColumn(size_t col)", asMETHOD(Ogre::Matrix3,Determinant), asCALL_THISCALL); assert( r >= 0 );
//     r = m_engine->RegisterObjectMethod("mat3", "float spectralNorm()", asMETHOD(Ogre::Matrix3,Determinant), asCALL_THISCALL); assert( r >= 0 );
//     r = m_engine->RegisterObjectType("mat4", sizeof(Ogre::Matrix4), asOBJ_VALUE | asOBJ_POD); assert( r >= 0 );
//     r = m_engine->RegisterObjectType("vec2", sizeof(Ogre::Vector2), asOBJ_VALUE | asOBJ_POD); assert( r >= 0 );
    r = m_engine->RegisterObjectType("vec3", sizeof(Ogre::Vector3), asOBJ_VALUE | asOBJ_POD); assert( r >= 0 );
    r = m_engine->RegisterObjectMethod("vec3", "float absDotProduct(const vec3&)", asMETHOD(Ogre::Vector3,absDotProduct), asCALL_THISCALL); assert( r >= 0 );
    r = m_engine->RegisterObjectMethod("vec3", "vec3 crossProduct(const vec3&)", asMETHOD(Ogre::Vector3,crossProduct), asCALL_THISCALL); assert( r >= 0 );
    r = m_engine->RegisterObjectMethod("vec3", "float distance(const vec3&)", asMETHOD(Ogre::Vector3,distance), asCALL_THISCALL); assert( r >= 0 );
    r = m_engine->RegisterObjectMethod("vec3", "float dotProduct(const vec3&)", asMETHOD(Ogre::Vector3,dotProduct), asCALL_THISCALL); assert( r >= 0 );
    r = m_engine->RegisterObjectMethod("vec3", "float length() const", asMETHOD(Ogre::Vector3,length), asCALL_THISCALL); assert( r >= 0 );
    r = m_engine->RegisterObjectMethod("vec3", "real normalise()", asMETHOD(Ogre::Vector3,normalise), asCALL_THISCALL); assert( r >= 0 );
//     r = m_engine->RegisterObjectType("vec4", sizeof(Ogre::Vector2), asOBJ_VALUE | asOBJ_POD); assert( r >= 0 );
}

void ScriptEngine::setPreprocessorDefine(const std::string& word)
{
    m_scriptBuilder->DefineWord(word.c_str());
}

bool ScriptEngine::importModule(const std::string& moduleName, const std::string& scriptResource)
{
    nDebug << "importModule(" << moduleName << ", " << scriptResource << ")" << std::endl;
    try {
        auto resource = ScriptFileManager::getSingleton().load(scriptResource, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        auto code = resource->code();
        auto r = m_scriptBuilder->StartNewModule(m_engine.get(), moduleName.c_str());
        if(r < 0) {
            nDebug << "Failed to start module, return code " << r << std::endl;
            return false;
        }
        r = m_scriptBuilder->AddSectionFromMemory(scriptResource.c_str(), code.c_str(), code.length());
        if(r < 0) {
            nDebug << "Failed to add script section, return code " << r << std::endl;
            return false;
        }
        r = m_scriptBuilder->BuildModule();
        if(r < 0) {
            nDebug << "Failed to build module, return code " << r << std::endl;
            return false;
        }
    }
    catch(const Ogre::FileNotFoundException& e) {
        logError() << "Cannot locate script file: " << scriptResource << std::endl;
        nDebug << "Exception: " << e.what() << std::endl;
        return false;
    }
    return true;
}

void ScriptEngine::PrintException(asIScriptContext* ctx) const
{
    // Determine the exception that occurred
    logError() << "An AngelScript exception occured:" << ctx->GetExceptionString() << std::endl;
    // Determine the function where the exception occurred
    const asIScriptFunction *function = ctx->GetExceptionFunction();
    logInfo() << function->GetModuleName() << " " << function->GetScriptSectionName() << ":" << ctx->GetExceptionLineNumber() << " : " << function->GetDeclaration() << std::endl;
}

void ScriptEngine::PrintException(std::shared_ptr< asIScriptContext > ctx) const
{
    PrintException(ctx.get());
}

