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

#include <devcore/easylog.h>

#include <algorithm/RingSig.h>

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
    int setup_ring(std::string& param_info, 
                   const int& bit_len)
    {
       LOG(DEBUG)<<"setup ring...";
       CryptoPP::Integer p;
       return dev::eth::LinkableRingSigImpl::setup_ring(param_info, bit_len);
    }
    
    int join_ring(std::string& private_key_info,
            std::string& public_key_info,
            const std::string& param_info,
            const std::string& pos)
    {
        LOG(DEBUG)<<"join ring...";
        return dev::eth::LinkableRingSigImpl::join_ring(private_key_info,
                public_key_info, param_info, pos);
    }

    int ring_sig(std::string& sig,
            const std::string& message,
            const std::vector<std::string>& public_key_list,
            const std::string& private_key_info,
            const std::string& param_info)
    {
        LOG(DEBUG)<<"generate sig...";
        return dev::eth::LinkableRingSigImpl::linkable_ring_sig(sig, message,
                    public_key_list, private_key_info, param_info);
    }
    
    int ring_verify(bool &valid, const std::string sig,
                    const std::string& message, 
                    const std::string& param_info)
    {
        LOG(DEBUG)<<"ring sig verify...";
        return dev::eth::LinkableRingSigImpl::linkable_ring_verify(valid,
                sig, message, param_info);
    }
#if defined(__cplusplus)
}
}
#endif
#if !defined(__cplusplus)
}
#endif
