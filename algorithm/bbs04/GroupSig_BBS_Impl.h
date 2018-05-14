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

#include "algorithm/LinearPair.h"
#include "algorithm/GroupSigInterface.h"

namespace dev
{
namespace eth
{
//signature struct of bbs04 algorithm
struct BBS04Signature
{
    //elements calcualted by random number,
    //group public key and private key of group member
    element_t T1;
    element_t T2;
    element_t T3;

    element_t c; //hash of signature
    element_t ralpha; //random number, one part of the signature
    element_t rbeta;  //random numberm, one part of the signature
    element_t rx;     //random number, one part of the signature
    element_t rdelta1;  //random number, one part of the signature
    element_t rdelta2;  //random number, one part of the signature
    
    //elements calculated by 
    //random number(ralpha, rbeta, rx , rdelta1, rdelta2)
    //and (T1, T3, T3)
    //but (R1, R2, R3, R4, R5) is private to others
    //verify procedure can recover (R1, R2, R3, R4, R5) according to
    //public key of group, signature and plain text of the signature.
    element_t R1;
    element_t R2;
    element_t R3;
    element_t R4;
    element_t R5;
    
    //allocate memory for signature struct
    BBS04Signature(pairing_ptr pairing)
    {
        element_init_G1(T1, pairing);
        element_init_G1(T2, pairing);
        element_init_G1(T3, pairing);
        element_init_G1(R1, pairing);
        element_init_G1(R2, pairing);
        element_init_GT(R3, pairing);
        element_init_G1(R4, pairing);
        element_init_G1(R5, pairing);
        
        element_init_Zr(c, pairing); 
        element_init_Zr(ralpha, pairing);
        element_init_Zr(rbeta, pairing);
        element_init_Zr(rx, pairing);
        element_init_Zr(rdelta1, pairing);
        element_init_Zr(rdelta2, pairing);
    }
     
    //free memory space when signature procedure ended 
    //or verification procedure ended
    ~BBS04Signature()
    {
        element_clear(T1);
        element_clear(T2);
        element_clear(T3);
        
        element_clear(R1);
        element_clear(R2);
        element_clear(R3);
        element_clear(R4);
        element_clear(R5);
        
        element_clear(c);
        element_clear(ralpha);
        element_clear(rbeta);
        element_clear(rx);
        element_clear(rdelta1);
        element_clear(rdelta2);
    } 

    /* @function: generate random number with specified linear pair
     * @param: 1. random_e: generated random number
     *         2. pairing: linear pair used to generate random number
     *         3. need_init: indicates the "random_e" is allocated memory or
     *                       not, if "random_e" is not allocated memory,
     *                       need_init should be "true", default is "true"
     */
    static void gen_random_element(element_t& random_e, 
            pairing_ptr pairing,
            bool need_init=true)
      {
        assert(random_e && pairing);
        if(need_init)
            element_init_Zr(random_e, pairing);
        element_random(random_e);
     }
};

//implement signature and verification algorithm of bbs04 algorithm
class BBS04SignatureImpl
{
    public:
        /* @function: calculate sha3 of (signature, plain text),
         *            and trans it to element_t type
         * @params: 1.c: ret value, in accordance with element "c"
         *               of bbs04 algorithm
         *          2.sig: signature structure of bbs04 algorithm
         *          3.M: plain text
         *          4.digest_len: length of hash string,
         *            default is 32Bytes(use sha3, 256 bits)
         */
        static void bbs_cal_sha3(element_t& c,BBS04Signature& sig,
                element_ptr M,
                unsigned int digest_len=32);

        /* @function: implement group signature method of bbs04 algorithm
         * @params: 1.sig: ret value, signature result of specified plain text 
         *          2.hashlen: length of plain text need to be signed
         *          3.hash: plain text information need to generate signature 
         *          4.gpk: public key of specified group
         *          5.gsk: private key of specified group member
         */
        static void bbs_group_sig(string& sig, 
               int hashlen, const char* hash,
               bbs_group_public_key_ptr gpk, 
               bbs_group_private_key_ptr gsk);

       /* @function: implement group verification method of bbs04 algorithm
        * @params: 1. sig: signature need to be verified
        *          2. hashlen: length of the plain text
        *          3. hash: plain text of specified sig
        *          4. gpk: public key of specified group
        * @ret: 1.signature is valid: return 1
        *       2.signature is invalid: return 0
        */
       static int bbs_group_verify(const string& sig,
               int hashlen, const char* hash,
               bbs_group_public_key_t gpk);
        
       /* @function: implement group verification method of bbs04 algorithm
        * @params: 1. sig: signature need to be verified
        *          2. hashlen: length of the plain text
        *          3. hash: plain text of specified sig
        *          4. gpk: public key of specified group
        *          5. sig_struct: signature struct transformed from string
        * @ret: 1.signature is valid: return 1
        *       2.signature is invalid: return 0
        */
       static int bbs_group_verify(const string& sig,
               int hashlen, const char* hash,
               bbs_group_public_key_t gpk,
               BBS04Signature& sig_struct);

       /* @function: obtain owner cert of specified sig(group manager callback)
        * @params: 1. cert: return value, owner cert of specified sig
        *          2. p_gpk: public key of the group the sigature belongs to
        *          3. p_gmsk: private key of group manager  
        *          4. hashlen: length of the plain text
        *          5. hash: plain text of the specified sig
        *          6. sig: signature that need to trace its owner
        * @ret: the signature is valid && get the owner cert of the signature:1
        *       the signature is invalid or get the owner cert of the signature:0
        *
        */
       static int bbs_group_open(element_t& cert,
                bbs_group_public_key_t p_gpk,
                bbs_manager_private_key_t p_gmsk,
                int hashlen, const char* hash, 
                const string& sig);
   
};
}
}
