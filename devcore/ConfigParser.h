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
 * @file: ConfigParser.h
 * @author: fisco-dev
 * @date: 2018.2.5
 * @function: config parser
 */

#pragma once
#include <exception>
#include <map>
#include <string>

#include "boost/property_tree/json_parser.hpp"
#include "boost/property_tree/ptree.hpp"

#include "devcore/easylog.h"

using namespace boost::property_tree;
using namespace std;

namespace dev
{
namespace eth
{
    //interface of Configuration class
    template <typename T>
    class ConfigParserInterface
    {
        public:
            ConfigParserInterface()
            {}
             
            template<typename... Args> 
            void create(Args&&... args)
            {
                //LOG(DEBUG)<<"Begin create config_obj"<<std::endl;
                config_obj = make_shared<T>(std::forward<Args>(args)...); 
            }
            
            /*
             * @function: read configuration value of key, default is default_value 
             * @param: 1. key: configuration key; 2. value: value of key;
             *         3. default_value: default value of key
             * @ret: 1. true: value of key is set by configuration;
             *       2. false：value of key is set by default_value;
             */
            template<typename... Args> 
            bool get_value(Args&&... args)
            {
                return config_obj->get_value(std::forward<Args>(args)...);
            }
             
            template <typename S, typename... Args>
            S get_value(Args&&... args)
            {
                return config_obj->get_value<S>(std::forward<Args>(args)...);
            }

            /*
             *@function: set value for key;
             *@params: key; 
             */
            template<typename... Args>
            void set_value(Args&&... args)
            { return config_obj->set_value(std::forward<Args>(args)...); }
            
            //dump configuration
            template<typename... Args>
            void dump_config(Args&&... args)
            {return config_obj->dump_config(std::forward<Args>(args)...); } 
            
            //load configuration
            template<typename... Args>
            void load_config(Args&&... args)
            { return config_obj->load_config(std::forward<Args>(args)...);}
        private:
            shared_ptr<T> config_obj; 
    };
    
    //####base class of json parser######
    enum JsonConfigType{JsonString, JsonFile};
    class JsonConfigParser
    {
        private:
            //forbid assignment and default transition 
            JsonConfigParser(const JsonConfigParser&);
            JsonConfigParser();
            JsonConfigParser& operator=(const JsonConfigParser&);

        public:
            //init ptree according to json string or json file
            JsonConfigParser(string const& json_str, 
                    JsonConfigType type=JsonFile):
                    input_type(type)
            {
                if (JsonString == type)
                {
                    try{
                        //LOG(DEBUG) << "parse json string "<< JsonString;
                        stringstream json_stream;
                        json_stream << json_str;
                        read_json<ptree>(json_stream, json_pt);
                    }
                    catch(exception& error_msg){
                        LOG(ERROR)<<"LOAD JSON STR:"<<json_str<<" FAILED, error_msg:"
                                  <<error_msg.what();
                    }

                }
                //default is JsonFile
                else
                {
                    try{
                        config_file = json_str; 
                        read_json<ptree>(json_str, json_pt);
                    }
                    catch(exception& error_msg){
                        LOG(ERROR)<<"LOAD JSON FROM FILE "<<json_str<<" FAILED, error_msg:"
                                  <<error_msg.what();
                    }
                }
            }

            /*@function: get value of key from json string, result is stored in value
             *@param: 1. key; 
             *        2. value: value of key;
             *        3. default_value: default value of key
             *@ret: case 1. value of key is obtained from configuration: true;
             *      case 2. value of key is obtained from default_value: false;
             */
            template<typename T> 
            bool get_value(const string &key, T& value, ptree& pt)
            { 
                try{
                    //value = json_pt.get<T>(key);
                    value = pt.get<T>(key);
                    return true;
                }
                catch(ptree_error err_msg){
                    LOG(WARNING)<<"key "<<key<<" doesn't exist, msg:"<<err_msg.what();
                    //LOG(WARNING)<<"key="<<key<<" not exists config file, error msg:"<<(err_msg.what());
                    //LOG(ERROR)<<"get "<<key<<" from json_str failed";
                }
                return false;
            } 
            
            template<typename T>
            bool get_value(const string &key, T& value, const T& default_value, ptree& pt)
            {
                value = default_value;
                return get_value(key, value, pt);
            }
            
            template<typename T>
            bool get_value(const string &key, T& value, const T& default_value)
            {
                return get_value(key, value, default_value, json_pt);
            }
            
            template<typename T>
            bool get_value(const string &key, T& value)
            {
                return get_value(key, value, json_pt);
            }
        
            template<typename T>
            T get_value(const string &key)
            {
                T value;
                get_value(key, value, json_pt);
                return value;
            }
            
            /*
             *@function: set value of key;
             *@params: key;
             */
            template<typename T>
            void set_value(const string& key, const T& value)
            {
               json_pt.put(key, value); 
            }
            
            template<typename T>
            static void convert_to_json_str(std::string& json_str, 
                    std::map<string, T> value_map)
            {
                ptree root_json;
                for(auto item : value_map)
                    root_json.put(item.first, item.second);
                std::stringstream json_stream;
                write_json(json_stream, root_json);
                json_str = json_stream.str();    
            }

        protected:
            ptree json_pt;
            JsonConfigType input_type;
            string config_file = "";
    };
}
}
