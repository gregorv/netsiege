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

#ifndef CAMPAIGN_SCRIPTENGINE_H
#define CAMPAIGN_SCRIPTENGINE_H

#include <memory>
#include "debug/ndebug.h"

class asIScriptEngine;
class CScriptBuilder;

namespace script {

/**
Call Release() on arbitrary AngelScript object pointers.

This function acts as 'delete statement' for interal AngelScript
reference-counted objects, e.g. asIScriptEngine. The primary use
for this function is to use it as a custom deleter in std::shared_ptr.

Normaly you do not want to call the std::shared_ptr constructor directly,
but use one of the two functions script::shared or script::shared_addref

@example
    asObjPtr* foo = [...]
    std::shared_ptr ptr(foo, script::ASRefRelease<asObjPtr>);

@tparam T Type of the AngelScript object.
@param object Object pointer to call Release() on.
@sa script::shared, script::shared_addref
 */
template <class T>
void ASRefRelease(T* object)
{
    nDebugL(4) << "ASRefRelease on object" << std::hex << object << std::dec << ", of type " << typeid(object).name() << std::endl;
    object->Release();
}

/**
 Generate shared ptr on not-used AngelScript ref-counted objects

 The function creates a std::shared_ptr on the supplied object
 pointer. Additionaly, obj->AddRef() is called to increase
 the reference count of the object.

 Use this function on any object returned by a Get* function
 from AngelScript that you want to store over a period of time
 and therefore have automaticaly managed.

 @warning
 You *must not* use this function if the ref count is already
 increased for your use, e.g. when creating a context with
 engine->CreateContext, since it "increases" the refcount by one
 prior to returning.

 @example
 auto func = script::shared_addref(module->GetFuncByDecl("void foo()"));

 @sa script::shared
*/
template <class T>
std::shared_ptr<T> shared_addref(T* obj)
{
    if(obj) obj->AddRef();
    return std::shared_ptr<T>(obj, ASRefRelease<T>);
}

/**
 Generate shared ptr on created AngelScript ref-counted objects

 The function creates a std::shared_ptr on the supplied object
 pointer. It will not call AddRef on the object by itself, which
 is the required behaviour for any AngelScript function returning
 pointers to new objects, the Create* functions.

 @warning
 You *must not* use this function if the ref count is *not*
 increased for your use, e.g. querying a function pointer from
 a module, since your object might cease its existence while
 you still need it!

 @example
 auto ctx = script::shared(engine-CreateContext());

 @sa script::shared_addref
*/
template <class T> std::shared_ptr<T> shared(T* obj)
{
    return std::shared_ptr<T>(obj, ASRefRelease<T>);
}

class ScriptEngine
{
public:
    ScriptEngine();
    ~ScriptEngine();

    void setPreprocessorDefine(const std::string& word);

    bool importModule(const std::string& moduleName, const std::string& scriptResource);

    void registerOgreMath();
    void registerObjectSystem();

    std::shared_ptr<asIScriptEngine> engine() { return m_engine; }

private:
    std::shared_ptr<asIScriptEngine> m_engine;
    std::unique_ptr<CScriptBuilder> m_scriptBuilder;
};
}

#endif // CAMPAIGN_SCRIPTENGINE_H
