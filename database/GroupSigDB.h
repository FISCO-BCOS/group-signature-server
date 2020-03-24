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

/* @file: GroupSigDB.h 
 * @author: fisco-dev
 * @date: 2018.03.10
 * @function: concrete implementation of db operation
 *            related to group_sig
 */
#pragma once

#include <string>
#include <memory>

#include "easylog/easylog.h"
#include <group_sig/devcore/ConfigParser.h>

#include "database/DBFactory.h"

namespace DB
{
//interface of group sig db operation
template <typename T>
class GroupSigDBInterface
{
public:
        GroupSigDBInterface(std::shared_ptr<T> &_db_handler) : db_handler(_db_handler)
        {
        }

        template <typename... Args>
        int store_gpk(Args &&... args)
        {
                return db_handler->store_gpk(std::forward<Args>(args)...);
        }

        template <typename... Args>
        int store_gmsk(Args &&... args)
        {
                return db_handler->store_gmsk(std::forward<Args>(args)...);
        }

        template <typename... Args>
        int store_gamma(Args &&... args)
        {
                return db_handler->store_gamma(std::forward<Args>(args)...);
        }

        template <typename... Args>
        int store_gsk(Args &&... args)
        {
                return db_handler->store_gsk(std::forward<Args>(args)...);
        }

        template <typename... Args>
        int store_revoked_list(Args &&... args)
        {
                return db_handler->store_revoked_list(std::forward<Args>(args)...);
        }

        template <typename... Args>
        int store_pbc_param(Args &&... args)
        {
                return db_handler->store_pbc_param(std::forward<Args>(args)...);
        }

        //////load interfaces
        template <typename... Args>
        int load_gpk(Args &&... args)
        {
                return db_handler->load_gpk(std::forward<Args>(args)...);
        }

        template <typename... Args>
        int load_gmsk(Args &&... args)
        {
                return db_handler->load_gmsk(std::forward<Args>(args)...);
        }

        template <typename... Args>
        int load_gamma(Args &&... args)
        {
                return db_handler->load_gamma(std::forward<Args>(args)...);
        }

        template <typename... Args>
        int load_gsk(Args &&... args)
        {
                return db_handler->load_gsk(std::forward<Args>(args)...);
        }

        template <typename... Args>
        int load_revoked_list(Args &&... args)
        {
                return db_handler->load_revoked_list(std::forward<Args>(args)...);
        }

        template <typename... Args>
        int load_gone_list(Args &&... args)
        {
                return db_handler->load_gone_list(std::forward<Args>(args)...);
        }

        template <typename... Args>
        int load_pbc_param(Args &&... args)
        {
                return db_handler->load_pbc_param(std::forward<Args>(args)...);
        }

private:
        std::shared_ptr<T> db_handler;
};

class GroupSigDB
{
public:
        GroupSigDB(std::shared_ptr<dev::eth::JsonConfigParser> &p_config)
        {
                db_interface = create_db(p_config);
        }
        //store group sig related params to database
        //gpk info
        int store_gpk(const std::string &group_name,
                      const std::string &gpk_info);

        int store_pbc_param(const std::string &group_name,
                            const std::string &pbc_param_info);
        //gmsk info
        int store_gmsk(const std::string &group_name,
                       const std::string &gmsk_pass,
                       const std::string &gmsk_info);

        int store_gamma(const std::string &group_name,
                        const std::string &gmsk_pass,
                        const std::string &gamma_info);
        //gsk info
        int store_gsk(const std::string &group_name,
                      const std::string &member_id,
                      const std::string &gsk_info,
                      const std::string &gsk_pass = "");
        //revoke info
        int store_revoked_list(const std::string &group_name,
                               const std::string &revoked_member,
                               const std::string &revoked_info,
                               const std::string &gone_info);

        /////load information
        int load_gpk(std::string &gpk_info,
                     const std::string &group_name);

        int load_gmsk(std::string &gmsk_info,
                      const std::string &group_name,
                      const std::string &gmsk_pass = "");

        int load_gamma(std::string &gamma_info,
                       const std::string &group_name,
                       const std::string &gmsk_pass = "");

        int load_gsk(std::string &gsk,
                     const std::string &group_name,
                     const std::string &member_name,
                     const std::string &member_pass = "");

        int load_revoked_list(std::string &revoked_info,
                              const std::string &group_name,
                              const std::string &sig_index = "");

        int load_gone_list(std::string &gone_list,
                           const std::string &group_name,
                           const std::string &sig_index = "");

        int load_pbc_param(std::string &pbc_param,
                           const std::string &group_name);
        virtual ~GroupSigDB() { LOG(DEBUG) << "FREE GROUP SIG DB"; }

private:
        inline int load_info_by_index(std::string &result,
                                      const std::string &db_name, const std::string &db_value,
                                      const std::string &index_str);

private:
        DBInterface *db_interface;

        const static std::string KEY_SPLIT_SYMBOL;
        const static std::string VALUE_SPLIT_SYMBOL;
        const static std::string GPK_KEY;
        const static std::string PARAM_KEY;
        const static std::string GM_KEY;
        const static std::string GAMMA_KEY;
        const static std::string REVOKE_DEBUG_KEY;
        const static std::string GONE_DEBUG_KEY;
};

} // namespace DB
