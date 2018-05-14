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

/* @file: CommonFunc.h
 * @author: fisco-dev
 * @date: 2018.02.11
 */ 
#pragma once
#include <string>
#include <unistd.h>
#include <exception>
#include <vector>
#include <iomanip>
#include <time.h>
#include "boost/filesystem.hpp"
#include "pbc/pbc.h"

#include "devcore/easylog.h"
#define MAX_PATH_LEN 255

using namespace std;
namespace dev
{
namespace eth
{

enum LinearPairType
{
    A_LINEAR=0,
    A_ONE_LINEAR,
    D_LINEAR,
    E_LINEAR,
    F_LINEAR,
    G_LINEAR
};

static inline std::string _get_split_symbol()
{ return "*"; }

template<typename T>
static inline std::string num_to_string(const T& big_num)
{
    std::stringstream ss;
    ss<<big_num;
    return ss.str();
}

static inline int string_cmp_ignore_case(const std::string& lhs,
        const std::string& rhs)
{
 #if defined(_WIN32)
    return _stricmp(lhs.c_str(), rhs.c_str());
 #else
    return strcasecmp(lhs.c_str(), rhs.c_str());
 #endif
}

static inline void trans_to_linear_type(LinearPairType& type,
        const std::string& type_str)
{
    if(!string_cmp_ignore_case("a", type_str))
        type = A_LINEAR;
    else if(!string_cmp_ignore_case("a_one", type_str))
        type = A_ONE_LINEAR;
    else if(!string_cmp_ignore_case("d", type_str))
        type = D_LINEAR;
    else if(!string_cmp_ignore_case("e", type_str))
        type = E_LINEAR;
    else if(!string_cmp_ignore_case("f", type_str))
        type = F_LINEAR;
    else if(!string_cmp_ignore_case("g", type_str))
        type = G_LINEAR;
    else
    {
        LOG(WARNING)<<"type has not been set, used A_LINEAR as DEFAULT";
        type = A_LINEAR;
    }
}

//签名算法枚举
enum GroupSigMethod
{
    BBS04=0x00,
    BS04_VLR=0x01,
    ACHM05=0x02,
    METHOD_UNKOWN
};

static inline GroupSigMethod get_algo_method_by_string(const std::string& method_str)
{
    LOG(DEBUG)<<"get_algo_method_by_string, method_str="<<method_str;
    if(!string_cmp_ignore_case("bbs04", method_str))
        return BBS04;
    if(!string_cmp_ignore_case("bs04_vlr", method_str))
        return BS04_VLR;
    if(!string_cmp_ignore_case("achm05", method_str))
        return ACHM05;
    else
    {
        LOG(DEBUG)<<"UNKOWN METHOD:"<<method_str;
        return METHOD_UNKOWN;
    }
}

enum DBType
{
    LEVELDB=0x00,
    MYSQL=0x01,
    REDIS=0x02,
    DB_TYPE_UNKOWN
};

static inline DBType get_dbtype_by_string(const std::string& db_type_str)
{
    LOG(DEBUG)<<"get_dbtype_by_string, db_type_str = "<<db_type_str;
    if(!string_cmp_ignore_case("leveldb", db_type_str))
        return LEVELDB;
    if(!string_cmp_ignore_case("mysql", db_type_str))
        return MYSQL;
    if(!string_cmp_ignore_case("redis", db_type_str))
        return REDIS;
    LOG(ERROR)<<"UNKOWN DB TYPE:"<<db_type_str;
    return DB_TYPE_UNKOWN;
        
}

/* @functions: 将pbc参数序列化成字符串,写入到文件中
 * @params: 1. gpk_file: 存储参数信息的文件名; 
 *          2. pbc_param: 指向pbc参数对象的指针
 * @ret: pbc param字符串
 */
static inline string dump_pbc_param(const string& group_name, 
        string& dir, pbc_param_ptr pbc_param)
{
    assert(pbc_param);
    std::string param_file_path = dir + "/" + group_name + ".param";
    if("" == group_name)
    {
        srand((int)time(0));
        int random_num = rand()%1000000;
        param_file_path= dir + "/tmp" + to_string(random_num) + ".param";
    }

    LOG(DEBUG) << "param file path for "<<group_name<<" is "<<param_file_path; 
    FILE* fp_sys_param = fopen(param_file_path.c_str(), "w");
    pbc_param_out_str(fp_sys_param, pbc_param);
    fclose(fp_sys_param);
    
    //读取并返回系统参数信息
    ifstream pbc_param_stream(param_file_path);
    stringstream buffer;
    buffer << pbc_param_stream.rdbuf();
    //remove param_file_path
    int ret = remove(param_file_path.c_str());
    if(ret != 0)
        LOG(ERROR)<<"remove tmp file:"<<param_file_path<<" failed";
    return string(buffer.str());
}

static inline string trans_bytes_to_string(unsigned char* bytes_buf, int len)
{
    assert(bytes_buf);
    std::ostringstream buf;
    for(int i=0; i<len; i++)
    {
        buf << std::hex << std::setfill('0') << std::setw(2) <<(int)(*bytes_buf);
        bytes_buf++;
    }
    //LOG(DEBUG)<<"HEX STRING OF BYTES_BUF:"<<buf.str();
    return buf.str();
}



//将element_t转化成字符串
static inline string trans_to_string(element_ptr big_num, int len=-1)
{
    try
    {
        assert(big_num);
        std::ostringstream buf;
        if( -1 == len)
            len = element_length_in_bytes(big_num);
        unsigned char c_buf[len];
        element_to_bytes(c_buf, big_num);
        element_to_bytes(c_buf, big_num);
        string result = trans_bytes_to_string(&c_buf[0], len);
        return result;
    }
    
    catch(exception& err)
    {
        LOG(ERROR)<<"trans big_num to string failed, error msg:"
            << err.what();
        return err.what();
    }
   
}
 
static inline int from_hex_char(char _i)
{
    if (_i >= '0' && _i <= '9')
        return _i - '0';
    if (_i >= 'a' && _i <= 'f')
        return _i - 'a' + 10;
    if (_i >= 'A' && _i <= 'F')
        return _i - 'A' + 10;
    return -1;
}


static inline std::vector<uint8_t> from_hex(std::string const& hex_str)

{

    std::vector<uint8_t> ret;
	
    ret.reserve((hex_str.length() + 1) / 2);
	
    if (hex_str.length() % 2)
    
    {
	
        int h = from_hex_char(hex_str[hex_str.length()-1]);
    	
        if (h != -1)
            ret.push_back(h);
        else
            return ret;
    }
    for (unsigned i = 0; i < hex_str.length(); i += 2)
    {
        int h = from_hex_char(hex_str[i]);
        int l = from_hex_char(hex_str[i+1]);
        if (h != -1 && l != -1)
            ret.push_back(h * 16 + l);
        else
            return ret;
    }
    return ret;
}

static inline void trans_string_to_element(element_ptr p_big_num,
        const string& e_str)
{
    try
    {
        assert(p_big_num);
        LOG(DEBUG)<<"trans "<<e_str<<" to bignum";
        std::vector<uint8_t> element_buf = from_hex(e_str);
        if( !element_buf.empty())
            element_from_bytes(p_big_num, &element_buf[0]);
    }
    catch(exception& err)
    {
        LOG(ERROR)<<"trans e_str "<<e_str
            <<" to big num failed, error_msg:"<<err.what();
    }
}

static inline bool create_directory(string& path)
{
    assert(path.length() > 0);
    //相对路径
    if( '/' != path[0] )
    {
        char abs_path[MAX_PATH_LEN];
        getcwd(abs_path, MAX_PATH_LEN);
        string cur_dir(abs_path);
        path = cur_dir + "/" + path;
    }
    
    //创建文件夹
    try{
        boost::filesystem::create_directories(path);
        LOG(DEBUG)<<"CREATE DIR="<<path<<" SUCCEED";
        return true;
    }
    catch(exception& err_msg)
    {
        LOG(ERROR)<<"CREATE DIR="<<path<<" FAILED"
            <<"error msg:"<<err_msg.what();
    }
    return false;
}

static inline vector<string> split(const std::string& str, const string& delim)
{
    char *outer = NULL;
    char *result = strtok_r((char*)str.c_str(), delim.c_str(), &outer);
    vector<string> result_vec;
    while(result)
    {
        result_vec.push_back(std::string(result));
        result = strtok_r(NULL, delim.c_str(), &outer);
    }
    //LOG(DEBUG)<<"result string:"<<result_vec;
    return result_vec;
}

static inline void get_substring(std::string& result, 
                           const std::string& delim,
                           unsigned int index)
{
    LOG(DEBUG)<<"get result substring for "<<result;
    vector<string> delimed_vec = split(result, delim);
    result = "";
    if(index>=delimed_vec.size())
        return;

    for(unsigned int i=index; i< delimed_vec.size(); i++)
    {
        result += delimed_vec[i] + delim; 
    }
    LOG(DEBUG)<<"get_substring, index:"<<index
             <<" result:"<<result;

}


static inline double pbc_get_time(void) {
  static struct timeval last_tv, tv;
  static int first = 1;
  static double res = 0;

  if (first) {
    gettimeofday(&last_tv, NULL);
    first = 0;
    return 0;
  } else {
    gettimeofday(&tv, NULL);
    res += tv.tv_sec - last_tv.tv_sec;
    res += (tv.tv_usec - last_tv.tv_usec) / 1000000.0;
    last_tv = tv;

    return res;
  }
}

}
}
