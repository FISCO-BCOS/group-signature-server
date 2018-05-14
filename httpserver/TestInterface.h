/*
	This file is part of FISCO BCOS.

	FISCO BCOS is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	FISCO BCOS is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with FISCO BCOS.  If not, see <http://www.gnu.org/licenses/>.
*/

/* @file: TestInterface.h
 * @author: fisco-dev
 * @date: 2018.03.07
 * @function: 
 */
#pragma once
#include <jsonrpccpp/server.h>
#include <jsonrpccpp/server/connectors/httpserver.h>

#include "devcore/easylog.h"

#include "httpserver/ServerInterface.h"

using namespace jsonrpc;
class TestInterface: public ServerInterface<TestInterface>
{
public:
    TestInterface()
    {
        LOG(DEBUG)<<"bind and add method print_hello";
        bind_and_add_method(jsonrpc::Procedure("print_hello",
               PARAMS_BY_NAME, JSON_STRING,
               "name", JSON_STRING, NULL), &TestInterface::print_hello);
    }

    inline virtual void print_hello(const Json::Value& request,
            Json::Value & response)
    {
        LOG(DEBUG)<<"PRINT HELLO TEST";
        LOG(DEBUG)<<"RESULT IS:"<<request["name"];
        response = "Hello " + request["name"].asString();
    }
    virtual RpcModuleVec ImplementedModules() const
    {
        return  RpcModuleVec{RpcModule{"test", "1.0"}};
    };
};
