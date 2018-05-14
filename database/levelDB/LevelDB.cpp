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

/* @file: LevelDB.cpp
 * @author: fisco-dev
 * @date: 2018.03.10
 * @function: interface of level db
 */ 

#include "devcore/easylog.h"
#include "devcore/CommonFunc.h"

#include "database/levelDB/LevelDB.h"

using namespace RetCode::DBAccessStatus;
using namespace RetCode::GroupSigStatusCode;
namespace DB
{

    const std::string LevelDB::DEFAULT_DBPATH="db_data";
    //std::shared_ptr<leveldb::DB> LevelDB::db_handler;
    //init level DB
    LevelDB::LevelDB(const std::string& _db_path): 
            db_path(_db_path)
    {
        if("" == db_path)
            db_path = DEFAULT_DBPATH;
        bool succ = dev::eth::create_directory(db_path);
        if( !succ )
            LOG(ERROR)<<"create direcotry "<<db_path
                      <<" to store leveldb data failed";
        leveldb::Options ldb_option;
        ldb_option.create_if_missing = true;
        LOG(DEBUG)<<"open leveldb handler";

        leveldb::DB* pdb;
        leveldb::Status status = leveldb::DB::Open(ldb_option, 
                db_path, &(pdb));
        db_handler = std::shared_ptr<leveldb::DB>(pdb);

        if(!status.ok())
        {
            LOG(ERROR)<<"open leveldb handler failed,"
                      <<" error_msg:"<<status.ToString();
        }
    }
   
    //put value to leveldb
    int LevelDB::db_put(std::string const& db_name, 
                         std::string const& db_key, 
                         std::string const& db_value)
    {
        std::string key = db_name + "_" + db_key;
        leveldb::Status status = db_handler->Put(leveldb::WriteOptions(),
                key, db_value);
        if(status.ok())
        {
            //LOG(DEBUG)<<"get value for "<<key<<" succ, "
                      //<<"value = "<<db_value;
            return KEY_EXIST;
        }
        if(status.IsNotFound())
            return KEY_NOT_EXIST;
        if(status.IsCorruption())
            return DB_CORRUPTION;
        if(status.IsIOError())
            return IO_ERROR;
        return DB_ACCESS_FAILED; 
    }
    
    /*@function: get value for "${db_name}_${db_key}"
     *@param: 1.db_name: logical concept of database name
     *        2.db_key: key segment to obtain key   
     */ 
    int LevelDB::db_get(std::string& db_value, 
                        std::string const& db_name, 
                        std::string const& db_key)
    {
        std::string key = db_name + "_" + db_key;
        //store information with sha3 to ensure security
        try{
        leveldb::Status status = db_handler->Get(
                leveldb::ReadOptions(), key, &db_value);
        if(status.ok())
        {
            //LOG(DEBUG)<<"get value for "<<key<<" succ, "
            //          <<"value = "<<db_value;
            return KEY_EXIST;
        }
        if(status.IsNotFound())
            return KEY_NOT_EXIST;
        if(status.IsCorruption())
            return DB_CORRUPTION;
        if(status.IsIOError())
            return IO_ERROR;
        }
        catch(exception& e)
        {
            //LOG(DEBUG)<<"access levelDB failed:"<<e.what();
            return DB_ACCESS_FAILED; 
        }
        return DB_ACCESS_FAILED;
    }

}
