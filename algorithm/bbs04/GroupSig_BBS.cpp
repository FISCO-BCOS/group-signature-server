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

/* @file: GroupSig_BBS.cpp
 * @author:fisco-bcos
 * @date:2018.02.08
 */
#include "time.h"

#include "devcore/CommonFunc.h"

#include "algorithm/KeyLoaderDumper.h"
#include "algorithm/bbs04/GroupSig_BBS_Impl.h"
#include "algorithm/bbs04/GroupSig_BBS.h"

namespace dev
{
namespace eth
{
    const string BBSGroupSig::RL_DELIM="#";
    const string BBSGroupSig::RL_GONE_DELIM="#";
    const string BBSGroupSig::PARAM_DELIM="*";
    const string BBSGroupSig::RET_DELIM="*"; 
    
    void BBSGroupSig::copy_string_to_result(string& result,
            const string& src, bool need_split)
    {
        result += src;
        if(need_split)
            result += RET_DELIM;
        LOG(DEBUG)<<"copy_string_to_result, result:"<<result;
    }


    //#######BBS04群签名算法接口###
    //###创建群
    int BBSGroupSig::create_group(string& result,const std::string& param)
    {
        //参数解析，各个参数之间用*号码隔开
        vector<string> param_vec;
        string group_name;
        string pbc_param_setting = "";
        if( "" != param )
        {
           int is_valid = parse_param(param_vec, group_name, param, 2);
           if(is_valid != SUCCESS)
               return is_valid;
           pbc_param_setting = param_vec[0];
        }
        else
        {
            int is_valid = parse_param(param_vec, group_name, param, 1);
            if(is_valid != SUCCESS)
                return is_valid;
        }

        update_linear(pbc_param_setting);
        std::string pbc_param_str = "";
        //TODO:当前接口需要从string拷贝到vector_ref，
        //     效率低于直接操作vector_ref
        int ret_code = create_group_key(result, pbc_param_str, group_name);
        if( ret_code != SUCCESS)
            return ret_code;
        LOG(DEBUG)<<"FINAL RESULT AFTER CREATE GROUP:"<<result;
        return SUCCESS;
    }
    
    int BBSGroupSig::parse_param(vector<string> &param_vec,
            std::string& group_name,
            const std::string& param, int param_num)
    {
        param_vec = split(param, PARAM_DELIM);
        if( (int)param_vec.size()!= param_num && 
             (int)param_vec.size()!= param_num-1)
        {
            return INTERNAL_PARAM_PARSE_FAILED;
        }

        group_name = "";
        if((int)param_vec.size() == param_num)
            group_name = param_vec[param_num-1];
        return SUCCESS;
    }

    //###使用群签名算法签名消息
    int BBSGroupSig::group_sig(string& result, const std::string& param)
    {
        vector<string> param_vec;
        string group_name;
        int is_valid = parse_param(param_vec, group_name, param, 5);
        if(is_valid != SUCCESS)
            return is_valid;
        string sig;
        //param_vec[0]: gpk_info
        //param_vec[1]: gsk_info
        //param_vec[2]: pbc_param_info
        //param_vec[3]: message
        int ret_code = group_sig(result, param_vec[0], param_vec[1],
                param_vec[2], param_vec[3], group_name);
        if( ret_code != SUCCESS )
            return ret_code;
        //将sig拷贝到result
        LOG(DEBUG)<<"GROUP SIG RESULT:"<<result;
        return SUCCESS;
    }

    //###群签名验证
    int BBSGroupSig::group_verify(int& valid, const std::string& param)
    {
        vector<string> param_vec;
        string group_name;
        LOG(DEBUG)<<"GROUP VERFY, PARAM:"<<param;
        int param_valid = parse_param(param_vec, group_name, param, 5);
        if( param_valid != SUCCESS)
            return param_valid;
        //param_vec[0]: sig
        //param_vec[1]: message
        //param_vec[2]: gpk_info
        //param_vec[3]: pbc_param_str
        int ret_code = group_verify(valid, param_vec[0], 
                param_vec[1], param_vec[2], param_vec[3], 
                group_name);
        if(ret_code != SUCCESS)
            return ret_code;
        LOG(DEBUG)<<"GROUP VERIFY RESULT:"<<valid;
        return SUCCESS;
    }


    //###打开群签名
    int BBSGroupSig::group_open(string& result, const std::string& param)
    {
        vector<string> param_vec;
        string group_name;
        //LOG(DEBUG)<<"GROUP OPEN, PARAM:"<<param;
        int param_valid = parse_param(param_vec, group_name, param, 6);
        if(param_valid != SUCCESS )
            return param_valid;
        string cert;
        //param_vec[0]: sig
        //param_vec[1]: message
        //param_vec[2]: gpk_info
        //param_vec[3]: gmsk_info
        //param_vec[4]: pbc_param_str
        int ret_code = group_open(result, param_vec[0], param_vec[1], 
                param_vec[2], param_vec[3], 
                param_vec[4], group_name);
        if( ret_code != SUCCESS)
            return ret_code;
        LOG(DEBUG)<<"GROUP OPEN RESULT:"<<result;
        return SUCCESS;
    }

    //###为成员颁发私钥
    int BBSGroupSig::gen_gsk(string& gsk_info,const std::string& param)
    {
        vector<string> param_vec;
        string group_name;
        LOG(DEBUG)<<"GEN GSK, PARAM:"<<param;
        int param_valid = parse_param(param_vec, group_name, param, 5);
        if(param_valid != SUCCESS)
            return param_valid;
        LOG(DEBUG)<<"pbc_param_info:"<< param_vec[0];
        LOG(DEBUG)<<"gmsk_info:"<< param_vec[1];
        LOG(DEBUG)<<"gpk_info:"<< param_vec[2];
        LOG(DEBUG)<<"gamma_info:"<< param_vec[3];
        LOG(DEBUG)<<"#######";
        //param_vec[0]: pbc_param_str
        //param_vec[1]: gmk_info
        //param_vec[2]: gpk_info
        //param_vec[3]: gama_info
        int ret_code = gen_private_key(gsk_info, param_vec[0],
                param_vec[1], param_vec[2], 
                param_vec[3], group_name);
        if( ret_code != SUCCESS)
            return ret_code;
        /*LOG(DEBUG)<<"GEN GSK, gsk_info:"<<gsk_info;
        LOG(DEBUG)<<"GEN GSK RESULT LEN:"<<result.count();
        LOG(DEBUG)<<"GEK GSK, GSK DEBUG LEN:"<<gsk_info.length();*/
        LOG(DEBUG)<<"GEN GSK, RESULT:"<<gsk_info;
        return SUCCESS;
    }


    //###撤销群成员
    int BBSGroupSig::gm_revoke(string& updated_gpk,const std::string& param)
    {
        vector<string> param_vec;
        string group_name;
        //LOG(DEBUG)<<"GM REVOKE, PARAM:"<<param;
        int param_valid = parse_param(param_vec, group_name, param, 4);
        if(param_valid != SUCCESS)
            return param_valid;
        string gpk_info;
        //param_vec[0]: pbc_param_str
        //param_vec[1]: revoke_info 
        //param_vec[2]: gamma
        int ret_code = gm_revoke(updated_gpk, param_vec[0], 
                param_vec[1], param_vec[2], group_name);
        if( ret_code != SUCCESS)
            return ret_code;
        LOG(DEBUG)<<"GM_REVOKE RESULT:"<<updated_gpk;
        return SUCCESS;
    }

    //###群成员撤销后，更新私钥
    int BBSGroupSig::update_gsk(string& updated_gsk,const std::string& param)
    {
        vector<string> param_vec;
        string group_name;
        LOG(DEBUG)<<"UPDATE GSK AFTER GROUP MEMBER REVOKE, param:"<<param;
        int param_valid = parse_param(param_vec, group_name, param, 5);
        if(param_valid != SUCCESS)
            return param_valid;
        //param_vec[0]: pbc_param_str
        //param_vec[1]: revoke_list: RL_DELIM隔开的撤销者信息列表
        //param_vec[2]: gone_list: RL_GONE_DELIM隔开的g1列表
        //param_vec[3]: gtwo: g2
        int ret = update_gsk(updated_gsk, param_vec[0],
                param_vec[1],param_vec[2],param_vec[3],
                group_name);
        if( ret != SUCCESS )
            return ret;
        LOG(DEBUG)<<"UPDATED GSK RESULT:"<<updated_gsk;    
        return SUCCESS;
    }


    //#############################

    /* @function: 初始化群签名相关的数据结构，包括双线性对&&系统参数
     * @params: 1. sys_param:
     *          2. pairing:
     *          3. type:
     *          4. group_name:
     */
    int BBSGroupSig::init_sig_param(bbs_sys_param_ptr sys_param, 
            pairing_ptr pairing, const string& group_name, string& pbc_param_str)
    {
        LOG(DEBUG)<<"begin init_sig_param";
        if( !sys_param || !pairing )
            return INTERNAL_PARAM_INIT_FAILED;
        LOG(DEBUG)<<"sys_param has been inited";
        //1. 初始化PBC参数 && 通过PBC参数初始化pbc线性对
        //(1) pbc_param_str不为空: 从字符串加载pbc param
        //###########begin of m_lock
        {
            GuardLock g_lock(m_lock);
            pbc_param_ptr p_param = NULL;
            if(pbc_param_str != "")
            {
                try{
                    int ret_code = SUCCESS;
                    LOG(DEBUG)<<"load pbc param from "<<pbc_param_str;
                    p_param = p_linear->load_pbc_param(pbc_param_str, ret_code);
                    if(ret_code != SUCCESS)
                    {
                        LOG(ERROR)<<"LOAD PBC PARAM FROM "<<pbc_param_str<<" FAILED";
                        return ret_code;
                    }
                    p_linear->set_inited();
                }
                catch(exception& err)
                {
                    LOG(ERROR)<<"LOAD PBC PARAM FROM "<<pbc_param_str<<" FAILED"
                              <<"error msg:"<<err.what();
                    return INTERNAL_PARAM_INIT_FAILED; 
                }
            }
            else
            {
                //(2) pbc_parma_str为空: 初始化pbc param
                try
                {
                    p_param = p_linear->init_pbc_param();
                    p_linear->set_inited();
                }
                catch(exception& err)
                {
                    LOG(ERROR)<<"INIT PBC PARAM WHEN GEN GROUP "<<group_name<<"FAILED"
                              <<"error msg:"<<err.what();
                    return INTERNAL_PARAM_INIT_FAILED;
                }
                LOG(DEBUG)<<"DUMP PBC PARAM TO FILE FOR RECOVERY";
                //##保存pbc_param,用于签名和verify时初始化线性对
                std::string dir = get_gpk_dir();
                pbc_param_str = dump_pbc_param(group_name, dir, p_param); 
                LOG(DEBUG)<<"PBC PARAM TO STRING IS:"<<pbc_param_str;
            }
            if(!p_param || !p_param->data)
            {
                return INTERNAL_PARAM_INIT_FAILED;
            }
            //用pbc param 初始化线性对pairing
            try{
                pairing_init_pbc_param(pairing, p_param);
                pbc_param_clear(p_param);
            }
            catch(exception& err)
            {
                LOG(ERROR)<<"INIT PBC PAIRING FAILED"
                          <<"error_msg:"<<err.what();
                return INTERNAL_PARAM_INIT_FAILED;
            }
    }
    ///##############end of m_lock
        //2. 初始化系统参数
        return init_sys_param(sys_param, pairing); 
    }

    int BBSGroupSig::bbs_free_gpk(bbs_group_public_key_ptr gpk)
    {
        if( !gpk)
            return INTERNAL_GPK_FREE_FAILED;
        
        element_clear(gpk->g1);
        element_clear(gpk->g2);
        element_clear(gpk->h);
        element_clear(gpk->u);
        element_clear(gpk->v);
        element_clear(gpk->w);

        element_clear(gpk->pr_g1_g2);
        element_clear(gpk->pr_g1_g2_inv);
        element_clear(gpk->pr_h_g2);
        element_clear(gpk->pr_h_w);
        return SUCCESS;
    }

    int BBSGroupSig::bbs_init_gpk(bbs_group_public_key_ptr gpk,
            pairing_ptr pairing)
    {
        //assert(gpk && pairing);
        if( !gpk || !pairing)
            return INTERNAL_GPK_INIT_FAILED;

        element_init_G1(gpk->g1, pairing);
        element_init_G2(gpk->g2, pairing);
        element_init_G1(gpk->h, pairing);
        element_init_G1(gpk->u, pairing);
        element_init_G1(gpk->v, pairing);
        element_init_G2(gpk->w, pairing);

        element_init_GT(gpk->pr_g1_g2, pairing);
        element_init_GT(gpk->pr_g1_g2_inv, pairing);
        element_init_GT(gpk->pr_h_g2, pairing);
        element_init_GT(gpk->pr_h_w, pairing);
        return SUCCESS;
    }

    int BBSGroupSig::bbs_free_gmsk(bbs_manager_private_key_ptr gmsk)
    {
        if(!gmsk)
            return INTERNAL_GMSK_FREE_FAILED;
        element_clear(gmsk->xi1);
        element_clear(gmsk->xi2);
        return SUCCESS;
    }
    int BBSGroupSig::bbs_init_gmsk(bbs_manager_private_key_ptr gmsk,
            pairing_ptr pairing)
    {
        //assert(gmsk && pairing);
        if(!gmsk || !pairing)
            return INTERNAL_GMSK_INIT_FAILED;
        element_init_Zr(gmsk->xi1, pairing);
        element_init_Zr(gmsk->xi2, pairing);
        return SUCCESS;
    }

    int BBSGroupSig::bbs_free_gsk(bbs_group_private_key_ptr gsk)
    {
        if(!gsk)
            return INTERNAL_GSK_FREE_FAILED;
        element_clear(gsk->A);
        element_clear(gsk->x);
        element_clear(gsk->pr_A_g2);
        return SUCCESS;
    }


    int BBSGroupSig::bbs_init_gsk(bbs_group_private_key_ptr gsk,
            pairing_t pairing)
    {
        //assert(gsk && pairing);
        if(!gsk || !pairing)
            return INTERNAL_GSK_INIT_FAILED;
        element_init_G1(gsk->A, pairing);
        element_init_Zr(gsk->x, pairing);
        element_init_GT(gsk->pr_A_g2, pairing);
        return SUCCESS;
    }
    /*@function: 采用BBS04算法产生群公钥/群主私钥，并产生n个群成员私钥
     *@param: gpk: 群公钥;  gmsk:群主私钥; gamma:秘密信息(当前版本群主管理);
     *        n: 群成员数目;   gsk: 群成员私钥;
     *        param: 系统参数
     *@说明: pbc-sig bbs_gen接口没有保留gamma信息，导致群生成后，无法
     *       为加入的群成员颁发私钥，也无法撤销群成员,因此重写了接口，
     *       将gamma传给群主管理(后面会设计gamma的多方管理，提升系统安全性)
     */  
    int BBSGroupSig::bbs_gen_key(bbs_group_public_key_ptr gpk, 
         bbs_manager_private_key_ptr gmsk, element_ptr gamma,
         bbs_sys_param_ptr param, int n, bbs_group_private_key_t *gsk)
    {
        pairing_ptr pairing = param->pairing;
        element_t z0;
        
        gpk->param = param;
        gmsk->param = param;

        int gpk_init_succ = bbs_init_gpk(gpk, pairing);
        if(gpk_init_succ != SUCCESS)
            return gpk_init_succ;
        int gmsk_init_succ = bbs_init_gmsk(gmsk, pairing);
        if(gmsk_init_succ != SUCCESS)
            return gmsk_init_succ;
        //初始化公钥gpk && 群主私钥
        
        element_random(gmsk->xi1);
        element_random(gmsk->xi2);

        element_random(gpk->g1);
        element_random(gpk->g2);
        
        element_random(gpk->h);
        element_init_Zr(z0, pairing);

        element_invert(z0, gmsk->xi1);
        element_pow_zn(gpk->u, gpk->h, z0); //u=(h)^(1/xi1)

        element_invert(z0, gmsk->xi2);
        element_pow_zn(gpk->v, gpk->h, z0);

        //gamma
        element_random(gamma);
        element_pow_zn(gpk->w, gpk->g2, gamma);
        //群成员私钥gsk
        for(int i=0; i<n; i++)
        {
            gsk[i]->param = param;
            bbs_init_gsk(gsk[i], pairing);
            
            element_random(gsk[i]->x);

            element_add(z0, gamma, gsk[i]->x);
            element_invert(z0, z0);
            element_pow_zn(gsk[i]->A, gpk->g1, z0);

            //预先计算的值
            pairing_apply(gsk[i]->pr_A_g2, gsk[i]->A, gpk->g2, pairing);
        }
        //预先计算的公钥相关值
        load_gpk_precompute(gpk, pairing);
        element_clear(z0);
        return SUCCESS;
    }
   
    void BBSGroupSig::load_gpk_precompute(bbs_group_public_key_ptr& gpk,
            pairing_ptr pairing)
    {
        LOG(DEBUG)<<"LOAD PRECOMPUTE VALUE";
        pairing_apply(gpk->pr_g1_g2, gpk->g1, gpk->g2, pairing);
        element_invert(gpk->pr_g1_g2_inv, gpk->pr_g1_g2);
        pairing_apply(gpk->pr_h_g2, gpk->h, gpk->g2, pairing);
        pairing_apply(gpk->pr_h_w, gpk->h, gpk->w, pairing);
    }

    /*@function: 生成群签名
     *@params: 1. group_name:
     *         2. group_id:
     *         3. gmsk:
     */ 
    int BBSGroupSig::create_group_key(std::string& result,
                std::string& pbc_param_str,
                const std::string& group_name)
    {
        LOG(DEBUG)<<"CREATE GROUP KEY for "<<group_name;
        bbs_group_public_key_t gpk; //群公钥
        bbs_manager_private_key_t gmsk; //群主私钥
        element_t gamma;
        bbs_sys_param_t sys_param;
        pairing_t pairing;
        //初始化系统参数
        LOG(DEBUG)<<"INIT SIG PARAM";
        int init_succ = init_sig_param(sys_param, pairing, group_name, pbc_param_str);
        if( init_succ != SUCCESS)
            return init_succ;
        LOG(DEBUG)<<"CREATE_GROUP_KEY, signature_len:"<<sys_param->signature_length;
        //group_size > 0, gsk != NULL
        //or group_size == 0, gsk == NULL
        //产生gpk, gmsk
        LOG(DEBUG)<<"CALL BACK bbs_gen_key";
        element_init_Zr(gamma, pairing);
        int group_size = 0;
        bbs_gen_key(gpk, gmsk, gamma, sys_param);
        //将gpk && gmsk序列化成字符串
        LOG(DEBUG)<<"STORE GPK";
        int key_store_succ = KeyManager<BBSKey>::store_gpk(result, gpk);
        if(key_store_succ!= SUCCESS)
            return key_store_succ;
        result += RET_DELIM;
        string tmp;
        LOG(DEBUG)<<"STORE GMSK";
        key_store_succ = KeyManager<BBSKey>::store_gmsk(tmp, gmsk);
        if(key_store_succ!= SUCCESS)
            return key_store_succ;
        copy_string_to_result(result, tmp);
        LOG(DEBUG)<<"STORE GAMMA";
        tmp = trans_to_string(gamma);
        copy_string_to_result(result, tmp);
        element_clear(gamma);
        LOG(DEBUG)<<"STORE pbc_param";
        copy_string_to_result(result, pbc_param_str, false);
        //free pbc_paring 

        int free_succ = bbs_free_gpk(gpk);
        if(free_succ != SUCCESS)
            return free_succ;
        free_succ = bbs_free_gmsk(gmsk);
        if(free_succ != SUCCESS)
            return free_succ;
        pairing_clear(pairing);
        return SUCCESS;
    }

    //###群签名: 使用群成员私钥对message签名
    int BBSGroupSig::group_sig(std::string &sig, std::string& gpk_info,
                std::string &gsk_info,
                std::string& pbc_param_str, std::string& message, 
                std::string group_name)
    {
        LOG(DEBUG)<<"Generate Group Sig...";
        //加载系统参数: sys_param
        bbs_sys_param_t sys_param;
        pairing_t pairing;
        int init_succ = init_sig_param(sys_param, pairing, 
                        group_name, pbc_param_str);
        if(init_succ != SUCCESS)
            return init_succ;
        //通过gsk_info恢复gsk
        LOG(DEBUG)<<"RECOVER GSK THROUGH GSK_DEBUG:"<<gsk_info;
        bbs_group_private_key_t p_gsk;
        int key_load_succ = KeyManager<BBSKey>::load_gsk(p_gsk, gsk_info, sys_param);
        if(key_load_succ != SUCCESS)
            return key_load_succ;
        //通过gpk_info恢复gpk
        LOG(DEBUG)<<"RECOVER GPK THROUGH GPK_DEBUG:"<<gpk_info;
        bbs_group_public_key_t p_gpk;
        key_load_succ = KeyManager<BBSKey>::load_gpk(p_gpk, gpk_info, sys_param);
        if(key_load_succ != SUCCESS)
            return key_load_succ;
        //签名
        LOG(DEBUG)<<"GEN SIGNATURE FOR MESSAGE: "<<message;
        BBS04SignatureImpl::bbs_group_sig(sig, message.length(), 
                message.c_str(), p_gpk, p_gsk);
       

        int free_succ = bbs_free_gpk(p_gpk);
        if(free_succ != SUCCESS)
            return free_succ;
        free_succ = bbs_free_gsk(p_gsk);
        if(free_succ != SUCCESS )
            return free_succ;
        pairing_clear(pairing);
        return SUCCESS;
    }
 
    //###群签名验证
    int BBSGroupSig::group_verify(int &valid, 
            std::string &sig, std::string& message,
            std::string &gpk_info, std::string& pbc_param_str, 
            std::string group_name)
    {
        LOG(DEBUG)<<"VERIFY GROUP SIG:"<<sig<<"...";
        //加载系统参数，根据gpk_info初始化gpk
        LOG(DEBUG)<<"RECOVER SYS PARAM THROUGH PARAM:"<<pbc_param_str;
        bbs_sys_param_t sys_param;
        pairing_t pairing;
        int init_succ = init_sig_param(sys_param, pairing, 
                group_name, pbc_param_str);
        if(init_succ != SUCCESS)
            return init_succ;
        LOG(DEBUG)<<"RECOVER GPK THROUG GPK_DEBUG:"<<gpk_info;
        bbs_group_public_key_t p_gpk;
        int key_load_succ = KeyManager<BBSKey>::load_gpk(p_gpk, gpk_info, sys_param);
        if(key_load_succ != SUCCESS)
            return key_load_succ;

        LOG(DEBUG)<<"GROUP VERFIY, message:"<<message;
        //验证签名
        valid = BBS04SignatureImpl::bbs_group_verify(sig, message.length(),
                message.c_str(), p_gpk);
        LOG(DEBUG)<<"VERIFY RESULT:"<<valid;


        int free_succ = bbs_free_gpk(p_gpk);
        if(free_succ != SUCCESS)
            return free_succ;

        pairing_clear(pairing);
        return SUCCESS;
    }
    
    //###打开群签名
    int BBSGroupSig::group_open(std::string& cert, std::string& sig, 
            std::string& message, std::string& gpk_info,
            std::string& gmsk_info, std::string& pbc_param_str,
            std::string group_name)
    {
        LOG(DEBUG)<<"OPEN GROUP...";
        //加载系统参数，根据gpk_info初始化gpk
        LOG(DEBUG)<<"RECOVER SYS PARAM THROUGH PARAM:"<<pbc_param_str;
        bbs_sys_param_t sys_param;
        pairing_t pairing;
        int init_succ = init_sig_param(sys_param, pairing, 
                group_name, pbc_param_str);
        if(init_succ != SUCCESS)
            return init_succ;
        LOG(DEBUG)<<"RECOVER GPK THROUGH GPK_DEBUG:"<<gpk_info;
        bbs_group_public_key_t p_gpk;
        int key_load_succ = KeyManager<BBSKey>::load_gpk(p_gpk, gpk_info, sys_param);
        if(key_load_succ != SUCCESS)
            return key_load_succ;
        LOG(DEBUG)<<"RECOVER GMSK THROUGH GMSK_DEBUG:"<<gmsk_info;
        bbs_manager_private_key_t p_gmsk;
        key_load_succ = KeyManager<BBSKey>::load_gmsk(p_gmsk, gmsk_info, sys_param);
        if(key_load_succ != SUCCESS)
            return key_load_succ;
        element_t A_cert;
        element_init_G1(A_cert, pairing);
        LOG(DEBUG)<<"CALL BACK bbs_open";
        int ret = BBS04SignatureImpl::bbs_group_open(A_cert, p_gpk, 
                  p_gmsk, message.length(), message.c_str(),
                  sig);

        if (ret)
        {
            cert = trans_to_string(A_cert);
            LOG(DEBUG)<<"opened cert:"<<cert;
        }
        else
        {
            string error_msg = "GET OPENED CERT FAILED";
            LOG(ERROR)<<error_msg;
            return OPEN_CERT_FAILED;
        }


        int free_succ = bbs_free_gpk(p_gpk);
        if(free_succ != SUCCESS)
            return free_succ;
        free_succ = bbs_free_gmsk(p_gmsk);
        if(free_succ != SUCCESS)
            return free_succ;
        element_clear(A_cert);
        pairing_clear(pairing);
        return SUCCESS;
    }

    //###为新加入群成员产生私钥
    int BBSGroupSig::gen_private_key(std::string& gsk_info,
            std::string& pbc_param_str, std::string& gmsk_info,
            std::string& gpk_info, std::string& gama_info,
            std::string group_name)
    {
       LOG(DEBUG)<<"Gen Private Key...";
       LOG(DEBUG)<<"INIT SYS PARAM THROUGH:"<<pbc_param_str;
       bbs_sys_param_t sys_param;
       pairing_t pairing;
       //初始化系统参数
       int init_succ = init_sig_param(sys_param, pairing, 
               group_name, pbc_param_str);
       if(init_succ != SUCCESS)
           return init_succ;
       //通过gpk_info获取gpk
       LOG(DEBUG)<<"LOAD GPK DEBUG THROUGH:"<<gpk_info;
       bbs_group_public_key_t p_gpk;
       int key_load_succ = KeyManager<BBSKey>::load_gpk(p_gpk, gpk_info, sys_param);
       if(key_load_succ != SUCCESS)
           return key_load_succ;
       //通过gmsk_info获取gmsk
       LOG(DEBUG)<<"LOAD GMSK DEBUG THROUGH:"<<gmsk_info;
       bbs_manager_private_key_t p_gmsk;
       key_load_succ = KeyManager<BBSKey>::load_gmsk(p_gmsk, gmsk_info, sys_param);
       if(key_load_succ != SUCCESS)
           return key_load_succ;
       //获取gama（群主保存）
       LOG(DEBUG)<<"INIT GAMA DEBUG:"<<gama_info;
       element_t p_gama;
       element_init_Zr(p_gama, pairing);
       trans_string_to_element(p_gama, gama_info.c_str());
       //产生群成员私钥
       bbs_group_private_key_t p_gsk;
       p_gsk->param = sys_param;
       gen_gsk(p_gsk, pairing, p_gama, p_gpk, p_gmsk);
       element_clear(p_gama);
       //将私钥p_gsk转换成json字符串
       LOG(DEBUG)<<"STORE GSK:";
       int ret = KeyManager<BBSKey>::store_gsk(gsk_info, p_gsk);
       if(ret != SUCCESS)
           return ret;
       ret = bbs_free_gpk(p_gpk);
       if( ret != SUCCESS)
           return ret;
       ret = bbs_free_gmsk(p_gmsk);
       if( ret != SUCCESS)
           return ret;
       ret = bbs_free_gsk(p_gsk);
       if(ret != SUCCESS)
           return ret;
        
       pairing_clear(pairing);
       return SUCCESS;
    }

    void BBSGroupSig::gen_gsk(bbs_group_private_key_ptr p_gsk,
            pairing_ptr p_pairing, element_ptr p_gama,
            bbs_group_public_key_ptr p_gpk,
            bbs_manager_private_key_ptr p_gmsk)
    {
        bbs_init_gsk(p_gsk, p_pairing);
        LOG(DEBUG)<<"init private key";
        element_random(p_gsk->x);
        //初始化z0
        element_t z0;
        element_init_Zr(z0, p_pairing);
        element_add(z0, p_gama, p_gsk->x);
        element_invert(z0, z0);
        //计算证书
        element_pow_zn(p_gsk->A, p_gpk->g1, z0);
        pairing_apply(p_gsk->pr_A_g2, p_gsk->A, p_gpk->g2, p_pairing);
        element_clear(z0);
    }

    //####撤销群签名,群主撤销
    int BBSGroupSig::gm_revoke(string& gpk_info, 
            std::string& pbc_param_str,
            std::string& revoked_info,
            std::string& gama_info,
            std::string group_name)
    {
       LOG(DEBUG)<<"REVOKE GROUP SIGNATURE";
       //重新计算公钥
       LOG(DEBUG)<<"LOAD SYSTEM PARAM";
       bbs_sys_param_t sys_param;
       pairing_t pairing;
       //初始化系统参数
       LOG(DEBUG)<<"INIT SYSTEM PARAM";
       int init_succ = init_sig_param(sys_param, pairing, 
               group_name, pbc_param_str);
       if( init_succ != SUCCESS)
           return init_succ;
       //获取公钥
       bbs_group_public_key_t p_gpk;
       int key_load_succ = KeyManager<BBSKey>::load_gpk(p_gpk, gpk_info, sys_param);
       if(key_load_succ != SUCCESS)
           return key_load_succ;
       //将revoked_info转换成private_key结构
       LOG(DEBUG)<<"LOAD REVOKE DEBUG";
       bbs_group_private_key_t p_gsk;
       key_load_succ = KeyManager<BBSKey>::load_gsk(p_gsk, revoked_info, sys_param);
       if(key_load_succ != SUCCESS)
           return key_load_succ;
       LOG(DEBUG)<<"LOAD GAMA";
       element_t gama;
       element_init_Zr(gama, pairing);
       trans_string_to_element(gama, gama_info.c_str());
       //更新公钥
       LOG(DEBUG)<<"UPDATE PUB KEY";
       update_gpk(p_gpk, p_gsk, gama, pairing);
       //将公钥信息写入gpk_info存储区
       int ret = KeyManager<BBSKey>::store_gpk(gpk_info, p_gpk);
       if(ret != SUCCESS)
           return ret;
       ret = bbs_free_gpk(p_gpk);
       if( ret != SUCCESS)
           return ret;
       ret = bbs_free_gsk(p_gsk);
       if( ret != SUCCESS)
           return ret;
       element_clear(gama);
       pairing_clear(pairing);
       return SUCCESS;
    }
   

    void BBSGroupSig::update_gpk(bbs_group_public_key_ptr p_gpk,
            bbs_group_private_key_ptr p_gsk, element_ptr gamma, 
            pairing_ptr p_pairing)
    {
        //assert(p_gpk && p_gsk && gamma && p_pairing);
        //计算新的gpk, TODO: g1==gsk->A
        //更新g1
        element_t z0;
        element_init_Zr(z0, p_pairing);
        element_add(z0, p_gsk->x, gamma);
        element_invert(z0, z0);
        //update g1
        element_pow_zn(p_gpk->g1, p_gpk->g1, z0);
        //更新g2
        element_pow_zn(p_gpk->g2, p_gpk->g2, z0);
        //更新w
        element_pow_zn(p_gpk->w, p_gpk->g2, gamma);
        //更新pr_g1_g2, pr_h_g2, pr_h_w, pr_g2_g2_inv
        pairing_apply(p_gpk->pr_g1_g2, p_gpk->g1, p_gpk->g2, p_pairing);
        pairing_apply(p_gpk->pr_h_g2, p_gpk->h, p_gpk->g2, p_pairing);
        pairing_apply(p_gpk->pr_h_w, p_gpk->h, p_gpk->w, p_pairing);
        element_invert(p_gpk->pr_g1_g2_inv, p_gpk->pr_g1_g2);
        element_clear(z0);
    }


    int BBSGroupSig::update_gsk(string& gsk_info,
            std::string& pbc_param_str,
            std::string& revoke_list,
            std::string& gone_list,
            std::string& gtwo,
            std::string group_name)
    {
       LOG(DEBUG)<<"UPDATE GSK FOR GPK UPDATED";
       //初始化system_param
       LOG(DEBUG)<<"LOAD SYSTEM PARAM";
       bbs_sys_param_t sys_param;
       pairing_t pairing;
       int init_succ = init_sig_param(sys_param, pairing, 
               group_name, pbc_param_str);
       if(init_succ != SUCCESS)
           return init_succ;
       //获取RL列表(RL列表用#相隔)
       LOG(DEBUG)<<"LOAD REVOKE DEBUG FROM:"<<revoke_list;
       vector<string> revoke_vec = split(revoke_list, RL_DELIM);
       LOG(DEBUG)<<"LOAD G1 FROM:"<<gone_list;
       vector<string> gone_vec = split(gone_list, RL_GONE_DELIM);
       //更新私钥
       bbs_group_private_key_t p_gsk;
       int key_load_succ = KeyManager<BBSKey>::load_gsk(p_gsk, gsk_info, sys_param);
       if(key_load_succ != SUCCESS)
           return key_load_succ;
       int ret = update_gsk(p_gsk, sys_param, pairing, revoke_vec, gone_vec, gtwo);
       if( ret != SUCCESS)
           return ret;
       //存储私钥
       ret = KeyManager<BBSKey>::store_gsk(gsk_info, p_gsk);
       if( ret != SUCCESS)
           return ret;
       ret = bbs_free_gsk(p_gsk);
       if(ret != SUCCESS)
           return ret;
       pairing_clear(pairing);
       return SUCCESS;
       //LOG(DEBUG)<<"UPDATED GSK_DEBUG RESULT:"<<gsk_info;
    }


    int BBSGroupSig::update_gsk(bbs_group_private_key_ptr p_gsk,
            bbs_sys_param_ptr sys_param,
            pairing_ptr p_pairing,
            const vector<string>& revoke_vec, 
            const vector<string>& gone_vec,
            const string& gtwo)
     {
        //assert(p_gsk && sys_param && p_pairing);
        bbs_group_private_key_t tmp_gsk;
        int i = 0;
        element_t g_one;
        element_init_G1(g_one, p_pairing);
        
        element_t z0;
        element_init_Zr(z0, p_pairing);
        int gsk_succ;
       
        //循环计算新的gsk
        for(auto revoke_info: revoke_vec)
        {
            //撤销者信息
            gsk_succ = KeyManager<BBSKey>::load_gsk(tmp_gsk, revoke_info, sys_param);
            if(gsk_succ != SUCCESS)
                return gsk_succ;
            //assert(tmp_gsk);
            //g1^(1/(x-x1)) * 1/A^(1/(x-x1))
            element_sub(z0, p_gsk->x, tmp_gsk->x);
            element_invert(z0, z0); // 1/(x-x1)
            trans_string_to_element(g_one, gone_vec[i].c_str());
            element_pow_zn(g_one, g_one, z0);
            element_pow_zn(p_gsk->A, p_gsk->A, z0);
            element_invert(p_gsk->A, p_gsk->A); 
            element_mul(p_gsk->A, p_gsk->A, g_one);
            gsk_succ = bbs_free_gsk(tmp_gsk);
            if( gsk_succ != SUCCESS )
                return gsk_succ;
            i++;
        }
        element_t g_two;
        element_init_G2(g_two, p_pairing);
        trans_string_to_element(g_two, gtwo.c_str());
        pairing_apply(p_gsk->pr_A_g2, p_gsk->A, g_two , p_pairing);

        element_clear(z0);
        element_clear(g_one);
        element_clear(g_two);
        return SUCCESS;
    }
}
}
