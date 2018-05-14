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
/* @file: GroupFactory.h
 * @author: fisco-dev
 * @date: 2018.02.11
 */ 

#pragma once
#include <memory>

#include "devcore/CommonStruct.h"
#include "devcore/easylog.h"

#include "algorithm/bbs04/GroupSig_BBS.h"
#include "algorithm/GroupSigInterface.h"

namespace dev
{
namespace eth
{
    class GroupSigFactory
    {
        public:
            //static shared_ptr<GroupSigInterface> instance(GroupSigMethod& method)
            static shared_ptr<GroupSigInterface> instance(GroupSigMethod method)
            {
                switch(method)
                {
                    case BBS04:
                        LOG(DEBUG)<<"BBS04 METHOD";
                        return singleton<BBSGroupSig>::instance();
                    case BS04_VLR:
                        LOG(WARNING)<<"BS04_VLR not implemented yet"; 
                        break;
                    case ACHM05:
                        LOG(WARNING)<<"ACHM05 not implemented yet";
                        break;
                    default:
                        unkown_method(method);
                        break;
                }
                LOG(INFO)<<"UNKOWN GROUP SIG METHOD, USE BBS04 as default";
                return singleton<BBSGroupSig>::instance();
            }

            static void unkown_method(GroupSigMethod& group_method)
            {
                LOG(ERROR)<<"GROUP_METHOD = "<<group_method<<" UNKOWN GROUP_METHOD";
            }
    };
}
}
