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

#include "clientlogic.h"
#include "debug/ndebug.h"
#include "script/scriptengine.h"
#include "campaign/manager.h"
#include "debug/ndebug.h"
#include <angelscript.h>
#include <cassert>

using namespace campaign;

ClientLogic::ClientLogic(std::shared_ptr<Manager> manager)
 : m_manager(manager)
{
    m_manager->scriptEngine()->setPreprocessorDefine("CLIENT");
    m_manager->scriptEngine()->importModule("campaign", "main.as");
}

ClientLogic::~ClientLogic()
{

}

bool ClientLogic::init()
{
    auto engine = m_manager->scriptEngine()->engine();
    auto ctx = m_manager->scriptEngine()->context();
    assert(engine->GetModule("campaign"));
    auto func = engine->GetModule("campaign")->GetFunctionByDecl("void initClient()");
    if(!func) {
        logError() << "Cannot locate required AngelScript function 'initClient'" << std::endl;
        return false;
    }
    ctx->Prepare(func);
    int r = ctx->Execute();
    if(r != asEXECUTION_FINISHED) {
        logError() << "Calling function 'initClient' failed" << std::endl;
        return false;
    }
    return true;
}

bool ClientLogic::step(float dt)
{
    auto engine = m_manager->scriptEngine()->engine();
    auto ctx = m_manager->scriptEngine()->context();
    auto func = engine->GetModule("campaign")->GetFunctionByDecl("void stepClient(float)");
    if(!func) {
        logError() << "Cannot locate required AngelScript function 'stepClient'" << std::endl;
        return false;
    }
    ctx->Prepare(func);
    int r = ctx->Execute();
    if(r != asEXECUTION_FINISHED) {
        logError() << "Calling function 'stepClient' failed" << std::endl;
        return false;
    }
    return true;
}
