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

/* @file: RingSig.h 
 * @author: fisco-dev
 * @date: 2018.03.10
 * @function: concrete implementation of db operation
 *            related to ring_sig
 */
#pragma once

#include <memory>
#include <string>

#include "devcore/ConfigParser.h"
#include "devcore/easylog.h"

#include "database/DBFactory.h"

namespace DB
{
//interface of ring sig db operation
template<typename T>
class RingSigDBInterface
{
    public:
        RingSigDBInterface(std::shared_ptr<T>& _db_handler):
            db_handler(_db_handler)
        {}

        template<typename... Args>
        int store_ring_param(Args&&... args)
        { return db_handler->store_ring_param(std::forward<Args>(args)...); }

        template<typename... Args>
        int store_public_key(Args&&... args)
        { return db_handler->store_public_key(std::forward<Args>(args)...);}

        template<typename... Args>
        int load_public_key(Args&&... args)
        { return db_handler->load_public_key(std::forward<Args>(args)...); }

        template<typename... Args>
        int load_ring_param(Args&&... args)
        { return db_handler->load_ring_param(std::forward<Args>(args)...); }

        template<typename... Args>
        int store_private_key(Args&&... args)
        { return db_handler->store_private_key(std::forward<Args>(args)...);}
        
        template<typename... Args>
        int load_private_key(Args&&... args)
        { return db_handler->load_private_key(std::forward<Args>(args)...);}
        
        template<typename... Args>
        int store_ring_pos(Args&&... args)
        { return db_handler->store_ring_pos(std::forward<Args>(args)...); }

        template<typename... Args>
        int load_ring_pos(Args&&... args)
        { return db_handler->load_ring_pos(std::forward<Args>(args)...); }

    private:
        std::shared_ptr<T> db_handler;
};

class RingSigDB
{
    public:
    RingSigDB(std::shared_ptr<dev::eth::JsonConfigParser>& p_config)
    { db_interface = create_db(p_config);}

    int store_ring_param(const std::string& ring_name,
            const std::string& param_info);

    int store_public_key(const std::string& ring_name,
                        const std::string& public_key,
                        const std::string& index);

    int load_public_key(std::string& public_key_str,
                        const std::string& ring_name,
                        const std::string& index);

    int load_ring_param(std::string& param_info,
                        const std::string& ring_name);
    
    int store_private_key(const std::string& ring_name,
            const std::string& private_key_info,
            const std::string& index);

    int load_private_key(std::string& private_key_info,
            const std::string& ring_name,
            const std::string& index);

    int store_ring_pos(const std::string& ring_name,
            const std::string& pos);

    int load_ring_pos(std::string& pos,
            const std::string& ring_name);
    
    virtual ~RingSigDB(){ LOG(DEBUG)<<"FREE RING SIG DB";}
    private:
    DBInterface* db_interface;
    const static std::string RING_SIG_DB_PREFIX;
};

}

