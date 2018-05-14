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

/* @file: KeyLoaderDumper.CPP
 * @author: fisco-dev
 * @date: 2018.02.12
 * @function:定义了群公钥 && 群主私钥加载&&存储方法
 */
#include "algorithm/KeyLoaderDumper.h"

namespace dev
{
namespace eth
{
//根据gpk_str 恢复gpk系统参数
int BBSKey::load_gpk(bbs_group_public_key_ptr p_gpk, 
             const string& gpk_str, bbs_sys_param_ptr p_sys_param)
 {
   try{
        //恢复gpk
        p_gpk->param = p_sys_param;
        LOG(DEBUG)<<"INIT GPK FIRST";
        BBSGroupSig::bbs_init_gpk( p_gpk, p_sys_param->pairing);
        JsonConfigParser pub_loader(gpk_str, JsonString);
        LOG(DEBUG)<<"GPK DEBUG, g1="<<(pub_loader.get_value<string>("g1").c_str())
                 <<" g2="<<(pub_loader.get_value<string>("g2").c_str());
        trans_string_to_element(p_gpk->g1,pub_loader.get_value<string>("g1").c_str());
        trans_string_to_element(p_gpk->g2, pub_loader.get_value<string>("g2").c_str());
        trans_string_to_element(p_gpk->h, pub_loader.get_value<string>("h").c_str());
        trans_string_to_element(p_gpk->u, pub_loader.get_value<string>("u").c_str());
        trans_string_to_element(p_gpk->v, pub_loader.get_value<string>("v").c_str());
        trans_string_to_element(p_gpk->w, pub_loader.get_value<string>("w").c_str());
        //加载预先计算的值
        trans_string_to_element(p_gpk->pr_g1_g2, pub_loader.get_value<string>("pr_g1_g2").c_str());
        trans_string_to_element(p_gpk->pr_h_g2, pub_loader.get_value<string>("pr_h_g2").c_str());
        trans_string_to_element(p_gpk->pr_h_w, pub_loader.get_value<string>("pr_h_w").c_str());
        trans_string_to_element(p_gpk->pr_g1_g2_inv, pub_loader.get_value<string>("pr_g1_g2_inv").c_str());
   }
   catch(exception& err)
   {
       LOG(ERROR)<<"load_gpk failed, error_msg:"<<err.what();
       return LOAD_KEY_FAILED;
   }
   return SUCCESS;
 }

//将gpk信息存储到gpk_info(json格式)
int BBSKey::store_gpk(std::string& gpk_info,
          bbs_group_public_key_ptr p_gpk)
{

    try{
        std::map<string, string> pubs;
        //trans gpk info to json string 
        pubs["g1"] = trans_to_string(p_gpk->g1);
        LOG(DEBUG)<<"#G1:"<<pubs["g1"];
        pubs["g2"] = trans_to_string(p_gpk->g2);
        LOG(DEBUG)<<"#G2:"<<pubs["g2"];
        pubs["h"] = trans_to_string(p_gpk->h);
        LOG(DEBUG)<<"#H:"<<pubs["h"];
        pubs["u"] = trans_to_string(p_gpk->u);
        LOG(DEBUG)<<"#U:"<<pubs["u"];
        pubs["v"] = trans_to_string(p_gpk->v);
        LOG(DEBUG)<<"#V:"<<pubs["v"];
                
        LOG(DEBUG)<<"BEGIN CAL W";
        pubs["w"] = trans_to_string(p_gpk->w);
        //LOG(DEBUG)<<"#W:"<<pubs["w"];
        LOG(DEBUG)<<"BEGIN CAL PR_G1_G2";

        pubs["pr_g1_g2"] = trans_to_string(p_gpk->pr_g1_g2);
        LOG(DEBUG)<<"#PR_G1_G2:"<<pubs["pr_g1_g2"];

        pubs["pr_h_g2"] = trans_to_string(p_gpk->pr_h_g2);
        LOG(DEBUG)<<"#PR_H_G2:"<<pubs["pr_h_g2"];

        pubs["pr_h_w"] = trans_to_string(p_gpk->pr_h_w);
        LOG(DEBUG)<<"#PR_H_W:"<<pubs["pr_h_g2"];
        pubs["pr_g1_g2_inv"] = trans_to_string(p_gpk->pr_g1_g2_inv);
        LOG(DEBUG)<<"#PR_G1_G2_INV:"<<pubs["pr_g1_g2_inv"];
        LOG(DEBUG)<<"g1:"<<pubs["g1"]<<" g2:"<<pubs["g2"];
                
        LOG(DEBUG)<<"CONVERT TO JSON";
        JsonConfigParser::convert_to_json_str(gpk_info, pubs);
        LOG(DEBUG)<<"STORE GPK, GPK INFO:"<<gpk_info;
    }
    catch(exception& err)
    {
        LOG(ERROR)<<"STORE GPK failed, error msg:"<<err.what();
        return STORE_KEY_FAILED;
    }
    return SUCCESS;
}


//保存私钥信息: v1:私钥不加密
//TODO: 私钥加密存储
int BBSKey::store_gmsk(std::string& gmsk_info, 
            bbs_manager_private_key_ptr p_gmsk)
{
    try{
        std::map<string, string> gmsk;
        gmsk["xi1"] = trans_to_string(p_gmsk->xi1);
        LOG(DEBUG)<<"***XI1:"<<gmsk["xi1"];
        gmsk["xi2"] = trans_to_string(p_gmsk->xi2);
        LOG(DEBUG)<<"***XI2:"<<gmsk["xi2"];
        LOG(DEBUG)<<"PRIVATE KEY FOR GROUP MANAGER: ("<<(gmsk["xi1"])
                 <<","<<(gmsk["xi2"])<<")";
        JsonConfigParser::convert_to_json_str(gmsk_info, gmsk);
        LOG(DEBUG)<<"STORE GMSK RESULT:"<<gmsk_info;
    }
    catch(exception& err)
    {
        LOG(ERROR)<<"STORE GMSK FAILED, error msg:"<<err.what();
        return STORE_KEY_FAILED;
    }
    return SUCCESS;

}

  
//加载私钥信息
//TODO: 私钥加密存储 && 解密load
int BBSKey::load_gmsk(bbs_manager_private_key_ptr p_gmsk,
        const std::string& gmsk_info, bbs_sys_param_ptr p_sys_param)
{
    try{
        BBSGroupSig::bbs_init_gmsk( p_gmsk, p_sys_param->pairing);
        p_gmsk->param = p_sys_param;
        JsonConfigParser gmsk_loader(gmsk_info, JsonString);
        LOG(DEBUG)<<"LOADED GMSK IS ("<<(gmsk_loader.get_value<string>("xi1").c_str())
            <<","<<(gmsk_loader.get_value<string>("xi2"))<<")";
        trans_string_to_element(p_gmsk->xi1, gmsk_loader.get_value<string>("xi1").c_str());
        trans_string_to_element(p_gmsk->xi2, gmsk_loader.get_value<string>("xi2").c_str());
    }
    catch(exception& err)
    {
        LOG(ERROR)<<"LOAD GMSK FAILED, error msg:"<<err.what();
        return LOAD_KEY_FAILED;
    }
    return SUCCESS;
}


//###群成员私钥信息管理
//TODO: 私钥加密管理
int BBSKey::store_gsk(std::string& gsk_info, 
        bbs_group_private_key_ptr gsk)
{
    try{
        std::map<string, string> gsk_map;
        gsk_map["A"] = trans_to_string(gsk->A); //证书
        gsk_map["x"] = trans_to_string(gsk->x); //私钥信息
        gsk_map["pr_A_g2"] = trans_to_string(gsk->pr_A_g2); //预先计算的值
        LOG(DEBUG)<<"GSK INFO: ("<<(gsk_map["A"])<<","
                <<(gsk_map["x"])<<")";
        //gsk信息写入gsk_info
        JsonConfigParser::convert_to_json_str(gsk_info,gsk_map);
    }
    catch(exception& err)
    {
       LOG(ERROR)<<"STORE GSK FAILED, error msg:"<<err.what();
       return LOAD_KEY_FAILED;
    }
    return SUCCESS;
}


//加载群成员私钥
//TODO: 加密后私钥解密
int BBSKey::load_gsk(bbs_group_private_key_ptr gsk,
        const std::string& gsk_info, bbs_sys_param_ptr p_sys_param)
{
    try{
        gsk->param = p_sys_param;
        BBSGroupSig::bbs_init_gsk( gsk, p_sys_param->pairing);
        JsonConfigParser gsk_loader(gsk_info, JsonString);
        LOG(DEBUG)<<"LOAD GSK: ("<<(gsk_loader.get_value<string>("A").c_str())
            <<","<<(gsk_loader.get_value<string>("x").c_str());
        trans_string_to_element(gsk->A, gsk_loader.get_value<string>("A").c_str());
        trans_string_to_element(gsk->x, gsk_loader.get_value<string>("x").c_str());
        trans_string_to_element(gsk->pr_A_g2, gsk_loader.get_value<string>("pr_A_g2").c_str());
    }
    catch (exception& err){
        LOG(ERROR)<<"LOAD GSK FAILED, error msg:"<<err.what();
        return LOAD_KEY_FAILED;
    }
    return SUCCESS;
}
            

int BBSKey::store_sig(string& sig, BBS04Signature& sig_struct)
{   
    try{
        //T1, T2, T3,c, ralpha, rbeta, rx, rdelta1, rdelta2
        std::map<string, string> sig_map;
        sig_map["T1"] = trans_to_string(sig_struct.T1);
        sig_map["T2"] = trans_to_string(sig_struct.T2);
        sig_map["T3"] = trans_to_string(sig_struct.T3);
        sig_map["c"] = trans_to_string(sig_struct.c);
        sig_map["ralpha"] = trans_to_string(sig_struct.ralpha);
        sig_map["rbeta"] = trans_to_string(sig_struct.rbeta);
        sig_map["rx"] = trans_to_string(sig_struct.rx);
        sig_map["rdelta1"] = trans_to_string(sig_struct.rdelta1);
        sig_map["rdelta2"] = trans_to_string(sig_struct.rdelta2);
        JsonConfigParser::convert_to_json_str(sig,sig_map);
        LOG(DEBUG)<<"SIG RESULT:"<<sig;
    }
    catch(exception& err)
    {
        LOG(ERROR)<<"STORE SIG FAILED,error msg:"<<err.what();
        return STORE_KEY_FAILED;
    }
    return SUCCESS;
}


int BBSKey::load_sig(BBS04Signature& sig_struct, const string& sig)

{
    try{
        JsonConfigParser sig_loader(sig, JsonString);
        LOG(DEBUG)<<"LOAD T1";
        trans_string_to_element(sig_struct.T1, sig_loader.get_value<string>("T1").c_str());
        LOG(DEBUG)<<"LOAD T2";
        trans_string_to_element(sig_struct.T2, sig_loader.get_value<string>("T2").c_str());
        LOG(DEBUG)<<"LOAD T3";
        trans_string_to_element(sig_struct.T3, sig_loader.get_value<string>("T3").c_str());
    
        LOG(DEBUG)<<"LOAD c";
        trans_string_to_element(sig_struct.c, sig_loader.get_value<string>("c").c_str());
        LOG(DEBUG)<<"LOAD ralpha";
        trans_string_to_element(sig_struct.ralpha, sig_loader.get_value<string>("ralpha").c_str());

        LOG(DEBUG)<<"LOAD rbeta";
        trans_string_to_element(sig_struct.rbeta, sig_loader.get_value<string>("rbeta").c_str());
        LOG(DEBUG)<<"LOAD rx";
        trans_string_to_element(sig_struct.rx, sig_loader.get_value<string>("rx").c_str());
        LOG(DEBUG)<<"LOAD rdelta1";
        trans_string_to_element(sig_struct.rdelta1, sig_loader.get_value<string>("rdelta1").c_str());
        LOG(DEBUG)<<"LOAD rdelta2";
        trans_string_to_element(sig_struct.rdelta2, sig_loader.get_value<string>("rdelta2").c_str());
    }
    catch(exception& err)
    {
        LOG(ERROR)<<"LOAD SIG FAILED, error msg:"<<err.what();
        return LOAD_KEY_FAILED;
    }
    return SUCCESS;
}

}
}
