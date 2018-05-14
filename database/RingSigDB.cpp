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

#include "database/RingSigDB.h"

namespace DB
{
const std::string RingSigDB::RING_SIG_DB_PREFIX="RING_SIG";
int RingSigDB::store_ring_param(
        const std::string& ring_name,
        const std::string& param_info)
{
    return db_interface->db_put(RING_SIG_DB_PREFIX, ring_name, param_info);
}


int RingSigDB::store_public_key(
        const std::string& ring_name,
        const std::string& public_key,
        const std::string& index)
{
    std::string db_key = ring_name + "_pk" + index;
    LOG(DEBUG)<<"db_key:"<<db_key;
    return db_interface->db_put(RING_SIG_DB_PREFIX, db_key, public_key);
}

int RingSigDB::load_public_key(
        std::string& public_key_str,
        const std::string& ring_name,
        const std::string& index)
{
    std::string db_key = ring_name + "_pk" + index;
    LOG(DEBUG)<<"db_key:"<<db_key;
    return db_interface->db_get(public_key_str, RING_SIG_DB_PREFIX, db_key);
}

int RingSigDB::load_ring_param(
        std::string& param_info,
        const std::string& ring_name)
{
    return db_interface->db_get(param_info, RING_SIG_DB_PREFIX, ring_name);
}

int RingSigDB::store_private_key(
        const std::string& ring_name,
        const std::string& private_key_info,
        const std::string& index)
{
    std::string db_key = ring_name + "_prk" + index;
    LOG(DEBUG)<<"db key of private key:"<<db_key;
    return db_interface->db_put(RING_SIG_DB_PREFIX, 
                            db_key, private_key_info);
}

int RingSigDB::load_private_key(
        std::string& private_key_info,
        const std::string& ring_name,
        const std::string& index)
{
    std::string db_key = ring_name + "_prk" + index;
    LOG(DEBUG)<<"db key of private key:"<<db_key;
    return db_interface->db_get(private_key_info, 
            RING_SIG_DB_PREFIX, db_key);
}

int RingSigDB::store_ring_pos(const std::string& ring_name,
        const std::string& pos)
{
    std::string pos_key = ring_name + "_pos";
    LOG(DEBUG)<<"store ring_pos "<<pos;
    return db_interface->db_put(RING_SIG_DB_PREFIX,pos_key, pos);
}

int RingSigDB::load_ring_pos(std::string& pos,
            const std::string& ring_name)
{
    std::string pos_key = ring_name + "_pos";
    LOG(DEBUG)<<"load ring_pos";
    return db_interface->db_get(pos, RING_SIG_DB_PREFIX, pos_key);
}

}

