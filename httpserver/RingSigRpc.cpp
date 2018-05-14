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

/* @file: RingSigRpc.cpp
 * @author: fisco-dev
 * @date: 2018.03.16
 */

#include "httpserver/Common.h"
#include "httpserver/RingSigRpc.h"

#include "algorithm/RingSig.h"
#include "devcore/StatusCode.h"

using namespace RetCode::CommonStatusCode;
using namespace RetCode::RingSigStatusCode;

Json::Value RingSigRpc::setup_ring(const std::string& ring_name,
        const int& bit_len, bool &force_cover)
{
    Json::Value ret_json;
    //ensure ring doesn't exist
    std::string param_info;
    int db_ret;
    bool ret;
    std::string prefix="ring_name "+ring_name;
    if(!force_cover)
    {
        db_ret = db_interface->load_ring_param(param_info, ring_name);
        ret = Common::DB_access_ret(ret_json, db_ret, KEY_NOT_EXIST, prefix);
        if(!ret)
            return ret_json;
    }
    //gen param information
    bool internal_ret = RingSigApi::LinkableRingSig::setup_ring(
            param_info, bit_len);
    if(internal_ret != SUCCESS)
    {
        std::string err_msg = "call back linkable ring algorithm failed";
        LOG(ERROR)<<err_msg;
        Common::get_ret_info(ret_json, internal_ret, err_msg);
    }
    LOG(DEBUG)<<"##generated param_info:"<<param_info;
    //write param to DB
    prefix = "param of "+ring_name;
    db_ret = db_interface->store_ring_param(ring_name, param_info);
    ret = Common::DB_access_ret(ret_json, db_ret, KEY_EXIST, prefix);
    if(!ret)
        return ret_json;
    Common::get_ret_info(ret_json, (Json::Value)(param_info));
    LOG(DEBUG)<<"##gen param info succeed, param_info:"<<param_info;
    return ret_json;
}

//join ring
Json::Value RingSigRpc::join_ring(const std::string& ring_name)
{
    Json::Value ret_json;
    //ring must exist first
    std::string param_info;
    std::string prefix="ring_name "+ring_name;
    int db_ret = db_interface->load_ring_param(param_info, ring_name);
    bool succ = Common::DB_access_ret(ret_json, db_ret, KEY_EXIST, prefix);
    if(!succ)
        return ret_json;
    LOG(DEBUG)<<"##param_info"<<param_info;
    ////get pos to store public key and private key
    std::string pos;
    prefix = "pos of "+ring_name;
    db_ret = db_interface->load_ring_pos(pos, ring_name);
    succ = Common::DB_access_ret(ret_json, db_ret, KEY_EXIST, prefix);
    if(!succ)
    {
        LOG(INFO)<<"init first ring member";
        pos = "0";
    }
    LOG(DEBUG)<<"pos:"<<pos;
    ////join ring
    std::string private_key_info;
    std::string public_key_info;
    bool internal_ret = RingSigApi::LinkableRingSig::join_ring
        (private_key_info, public_key_info, param_info, pos);
    if(internal_ret != SUCCESS)
    {
        std::string err_msg = "join ring failed";
        LOG(ERROR)<<err_msg;
        Common::get_ret_info(ret_json, internal_ret, err_msg);
        return ret_json;
    }
    /////store public key
    LOG(DEBUG)<<"STORE PUBLIC KEY:"<<public_key_info;
    prefix = "public key for "+pos+"th of "+ring_name;
    db_ret = db_interface->store_public_key(ring_name,
            public_key_info, pos);
    succ = Common::DB_access_ret(ret_json, db_ret, KEY_EXIST, prefix);
    if(!succ)
        return ret_json;
    /////store private_key
    LOG(DEBUG)<<"STORE PRIVATE KEY:"<<private_key_info;
    prefix="private key for "+pos+"th of "+ring_name;
    db_ret = db_interface->store_private_key(ring_name,
            private_key_info, pos);
    succ = Common::DB_access_ret(ret_json, db_ret, KEY_EXIST, prefix);
    if(!succ)
        return ret_json;
    ////update pos
    std::string modified_pos = to_string(atoi(pos.c_str())+1);
    LOG(DEBUG)<<"modified pos "<<pos;
    //write modified pos to DB
    db_ret = db_interface->store_ring_pos(ring_name, modified_pos);
    prefix = "store pos to database";
    succ = Common::DB_access_ret(ret_json, db_ret, KEY_EXIST, prefix);
    if(!succ)
        return ret_json;
    Common::get_ret_info(ret_json, Json::Value(pos));
    ret_json["private_key"] = private_key_info;
    ret_json["public_key"] = public_key_info;
    return ret_json;
}


Json::Value RingSigRpc::linkable_ring_sig(
        const std::string& ring_name,
        const std::string& message,
        const int& id,
        const int& ring_size)
{
    Json::Value ret_json;
    //ring param
    std::string param_info;
    std::string prefix = "ring "+ring_name;
    int db_ret = db_interface->load_ring_param(
            param_info, ring_name);
    bool succ = Common::DB_access_ret(ret_json, db_ret, KEY_EXIST, prefix);
    if(!succ)
        return ret_json;
    LOG(DEBUG)<<"param info:"<<param_info;
    ////get private_key_info
    std::string private_key_info;
    prefix = "private_key of "+ring_name + "member "+ to_string(id);
    db_ret = db_interface->load_private_key(private_key_info, 
            ring_name, to_string(id));
    succ = Common::DB_access_ret(ret_json, db_ret, KEY_EXIST, prefix);
    if(!succ)
        return ret_json;
    /////get public_key_list
    std::vector<std::string> public_keys;
    bool ret = get_public_key_list(ret_json, public_keys, ring_name,
            ring_size, id);
    if (!ret)
        return ret_json;
    /////call back ring sig algorithm
    std::string sig;
    int internal_ret = RingSigApi::LinkableRingSig::ring_sig(sig,
            message, public_keys, private_key_info, param_info);
    if(internal_ret != SUCCESS)
    {
        std::string err_msg = "callback LinkableRingSig algorithm failed";
        LOG(ERROR)<<err_msg;
        Common::get_ret_info(ret_json, internal_ret, err_msg);
        return ret_json;
    }
    ret_json["param_info"] = param_info;
    ret_json["sig"] = sig;
    ret_json["message"] = message;
    ret_json["ret_code"] = SUCCESS;
    return ret_json;
}

inline bool RingSigRpc::get_public_key_list(
        Json::Value& ret_json,
        std::vector<string>& public_keys,
        const std::string& ring_name,
        const int& ring_size,
        const int& id)
{
    //get pos
    std::string pos;
    int db_ret = db_interface->load_ring_pos(pos, ring_name);
    std::string prefix="pos of ring "+ring_name;
    bool succ = Common::DB_access_ret(ret_json, db_ret, KEY_EXIST, prefix);
    if(!succ)
        return false;
    /////compare ring_size and pos
    int pos_num = atoi(pos.c_str());
    if( pos_num < ring_size)
    {
        std::string err_msg = "max ring size is "+pos+
            " not satified with required "+ to_string(ring_size);
        Common::get_ret_info(ret_json, RING_SIZE_TOO_BIG, err_msg);
        return false;
    }
    LOG(DEBUG)<<"pos:"<<pos<<" pos_num:"<<pos_num;
    ////get public_keys
    std::string tmp_public_key;
    int start_set = id/ring_size;
    LOG(DEBUG)<<"###start_set:"<<start_set;
    prefix = "public key of "+ring_name;
    // get public key list(and support customized ring size)
    // pubic key position:
    // [(mem_pos/ring_size * ring_size) % total_member, (mem_pos/ring_size * ring_size+ring_size-1)%total_member]
    // ensure that total number of public keys used to gen signature should be equal to customized ring size 
    for(int i= (start_set*ring_size)% pos_num;
            (public_keys.size() != (unsigned)ring_size);)
    {
        LOG(DEBUG)<<"get public from pos:"<<i;
        db_ret = db_interface->load_public_key(tmp_public_key, 
                 ring_name, to_string(i));
        succ = Common::DB_access_ret(ret_json, db_ret, KEY_EXIST,
                prefix+to_string(i));
        if(!succ)
            return false;
        public_keys.push_back(tmp_public_key);
        LOG(DEBUG)<<"public key "<<i<<":"<<tmp_public_key;
        i = (i+1) % pos_num;
    }
    return true;
}

///ring sig verify
Json::Value RingSigRpc::linkable_ring_verify(
        const std::string& ring_name,
        const std::string& message,
        const std::string& sig)
{
    Json::Value ret_json;
    //load ring param
    std::string param_info;
    std::string prefix = "param of ring "+ring_name;
    int db_ret = db_interface->load_ring_param(param_info, ring_name);
    bool succ = Common::DB_access_ret(ret_json, db_ret, KEY_EXIST,
            prefix); 
    //call back ring sig verify
    bool valid;
    int internal_ret = RingSigApi::LinkableRingSig::ring_verify(valid,
            sig, message, param_info);
    if(internal_ret != SUCCESS)
    {
        std::string err_msg = "failed to verify ring signature";
        Common::get_ret_info(ret_json, RING_SIG_VERIFY_FAILED,
                err_msg);
        LOG(ERROR)<<err_msg;
        return ret_json;
    }
    Common::get_ret_info<bool>(ret_json, valid);
    return ret_json;
}
 
Json::Value RingSigRpc::get_ring_param(
            const std::string& ring_name)
{
    Json::Value ret_json;
    std::string param_info;
    int db_ret = db_interface->load_ring_param(
            param_info, ring_name);
    std::string prefix = "param of ring "+ring_name;
    bool succ = Common::DB_access_ret(ret_json,
            db_ret, KEY_EXIST, prefix);
    if(!succ)
        return ret_json;
    Common::get_ret_info<std::string>(ret_json, 
            param_info);
    return ret_json;
}

Json::Value RingSigRpc::get_public_key(
            const std::string& ring_name,
            const int& pos)
{
    Json::Value ret_json;
    std::string public_key;
    std::string prefix = "public key for "+to_string(pos)+
        "th member of "+ring_name;
    int db_ret = db_interface->load_public_key(
            public_key, ring_name, to_string(pos));
    bool succ = Common::DB_access_ret(ret_json, 
            db_ret, KEY_EXIST, prefix);
    if(!succ)
        return ret_json;
    Common::get_ret_info<std::string>(ret_json, public_key);
    return ret_json;
}

Json::Value RingSigRpc::get_private_key(
            const std::string& ring_name,
            const int& pos)
{
    Json::Value ret_json;
    std::string private_key;
    std::string prefix = "private key for "+to_string(pos) +
        "th member of "+ring_name;
    int db_ret = db_interface->load_private_key(private_key,
            ring_name, to_string(pos));
    bool succ = Common::DB_access_ret(ret_json,
            db_ret, KEY_EXIST, prefix);
    if(!succ)
        return ret_json;
    Common::get_ret_info<std::string>(ret_json, private_key);
    return ret_json;
}
