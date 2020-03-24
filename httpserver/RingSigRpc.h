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

/* @file: RingSigRpc.h
 * @author: fisco-dev
 * @date: 2018.03.11
 */
#pragma once

#include <jsonrpccpp/server.h>
#include <jsonrpccpp/server/connectors/httpserver.h>

#include "database/RingSigDB.h"
#include "easylog/easylog.h"
#include <group_sig/devcore/ConfigParser.h>
#include <group_sig/devcore/CommonStruct.h>

#include "httpserver/RingSigRpcInterface.h"

class RingSigRpc : public RingSigRpcInterface
{
public:
    RingSigRpc(const std::string &config_path = "")
    {
        config_parser.reset();
        if ("" != config_path)
        {
            LOG(DEBUG) << "load config:" << config_path;
            config_parser = std::make_shared<dev::eth::JsonConfigParser>(config_path);
        }
        LOG(DEBUG) << "init database";
        shared_ptr<DB::RingSigDB> db_handler =
            dev::eth::singleton<DB::RingSigDB>::instance(config_parser);
        db_interface = dev::eth::singleton<DB::RingSigDBInterface<DB::RingSigDB>>::instance(db_handler);
    }

    virtual RpcModuleVec ImplementedModules() const
    {
        return RpcModuleVec{RpcModule{"Linkable Ring Sig rpc", "1.0"}};
    }

    virtual Json::Value setup_ring(
        const std::string &ring_name,
        const int &bit_len, bool &force_cover);

    virtual Json::Value join_ring(const std::string &ring_name);

    virtual Json::Value linkable_ring_sig(
        const std::string &ring_name,
        const std::string &message,
        const int &id,
        const int &ring_size = 32);

    virtual Json::Value linkable_ring_verify(
        const std::string &ring_name,
        const std::string &message,
        const std::string &sig);
    /////get interface(for debug)
    virtual Json::Value get_ring_param(
        const std::string &ring_name);

    virtual Json::Value get_public_key(
        const std::string &ring_name,
        const int &pos);

    virtual Json::Value get_private_key(
        const std::string &ring_name,
        const int &pos);

private:
    inline bool get_public_key_list(Json::Value &ret_json,
                                    std::vector<string> &public_keys,
                                    const std::string &ring_name,
                                    const int &ring_size, const int &id);

private:
    std::shared_ptr<dev::eth::JsonConfigParser> config_parser;
    std::shared_ptr<DB::RingSigDBInterface<DB::RingSigDB>> db_interface;
};
