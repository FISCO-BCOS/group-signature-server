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

/* @file: Common.h
 * @author:fisco-dev
 * @date:2018.03.15
 * @function:implementation of ring sig common function 
 */
#pragma once
#include <string>
#include <cryptopp/integer.h>
#include <cryptopp/hex.h>
#include <cryptopp/sha.h>

#include "devcore/CommonFunc.h"
#include "devcore/ConfigParser.h"
#include "devcore/StatusCode.h"
#include "devcore/SHA3.h"

using namespace CryptoPP;
using namespace dev::eth;
using namespace RetCode::RingSigStatusCode;
namespace RingSig
{
    static inline void sha3(
            std::string& result,
            const std::string& input,
            unsigned int digest_len=32)
    {
       result = "";
        unsigned char digest[digest_len];
        GroupSig::sha3(&digest[0], 
                const_cast<unsigned char*>((const unsigned char*)input.c_str()),
                input.length());
        //result = trans_bytes_to_string(&digest[0], digest_len);
         CryptoPP::HexEncoder encoder(new CryptoPP::StringSink(result), true);
         encoder.Put(digest, sizeof(digest));
         encoder.MessageEnd();
         result = "0x" + result;
         LOG(DEBUG)<<"SHA3 RESULT:"<<result;
    }

   template<class T>
   static inline int get_value(T& big_num, 
           JsonConfigParser& loader,
           const std::string& key,
           const int& err_code = CORRUPTED_KEY)
   {
       std::string value_str;
       bool obtained = loader.get_value<std::string>(key, value_str);
       if(!obtained)
       {
           LOG(ERROR)<<"obtained value of "<<key<<" failed";
           return err_code;
       }
       big_num = T(value_str.c_str());
       return RetCode::CommonStatusCode::SUCCESS;
   }
}
