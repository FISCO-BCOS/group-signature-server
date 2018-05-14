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

/* @file: DBInterface.h 
 * @author: fisco-dev
 * @date: 2018.03.10
 * @function: interface of level db
 */
#pragma once

#include <string>

namespace DB
{
namespace Status
{
    const static int SUCCESS=0;
    const static int KEY_NOT_EXIST=1;
    const static int DB_CORRUPTION=2;
    const static int IO_ERROR=3;
}

class DBInterface
{
   public:
       DBInterface()
       {}

       virtual int db_put(const std::string& db_name,
                    const std::string& db_key,
                    const std::string& db_value) = 0;
       
       virtual int db_get(std::string& value,
                    const std::string& db_name, 
                    const std::string& db_key) = 0;

       virtual ~DBInterface(){};
};
}
