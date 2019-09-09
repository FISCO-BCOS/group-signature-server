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

/* @file: GroupSigRpc.cpp
 * @author: fisco-dev
 * @date: 2018.03.11
 * @function: implementation of group sig related RPC
 */

#include "httpserver/Common.h"
#include "httpserver/GroupSigRpc.h"

#include <group_sig/algorithm/GroupSig.h>
#include <group_sig/algorithm/bbs04/GroupSig_BBS.h>
#include <group_sig/devcore/CommonFunc.h>

using namespace RetCode::CommonStatusCode;
using namespace RetCode::GroupSigStatusCode;
using namespace RetCode::DBAccessStatus;

const std::string GroupSigRpc::PARAM_DELIM = "#";
//generate gpk, gmsk for group specified by group_name
Json::Value GroupSigRpc::create_group(const std::string &algorithm_method,
                                      const std::string &gmsk_pass,
                                      const std::string &group_name, const std::string &pbc_param,
                                      int create_always)
{
    Json::Value ret_json;
    //create always with force param
    if (!create_always)
    {
        std::string gpk_info;
        std::string prefix = "group " + group_name;
        bool group_not_exist = get_group_public_info(gpk_info,
                                                     ret_json, group_name, KEY_NOT_EXIST, prefix);
        if (!group_not_exist)
        {
            LOG(WARNING) << "group " << group_name << " already exists";
            return ret_json;
        }
    }

    LOG(DEBUG) << "CREATE GROUP NOW";
    std::string result;
    int internal_ret = GroupSigApi::create_group(result,
                                                 algorithm_method, pbc_param);
    vector<string> result_vec = dev::eth::split(result, dev::eth::BBSGroupSig::RET_DELIM);
    //vector[0]: gpk --return and store
    //vector[1]: gmsk --store
    //vector[2]: gamma --store
    //vector[3]: pbc_param --store
    if (internal_ret != SUCCESS || result_vec.size() != 4)
    {
        std::string error_msg = "create group failed for group sig alogorithm error";
        Common::get_ret_info(ret_json, internal_ret, error_msg);
        return ret_json;
    }
    LOG(DEBUG) << "store gpk:" << result_vec[0];

    int status = db_interface->store_gpk(group_name, result_vec[0]);
    std::string prefix = "store gpk";
    bool succ = Common::DB_access_ret(ret_json, status, KEY_EXIST, prefix);
    if (!succ)
        return ret_json;

    LOG(DEBUG) << "store gmsk:" << result_vec[1];
    prefix = "store gmsk";
    status = db_interface->store_gmsk(group_name, gmsk_pass, result_vec[1]);
    succ = Common::DB_access_ret(ret_json, status, KEY_EXIST, prefix);
    if (!succ)
        return ret_json;

    LOG(DEBUG) << "store gamma:" << result_vec[2];
    status = db_interface->store_gamma(group_name, gmsk_pass, result_vec[2]);
    prefix = "store gamma";
    succ = Common::DB_access_ret(ret_json, status, KEY_EXIST, prefix);
    if (!succ)
        return ret_json;

    LOG(DEBUG) << "store pbc_param info" << result_vec[3];
    std::string sys_param = result_vec[3] + PARAM_DELIM + algorithm_method;

    status = db_interface->store_pbc_param(group_name, sys_param);
    prefix = "store sys param";
    succ = Common::DB_access_ret(ret_json, status, KEY_EXIST, prefix);
    if (!succ)
        return ret_json;
    //return Json::Value(result_vec[0]);
    Common::get_ret_info<std::string>(ret_json, result_vec[0]);
    return ret_json;
}

//get group public info
inline bool GroupSigRpc::get_group_public_info(
    std::string &gpk_info,
    Json::Value &ret_json,
    const std::string &group_name,
    const int &expected_status,
    const std::string &prefix)
{
    int status = db_interface->load_gpk(gpk_info, group_name);
    return Common::DB_access_ret(ret_json, status, expected_status, prefix);
}

//get group gpk info and pbc param info
Json::Value GroupSigRpc::get_group_public_info(
    const std::string &group_name)
{
    std::string gpk_info;
    Json::Value ret_json;
    std::string prefix = "group " + group_name;
    bool success = get_group_public_info(gpk_info, ret_json,
                                         group_name, KEY_EXIST, prefix);
    if (success)
        Common::get_ret_info<std::string>(ret_json,
                                          gpk_info);
    return ret_json;
}

inline bool GroupSigRpc::get_gmsk_info(std::string &gmsk_info,
                                       Json::Value &ret_json, const std::string &group_name,
                                       const int &expected_status, const std::string &prefix)
{
    int status = db_interface->load_gmsk(gmsk_info, group_name);
    return Common::DB_access_ret(ret_json, status, expected_status, prefix);
}

Json::Value GroupSigRpc::get_gmsk_info(
    const std::string &group_name,
    const std::string &gm_pass)
{
    //TODO group manager authentication:gm_pass
    std::string gmsk_info;
    Json::Value ret_json;
    std::string prefix = "group " + group_name;
    bool success = get_gmsk_info(gmsk_info, ret_json,
                                 group_name, KEY_EXIST, prefix);
    if (success)
        Common::get_ret_info<std::string>(ret_json, gmsk_info);
    return ret_json;
}

inline bool GroupSigRpc::get_gsk_info(std::string &gsk_info,
                                      Json::Value &ret_json, const std::string &group_name,
                                      const std::string &member_id,
                                      const int &expected_status, const std::string &prefix)
{
    int status = db_interface->load_gsk(gsk_info, group_name, member_id);
    return Common::DB_access_ret(ret_json, status, expected_status, prefix);
}

//get gsk info
Json::Value GroupSigRpc::get_gsk_info(const std::string &group_name,
                                      const std::string &member_id,
                                      const std::string &pass)
{
    std::string gsk_info;
    Json::Value ret_json;
    std::string prefix = "member " + member_id + " of group " + group_name;
    bool success = get_gsk_info(gsk_info, ret_json,
                                group_name, member_id, KEY_EXIST, prefix);
    if (success)
        Common::get_ret_info<std::string>(ret_json, gsk_info);
    return ret_json;
}

Json::Value GroupSigRpc::get_revoked_list(const std::string &group_name)
{

    return Json::Value(Json::nullValue);
}

Json::Value GroupSigRpc::get_gone_list(const std::string &group_name)
{
    return Json::Value(Json::nullValue);
}

//get pbc_param_info
inline bool GroupSigRpc::get_pbc_param(std::string &pbc_param,
                                       Json::Value &ret_json, const std::string &group_name,
                                       const int &expected_status, const std::string &prefix)
{
    int status = db_interface->load_pbc_param(pbc_param, group_name);
    return Common::DB_access_ret(ret_json, status, expected_status, prefix);
}

inline bool GroupSigRpc::get_gamma(std::string &gamma_info,
                                   Json::Value &ret_json, const std::string &group_name,
                                   const int &expected_status, const std::string &prefix)
{
    int status = db_interface->load_gamma(gamma_info, group_name);
    return Common::DB_access_ret(ret_json, status, expected_status, prefix);
}

//create private key for group
Json::Value GroupSigRpc::join_group(const std::string &group_name,
                                    const std::string &gsk_pass,
                                    const std::string &member_id)
{
    //judge group existence
    std::string gpk_info;
    Json::Value ret_json;
    std::string prefix = "member " + member_id + " of group " + group_name;
    //check group member existence
    std::string gsk_info;
    bool member_not_exist = get_gsk_info(gsk_info, ret_json,
                                         group_name, member_id,
                                         KEY_NOT_EXIST, prefix);
    if (!member_not_exist)
    {
        LOG(WARNING) << "member of group " << group_name << " has already joined";
        return ret_json;
    }
    //check group existence
    prefix = "group " + group_name;
    bool group_exist = get_group_public_info(gpk_info, ret_json,
                                             group_name, KEY_EXIST, prefix);
    if (!group_exist)
    {
        LOG(WARNING) << prefix << " doesn't exist";
        return ret_json;
    }
    //get gmsk info
    LOG(DEBUG) << "get gmsk info";
    std::string gmsk_info;
    prefix = "group " + group_name;
    bool gmsk_exist = get_gmsk_info(gmsk_info, ret_json,
                                    group_name, KEY_EXIST, prefix);
    if (!gmsk_exist)
    {
        LOG(ERROR) << "gmsk of group " << group_name << " doesn't exist";
        return ret_json;
    }
    ///get gamma info
    std::string gamma_info;
    prefix = "gamma of " + group_name;
    bool gamma_exist = get_gamma(gamma_info, ret_json,
                                 group_name, KEY_EXIST, prefix);
    if (!gamma_exist)
    {
        LOG(ERROR) << prefix << " failed";
        return ret_json;
    }
    ////get system param
    std::string pbc_param;
    std::string algorithm_method;
    bool get_param_succ = parse_param(pbc_param,
                                      algorithm_method, ret_json, group_name);
    if (!get_param_succ)
        return ret_json;
    LOG(DEBUG) << "pbc param info:" << pbc_param;
    //group member join
    GroupSigApi::group_member_join(gsk_info, algorithm_method,
                                   pbc_param, gmsk_info, gpk_info, gamma_info);

    int status = db_interface->store_gsk(group_name,
                                         member_id, gsk_pass, gsk_info);
    prefix = "store private key for " + member_id;
    bool success = Common::DB_access_ret(ret_json, status, KEY_EXIST,
                                         prefix);
    if (!success)
    {
        LOG(ERROR) << prefix << " failed";
        return ret_json;
    }

    LOG(DEBUG) << "JOIN MEMBER SUCCE:" << gsk_info;
    Common::get_ret_info(ret_json, gsk_info);
    return ret_json;
}

//parse system param to pbc param and algorithm_info
inline bool GroupSigRpc::parse_param(std::string &pbc_param,
                                     std::string &algorithm_info,
                                     Json::Value &ret_json, const std::string &group_name)
{
    LOG(DEBUG) << "get pbc param info and algorithm method";
    std::string prefix = "param of group " + group_name;
    bool param_exist = get_pbc_param(pbc_param, ret_json,
                                     group_name, KEY_EXIST, prefix);
    if (!param_exist)
    {
        LOG(WARNING) << prefix << " doesn't exist";
        return false;
    }
    std::vector<std::string> param_vec = dev::eth::split(pbc_param, PARAM_DELIM);
    if (param_vec.size() != PARAM_NUM)
    {
        std::string error_msg = "get pbc param information failed";
        LOG(ERROR) << error_msg;
        Common::get_ret_info(ret_json, OBTAIN_ALGORITHM_FAILED, error_msg);
        return false;
    }
    LOG(DEBUG) << "pbc param info:" << param_vec[0];
    algorithm_info = param_vec[1];
    pbc_param = param_vec[0];
    return true;
}

//generate signature for group member specified by member_id
Json::Value GroupSigRpc::group_sig(const std::string &group_name,
                                   const std::string &member_id,
                                   const std::string &message,
                                   const std::string &gsk_pass)
{
    Json::Value ret_json;
    //###load gsk info
    LOG(DEBUG) << "get gsk info for group sig";
    std::string gsk_info;
    std::string prefix = "member " + member_id;
    bool member_exist = get_gsk_info(gsk_info, ret_json,
                                     group_name, member_id, KEY_EXIST, prefix);
    if (!member_exist)
        return ret_json;
    //####load algorithm_method && pbc_param_info
    std::string pbc_param, algorithm_method;
    bool get_param_succ = parse_param(pbc_param,
                                      algorithm_method, ret_json, group_name);
    if (!get_param_succ)
        return ret_json;
    //####load gpk info
    std::string gpk_info;
    prefix = "get gpk for group " + group_name + " failed";
    bool group_exist = get_group_public_info(gpk_info,
                                             ret_json, group_name, KEY_EXIST, prefix);
    if (!group_exist)
        return ret_json;
    //group sig
    std::string sig;
    GroupSigApi::group_sig(sig, algorithm_method,
                           gpk_info, gsk_info, pbc_param, message);
    LOG(DEBUG) << "sig:" << Json::Value(sig);
    ret_json["sig"] = Json::Value(sig);
    ret_json["gpk"] = Json::Value(gpk_info);
    ret_json["pbc_param"] = Json::Value(pbc_param);
    ret_json["message"] = Json::Value(message);
    ret_json["ret_code"] = RetCode::CommonStatusCode::SUCCESS;
    return ret_json;
}

//generate signature for given group
Json::Value GroupSigRpc::group_verify(const std::string &group_name,
                                      const std::string &message,
                                      const std::string &sig)
{
    Json::Value ret_json;
    //####load gpk info
    std::string gpk_info;
    std::string prefix = "group " + group_name;
    bool group_exist = get_group_public_info(gpk_info,
                                             ret_json, group_name, KEY_EXIST, prefix);
    if (!group_exist)
        return ret_json;
    //###load system param info
    std::string pbc_param, algorithm_info;
    bool parse_succ = parse_param(pbc_param, algorithm_info,
                                  ret_json, group_name);
    if (!parse_succ)
        return ret_json;
    //verify group_sig
    int is_valid;
    int internal_ret = GroupSigApi::group_verify(
        is_valid, sig,
        message, algorithm_info,
        gpk_info, pbc_param);
    if (internal_ret == SUCCESS)
    {
        bool valid;
        is_valid == 1 ? valid = true : valid = false;
        Common::get_ret_info<bool>(ret_json, valid);
    }
    else
    {
        std::string error_msg = "callback group sig algorithm failed";
        Common::get_ret_info(ret_json, internal_ret, error_msg);
    }
    return ret_json;
}

Json::Value GroupSigRpc::open_cert(const std::string &group_name,
                                   const std::string &sig, const std::string &message,
                                   std::string &gm_pass)
{
    //group exists?
    std::string gpk_info;
    Json::Value ret_json;
    std::string prefix = "group " + group_name;
    bool group_exist = get_group_public_info(gpk_info,
                                             ret_json, group_name, KEY_EXIST, prefix);
    if (!group_exist)
        return ret_json;
    //get system param
    std::string pbc_param;
    std::string algorithm_info;
    bool get_param_succ = parse_param(pbc_param, algorithm_info,
                                      ret_json, group_name);
    if (!get_param_succ)
        return ret_json;
    //get group manager infomation
    std::string gmsk_info;
    prefix = "gmsk info of " + group_name;
    bool gmsk_obtained = get_gmsk_info(gmsk_info, ret_json,
                                       group_name, KEY_EXIST, prefix);
    if (!gmsk_obtained)
        return ret_json;
    std::string cert;
    int internal_ret = GroupSigApi::open_cert(cert,
                                              algorithm_info, sig, message, gpk_info, gmsk_info,
                                              pbc_param);
    if (internal_ret != SUCCESS)
    {
        std::string info = "open cert failed";
        Common::get_ret_info(ret_json, internal_ret, info);
        return ret_json;
    }
    Common::get_ret_info<string>(ret_json, cert);
    return ret_json;
}

Json::Value GroupSigRpc::revoke_member(const std::string &group_name,
                                       const std::string &revoked_member, std::string &gm_pass)
{
    return Json::Value(Json::nullValue);
}

Json::Value GroupSigRpc::revoke_update_private_key(const std::string &group_name,
                                                   const std::string &member_id)
{
    return Json::Value(Json::nullValue);
}
