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
#include "debug/ndebug.h"

using namespace script;

void debug(std::string &msg)
{
#ifndef PERMANENTLY_DISABLE_DEBUG_OUTPUT
    if(!ndebug::g_disableDebugOutput)
        std::cerr << "AngelScript: " << msg << std::endl;
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
 : m_engine(asCreateScriptEngine(ANGELSCRIPT_VERSION)),
 m_scriptBuilder(std::unique_ptr<CScriptBuilder>(new CScriptBuilder))
{
    int r = m_engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);
    assert(r >= 0);
    RegisterStdString(m_engine);
    RegisterScriptMath(m_engine);
    r = m_engine->RegisterGlobalFunction("void debug(const string &in)", asFUNCTION(debug), asCALL_CDECL); assert( r >= 0 );
    m_scriptBuilder->SetIncludeCallback(IncludeCallback, this);
}

ScriptEngine::~ScriptEngine()
{
    m_engine->Release();
}

void ScriptEngine::registerObjectSystem()
{

}

void ScriptEngine::registerOgreMath()
{

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
        auto r = m_scriptBuilder->StartNewModule(m_engine, moduleName.c_str());
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

