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

/* @file: GroupSigInterface.h
 * @author: fisco-dev
 * @date: 2018.02.11
 */ 
#pragma once
#include "devcore/CommonStruct.h"
#include "devcore/ConfigParser.h"
#include "devcore/CommonFunc.h"
#include "devcore/easylog.h"

#include "algorithm/LinearPair.h"

namespace dev
{
namespace eth
{
//签名算法枚举
/*enum GroupSigMethod
{
    BBS04=0x00,
    BS04_VLR=0x01,
    ACHM05=0x02
};*/
   
class GroupSigInterface
{
    public:
     string gpk_info_dir;
     std::string linear_config_path; //线性对配置文件路径
     shared_ptr<LinearPair> p_linear;
     Locker m_lock; //lock p_linear access
     shared_ptr<ConfigParserInterface<JsonConfigParser>> pconfig_interface;
    public:
 
     GroupSigInterface(const std::string& config_path,
         std::string dir="data"): gpk_info_dir(dir),
         linear_config_path(config_path), m_lock(Locker())
        { 
            if("" == dir )
                gpk_info_dir = "data";
                
            if (!create_directory(gpk_info_dir))
                LOG(ERROR)<<"CREATE DIRECTORY "<<gpk_info_dir<<" FAILED";
               
            //创建线性表配置对象
            if(linear_config_path != "")
            {
                pconfig_interface=make_shared<ConfigParserInterface<JsonConfigParser>>();
                //加载json类型配置对象
                pconfig_interface->create(linear_config_path);
            }
            //创建线性对接口
            p_linear = LinearPairFactory::create(pconfig_interface);
        }

          
     std::string get_gpk_dir()
     {
         return gpk_info_dir;
     }
     
     //生成群: (群公钥，群主私钥)
     virtual int create_group(string& result,const std::string& param)=0;
    
     //使用群签名算法签名消息
     virtual int group_sig(string& result, const std::string& param)=0;
     
     //群签名验证
     virtual int group_verify(int& valid, const std::string& param)=0;

     //打开群签名
     virtual int group_open(string& result, const std::string& param)=0;
     
     //为成员颁发私钥
     virtual int gen_gsk(string& result, const std::string& param)=0;
     
     //撤销群成员
     virtual int gm_revoke(string& result,const std::string& param)=0;
     
     //群成员撤销后，更新私钥
     virtual int update_gsk(string& result,const std::string& param) =0;

     void unkown_method(GroupSigMethod group_method)
     {
         LOG(ERROR)<<"GROUP_METHOD = "<<group_method<<" UNKOWN GROUP_METHOD";
     }
   
};
}
}
