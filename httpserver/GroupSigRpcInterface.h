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

/* @file: GroupSigRpcInterface.h
 * @author: fisco-dev
 * @date: 2018.03.11
 */
#pragma once
#include <jsonrpccpp/server.h>
#include <jsonrpccpp/server/connectors/httpserver.h>

#include "devcore/easylog.h"

#include "httpserver/ServerInterface.h"

class GroupSigRpcInterface: public ServerInterface<GroupSigRpcInterface>
{
public:
    GroupSigRpcInterface()
    {
        LOG(DEBUG)<<"bind and add method print_hello";
        //create_group
        bind_and_add_method(jsonrpc::Procedure("create_group",
                jsonrpc::PARAMS_BY_NAME, jsonrpc::JSON_STRING,
                "gm_pass", jsonrpc::JSON_STRING, 
                "group_name", jsonrpc::JSON_STRING,
                NULL), &GroupSigRpcInterface::create_group);

        //join_group
        bind_and_add_method(jsonrpc::Procedure("join_group", 
                jsonrpc::PARAMS_BY_NAME, jsonrpc::JSON_STRING,
                "group_name", jsonrpc::JSON_STRING, 
                "member_name", jsonrpc::JSON_STRING, 
                NULL), &GroupSigRpcInterface::join_group);
        //group_sig
        bind_and_add_method(jsonrpc::Procedure("group_sig",
                jsonrpc::PARAMS_BY_NAME, jsonrpc::JSON_STRING,
                "group_name", jsonrpc::JSON_STRING,
                "member_name", jsonrpc::JSON_STRING,
                "message",  jsonrpc::JSON_STRING,
                NULL), &GroupSigRpcInterface::group_sig);
        //group verify
        bind_and_add_method(jsonrpc::Procedure("group_verify",
                jsonrpc::PARAMS_BY_NAME, jsonrpc::JSON_STRING,
                "group_name", jsonrpc::JSON_STRING,
                "group_sig", jsonrpc::JSON_STRING,
                "message", jsonrpc::JSON_STRING,
                NULL), &GroupSigRpcInterface::group_verify);
        //open cert
        bind_and_add_method(jsonrpc::Procedure("open_cert",
                jsonrpc::PARAMS_BY_NAME, jsonrpc::JSON_STRING,
                "group_name", jsonrpc::JSON_STRING,
                "group_sig", jsonrpc::JSON_STRING,
                "message", jsonrpc::JSON_STRING,
                "gm_pass", jsonrpc::JSON_STRING,
                NULL), &GroupSigRpcInterface::open_cert);
        //revoke member
        bind_and_add_method(jsonrpc::Procedure("revoke_memeber", 
                jsonrpc::PARAMS_BY_NAME, jsonrpc::JSON_STRING,
                "group_name", jsonrpc::JSON_STRING,
                "revoke_member", jsonrpc::JSON_STRING,
                "gm_pass", jsonrpc::JSON_STRING,
                NULL), &GroupSigRpcInterface::revoke_member);
        //revoke update private key
        bind_and_add_method(jsonrpc::Procedure("update_member_private_key",
                jsonrpc::PARAMS_BY_NAME, jsonrpc::JSON_STRING,
                "group_name", jsonrpc::JSON_STRING,
                "group_member", jsonrpc::JSON_STRING,
                NULL), &GroupSigRpcInterface::revoke_member);
        //get interfaces
        //gpk info
        bind_and_add_method(jsonrpc::Procedure("get_public_info",
                 jsonrpc::PARAMS_BY_NAME,jsonrpc::JSON_STRING,
                 "group_name", jsonrpc::JSON_STRING,
                 NULL), &GroupSigRpcInterface::get_group_public_info);
        //gmsk info
        bind_and_add_method(jsonrpc::Procedure("get_gmsk_info",
                    jsonrpc::PARAMS_BY_NAME,jsonrpc::JSON_STRING,
                    "group_name", jsonrpc::JSON_STRING,
                    "gm_pass", jsonrpc::JSON_STRING,
                     NULL), &GroupSigRpcInterface::get_gmsk_info);
        //gmsk info
        bind_and_add_method(jsonrpc::Procedure("get_gsk_info",
                    jsonrpc::PARAMS_BY_NAME,jsonrpc::JSON_STRING,
                    "group_name", jsonrpc::JSON_STRING,
                    "member_name", jsonrpc::JSON_STRING,
                    "pass", jsonrpc::JSON_STRING,
                     NULL), &GroupSigRpcInterface::get_gsk_info);
        //revoked list info
        bind_and_add_method(jsonrpc::Procedure("get_revoked_list",
                    jsonrpc::PARAMS_BY_NAME,jsonrpc::JSON_STRING,
                    "group_name", jsonrpc::JSON_STRING,
                    NULL), &GroupSigRpcInterface::get_revoked_list);
        //gone list info
        bind_and_add_method(jsonrpc::Procedure("get_gone_list",
                    jsonrpc::PARAMS_BY_NAME,jsonrpc::JSON_STRING,
                    "group_name", jsonrpc::JSON_STRING,
                    NULL), &GroupSigRpcInterface::get_gone_list);

    }
    
    //######LOAD RPC INTERFACE
    //gpk && pbc_param
    virtual void get_group_public_info(const Json::Value& request,
            Json::Value& response)
    {
        std::string group_name = request["group_name"].asString();
        response = this->get_group_public_info(group_name);
    }
    
    virtual void get_gmsk_info(const Json::Value& request,
            Json::Value& response)
    {
        std::string group_name = request["group_name"].asString();
        std::string gm_pass = request["gm_pass"].asString();
        response = this->get_gmsk_info(group_name, gm_pass);
    }

    virtual void get_gsk_info(const Json::Value& request,
            Json::Value& response)
    {
        std::string group_name = request["group_name"].asString();
        std::string member_name = request["member_name"].asString();
        std::string member_pass = request["pass"].asString();
        response = this->get_gsk_info(group_name, member_name, member_pass); 
    }
    
    //get all revoked_info of a given group
    virtual void get_revoked_list(const Json::Value& request,
            Json::Value& response)
    {
        std::string group_name = request["group_name"].asString();
        response = this->get_revoked_list(group_name);
    }
    
    virtual void get_gone_list(const Json::Value& request,
            Json::Value& response)
    {
        std::string group_name = request["group_name"].asString();
        response = this->get_gone_list(group_name);
    }
    
    virtual void create_group(const Json::Value& request,
            Json::Value& response)
    {
        LOG(DEBUG)<<"CREATE GROUP";
        //std::string algorithm_method = request["algorithm_method"].asString();
        std::string group_name = request["group_name"].asString();
        LOG(DEBUG)<<"group name:"<<group_name;
        
        std::string gm_pass = request["gm_pass"].asString();
        std::string pbc_param_info = "";
        if(request["pbc_param"].type() != Json::nullValue)
        { pbc_param_info = request["pbc_param"].asString(); }
        LOG(DEBUG)<<"pbc param info:"<<pbc_param_info;
        
        bool create_always = 0;
        if(request["create_always"].type()!= Json::nullValue)
        { create_always = request["create_always"].asInt(); }
        
        std::string algorithm_method = "bbs04";
        if(request["algorithm_method"].type() != Json::nullValue)
        {
            algorithm_method = request["algorithm_method"].asString();
        }
        LOG(DEBUG)<<"algorithm method:"<<algorithm_method;
        response = this->create_group(algorithm_method,gm_pass,
                   group_name, pbc_param_info, create_always);
    }
    

    virtual void join_group(const Json::Value& request,
            Json::Value& response)
    {
        std::string group_name = request["group_name"].asString();
        std::string member_id = request["member_name"].asString();
        LOG(DEBUG)<<"gen private key for "<<member_id<<" of "<<group_name;
        std::string gsk_pass = "";
        if(request["pass"].type() != Json::nullValue)
        {
            gsk_pass = request["pass"].asString();
        }
        response = this->join_group(group_name, gsk_pass, member_id);
    }
    
    virtual void group_sig(const Json::Value& request,
            Json::Value& response)
    {
        std::string group_name = request["group_name"].asString();
        std::string member_id = request["member_name"].asString();
        std::string message = request["message"].asString();
        std::string gsk_pass = "";
        if(request["pass"].type() != Json::nullValue)
        {
            gsk_pass = request["pass"].asString();
        }
        LOG(DEBUG)<<"sig for "<<group_name;
        response = group_sig(group_name, member_id, message, gsk_pass);
    }

    virtual void group_verify(const Json::Value& request,
            Json::Value& response)
    {
        std::string group_name = request["group_name"].asString();
        std::string sig = request["group_sig"].asString();
        std::string message = request["message"].asString();
        LOG(DEBUG)<<"verify "<<sig<<" generated by some member of group  "
                  <<group_name;
        response = group_verify(group_name, message, sig);
    }
    
    virtual void open_cert(const Json::Value& request,
            Json::Value& response)
    {
        std::string group_name = request["group_name"].asString();
        std::string sig = request["group_sig"].asString();
        std::string gm_pass = request["gm_pass"].asString();
        std::string message = request["message"].asString();
        LOG(DEBUG)<<"open cert for "<<sig;
        response =open_cert(group_name, sig, message, gm_pass);
    }

    virtual void revoke_member(const Json::Value& request, 
            Json::Value& response)
    {
        std::string group_name = request["group_name"].asString();
        std::string revoked_member = request["revoked_member"].asString();
        std::string gm_pass = request["gm_pass"].asString();
        LOG(DEBUG)<<"revoke member "<<revoked_member<<" for group "<<group_name;
        response = revoke_member(group_name, revoked_member, gm_pass);
    }
    
    virtual void revoke_update_private_key(const Json::Value& request,
            Json::Value& response)
    {
        std::string group_name = request["group_name"].asString();
        std::string member_id = request["group_member"].asString();
        LOG(DEBUG)<<"update private key for "<<member_id
                  <<" after some one revoked";
        response = revoke_update_private_key(group_name, member_id);
    }

    virtual RpcModuleVec ImplementedModules() const
    {
        return  RpcModuleVec{RpcModule{"test", "1.0"}};
    };
    
    //get interface
    virtual Json::Value get_group_public_info(const std::string& group_name) = 0;
    virtual Json::Value get_gmsk_info(const std::string& group_name,
            const std::string& gm_pass) = 0;
    virtual Json::Value get_gsk_info(const std::string& group_name,
            const std::string& member_id, const std::string& pass) = 0;
   
    virtual Json::Value get_revoked_list(const std::string& group_name) =0;
    virtual Json::Value get_gone_list(const std::string& group_name ) = 0; 
    //interface of implementation
    //generate gpk, gmsk for group specified by group_name
    virtual Json::Value create_group(const std::string& algorithm_method,
            const std::string& gm_pass,
            const std::string &group_name, const std::string& pbc_param="",
            int create_always = 0) = 0; 
    //create private key for group
    virtual Json::Value join_group(const std::string& group_name,
            const std::string& gsk_pass,
            const std::string& member_id)= 0;
    
    //generate signature for group member specified by member_id
    virtual Json::Value group_sig(const std::string& group_name,
            const std::string& member_id,
            const std::string& message,
            const std::string& gsk_pass) = 0;
    
    //generate signature for given group 
    virtual Json::Value group_verify(const std::string& group_name,
            const std::string& message,
            const std::string& sig) = 0;

    virtual Json::Value open_cert(const std::string& group_name,
            const std::string& sig, 
            const std::string& message,
            std::string& gm_pass) = 0;

    virtual Json::Value revoke_member(const std::string& group_name,
            const std::string& revoked_member, std::string& gm_pass) = 0;

    virtual Json::Value revoke_update_private_key(const std::string& group_name,
            const std::string &member_id) = 0;
};
