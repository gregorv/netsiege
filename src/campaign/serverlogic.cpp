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

#include "serverlogic.h"

#include <iostream>

#include "angelscript.h"
#include "manager.h"
#include "script/scriptengine.h"
#include "debug/ndebug.h"
#include "oms/objectmanager.h"

using namespace campaign;

ServerLogic::ServerLogic(const std::shared_ptr<Manager>& manager)
 : m_manager(manager)
{
    m_manager->scriptEngine()->setPreprocessorDefine("SERVER");
    m_manager->scriptEngine()->importModule("campaign", "main.as");
}

ServerLogic::~ServerLogic()
{

}

bool ServerLogic::init()
{
    auto engine = m_manager->scriptEngine()->engine();
    auto ctx = m_manager->scriptEngine()->context();
    auto func = engine->GetModule("campaign")->GetFunctionByDecl("void initServer()");
    if(!func) {
        logError() << "Cannot locate required AngelScript function 'initServer'" << std::endl;
        return false;
    }
    ctx->Prepare(func);
    int r = ctx->Execute();
    if(r != asEXECUTION_FINISHED) {
        logError() << "Calling function 'initServer' failed" << std::endl;
        return false;
    }
    return true;
}

bool ServerLogic::step(float dt)
{
    auto engine = m_manager->scriptEngine()->engine();
    auto ctx = m_manager->scriptEngine()->context();
    auto func = engine->GetModule("campaign")->GetFunctionByDecl("void stepServer(float)");
    if(!func) {
        logError() << "Cannot locate required AngelScript function 'stepServer'" << std::endl;
        return false;
    }
    ctx->Prepare(func);
    ctx->SetArgFloat(0, dt);
    int r = ctx->Execute();
    if(r != asEXECUTION_FINISHED) {
        logError() << "Calling function 'stepServer' failed" << std::endl;
        return false;
    }
    m_manager->objectManager()->step(dt);
    return true;
}
