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

/* @file: GroupSigDB.cpp 
 * @author: fisco-dev
 * @date: 2018.03.10
 * @function: concrete implementation of db operation
 *            related to group sig 
 */

#include "devcore/CommonFunc.h"

#include "database/GroupSigDB.h"

namespace DB
{
    const std::string GroupSigDB::KEY_SPLIT_SYMBOL = "_";
    const std::string GroupSigDB::VALUE_SPLIT_SYMBOL = "#";
    const std::string GroupSigDB::GPK_KEY = "gpk";
    const std::string GroupSigDB::PARAM_KEY = "param";
    const std::string GroupSigDB::GM_KEY = "gmsk";
    const std::string GroupSigDB::GAMMA_KEY="gamma";
    const std::string GroupSigDB::REVOKE_DEBUG_KEY = "revoke_info";
    const std::string GroupSigDB::GONE_DEBUG_KEY = "gone_info";
    //store group sig related params to database
    //gpk info
    int GroupSigDB::store_gpk(const std::string& group_name,
            const std::string& gpk_info)
    {
      return db_interface->db_put(group_name, GPK_KEY, gpk_info); 
    }
       
    int GroupSigDB::store_pbc_param(const std::string& group_name,
              const std::string& pbc_param_info)
    {
      return db_interface->db_put(group_name, PARAM_KEY, pbc_param_info);
    }
    //gmsk info
    int GroupSigDB::store_gmsk(const std::string& group_name,
            const std::string& gmsk_pass,
            const std::string& gmsk_info) 
    {
      //TODO: encode gmsk_info with gmsk_pass
      return db_interface->db_put(group_name, GM_KEY, gmsk_info);
    }
       
        
    int GroupSigDB::store_gamma(const std::string& group_name,
            const std::string& gmsk_pass,
            const std::string& gamma_info)
    {
        //TODO: encode gamma_info with gmsk_pass
       return db_interface->db_put(group_name, GAMMA_KEY, gamma_info);
    }
       
    //gsk info
    int GroupSigDB::store_gsk(const std::string& group_name,
            const std::string& member_id,
            const std::string& gsk_pass,
            const std::string& gsk_info)
    {
        //TODO:gsk_pass verify 
        return db_interface->db_put(group_name, member_id, gsk_info);
    }
       
    //revoke info
    int GroupSigDB::store_revoked_list(const std::string& group_name,
            const std::string& revoked_member,
            const std::string& revoked_info,
            const std::string& gone_info)
    {
        std::string all_revoked_info;
        int success = load_revoked_list(all_revoked_info, group_name);
        if(!success)
        {
            LOG(ERROR)<<"get revoked_info from database failed";
            return false;
        }
        if(""!= all_revoked_info)
            all_revoked_info += VALUE_SPLIT_SYMBOL + revoked_info;
        else
            all_revoked_info = revoked_info;
        LOG(DEBUG)<<"UPDATED ALL_REVOKE_DEBUG:"<<all_revoked_info;
        success = db_interface->db_put(group_name, REVOKE_DEBUG_KEY, 
                       all_revoked_info);
        if(!success)
        {
            LOG(ERROR)<<"store revoked info "<<all_revoked_info<<" failed";
            return false;
        }
        std::string all_gone_info;
        success = db_interface->db_get(all_gone_info, group_name, GONE_DEBUG_KEY); 
        if(!success)
        {
            LOG(ERROR)<<"load gone info from database failed";
            return false;
        }
        if("" == all_gone_info)
            all_gone_info = gone_info;
        else
            all_gone_info += VALUE_SPLIT_SYMBOL + gone_info;
        LOG(DEBUG)<<"UPDATED ALL_GONE_DEBUG:"<<all_gone_info;
        return db_interface->db_put(group_name, GONE_DEBUG_KEY,
                all_gone_info);
    }

    /////load information
    int GroupSigDB::load_gpk(std::string& gpk_info, 
            const std::string& group_name)
    {
        return db_interface->db_get(gpk_info, group_name, GPK_KEY);
    }

       
    int GroupSigDB::load_gmsk(std::string& gmsk_info,
            const std::string& group_name, 
            const std::string& gmsk_pass)
    {
        //TODO: identify authentication 
        return db_interface->db_get(gmsk_info, group_name, GM_KEY);
    }
    
    int GroupSigDB::load_gamma(std::string& gamma_info,
            const std::string& group_name,
            const std::string& gmsk_pass)
    {
        return db_interface->db_get(gamma_info, group_name, GAMMA_KEY);
    }

    int GroupSigDB::load_gsk(std::string& gsk,
            const std::string& group_name,
            const std::string& member_name,
            const std::string& member_pass)
    {
        return db_interface->db_get(gsk, group_name, member_name);
    }
    

    inline int GroupSigDB::load_info_by_index(std::string & result,
            const std::string& db_name, const std::string& db_key,
            const std::string &index_str)
    {
        int success = db_interface->db_get(result, db_name, db_key);
        if(!success)
        {
            LOG(ERROR)<<"get revoked info from database failed";
            return false;
        }
        if("" == index_str)
        {
            LOG(DEBUG)<<"get all info by default: "<<result;
            return true;
        }
        std::stringstream stream(index_str);
        unsigned int index;
        stream >> index;
        LOG(DEBUG)<<"get group info after "<<index;
        dev::eth::get_substring(result, VALUE_SPLIT_SYMBOL, index);
        LOG(DEBUG)<<"load_info_by_index, index: "<<index<<",reuslt:"<<result;
        return success;
    }


    int GroupSigDB::load_revoked_list(std::string &revoked_info,
            const std::string& group_name,
            const std::string& sig_index_str)
    {
        LOG(DEBUG)<<"GET REVOKE DEBUG, INDEX:"<<sig_index_str;
        return load_info_by_index(revoked_info, group_name, 
                REVOKE_DEBUG_KEY, sig_index_str);
    }
    
     
    int GroupSigDB::load_gone_list(std::string &gone_list,
                const std::string& group_name,
                const std::string& sig_index)
    {
        LOG(DEBUG)<<"GET GONE DEBUG, INDEX:"<<sig_index;
        return load_info_by_index(gone_list, group_name,
                GONE_DEBUG_KEY, sig_index);
    }

    int GroupSigDB::load_pbc_param(std::string& pbc_param,
            const std::string& group_name)
    {
        LOG(DEBUG)<<"load pbc param";
        return db_interface->db_get(pbc_param, group_name, PARAM_KEY);
    }

}
