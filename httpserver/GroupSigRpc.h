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

/* @file: GroupSigRpc.h
 * @author: fisco-dev
 * @date: 2018.03.11
 * @function: implementation of group sig algorithm
 */
#pragma once
#include <memory>

#include "database/GroupSigDB.h"
#include "easylog/easylog.h"

#include "httpserver/GroupSigRpcInterface.h"

//implementation of group sig algorithm
class GroupSigRpc : public GroupSigRpcInterface
{
public:
        GroupSigRpc(const std::string &config_path = "") : GroupSigRpcInterface()
        {
                config_handler.reset();
                if ("" != config_path)
                {
                        LOG(DEBUG) << "load config information, config path" << config_path;
                        config_handler = std::make_shared<dev::eth::JsonConfigParser>(config_path);
                }
                //create db interface
                shared_ptr<DB::GroupSigDB> p_db =
                    singleton<DB::GroupSigDB>::instance(config_handler);

                db_interface = singleton<DB::GroupSigDBInterface<DB::GroupSigDB>>::instance(p_db);
                LOG(DEBUG) << "GROUP SIG DB has been inited";
        }

        //get interface
        virtual Json::Value get_group_public_info(const std::string &group_name);
        virtual Json::Value get_gmsk_info(const std::string &group_name,
                                          const std::string &gm_pass);

        virtual Json::Value get_gsk_info(const std::string &group_name,
                                         const std::string &member_id, const std::string &pass);

        virtual Json::Value get_revoked_list(const std::string &group_name);
        virtual Json::Value get_gone_list(const std::string &group_name);

        //generate gpk, gmsk for group specified by group_name
        virtual Json::Value create_group(const std::string &algorithm_method,
                                         const std::string &gmsk_pass,
                                         const std::string &group_name, const std::string &pbc_param = "",
                                         int create_always = 0);
        //create private key for group
        virtual Json::Value join_group(const std::string &group_name,
                                       const std::string &gsk_pass,
                                       const std::string &member_id);

        //generate signature for group member specified by member_id
        virtual Json::Value group_sig(const std::string &group_name,
                                      const std::string &member_id,
                                      const std::string &message,
                                      const std::string &gsk_pass);

        //generate signature for given group
        virtual Json::Value group_verify(const std::string &group_name,
                                         const std::string &message,
                                         const std::string &sig);

        virtual Json::Value open_cert(const std::string &group_name,
                                      const std::string &sig,
                                      const std::string &message,
                                      std::string &gm_pass);

        virtual Json::Value revoke_member(const std::string &group_name,
                                          const std::string &revoked_member, std::string &gm_pass);

        virtual Json::Value revoke_update_private_key(const std::string &group_name,
                                                      const std::string &member_id);

        virtual RpcModuleVec ImplementedModules() const
        {
                return RpcModuleVec{RpcModule{"group_sig bbs04 rpc", "1.0"}};
        };

private:
        /*inline bool DB_access_ret(Json::Value& ret_json, 
                              const int& ret_code, 
                              const int& expected_ret_code,
                              const std::string& prefix_str="");*/

        inline bool get_group_public_info(std::string &gpk_info,
                                          Json::Value &ret_json, const std::string &group_name,
                                          const int &expected_status, const std::string &prefix = "");

        inline bool get_gmsk_info(std::string &gmsk_info,
                                  Json::Value &ret_json, const std::string &group_name,
                                  const int &expected_status, const std::string &prefix = "");

        inline bool get_gsk_info(std::string &gsk_info,
                                 Json::Value &ret_json, const std::string &group_name,
                                 const std::string &member_id,
                                 const int &expected_status, const std::string &prefix = "");

        inline bool get_pbc_param(std::string &pbc_param,
                                  Json::Value &ret_json, const std::string &group_name,
                                  const int &expected_status, const std::string &prefix = "");

        inline bool get_gamma(std::string &gamma_info,
                              Json::Value &ret_json, const std::string &group_name,
                              const int &expected_status, const std::string &prefix = "");

        inline bool parse_param(std::string &pbc_param,
                                std::string &algorithm_info,
                                Json::Value &ret_json,
                                const std::string &group_name);

private:
        std::shared_ptr<dev::eth::JsonConfigParser> config_handler;
        std::shared_ptr<DB::GroupSigDBInterface<DB::GroupSigDB>> db_interface;

        const static std::string PARAM_DELIM;
        const static int PARAM_NUM = 2;
};
