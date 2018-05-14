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


/* @file: Common.h
 * @author: fisco-dev
 * @date: 2018.03.13
 */
#pragma once
#include <string>
#include "json/json.h"

#include "devcore/easylog.h"
#include "devcore/StatusCode.h"

using namespace RetCode::GroupSigStatusCode;
using namespace RetCode::DBAccessStatus;
namespace Common
{
    static inline void get_ret_info(Json::Value& ret_json,
            const int& ret_code,
            const std::string& detail_info)
    {
        ret_json["ret_code"] = ret_code;
        ret_json["details"] = detail_info; 
    }

    template<typename T>
    static inline void get_ret_info(Json::Value& ret_json,
            const T& t_obj,
            const int& ret_code = RetCode::CommonStatusCode::SUCCESS,
            const std::string& detail_info="success")
    {
        ret_json["ret_code"] = ret_code;
        ret_json["details"] = detail_info;
        ret_json["result"] = t_obj;
    }

    static inline bool DB_access_ret(Json::Value& ret_json, 
           const int& ret_code, const int& expected_ret_code,
           const std::string& prefix_str)
    {
        if(ret_code == expected_ret_code)
            return true;
        std::string msg;
        switch(ret_code)
        {
            case DB_ACCESS_FAILED:
                msg = "access database failed";
                Common::get_ret_info(ret_json, DB_ACCESS_FAILED, msg);
                break;
            case KEY_NOT_EXIST:
                msg = prefix_str+" doesn't exist";
                Common::get_ret_info(ret_json, KEY_NOT_EXIST, msg);
                break;
            case KEY_EXIST:
                msg = prefix_str+" already exists";
                LOG(DEBUG)<<"###KEY_EXIST";
                Common::get_ret_info(ret_json, KEY_EXIST, msg);
                break;
            case DB_CORRUPTION:
                msg = "database corruption";
                Common::get_ret_info(ret_json, DB_CORRUPTION, msg);
                break;
            case IO_ERROR:
                msg = "database io error";
                Common::get_ret_info(ret_json, IO_ERROR, msg);
                break;
        }
        return false;
    }
}
