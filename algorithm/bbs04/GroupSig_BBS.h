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

/* @file: GroupSig_BBS.h
 * @author:fisco-bcos
 * @date:2018.02.08
 */

#pragma once
#include "pbc/pbc.h"
#include "pbc/pbc_sig.h"

#include "devcore/easylog.h"
#include "devcore/StatusCode.h"

#include "algorithm/LinearPair.h"
#include "algorithm/GroupSigInterface.h"

using namespace std;
using namespace RetCode::GroupSigStatusCode;
using namespace RetCode::CommonStatusCode;

namespace dev
{
namespace eth
{
 class BBSGroupSig: public GroupSigInterface
 {
     public:
         BBSGroupSig(string config_path="",string gpk_dir=""):
             GroupSigInterface(config_path, gpk_dir)
             {}

         //#########BBS群签名算法接口####
         //创建群
         int create_group(string& result,const std::string& param);
         
         //使用群签名算法签名消息
         int group_sig(string& result,const std::string& param);
         
         //群签名验证
         int group_verify(int &valid, const std::string& param);
         
         //打开群签名
         int group_open(string& result, const std::string& param);
         
         //为成员颁发私钥
         int gen_gsk(string& result,const std::string& param);
         
         //撤销群成员
         int gm_revoke(string& result,const std::string& param);
         
         //群成员撤销后，更新私钥
         int update_gsk(string& result, const std::string& param);
     private:
         //参数解析
         int parse_param(vector<string>& param_vec,
                 std::string& group_name,
                 const std::string& param, int param_num);

         //将src拷贝到result
         void copy_string_to_result(string& result,
                 const string& src, bool need_split=true);
         //###*****群签名最常见的算法*****#######################
         //###生成群: 群公钥，群私钥, 群线pbc_param参数
       
         int create_group_key(std::string& result, 
                std::string& pbc_param_str,
                const std::string& group_name);

         static int bbs_gen_key(bbs_group_public_key_ptr gpk,
                 bbs_manager_private_key_ptr gmsk, 
                 element_ptr gamma, 
                 bbs_sys_param_ptr param,
                 int n=0,
                 bbs_group_private_key_t *gsk=NULL); 

        static void load_gpk_precompute(bbs_group_public_key_ptr& gpk,
            pairing_ptr pairing);
        //###群签名: 使用群成员私钥对message签名
        int group_sig(std::string &sig, std::string& gpk_info,
                std::string &gsk_info,
                std::string& pbc_param_str, std::string& message, 
                std::string group_name="");
        //###群签名验证: 验证群签名sig是否有效
        int group_verify(int &valid, std::string &sig,
                std::string& message, std::string &gpk_info,
                std::string& pbc_param_str,
                std::string group_name="");

        //###打开群签名
        int group_open(std::string& cert, std::string& sig, 
                std::string& message, std::string& gpk_info,
                std::string& gmsk_info, std::string& pbc_param_str,
                std::string group_name="");

        //###为新加入群成员产生私钥
        int gen_private_key(std::string& gsk_info, std::string& pbc_param_str,
                std::string& gmsk_info, std::string& gpk_info,
                std::string& gama_info, 
                std::string group_name="");
        //产生私钥
        void gen_gsk(bbs_group_private_key_ptr p_gsk,
                pairing_ptr p_pairing, element_ptr p_gama,
                bbs_group_public_key_ptr p_gpk,
                bbs_manager_private_key_ptr p_gmsk);
        //撤销群成员
        
        int gm_revoke(string& gpk_info, 
                std::string& pbc_param_str,
                std::string& revoked_info,
                std::string& gamma,
                std::string group_name="");

    
        void update_gpk(bbs_group_public_key_ptr p_gpk,
                bbs_group_private_key_ptr p_gmsk, element_ptr gamma, 
                pairing_ptr p_pairing);

        //群成员撤销后，更新群成员私钥
        int update_gsk(string& gsk_info, 
                std::string& pbc_param_str,
                std::string& revoke_list,
                std::string& gone_list,
                std::string& gtwo_list,
                std::string group_name="");
       
        int update_gsk(bbs_group_private_key_ptr p_gsk,
                bbs_sys_param_ptr sys_param,
                pairing_ptr p_pairing,
                const vector<string>& revoke_vec,
                const vector<string>& gone_vec,
                const string& gtwo_vec);
        //########################################

        //初始化系统参数 线性对和系统参数
        int init_sig_param(bbs_sys_param_ptr sys_param, 
              pairing_ptr pairing, const string& group_name,
              string& pbc_param_str);

        inline void update_linear(const string& pbc_param_setting)
        {
            LOG(DEBUG)<<"UPDATE LINEAR, pbc_param_setting:"<<pbc_param_setting;
            if( pbc_param_setting != "" )
            {
                LOG(DEBUG)<<"INNER PBC_PARAM_SETTING";

                //############start of lock
                {
                    GuardLock g_lock(m_lock);
                    pconfig_interface=make_shared<ConfigParserInterface<JsonConfigParser>>();
                    //加载json类型配置对象
                    pconfig_interface->create(pbc_param_setting, JsonString);
                    p_linear = LinearPairFactory::create(pconfig_interface);
                }
                //###########end of lock
            }
        }
        
        //用线性对初始化系统参数
        inline int init_sys_param(bbs_sys_param_ptr sys_param, 
                pairing_ptr pairing)
        {
            LOG(DEBUG)<<"INIT SYS PARAM";
            try{
                sys_param->pairing = pairing;
                sys_param->signature_length = 3 * pairing->G1->fixed_length_in_bytes + 
                    6 * pairing->Zr->fixed_length_in_bytes;
            }
            catch(exception& err)
            {
                LOG(ERROR)<<"INIT SYS PARAM FAILED"
                          <<"ERROR msg:"<<err.what();
                return INTERNAL_PARAM_INIT_FAILED;
            }
            return SUCCESS;
        }

    public:
        //静态函数

        static int bbs_init_gpk(bbs_group_public_key_ptr gpk,
            pairing_ptr pairing);

        static int bbs_init_gmsk(bbs_manager_private_key_ptr gmsk,
            pairing_ptr pairing);

        static int bbs_init_gsk(bbs_group_private_key_ptr gsk,
            pairing_t pairing);

        static int bbs_free_gmsk(bbs_manager_private_key_ptr gmsk);
        static int bbs_free_gpk(bbs_group_public_key_ptr gpk);
        static int bbs_free_gsk(bbs_group_private_key_ptr gsk);


    public:
        const static std::string RL_DELIM; //撤销列表分割符，默认是#
        const static std::string RL_GONE_DELIM; //g1之间的分隔符,默认是#
        const static std::string PARAM_DELIM;   //参数之间分隔符(默认是*)
        const static std::string RET_DELIM;   //参数之间分隔符(默认是*)
 };
}
}
