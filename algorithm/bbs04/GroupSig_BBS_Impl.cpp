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

/* @file: GroupSig_BBS_Impl.cpp
 * @author:fisco-bcos
 * @date:2018.02.08
 * @function:BBS04群签名算法实现
 */
#include "devcore/SHA3.h"
#include "devcore/CommonFunc.h"

#include "algorithm/KeyLoaderDumper.h"
#include "algorithm/bbs04/GroupSig_BBS.h"
#include "algorithm/bbs04/GroupSig_BBS_Impl.h"

using namespace std;

namespace dev
{
namespace eth
{
    void BBS04SignatureImpl::bbs_cal_sha3(element_t& c, BBS04Signature& sig_struct,
            element_ptr M, 
            unsigned int digest_len)
    {
        assert(c && M);
        string hash_input_str = trans_to_string(sig_struct.T1);
        hash_input_str += trans_to_string(sig_struct.T2);
        hash_input_str += trans_to_string(sig_struct.T3);
        hash_input_str += trans_to_string(sig_struct.R1);
        hash_input_str += trans_to_string(sig_struct.R2);
        hash_input_str += trans_to_string(sig_struct.R3);
        hash_input_str += trans_to_string(sig_struct.R4);

        hash_input_str += trans_to_string(sig_struct.R5);
        //LOG(DEBUG)<<"#####M:"<<trans_to_string(M);
        hash_input_str += trans_to_string(M);
        LOG(DEBUG)<<"###T1:"<<trans_to_string(sig_struct.T1);
        LOG(DEBUG)<<"###T2:"<<trans_to_string(sig_struct.T2);
        LOG(DEBUG)<<"###T3:"<<trans_to_string(sig_struct.T3);
        LOG(DEBUG)<<"###R1:"<<trans_to_string(sig_struct.R1);
        LOG(DEBUG)<<"###R2:"<<trans_to_string(sig_struct.R2);
        LOG(DEBUG)<<"###R3:"<<trans_to_string(sig_struct.R3);
        LOG(DEBUG)<<"###R4:"<<trans_to_string(sig_struct.R4);
        LOG(DEBUG)<<"###R5:"<<trans_to_string(sig_struct.R5);
        //LOG(DEBUG)<<"####HASH INPUT:"<<hash_input_str;
        //计算hash值
        unsigned char digest[digest_len];
        GroupSig::sha3(&digest[0],
           const_cast<unsigned char*>((const unsigned char*)hash_input_str.c_str()), 
           hash_input_str.length());
        element_from_hash(c, &digest[0], sizeof(digest));
        LOG(DEBUG)<<"C#####:"<<trans_to_string(c);
    }


    void BBS04SignatureImpl::bbs_group_sig(string& sig, 
               int hashlen, const char* hash,
               bbs_group_public_key_ptr gpk, 
               bbs_group_private_key_ptr gsk)
    {
        assert(gpk && gsk);
        //初始化签名相关的变量
        pairing_ptr pairing= gpk->param->pairing;
        assert(pairing);
        BBS04Signature sig_struct(pairing);
        
        //初始化随机变量
        element_t alpha, beta; 
        sig_struct.gen_random_element(alpha, pairing);
        sig_struct.gen_random_element(beta, pairing);
        sig_struct.gen_random_element(sig_struct.ralpha, pairing, false);
        sig_struct.gen_random_element(sig_struct.rbeta, pairing, false);
        sig_struct.gen_random_element(sig_struct.rx, pairing, false);
        sig_struct.gen_random_element(sig_struct.rdelta1, pairing,false);
        sig_struct.gen_random_element(sig_struct.rdelta2, pairing, false);
        //签名
        element_pow_zn(sig_struct.T1, gpk->u, alpha);
        element_pow_zn(sig_struct.T2, gpk->v, beta);
        
        element_t z0;
        element_init_Zr(z0, pairing);
        element_add(z0, alpha, beta);
        element_pow_zn(sig_struct.T3, gpk->h, z0);
        element_mul(sig_struct.T3, sig_struct.T3, gsk->A);

        element_pow_zn(sig_struct.R1, gpk->u, sig_struct.ralpha);
        element_pow_zn(sig_struct.R2, gpk->v, sig_struct.rbeta);
        //R3 
        element_t et0;
        element_init_GT(et0, pairing);
        element_pow_zn(et0, gpk->pr_h_g2, z0);
        element_mul(et0, et0, gsk->pr_A_g2);

        element_add(z0, sig_struct.ralpha, sig_struct.rbeta);
        element_neg(z0, z0);
        
        element_t z1;
        element_init_Zr(z1, pairing);
        element_add(z1, sig_struct.rdelta1, sig_struct.rdelta2);
        element_neg(z1, z1);

        element_pow3_zn(sig_struct.R3, et0, sig_struct.rx,
                gpk->pr_h_w, z0, gpk->pr_h_g2, z1);

        element_neg(z0, sig_struct.rdelta1);
        element_pow2_zn(sig_struct.R4, sig_struct.T1, sig_struct.rx, gpk->u, z0);

        element_neg(z0, sig_struct.rdelta2);
        element_pow2_zn(sig_struct.R5, sig_struct.T2, sig_struct.rx, gpk->v, z0);

        element_t M;
        element_init_G1(M, pairing);
        //string hash_hex = trans_bytes_to_string(hash, strlen(hash));
        LOG(DEBUG)<<"INNER M VERIFY:"<<hash;
        //element_from_bytes(M, const_cast<unsigned char*>((const unsigned char*)hash_hex.c_str()));
        element_from_hash(M, const_cast<unsigned char*>((const unsigned char*)hash),
                strlen(hash));

        LOG(DEBUG)<<"######HASH OF SIG";
        bbs_cal_sha3(sig_struct.c, sig_struct, M); //计算sha3

        element_mul(z0, sig_struct.c, alpha);
        element_add(sig_struct.ralpha, sig_struct.ralpha, z0); 

        element_mul(z0, sig_struct.c, beta);
        element_add(sig_struct.rbeta, sig_struct.rbeta, z0);

        element_mul(z1, sig_struct.c, gsk->x);
        element_add(sig_struct.rx, sig_struct.rx, z1);

        element_mul(z0, z1, alpha);
        element_add(sig_struct.rdelta1, sig_struct.rdelta1, z0);

        element_mul(z0, z1, beta);
        element_add(sig_struct.rdelta2, sig_struct.rdelta2, z0);
        
        //保存签名
        KeyManager<BBSKey>::store_sig(sig, sig_struct);        
        //清除临时变量
        element_clear(alpha);
        element_clear(beta);
        element_clear(z0);
        element_clear(z1);
	    element_clear(M);
        element_clear(et0);
    }
    
   int BBS04SignatureImpl::bbs_group_verify(const string& sig,
               int hashlen, const char* hash,
               bbs_group_public_key_t gpk)
   {
       pairing_ptr pairing = gpk->param->pairing;
       assert(pairing);
       //初始化签名相关的对象: BBS04Signatrue
       BBS04Signature sig_struct(pairing);
       KeyManager<BBSKey>::load_sig(sig_struct, sig);
       return bbs_group_verify(sig, hashlen, hash, gpk, sig_struct);
   }

   int BBS04SignatureImpl::bbs_group_verify(const string& sig,
               int hashlen, const char* hash,
               bbs_group_public_key_t gpk,
               BBS04Signature& sig_struct)
   {
       assert(gpk);
       pairing_ptr pairing = gpk->param->pairing;
       assert(pairing);
       
       element_t z0;
       element_init_Zr(z0, pairing);
       element_neg(z0, sig_struct.c);
       //计算R1
       element_pow2_zn(sig_struct.R1, gpk->u, sig_struct.ralpha, 
               sig_struct.T1, z0);
       //R2
       element_pow2_zn(sig_struct.R2, gpk->v, sig_struct.rbeta,
               sig_struct.T2, z0);
       //R4
       element_neg(z0, sig_struct.rdelta1);
       element_pow2_zn(sig_struct.R4, gpk->u, z0, sig_struct.T1, sig_struct.rx);
       //R5
       element_neg(z0, sig_struct.rdelta2);
       element_pow2_zn(sig_struct.R5, gpk->v, z0, sig_struct.T2, sig_struct.rx);
       //R3
       element_t e20;
       element_init_G2(e20, pairing);
       element_pow2_zn(e20, gpk->g2, sig_struct.rx, gpk->w, sig_struct.c);
       //element_pow_zn(e20, gpk->g2, sig_struct.rx); //g2^rx
       //e(T3, g2)
       pairing_apply(sig_struct.R3, sig_struct.T3, e20, pairing); //e(T3, g2^rx)
        
       element_add(z0, sig_struct.ralpha, sig_struct.rbeta);
       element_neg(z0, z0); 
    

       element_t z1;
       element_init_Zr(z1, pairing);
       element_add(z1, sig_struct.rdelta1, sig_struct.rdelta2);
       element_neg(z1, z1);

       element_t et0;
       element_init_GT(et0, pairing);
       //(g1,g2)^(-c), e(h,w)^(-ralpha-rbeta)*e(h,g2)^(-rdelta1-rdelta2)
       element_pow3_zn(et0, gpk->pr_g1_g2_inv, sig_struct.c, gpk->pr_h_w,
               z0, gpk->pr_h_g2, z1);

       element_mul(sig_struct.R3, sig_struct.R3, et0);

       //M
       element_t M;
       element_init_G1(M, pairing);
       LOG(DEBUG)<<"INNER M SIG:"<<hash;
       //element_from_bytes(M, const_cast<unsigned char*>((const unsigned char*)hash));
       element_from_hash(M, const_cast<unsigned char*>((const unsigned char*)hash),
               strlen(hash));
             
       element_t c1;
       element_init_Zr(c1, pairing);
       LOG(DEBUG)<<"####HASH OF VERIFY";
       bbs_cal_sha3(c1, sig_struct, M);
      
       //对比hash
       bool ret = 0;
       LOG(DEBUG)<<"SIG_STRUCT.C###:"<<trans_to_string(sig_struct.c);
       LOG(DEBUG)<<"SIG_STRUCT.C1###:"<<trans_to_string(c1);
       if(!element_cmp(sig_struct.c, c1))
       {
           LOG(DEBUG)<<"VERFIY SUCC";
           ret = 1;
       }
       //释放临时变量空间
       element_clear(z0);
       element_clear(e20);
       element_clear(z1);
       element_clear(et0);
       element_clear(M);
       element_clear(c1);
       return ret;
   }

   int BBS04SignatureImpl::bbs_group_open(element_t& cert, bbs_group_public_key_t p_gpk,
           bbs_manager_private_key_t p_gmsk, int hashlen, const char* hash, const string& sig )
   {
       assert(p_gpk && p_gmsk);
       pairing_ptr pairing = p_gpk->param->pairing;
       assert(pairing);
       BBS04Signature sig_struct(pairing);
       KeyManager<BBSKey>::load_sig(sig_struct, sig);
       //验证群签名是否有效
       int valid = bbs_group_verify(sig, hashlen, hash, p_gpk, sig_struct);
       int ret = 1; 
       if(!valid)
       {
           ret = 0;
           LOG(ERROR)<<"INVALID SIG:"<<sig;
           return ret; 
       }      
       LOG(DEBUG)<<"SIG IS VALID, BEGIN OPEN SIG";
       //计算证书       
       element_pow_zn(cert, sig_struct.T1, p_gmsk->xi1);
       element_t e10;
       element_init_G1(e10, pairing);
       element_pow_zn(e10, sig_struct.T2, p_gmsk->xi2);
       element_mul(cert, cert, e10);
       element_invert(cert, cert);
       element_mul(cert, cert, sig_struct.T3);
       LOG(DEBUG)<<"GET CERT SUCCEED";
       //清理临时变量
       element_clear(e10);
       return ret;
   } 

}
}
