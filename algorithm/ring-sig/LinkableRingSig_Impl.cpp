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

/* @file: LinkableRingSig.cpp
 * @author:fisco-dev
 * @date:2018.03.15
 * @function:implementation of linkable ring sig algorithm
 */

#include <iostream>
#include <string>
#include <map>
#include <cryptopp/hex.h>
#include <cryptopp/modarith.h>
#include <cryptopp/osrng.h>
#include <cryptopp/dh.h>

#include "devcore/CommonFunc.h"
#include "devcore/easylog.h"
#include "devcore/StatusCode.h"

#include "algorithm/ring-sig/Common.h"
#include "algorithm/ring-sig/LinkableRingSig_Impl.h"

using namespace CryptoPP;
using namespace RetCode::RingSigStatusCode;
using namespace RetCode::CommonStatusCode;
namespace dev
{
namespace eth
{
  //init ring sig param 
  int LinkableRingSigParam::init_param()
  {
      AutoSeededRandomPool random;
      DH dh;
      LOG(DEBUG)<<"BEGIN INIT RING SIG PARAM, bit_len:";
      try{
      
      dh.AccessGroupParameters().GenerateRandomWithKeySize(random, bit_len);
      if(!dh.GetGroupParameters().ValidateGroup(random, 3))
      {
          LOG(ERROR)<<"Faild to init ring group param";
          return INIT_PARAM_INVALID_PRIME;
      }
      //LOG(DEBUG)<<"gen p";
      //gen p
      p = dh.GetGroupParameters().GetModulus();
      //size_t count = p.BitCount();
      //LOG(DEBUG)<<"##P:"<<std::hex<<p;
      q = dh.GetGroupParameters().GetSubgroupOrder();
      //LOG(DEBUG)<<"##Q:"<<std::hex<<q;
      g = dh.GetGroupParameters().GetGenerator();
      //LOG(DEBUG)<<"##G:"<<std::hex<<g;
      }
      catch(exception& err)
      {
          LOG(ERROR)<<"init_param failed:"<<err.what();
          return SUCCESS;
      }
      return SUCCESS;
  }
  
  //init param
  int LinkableRingSigParam::init_param(const std::string& param_str)
  {
      JsonConfigParser param_loader(param_str, JsonString);
      std::string p_str;
      //////get p;
      bool obtained = param_loader.get_value<std::string>("p", p_str);
      if(!obtained)
      {
          LOG(ERROR)<<"invalid param for p missed";
          return CORRUPTED_PARAM;
      }
      LOG(DEBUG)<<"p:"<<p_str;
      p = Integer(p_str.c_str()); 
      ///////get q
      std::string q_str;
      obtained = param_loader.get_value<std::string>("q", q_str);
      if(!obtained)
      {
          LOG(ERROR)<<"invalid param for q missed";
          return CORRUPTED_PARAM;
      }
      LOG(DEBUG)<<"q:"<<q_str;
      q = Integer(q_str.c_str());
      ////get g
      std::string g_str;
      obtained = param_loader.get_value<std::string>("g", g_str);
      if(!obtained)
      {
          LOG(ERROR)<<"invalid param for g missed";
          return CORRUPTED_PARAM; 
      }
      LOG(DEBUG)<<"g:"<<g_str;
      g = Integer(g_str.c_str());
      return SUCCESS;
  }
    
   //store ring sig param to param_info
   int LinkableRingSigParam::store_param(std::string& param_info)
   {
       std::map<std::string, std::string> param_map;
       try{
           param_map["p"] = num_to_string<Integer>(p);
           param_map["q"] = num_to_string<Integer>(q);
           param_map["g"] = num_to_string<Integer>(g);
           LOG(DEBUG)<<"store p:"<<(param_map["p"])<<", q:"
                     <<(param_map["q"])<<", g:"<<param_map["g"];
           JsonConfigParser::convert_to_json_str(param_info, param_map);
       }
       catch(exception& err)
       {
           LOG(ERROR)<<"store param failed, msg:"<<err.what();
           return STORE_PARAM_FAILED;
       }
       return SUCCESS;
   }
   
   //load ring sig param from private_key_info
   int LinkableRingSigPrivatekeys::load_private_key(
           const std::string& private_key_info)
   {
       JsonConfigParser key_loader(private_key_info, JsonString);
       //load pos
       std::string pos_str;
       bool obtained = key_loader.get_value<std::string>("pos", pos_str);
       if(!obtained)
       {
           LOG(ERROR)<<"load pos from "<<private_key_info<<" failed";
           return CORRUPTED_KEY;
       }
       pos = atoi(pos_str.c_str());
       //LOG(DEBUG)<<"pos:"<<std::hex<<pos;
       //load private_x
       std::string prk_x_str;
       obtained = key_loader.get_value<std::string>("prk_x", prk_x_str);
       if(!obtained)
       {
           LOG(ERROR)<<"load private key from "<<private_key_info<<" failed";
           return CORRUPTED_KEY;
       }
       prk_x = Integer(prk_x_str.c_str());
       //LOG(DEBUG)<<"##private key:"<<prk_x_str;
       return SUCCESS;
   }
   
   //store ring sig param to private_key_info
   int LinkableRingSigPrivatekeys::store_private_key(
           std::string& private_key_info)
   {
       try{
       std::map<std::string, std::string> private_key_map;
       private_key_map["prk_x"] = num_to_string<Integer>(prk_x);
       private_key_map["pos"] = num_to_string<unsigned int>(pos);
       JsonConfigParser::convert_to_json_str(private_key_info, 
               private_key_map);
       LOG(DEBUG)<<"###store private key succ, prk_x:"
                 <<(private_key_map["prk_x"]);
       }
       catch(exception& err)
       {
           LOG(ERROR)<<"store private key failed, error msg:"<<err.what();
           return STORE_KEY_FAILED; 
       }
       return SUCCESS;
   }
   
   //load sig from sig_info
   int LinkableRingSignature::load_sig(
           const std::string& sig_info)
   {
       try
       {
           JsonConfigParser sig_loader(sig_info, JsonString);
           //get Y
           bool ret_code = RingSig::get_value<Integer>(Y, 
                        sig_loader, "Y", CORRUPTED_SIG);
           //LOG(DEBUG)<<"####Y:"<<num_to_string<Integer>(Y);
           if(ret_code != SUCCESS)
               return ret_code;
           //C
           ret_code = RingSig::get_value<Integer>(C, 
                   sig_loader, "C", CORRUPTED_SIG); 
           LOG(DEBUG)<<"###C:"<<num_to_string<Integer>(C);
           if(ret_code != SUCCESS)
               return ret_code;
           //num
           std::string num_str;
           ret_code= RingSig::get_value<std::string>(
                   num_str, sig_loader, "num", CORRUPTED_SIG);

           if(ret_code != SUCCESS)
               return ret_code;
           unsigned int pk_num = atoi(num_str.c_str());
           s_vec.resize(pk_num);
           pk_vec.resize(pk_num);
           std::string key;
           for(unsigned int i = 0; i < pk_num; i++)
           {
               key = "s" + num_to_string<unsigned int>(i);
               ret_code = RingSig::get_value<Integer>(s_vec[i], sig_loader, 
                                            key, CORRUPTED_SIG);
               if( ret_code != SUCCESS)
                   return ret_code;
               key = "pk" + num_to_string<unsigned int>(i);
               ret_code = RingSig::get_value<Integer>(pk_vec[i], sig_loader,
                                             key, CORRUPTED_SIG);
               if( ret_code != SUCCESS)
                   return ret_code;
           }

       }
       catch(exception& e)
       {
           LOG(ERROR)<<"load signature failed";
           return KEY_LOAD_FAILED;
       }
       return SUCCESS;
   }
   
   //store signature to sig_info
   int LinkableRingSignature::store_sig(
           std::string& sig_info,
           const std::vector<string>& public_key_list,
           const int& pk_num)
   {
       try
       {
           if((s_vec.size()<(unsigned int)pk_num) || 
              (public_key_list.size() != (unsigned int)pk_num))
           {
               LOG(ERROR)<<"mismatch between s_vec num:"
                         <<s_vec.size()<<" and pk_num:"<<pk_num;
               return PK_NUM_MISMATCH;
           }
           std::map<std::string, std::string> sig_map;
           sig_map["Y"] = num_to_string<Integer>(Y);
           sig_map["C"] = num_to_string<Integer>(C);
           sig_map["num"] = num_to_string<int>(pk_num);
           std::string key="s";
           for(int i=0; i<pk_num; i++)
           {
               key ="s" + num_to_string<int>(i);
               sig_map[key] = num_to_string<Integer>(s_vec[i]);
               key = "pk" + num_to_string<int>(i);
               sig_map[key] = public_key_list[i];
           }
           JsonConfigParser::convert_to_json_str(sig_info, sig_map);
           LOG(DEBUG)<<"stored sig:"<<sig_info;
       }
       catch(exception& err)
       {
           LOG(ERROR)<<"store sig failed";
           return KEY_STORE_FAILED;
       }
       return SUCCESS;
   }

   //////****########implement of ring signature
   int LinkableRingSigImpl::setup_ring(
           std::string& param_info,
           const int& bit_len)
   {
       LinkableRingSigParam param(bit_len);
       param.init_param();
       param.store_param(param_info);
       //Integer p, q, g;
       //RingSig::init_param(p, q, g);
       return SUCCESS;
   }
   
   //////####member join ring
   int LinkableRingSigImpl::join_ring(
           std::string& private_key_info,
           std::string& public_key_info,
           const std::string& param_info,
           const std::string& pos)
   {
       LinkableRingSigParam param;
       LOG(DEBUG)<<"param_info:"<<param_info;
       int ret_succ = param.init_param(param_info);
       if(ret_succ != SUCCESS )
           return ret_succ;
       ///generate private key
       LinkableRingSigPrivatekeys prk;
       RandomPool random;
       //gen private key
       prk.prk_x = Integer(random, 0, (param.q-1));
       prk.pos = atoi(pos.c_str());
       //gen public key according to private key
       Integer public_key = a_exp_b_mod_c(param.g, prk.prk_x, param.p);
       //trans to string
       public_key_info = num_to_string<Integer>(public_key);
       LOG(DEBUG)<<"gen public "<<public_key_info<<" for "
                  <<pos<<"th member";
       //store private key info
       ret_succ = prk.store_private_key(private_key_info);
       if( ret_succ != SUCCESS)
           return ret_succ;
       LOG(DEBUG)<<"gen private key "<<private_key_info
                 <<" for "<<pos<<"th member";
       return SUCCESS;
   }

   //generate sig
   int LinkableRingSigImpl::linkable_ring_sig(
           std::string& sig,
           const std::string& message,
           const std::vector<std::string>& public_key_list,
           const std::string& private_key_info,
           const std::string& param_info)
   {
       //load param info
       LinkableRingSigParam param;
       int ret_succ = param.init_param(param_info);
       if( ret_succ != SUCCESS )
           return ret_succ;
       //load private key info
       LinkableRingSigPrivatekeys prk;
       ret_succ = prk.load_private_key(private_key_info);
       if( ret_succ != SUCCESS)
           return ret_succ;
       //load public key info
       std::string public_keys_str = "";
       std::vector<Integer> public_keys;
       for(auto public_key: public_key_list)
       {
           public_keys_str += public_key;
           public_keys.push_back(Integer(public_key.c_str()));
       }
       LOG(DEBUG)<<"###SIG public key:"<<public_keys_str;
       //generate ring sig
       LinkableRingSignature sig_obj;
       generate_sig(sig_obj, message, public_keys_str,
               public_keys, param, prk, 
               public_key_list.size());
       //store sig
       //LOG(DEBUG)<<"store sig:";
       ret_succ = sig_obj.store_sig(sig, public_key_list, 
                 public_key_list.size());

       if(ret_succ != SUCCESS)
           return ret_succ;
       //LOG(DEBUG)<<"###FINAL SIG:"<<sig;
       return SUCCESS;
   }
   
   //sig verify
   int LinkableRingSigImpl::linkable_ring_verify(
           bool& valid,
           const std::string& sig,
           const std::string& message,
           const std::string& param_info)
   {
       //load param info
       LinkableRingSigParam param;
       int ret_succ = param.init_param(param_info);
       if(ret_succ != SUCCESS)
           return ret_succ;
       //load signature info
       LinkableRingSignature sig_obj;
       ret_succ = sig_obj.load_sig(sig);
       if(ret_succ != SUCCESS)
           return ret_succ;
       //verify
       std::string public_key_str="";
       for(auto pk: sig_obj.pk_vec)
       {
           public_key_str += num_to_string<Integer>(pk);
       }
       //LOG(DEBUG)<<"public_key_str:"<<public_key_str;
       valid = sig_verify(message, sig_obj,
               public_key_str, param, 
               sig_obj.s_vec.size());
       return SUCCESS;
   }
   
   
   /*Function: linkable ring sig implementation
    *params: 1. sig: signature generated by 
                     linkable ring sig algorithm
    *        2. message: message to be signed
    *        3. public_strings: strings appended by public keys
    *        4. prk: private key of user
    */
    int LinkableRingSigImpl::generate_sig(
            LinkableRingSignature& sig,
            const std::string& message,
            const std::string& public_keys_str,
            const std::vector<Integer>& public_keys, 
            const LinkableRingSigParam& param,
            const LinkableRingSigPrivatekeys& prk,
            const int &member_num)
   {
       //generate public key hash
       std::string h_str;
       RingSig::sha3(h_str, public_keys_str);
       //LOG(DEBUG)<<"sha3 of "<<public_keys_str<<" = "<<h_str;
       Integer h(h_str.c_str());
       ///obtain h from public key hash
       h = h % (param.q);
       h = a_exp_b_mod_c(param.g, h, param.p);
       //LOG(DEBUG)<<"###h:"<<(num_to_string<Integer>(h));
       //LOG(DEBUG)<<"###prk.prk_x:"<<(num_to_string<Integer>(prk.prk_x));
       ///calculate y
       Integer y = a_exp_b_mod_c(h, prk.prk_x, param.p);
       //LOG(DEBUG)<<"SIG Y:"<<y;
       //generate random u
       RandomPool random;
       Integer u(random, 0, param.q-1);
       /////generate init c[i]
       std::string b_plain_str = public_keys_str + 
                       num_to_string<Integer>(y) + message +  
                       num_to_string<Integer>(a_exp_b_mod_c(param.g, u, param.p))+
                       num_to_string<Integer>(a_exp_b_mod_c(h, u, param.p));
        //get hash of b_plain_str to obtain b
       std::string b_hash;
       RingSig::sha3(b_hash, b_plain_str.c_str()); 
       
       LOG(DEBUG)<<"###b:"<<b_hash;
       Integer b(b_hash.c_str());
       //calculate c[i]
       std::vector<Integer> c_vec(member_num);
       sig.s_vec.resize(member_num);
       //LOG(DEBUG)<<"###prk.pos:"<<prk.pos<<"  member_num:"<<member_num;
       unsigned int member_index = prk.pos % member_num;
       //LOG(DEBUG)<<"member_index:"<<member_index<<"  generate S...";
       c_vec[(member_index + 1) % member_num] = b;

       LOG(DEBUG)<<"generate C...";
       std::string hash_str;
       std::string common_str = public_keys_str + 
                     num_to_string<Integer>(y) + message;

       unsigned int i = (member_index + 1) % member_num;
       do{
           sig.s_vec[i] = Integer(random, 0, (param.q-1));
           //LOG(DEBUG)<<"###S"<<i<<":"<<(num_to_string<Integer>(sig.s_vec[i]));
           //public keys + y + message
           //((g^s[i] mod p) * (y[i](public key)^c[i] mod p))mod p
           //(h^s[i] mod p * y^c[i] mod p)mod p
           RingSig::sha3(hash_str, common_str + 
                    num_to_string<Integer>(a_times_b_mod_c(a_exp_b_mod_c(param.g , sig.s_vec[i], param.p), 
                    a_exp_b_mod_c(public_keys[i], c_vec[i], param.p), param.p))
                    + num_to_string<Integer>(a_times_b_mod_c(a_exp_b_mod_c(h, sig.s_vec[i], param.p),
                    a_exp_b_mod_c(y, c_vec[i], param.p), param.p)));

           //LOG(DEBUG)<<"###SIG C "<<(i % member_num)<<" :"
           //    <<(num_to_string<Integer>(c_vec[i]));
           c_vec[(i+1) % member_num] = Integer(hash_str.c_str()); 
           i = (i+1) % member_num;
       }while(i != member_index % member_num);
       //calculate si[member_index]
       sig.s_vec[member_index] = (u % param.q - a_times_b_mod_c(prk.prk_x, 
                   c_vec[member_index], param.q)) % param.q;
       LOG(DEBUG)<<"###S "<<member_index<<" :"<<(num_to_string<Integer>(sig.s_vec[member_index]));
       sig.C = c_vec[0];
       sig.Y = y;
       return SUCCESS;
   }

   /*@ function: verify linkable ring signature
    *@ params: 1. sig: signature
    *          2. public_keys_str: public keys used to 
    *             verify signature
    */ 
   bool LinkableRingSigImpl::sig_verify(
           const std::string& message,
           const LinkableRingSignature& sig,
           const std::string& public_keys_str,
           const LinkableRingSigParam& param, 
           const int& member_num)
   {
       //generate public key hash
       std::string h_str;
       RingSig::sha3(h_str, public_keys_str);
       LOG(DEBUG)<<"VERIFY public_key "<<public_keys_str;
       Integer h(h_str.c_str());
       ///obtain h from public key hash
       h = h % (param.q);
       h = a_exp_b_mod_c(param.g, h, param.p);
       //LOG(DEBUG)<<"###h:"<<(num_to_string<Integer>(h));
       //LOG(DEBUG)<<"SIG Y:"<<sig.Y; 
       Integer zi, zi_dash;
       Integer ci = sig.C;
       std::string hash_str;
       for(int i=0; i< member_num; i++)
       {
            //LOG(DEBUG)<<"######sig.pk_vec"<<i<<(num_to_string<Integer>(sig.pk_vec[i]));
            //LOG(DEBUG)<<"*****VERIFY C"<<i<<":"<<(num_to_string<Integer>(ci));
           //g^s[i] * yi^ci
           zi = a_times_b_mod_c(a_exp_b_mod_c(param.g, sig.s_vec[i], param.p),
                   a_exp_b_mod_c(sig.pk_vec[i], ci, param.p), param.p);
           //h^s[i] * y^ci
           zi_dash = a_times_b_mod_c(a_exp_b_mod_c(h, sig.s_vec[i], param.p),
                   a_exp_b_mod_c(sig.Y, ci, param.p), param.p);
           //ci: hash(public_keys, Y, m, zi, zi_dash)
           RingSig::sha3(hash_str, public_keys_str + 
                   num_to_string<Integer>(sig.Y) + message +
                   num_to_string<Integer>(zi) + 
                   num_to_string<Integer>(zi_dash));
           ci = Integer(hash_str.c_str());    
       }
       LOG(DEBUG)<<"ci:"<<hash_str;
       LOG(DEBUG)<<"C of sig:"<<num_to_string<Integer>(sig.C);
       return (ci == sig.C);
   }
}
}

