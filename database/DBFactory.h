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

/* @file: DBFactory.h 
 * @author: fisco-dev
 * @date: 2018.03.10
 * @function: interface of level db
 */ 

#pragma once
#include <memory>

#include "devcore/easylog.h"
#include "devcore/CommonFunc.h"
#include "devcore/ConfigParser.h"
#include "devcore/CommonStruct.h"

#include "DBInterface.h"
#include "levelDB/LevelDB.h"

using namespace std;
using namespace dev::eth;
namespace DB
{
    static DBInterface* create_db(
         const shared_ptr<dev::eth::JsonConfigParser>& p_config)
    {
        std::string db_type_str = "";
        std::string db_path = "";
        if(p_config)
        {
            db_type_str = p_config->get_value<std::string>("db_type"); 
            db_path = p_config->get_value<std::string>("db_path");
        }
        
        LOG(DEBUG)<<"create db, db_type:"<<db_type_str;
        dev::eth::DBType db_type = dev::eth::get_dbtype_by_string(db_type_str);
        DBInterface* p_db = NULL;
        switch(db_type)
        {
            case dev::eth::LEVELDB:
                LOG(DEBUG)<<"create levelDB";
                p_db = (singleton<LevelDB>::instance(db_path)).get();
                break;
            case dev::eth::MYSQL:
                LOG(WARNING)<<"MYSQL MODULE HAS NOT BEEN IMPLEMENTED YET "
                            <<"USE LEVEL DB";
                p_db = (singleton<LevelDB>::instance(db_path)).get();
                break;
            case dev::eth::REDIS:
                LOG(WARNING)<<"REDIS MODULE HAS NOT BEEN IMPLEMENTED YET "
                            <<"USE LEVELDB AS DEFAULT";
                p_db = (singleton<LevelDB>::instance(db_path)).get();
                break;
            default:
                LOG(ERROR)<<"UNKOWN DB TYPE:"<<db_type_str
                          <<"USE LEVELDB AS DEFAULT";
                p_db = (singleton<LevelDB>::instance(db_path)).get();
                break;
        }
        return p_db;
    }
}
