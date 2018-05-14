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
/*
 * @file: common.h
 * @author: fisco-dev
 * @date: 2018.2.5
 */

#pragma once
#include  <iostream>
#include <memory>
#include <pthread.h>
#include <utility>

namespace dev
{
namespace eth
{
 //mutex encapsulation 
 class Locker
 {
     private:
         pthread_mutex_t m_lock;
     public:
         Locker(){pthread_mutex_init(&m_lock, NULL); }

         ~Locker(){pthread_mutex_destroy(&m_lock); }

         void lock(){pthread_mutex_lock(&m_lock); }
         void unlock(){pthread_mutex_unlock(&m_lock); }

 };

 //manager mutx
 class GuardLock
 {
     private:
        Locker& m_lock;
     public:
        GuardLock(Locker& _m_lock):m_lock(_m_lock)
        {
            m_lock.lock();
        }

        ~GuardLock(){ m_lock.unlock(); }
 };

 //singleton pattern
 template<typename T>
 class singleton
 {
   private:
    //forbid assignment and construction
    singleton(const singleton&);
    singleton();
    singleton& operator = (const singleton&);
    
    static std::shared_ptr<T> p_value;
    static Locker mutex;
   public:
    //create instance with singleton pattern
    template<typename... Args>
    static std::shared_ptr<T> instance(Args&&... args)
    {
        if(!p_value)
        {
            mutex.lock();
            p_value =  std::make_shared<T>(std::forward<Args>(args)...);
            mutex.unlock();
        }
        return p_value;
     }
    
    /*template<typename... Args>
    static T* instance_ptr(Args&&... args)
    {
        return (intance(std::forward<Args>(args)...)).get();
    }*/

 };
 template<typename T> std::shared_ptr<T> singleton<T>::p_value;
 template<typename T>Locker singleton<T>::mutex = Locker(); //init mutex
}
}
