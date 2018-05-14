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

/* @file: RingSigRpcInterface.h
 * @author: fisco-dev
 * @date: 2018.03.11
 */
#pragma once

#include <jsonrpccpp/server.h>
#include <jsonrpccpp/server/connectors/httpserver.h> 

#include "devcore/easylog.h"
#include "devcore/StatusCode.h"

#include "httpserver/ServerInterface.h"

class RingSigRpcInterface:public ServerInterface<RingSigRpcInterface>
{
public:
    RingSigRpcInterface()
    {
        bind_and_add_method(jsonrpc::Procedure("setup_ring",
                  jsonrpc::PARAMS_BY_NAME, jsonrpc::JSON_STRING,
                  "ring_name", jsonrpc::JSON_STRING,                  
                  NULL), &RingSigRpcInterface::setup_ring);

        bind_and_add_method(jsonrpc::Procedure("join_ring",
                  jsonrpc::PARAMS_BY_NAME, jsonrpc::JSON_STRING,
                  "ring_name", jsonrpc::JSON_STRING,                  
                  NULL), &RingSigRpcInterface::join_ring);

        bind_and_add_method(jsonrpc::Procedure("linkable_ring_sig",
                    jsonrpc::PARAMS_BY_NAME, jsonrpc::JSON_STRING,
                    "ring_name", jsonrpc::JSON_STRING, 
                    "message", jsonrpc::JSON_STRING,
                    "id", jsonrpc::JSON_STRING, NULL), 
                &RingSigRpcInterface::linkable_ring_sig);

        bind_and_add_method(jsonrpc::Procedure("linkable_ring_verify",
                    jsonrpc::PARAMS_BY_NAME, jsonrpc::JSON_STRING,
                    "ring_name", jsonrpc::JSON_STRING,
                    "sig", jsonrpc::JSON_STRING,
                    "message", jsonrpc::JSON_STRING,NULL),
                &RingSigRpcInterface::linkable_ring_verify);

        /////get interfaces
        bind_and_add_method(jsonrpc::Procedure("get_ring_param",
                    jsonrpc::PARAMS_BY_NAME, jsonrpc::JSON_STRING,
                    "ring_name", jsonrpc::JSON_STRING,
                    NULL), &RingSigRpcInterface::get_ring_param);

        bind_and_add_method(jsonrpc::Procedure("get_public_key",
                    jsonrpc::PARAMS_BY_NAME, jsonrpc::JSON_STRING,
                    "ring_name", jsonrpc::JSON_STRING,
                    "id", jsonrpc::JSON_STRING, NULL), 
                    &RingSigRpcInterface::get_public_key);

        bind_and_add_method(jsonrpc::Procedure("get_private_key",
                    jsonrpc::PARAMS_BY_NAME, jsonrpc::JSON_STRING,
                    "ring_name", jsonrpc::JSON_STRING,
                    "id", jsonrpc::JSON_STRING, NULL),
                    &RingSigRpcInterface::get_private_key);
    }
    
    /////get interfaces
    virtual void get_ring_param(const Json::Value& request,
                    Json::Value& response)
    {
        std::string ring_name = request["ring_name"].asString();
        response = this->get_ring_param(ring_name);
    }

    virtual void get_public_key(const Json::Value& request,
                    Json::Value& response)
    {
        std::string ring_name = request["ring_name"].asString();
        std::string ret_message;
        bool valid = check_member_pos(ret_message, request["id"].asString());
        if(valid)
        {
            int id = atoi(request["id"].asString().c_str());
            response =  this->get_public_key(ring_name, id);
        }
        else{
            response["ret_code"] = RetCode::RingSigStatusCode::INVALID_MEMBER_POS;
            response["message"] = ret_message;
        }
    }

    virtual void get_private_key(const Json::Value& request,
                    Json::Value& response)
    {
        std::string ring_name = request["ring_name"].asString();
        std::string message;
        bool valid = check_member_pos(message, request["id"].asString());
        if(valid)
        {
            int id = atoi(request["id"].asString().c_str());
            response = this->get_private_key(ring_name, id);
        }
        else{
            response["ret_code"] = RetCode::RingSigStatusCode::INVALID_MEMBER_POS;
            response["message"] = message;
        }
    }
    /////ring sig related interface
    virtual void setup_ring(const Json::Value& request, 
                           Json::Value& response)
    {
        std::string ring_name = request["ring_name"].asString();
        int bit_len = 1024;
        if(request["bit_len"].type() != Json::nullValue)
            bit_len = request["bit_len"].asInt();
        bool force_cover = false;
        if(request["force_cover"].type() != Json::nullValue)
            force_cover = request["force_cover"].asBool();
        response = this->setup_ring(ring_name, 
                        bit_len, force_cover);
    }

    virtual void join_ring(const Json::Value& request, 
                           Json::Value& response)
    {
        std::string ring_name = request["ring_name"].asString();
        LOG(DEBUG)<<"join ring...";
        response = this->join_ring(ring_name);
    }

    virtual void linkable_ring_sig(const Json::Value& request,
            Json::Value& response)
    {
        std::string ring_name = request["ring_name"].asString();
        std::string message = request["message"].asString();
        std::string ret_message;
        bool valid = check_member_pos(ret_message, request["id"].asString());
        if(valid)
        {
            int id = atoi(request["id"].asString().c_str());
            int ring_size = DEFAULT_RING_SIZE;
            if(request["ring_size"].type() != Json::nullValue)
            {
                try{
                    ring_size = atoi(request["ring_size"].asString().c_str());
                }
                catch(exception &err)
                {
                    LOG(WARNING)<<"get ring_size failed, use default ring_size:"<<ring_size;
                }
            }
            //check ring_size;
            if( ring_size <= 1 )
            {
                response["ret_code"] = RetCode::RingSigStatusCode::INVALID_RING_SIZE;
                response["message"] = "invalid ring size " + to_string(ring_size) + 
                        "ring size must be larger than 1";
                return;
            }
            response = this->linkable_ring_sig(ring_name, 
                message, id, ring_size);
        }
        else{
            response["ret_code"] = RetCode::RingSigStatusCode::INVALID_MEMBER_POS;
            response["message"] = ret_message;
        }
    }

    virtual void linkable_ring_verify(const Json::Value& request,
            Json::Value& response)
    {
        std::string ring_name = request["ring_name"].asString();
        std::string message = request["message"].asString();
        std::string sig = request["sig"].asString();
        //LOG(DEBUG)<<"####SIG:"<<sig;
        response = this->linkable_ring_verify(ring_name, message, sig);
    }

    //pure virtual function
    virtual Json::Value setup_ring(const std::string& ring_name,
                    const int& bit_len, bool &force_cover) = 0;
    
    virtual Json::Value join_ring(const std::string& ring_name) = 0;

    virtual Json::Value linkable_ring_sig(
            const std::string& ring_name,
            const std::string& message,
            const int& id,
            const int& ring_size) = 0;
    
    virtual Json::Value linkable_ring_verify(
            const std::string& ring_name,
            const std::string& message,
            const std::string& sig) = 0;
    /////get interfaces
    virtual Json::Value get_ring_param(
            const std::string& ring_name) = 0;

    virtual Json::Value get_public_key(
            const std::string& ring_name,
            const int& pos) = 0;

    virtual Json::Value get_private_key(
            const std::string& ring_name,
            const int& pos) = 0;
    private:
        
        bool inline is_all_number(std::string& member_pos)
        {
            for(auto c : member_pos)
            {
                if( c >= '0' && c <= '9' )
                    continue;
                return false;
            }
            return true;
        }

        int inline check_member_pos(std::string& message,
                std::string member_pos)
        {
            //LOG(DEBUG)<<"check_member_pos";
            message = "";
            bool is_number_str = is_all_number(member_pos);
            if( !is_number_str )
            {
                message = "member position must be number and can't be negative";
                return false;
            }
            //whether overflow
            stringstream ss;
            int64_t mem_pos;
            ss << member_pos;
            ss >> mem_pos;
            if( (int64_t)mem_pos > (int64_t)(INT_MAX))
            {
                message = "member_postion is no more than:" + to_string(INT_MAX);
                return false;
            }
            return true;
        }

    const static int DEFAULT_RING_SIZE = 32;
};
