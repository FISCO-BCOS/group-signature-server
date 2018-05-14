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

/* @file: GroupSig.cpp
 * @author:fisco-bcos
 * @date:2018.03.07
 * @function: interface implementation of group signature algorithm 
 */

#include <time.h>

#include "devcore/easylog.h"
#include "devcore/CommonFunc.h"

#include "algorithm/GroupSig.h"

using namespace dev::eth;

#if defined(__cplusplus)
namespace GroupSigApi
{
#endif
int create_group_default(std::string& result,
        const std::string& algorithm_method)
{
    LOG(DEBUG)<<"CREATE GROUP WITH DEFAULT PARAM";
    GroupSigMethod method = get_algo_method_by_string(algorithm_method);
    std::string param = "";
    return GroupSigFactory::instance(method)->create_group(result, param);
    //LOG(DEBUG)<<"result:"<<result;
    //LOG(DEBUG)<<"GROUP CREATE DONE";
}

int create_group(std::string& result,
        const std::string& algorithm_method,
        const std::string& pbc_param_info)
{
    LOG(DEBUG)<<"CREATE GROUP WITH SPECIFIED PARAM:"<<pbc_param_info;
    LOG(DEBUG)<<"METHOD = "<<algorithm_method;
    GroupSigMethod method = get_algo_method_by_string(algorithm_method);
    return GroupSigFactory::instance(method)->create_group(result, pbc_param_info);
    //LOG(DEBUG)<<"result is:"<<result;
}


//group  member join: group manager generate private key && cert for group members
int group_member_join(std::string& gsk,
        const std::string& algorithm_method,
        const std::string& pbc_param_info,
        const std::string& gmsk_info, 
        const std::string &gpk_info,
        const std::string& gamma_info)
{
   LOG(DEBUG)<<"GROUP MEMBER JOIN";
   LOG(DEBUG)<<"METHOD = "<<algorithm_method;
   GroupSigMethod method = get_algo_method_by_string(algorithm_method);
   std::string param = pbc_param_info + _get_split_symbol();
   param += gmsk_info + _get_split_symbol();
   param += gpk_info + _get_split_symbol();
   param += gamma_info;

   LOG(DEBUG)<<"PARAM IS:"<<param;
   return GroupSigFactory::instance(method)->gen_gsk(gsk, param);
   //LOG(DEBUG)<<"generated gsk is:"<<gsk;
}


//implementation of group sig
int group_sig(std::string& sig, 
        const std::string& algorithm_method,
        const std::string& gpk_info,
        const std::string& gsk_info,
        const std::string& pbc_param_info,
        const std::string &message)
{
   LOG(DEBUG)<<"GROUP SIG";
   LOG(DEBUG)<<"METHOD = "<<algorithm_method;
   GroupSigMethod method = get_algo_method_by_string(algorithm_method);
   std::string param = gpk_info + _get_split_symbol();
   param += gsk_info + _get_split_symbol();
   param += pbc_param_info + _get_split_symbol();
   param += message;
   LOG(DEBUG)<<"PARAM is:"<<param;
   double start = pbc_get_time();
   int ret = GroupSigFactory::instance(method)->group_sig(sig, param);
   double end = pbc_get_time();
   double cost = (end - start);
   LOG(TRACE)<<"group sig time:"<<cost<<" s";
   return ret;
}

//implementation of group verify
int group_verify(int& valid,
        const std::string& sig,
        const std::string& message,
        const std::string& algorithm_method,
        const std::string& gpk_info,
        const std::string& pbc_param_info)
{
    LOG(DEBUG)<<"BBS04 GROUP VERFIFY";
    LOG(DEBUG)<<"METHOD = "<<algorithm_method;
    GroupSigMethod method = get_algo_method_by_string(algorithm_method);
    
    std::string param = sig + _get_split_symbol();
    param += message + _get_split_symbol();
    param += gpk_info + _get_split_symbol();
    param += pbc_param_info;
    LOG(DEBUG)<<"PARAM:"<<param;

    double start = pbc_get_time();
    int ret = GroupSigFactory::instance(method)->group_verify(valid, param);
    double end = pbc_get_time();
    double cost = (end - start);
    //std::cout<<"group verify time:"<<cost<<" s"<<std::endl;
    LOG(TRACE)<<"group verify time:"<<cost<<" s";
    LOG(DEBUG)<<"RESULT:"<<valid;
    return ret;
}


//implementation of group open with given signature
int open_cert(std::string& cert, 
        const std::string& algorithm_method,
        const std::string& sig,
        const std::string& message,
        const std::string& gpk_info,
        const std::string& gmsk_info,
        const std::string& pbc_param_info)
{
    LOG(DEBUG)<<"BBS04 OPEN CERT";
    LOG(DEBUG)<<"METHOD = "<<algorithm_method;
    GroupSigMethod method = get_algo_method_by_string(algorithm_method);

    std::string param = sig + _get_split_symbol();
    param += message + _get_split_symbol();
    param += gpk_info + _get_split_symbol();
    param += gmsk_info + _get_split_symbol();
    param += pbc_param_info;
    LOG(DEBUG)<<"PARAM:"<<param;
    return GroupSigFactory::instance(method)->group_open(cert, param);
    //LOG(DEBUG)<<"OPEN CERT:"<<cert;
}


//update gpk when group memeber revoked
int revoke_member(std::string& gpk,
        const std::string& algorithm_method,
        const std::string& pbc_param,
        const std::string& revoke_info,
        const std::string& gamma_info)
{
    LOG(DEBUG)<<"BBS04 REVOKE MEMBER";
    LOG(DEBUG)<<"METHOD = "<<algorithm_method;
    GroupSigMethod method = get_algo_method_by_string(algorithm_method);

    std::string param = pbc_param + _get_split_symbol();
    param += revoke_info + _get_split_symbol();
    param += gamma_info;
    LOG(DEBUG)<<"PARAM:"<<param;
    return GroupSigFactory::instance(method)->gm_revoke(gpk, param);
    //LOG(DEBUG)<<"UPDATED GPK:"<<gpk;
}


//update group member private key after some members revoked
int revoke_update_private_key(std::string& gsk, 
        const std::string& algorithm_method,
        const std::string& pbc_param, 
        const std::string& revoke_list,
        const std::string& gone_list, 
        const std::string& gtwo)
{
    LOG(DEBUG)<<"UPDATE GSK AFTER REVOKE member";
    LOG(DEBUG)<<"METHOD = "<<algorithm_method;
    GroupSigMethod method = get_algo_method_by_string(algorithm_method);

    std::string param = pbc_param + _get_split_symbol();
    param += revoke_list + _get_split_symbol();
    param += gone_list + _get_split_symbol();
    param += gtwo;
    LOG(DEBUG)<<"PARAM:"<<param;
    return GroupSigFactory::instance(method)->update_gsk(gsk, param);
    //LOG(DEBUG)<<"UPDATED gsk:"<<gsk;
}

#if defined(__cplusplus)
}
#endif
