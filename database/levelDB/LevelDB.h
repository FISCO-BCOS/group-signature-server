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

/* @file: LevelDB.h 
 * @author: fisco-dev
 * @date: 2018.03.10
 * @function: interface of level db
 */
#pragma once
#include <memory>
#include <string>
#include "leveldb/db.h"

#include "devcore/easylog.h"
#include "devcore/StatusCode.h"

#include "database/DBInterface.h"

namespace DB
{

class LevelDB: public DBInterface
{
  public:
      LevelDB(const std::string& _db_path);
      
      virtual int db_put(std::string const& db_name, 
              std::string const& db_key, std::string const& db_value);

      virtual int db_get(std::string& db_value, 
              std::string const& db_name, std::string const& db_key);
       

      virtual ~LevelDB()
      {
          LOG(DEBUG)<<"FREE LEVELDB"; 
      }

  private:
      std::shared_ptr<leveldb::DB> db_handler;
      std::string db_path;
      const static std::string DEFAULT_DBPATH;
};

}
