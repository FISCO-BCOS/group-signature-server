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

/* @file: RingSig.h
 * @author:fisco-bcos
 * @date:2018.03.06
 * @function: interface of group signature algorithm 
 */
#pragma once
#include <cryptopp/integer.h> 

#include <algorithm/ring-sig/Common.h>
#include <algorithm/ring-sig/LinkableRingSig_Impl.h>

#if !defined(__cplusplus)
extern "C"
{
#endif

#if defined(__cplusplus)
namespace RingSigApi
{
namespace LinkableRingSig
{
#endif
    /* @function: generate params for specified ring
     * @params: 1. param_info: return value, json
     *          string of ring params
     *          2. bit_len: length of ring param element
     *          default is 1024(1024-security)
     * @ret: SUCCESS: generate params for specified ring succeed
     *       other ret code: generate params for specified ring
     *       failed
     */
    int setup_ring(std::string& param_info, 
                   const int& bit_len=1024);

    /* @function: generate private key for member of 
     *            specified ring
     * @params: 1. private_key_info: return value, private
     *            key used to generate signature
     *          2. public_key_info: return value, public 
     *          key generated for ring member
     *          3. param_info: param info of specified ring
     *          4. pos: position of this ring member
     *
     * @ret: SUCCESS: generate private && public key for
     *       specified member succeed 
     *       other ret code: generate private && public key
     *       for specified member failed
     */ 
    int join_ring(std::string& private_key_info,
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
    int ring_sig(std::string& sig,
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
    int ring_verify(bool &valid, const std::string sig,
                    const std::string& message, 
                    const std::string& param_info);

#if defined(__cplusplus)
}
}
#endif
#if !defined(__cplusplus)
}
#endif
