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

/* @file: LinkableRingSig.h
 * @author:fisco-dev
 * @date:2018.03.15
 * @function:implementation of linkable ring sig algorithm
 */

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <cryptopp/integer.h>

#include "devcore/ConfigParser.h"

using namespace CryptoPP;

namespace dev
{
namespace eth
{
//param of linkable ring sig
//linkable ring signature algorithm is based on 
//discrete logarithm problem
class LinkableRingSigParam
{
public:
    Integer p; //big integer p, is open to everyone
    Integer q; //big integer q
    Integer g; //generator 
   int bit_len; //length of public key and private key
   
   LinkableRingSigParam(const int _bit_len=1024):
       bit_len(_bit_len){ }
   
   /* @function: initialize params of specified ring
    *            include p, q, g
    *
    * @ret: SUCCESS: init params succeed
    *       other ret code: init params failed
    *
    * @des: when generate a ring, callback this function
    *       to generate params
    */  
   virtual int init_param();
   
   /* @function: restore params of specified ring
    *            from given string 
    *
    * @param: init_param: string used to recover ring params
    *
    * @ret: SUCCESS: recover params from string succeed
    *       other code: recover params from string failed
    *
    * @des: when generate signature or verify signature,
    *       should load the param information first
    */
   virtual int init_param(const std::string& init_param);

   /* @function: store params to json string
    * @param: param_info: return value, transformed json
    *         string of params
    *
    * @ret: SUCCESS: store ring params succeed;
    *       other code: store ring params failed
    *
    * @des: after generate params for specified ring by callback
    *       "init_param()", the param should be transformed
    *       to string and stored in db/blockchain,...
    *       since signature and verification need these params
    */
   virtual int store_param(std::string& param_info);
};

//private key struct of specified ring member
struct LinkableRingSigPrivatekeys
{
    Integer prk_x; //private key of given member
    uint32_t pos; //position in the ring
     
    /* @function: load private key from given string
     * @params: private_key_info: json string of private
     *          key
     *
     * @ret: SUCCESS: load private key information from
     *       string succeed
     *       other ret code: load private key information from
     *       string failed
     */
    virtual int load_private_key(const std::string& private_key_info);
    
    /* @function: store private key elements to json string
     * @param: private_key_info: return value, json string
     *         transformed from specified private key
     *
     * @ret: SUCCESS: transform private key to string succeed;
     *       other code: transform private key to string failed
     */
    virtual int store_private_key(std::string& private_key_info);
};


//signature struct of linkable ring sig
struct LinkableRingSignature
{
    //vector s for verify
    std::vector<Integer> s_vec;
    //public keys used to generate signature 
    std::vector<Integer> pk_vec;
    //secrete element
    Integer C;  
    //secrete element
    //genearated by combination of public keys
    //and private key
    Integer Y;

    /* @function: load signature structure from string
     * @param: sig_info: string used to transformed into
     *         signature structure
     
     * @ret: 1. SUCCESS: load signature struct from string 
     *          succeed
     *       2. other ret_code: load signature struct from
     *          string failed
     */ 
    virtual int load_sig(const std::string& sig_info);

    /* @function: transform signature into json string
     * @params: 1. sig_info: return value, json string 
     *          transformed from signature struct
     *          2. public_key_list: public key list used
     *          in generating signature
     *          3. pk_num: number of public keys in 
     *          public_key_list
     *
     * @ret: 1. SUCCESS: store signature structure succeed
     *       2. other ret code: store signature to string 
     *          failed
     */ 
    virtual int store_sig(std::string& sig_info,
           const std::vector<std::string>& public_key_list,
           const int& pk_num);
};

//implementation of linkable ring sig algorithm
class LinkableRingSigImpl
{
public:
    /* @function: generate params for specified ring
     * @params: 1. param_info: return value, json
     *          string of ring params
     *          2. bit_len: length of ring param element
     *          default is 1024(1024-security)
     * @ret: SUCCESS: generate params for specified ring succeed
     *       other ret code: generate params for specified ring
     *       failed
     */
    static int setup_ring(std::string& param_info,
                         const int& bit_len = 1024);
    
    /* @function: generate private key for member of 
     *            specified ring
     * @params: 1. private_key_info: return value, private
     *            key used to generate signature
     *          2. public_key_info: return value, public 
     *          key generated for ring member
     *          3. param_info: param info of specified ring
     *          4. mem_pos: position of this ring member
     *
     * @ret: SUCCESS: generate private && public key for
     *       specified member succeed 
     *       other ret code: generate private && public key
     *       for specified member failed
     */ 
    static int join_ring(std::string& private_key_info,
            std::string& public_key_info,
            const std::string& param_info,
            const std::string& pos);

    /* @function: generate signature with linkable ring sig
     *      algorithm for specified ring member
     * @param: 1. sig: return value, generated signature
     *         2. message: plain text
     *         3. public_key_list: public key list used to
     *         generate linkable signature
     *         4. private_key_info:private key of ring member 
     *         to generate signature
     *         5. param_info: param info of ring the member 
     *         belongs to
     * @ret: SUCCESS: callback linkable ring signature algorithm succeed
     *       other ret code: callback linkable ring signature algorithm failed
     */
    static int linkable_ring_sig(std::string& sig,
            const std::string& message,
            const std::vector<std::string>& public_key_list,
            const std::string& private_key_info,
            const std::string& param_info);

    /* @function: verify given signature is valid or not
     * @params: 1. valid: return value, 
               true: signature is valid; false: signature is invalid;
     *         2. sig: signature to be verified
     *         3. message: plain text 
     *         4. param_info: param inforamtion of ring this signature 
     *         belongs to 
     * @ret: SUCCESS: callback linkable ring verify algorithm succeed
     *          other ret code: callback verify algorithm failed
     */
    static int linkable_ring_verify(
            bool &valid,
            const std::string &sig,
            const std::string& message,
            const std::string& param_info);

   const static int DEFAULT_MEMBER_NUM = 32;

private:
   /*@function: concrete implementation of linkable_ring_sig
    *@params: 
    *         1. sig: signature to be verified
    *         2. message: plain text 
    *         3. public_keys_str: string of public keys used 
    *            to generate ring signature
    *         4. public_keys: public key list used to generate
    *            ring signature
    *         5. param: param inforamtion of ring this signature
    *         6. prk: private key information
    *         7. member_num: number of public keys used to generate
    *            ring signature
    * @ret: SUCCESS: callback linkable ring sig algorithm succeed
    *          other ret code: callback linkable ring sig algorith failed
    */
   static int generate_sig(LinkableRingSignature& sig,
            const std::string& message,
            const std::string& public_keys_str,
            const std::vector<Integer>& public_keys, 
            const LinkableRingSigParam& param,
            const LinkableRingSigPrivatekeys& prk,
            const int& memeber_num);

   /* @function: concrete implementation of linkable_ring_verify
    * @params: 1. message: plain text
    *          2. sig: structure structure
    *          3. public_keys_str: string of public keys used to verify 
    *          given signature
    *          4. param: param of ring given signature belongs to
    *          5. member_num: number of public keys used to verify
    *           ring signature
    * @ret: SUCCESS: callback linkable ring verify algorithm succeed
    *       other ret code: callback verify algorithm failed
    */
   static bool sig_verify(
           const std::string& message,
           const LinkableRingSignature& sig,
           const std::string& public_keys_str,
           const LinkableRingSigParam& param, 
           const int& member_num);
};
}
}

