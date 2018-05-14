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

/* @file: GroupSig.h
 * @author:fisco-bcos
 * @date:2018.03.06
 * @function: interface of group signature algorithm 
 */

#pragma once
#include "algorithm/GroupSigFactory.h"

#if !defined(__cplusplus)
extern "C"
{
#endif

#if defined(__cplusplus)
namespace GroupSigApi
{
#endif

    /* @function: create group with default linear pair(A Linear pair)
     * @params: 1. result: return value, include "group public key", 
     *          "group manager private key(gmsk)",
     *          "group private key(gamma)" and 
     *          "pbc param used in the algorithm(pbc_param)",
     *          these information is splited with "*"
     *
     *          2. algorithm_method: algorithm of group sig,
     *          such as bbs04, bs04_vlr, achm05, only support bbs04 now
     *
     * @ret: ret_code, defined in file "devcore/StatusCode.h"
     *       SUCCESS(0): create group succeed
     *       other ret_code: create group failed
     *       (failed reasons maybe input param can't be parsed...)
     */
    int create_group_default(std::string& result,
            const std::string& algorithm_method);

    /* @function: create group with specified linear pair param 
     * @params: 1. result: return value, include "group public key(gpk)",
     *         "group mananger private key(gmsk)", 
     *         "group private key(gamma)" and 
     *         "pbc param used in the algorithm(pbc_param)", these information
     *         is splited with "*"
     *
     *         2. algorithm_method: algorithm of group signature implemented with,
     *         such as bbs04, bs04_vlr, achm05, only support bbs04 now
     *
     *         3. pbc_param_str: specified pbc param information:
     *         (1) A type linear param (q_bits_len and r_bits_len can be self-specified):
     *            "{\"linear_type\": \"a\", \"q_bits_len\": , \"r_bits_len\": 256, \"q_bits_len\":256}"
     *         (2) A1 type linear pair (order can be self-specified):
     *            "{\"linear_type\":\"a_one\", \"order\":512}"
     *         (3) E type linear pair (q_bits_len and r_bits_len can be self-specified):
     *            "{\"linear_type\":\"e\", \"q_bits_len\": 512, \"r_bits_len\":512}"
     *         (4) F type linear pair (bit_len can be self-specified):
     *            "{\"linear_type\":\"f\", \"bit_len\": 256}"
     *
     * @ret: ret_code, defined in file "devcore/StatusCode.h"
     *       SUCCESS(0): create group succeed
     *       other ret_code: create group failed
     *       (failed reasons maybe input param can't be parsed...)
     */ 
    int create_group(std::string& result,
            const std::string& algorithm_method,
            const std::string& pbc_param_str);
    
    //group  member join: group manager generate private key && cert for group members
    /* @function: generate private key and cert for joined member 
     * @params: 1. gsk: return value, private key and cert of joined group member;
     *         
     *         2. algorithm_method: algorithm of group signature implemented with,
     *         such as bbs04, bs04_vlr, achm05, only support bbs04 now
     *
     *         3. pbc_param_info: pbc param information of group to be joined
     *         (generally obtained upon group generation, and open to others) 
     *
     *         4. gmsk_info: private key of group manager
     *         (Thus, group mananger gen private key and cert for group member)
     *
     *         5. gpk_info: public key of the group
     *         (generated upon create_group)
     *
     *         6. gamma_info: private information maintained by group member 
     
     * @ret: ret_code, defined in file "devcore/StatusCode.h"
     *       SUCCESS(0): generate private key and cert for group member succeed
     *       other ret_code: generate private key and cert for group member failed
     *       (failed reasons maybe input param can't be parsed...)
     */ 
    int group_member_join(std::string& gsk, 
            const std::string &algorithm_method,
            const std::string &pbc_param_info,
            const std::string &gmsk_info, 
            const std::string &gpk_info,
            const std::string &gamma_info);

    
    /* @function: generate signature with specified group sig algorithm   
     * @params: 1. result: signature 
     *          2. algorithm_method: algorithm of group signature implemented with,
     *             such as bbs04, bs04_vlr, achm05, only support bbs04 now
     *          3. gpk_info: public key of the group 
     *          4. gsk_info: private key of the member to generate signature
     *          5. pbc_param_info: pbc param information of the group  
     *          6. message: plain text to generate group signature
     *
     * @ret: ret_code, defined in file "devcore/StatusCode.h"
     *       SUCCESS(0): generate signature succeed
     *       other ret_code: generate signature failed
     *       (failed reasons maybe input param can't be parsed...)
     */ 
    int group_sig(std::string& result, 
            const std::string& algorithm_method,
            const std::string& gpk_info,
            const std::string& gsk_info, 
            const std::string& pbc_param_info,
            const std::string &message);

    /* @function: verify specified signature 
     * @params: 1. ret: return value, 
     *          indicate specified signature is valid or not
     *          2. message: plain text of specified signature
     *          3. sig: signature
     *          4. algorithm_method: algorithm of group signature implemented with,
     *             such as bbs04, bs04_vlr, achm05, only support bbs04 now
     *          5. gpk_info: public key of the group the signature belongs to
     *          6. pbc_param_info: pbc param information of the group the signature belongs to
     
     * @ret: ret_code, defined in file "devcore/StatusCode.h"
     *       SUCCESS(0): callback verify function succeed
     *       other ret_code: callback verify function failed
     *       (failed reasons maybe input param can't be parsed...)
     */
    int group_verify(int& ret,
            const std::string& message,
            const std::string& sig,
            const std::string& algorithm_method,
            const std::string& gpk_info,
            const std::string& pbc_param_info);

    //implementation of group open with given signature
    /* @function: get cert according to given signature
                 (only group manager can calculate the cert)
                 (generally used in regulation cases)

     * @param: 1. cert:
     *         2.algorithm_method: algorithm of group signature implemented with,
     *             such as bbs04, bs04_vlr, achm05, only support bbs04 now
     *         3. sig: specified signature to get its signer
     *         4. message: plain message of this signature
     *         5. gpk_info: public key of the group
     *         6. gmsk_info: private key of group manager
     *         7. pbc_param_info: pbc param information of the group the signature belongs to
     *
     * @ret: ret_code, defined in file "devcore/StatusCode.h"
     *       SUCCESS(0): callback open_cert function succeed
     *       other ret_code: callback open_cert function failed
     *       (failed reasons maybe input param can't be parsed...)
     */ 
    int open_cert(std::string& cert, 
            const std::string& algorithm_method,
            const std::string& sig,
            const std::string& message,
            const std::string& gpk_info, 
            const std::string& gmsk_info,
            const std::string& pbc_param_info);

    //update gpk when group memeber revoked(executed by group manager)
    /* @function: update gpk when group member revoked
     * @params: 1. gpk: public key of the group the revoked member belongs to
     *                  (gpk is updated after member revoked)
     *          2.algorithm_method: algorithm of group signature implemented with,
     *             such as bbs04, bs04_vlr, achm05, only support bbs04 now
     *          3. pbc_param: pbc param of the group the revoked member belongs to
     *          4. revoke_info: private and cert information of revoked member
     *          5. gamma_info: private information of the group, it is maintained by
     *                         group manager
     *
     * @ret: ret_code, defined in file "devcore/StatusCode.h"
     *       SUCCESS(0): callback revoke_member function succeed
     *       other ret_code: callback revoke_member function failed
     *       (failed reasons maybe input param can't be parsed...)
     */ 
    int revoke_member(std::string& gpk,
         const std::string& algorithm_method,
         const std::string& pbc_param,
         const std::string& revoke_info,
         const std::string& gamma_info);

    /* @function: update group member private key after some members revoked
     *            (callback by group member when it callback group_sig and some people
     *            hava revoked from the group)
     * @params: 1. gsk: input value && output value,
     *                  input vaule: origin private key of specified group member
     *                  output value: updated private key used for generating signature
     *                  after some people revoked
     *
     *          2.algorithm_method: algorithm of group signature implemented with,
     *             such as bbs04, bs04_vlr, achm05, only support bbs04 now
     *           
     *          3. pbc_param: pbc param of the group the member belongs to
     *          4. revoked list: private keys and certs of revoked members since the 
     *                           member update its private key last time
     *          6. gone_list: gone information since the member update its private key
     *                        last time
     *          7. gpk_info: public key of group the member belongs to 
     *
     * @ret: ret_code, defined in file "devcore/StatusCode.h"
     *       SUCCESS(0): callback update private key function succeed
     *       other ret_code: callback update private key function succeed
     *       (failed reasons maybe input param can't be parsed...)
     */ 
    int revoke_update_private_key(std::string& gsk, 
            const std::string& algorithm_method,
            const std::string& pbc_param, 
            const std::string& revoke_list,
            const std::string& gone_list, 
            const std::string& gpk_info);
#if defined(__cplusplus)
}
#endif
#if !defined(__cplusplus)
}
#endif
