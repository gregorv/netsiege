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

class asIScriptEngine;
class CScriptBuilder;

namespace campaign {

class ScriptEngine
{
public:
    ScriptEngine();
    ~ScriptEngine();

    void setPreprocessorDefine(const std::string& word);

    bool importModule(const std::string& moduleName, const std::string& scriptResource);

    void registerOgreMath();
    void registerObjectSystem();

    const asIScriptEngine* engine() const { return m_engine; }
    asIScriptEngine* engine() { return m_engine; }

private:
    asIScriptEngine* m_engine;
    std::unique_ptr<CScriptBuilder> m_scriptBuilder;
};
}

#endif // CAMPAIGN_SCRIPTENGINE_H
