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

/* @file: LinerPair.h
 * @author: fisco-dev
 * @date: 2018.02.11
 * @function:定义了PBC支持的几种线性对
 */ 

#pragma once
#include <string>
#include <memory>
#include "pbc/pbc.h"

#include "devcore/CommonStruct.h"
#include "devcore/ConfigParser.h"
#include "devcore/CommonFunc.h"
#include "devcore/easylog.h"
#include "devcore/StatusCode.h"

#define DEFAULT_RLEN 512
#define DEFAULT_QLEN 512
#define DEFAULT_BITSLEN 512
#define DEFAULT_ORDER 512
#define DEFAULT_D 9563

#define DEFAULT_GROUP_SIZE 696

using namespace std;
namespace dev
{
namespace eth
{
    class LinearPair
    {
     public:
        //shared_ptr<pbc_param_s> param_ptr; //包含api&& data(包含参数)
        pbc_param_t param_ptr;
        pbc_cm_t p_cm;
        shared_ptr<ConfigParserInterface<JsonConfigParser>> pconfig_interface;
        bool pbc_param_inited;
        //配置对象，用于加载线性对参数(qbits, rbits, bits等)
        explicit LinearPair(shared_ptr<ConfigParserInterface<JsonConfigParser>>& config): 
            pconfig_interface(config), pbc_param_inited(false){}
       
        virtual ~LinearPair(){}

        virtual void set_inited()
        {  pbc_param_inited = true;   }
        //加载pbc param信息
        virtual pbc_param_ptr load_pbc_param(std::string& pbc_param_str, 
                int& ret_code)
        {
            assert( pbc_param_str != "" );
            if( pbc_param_str == "")
            {
                ret_code = RetCode::GroupSigStatusCode::INTERNAL_INVALID_PBC_PARAM; 
                return NULL;
            }
            LOG(DEBUG)<<"LOAD PBC PARAM STR: "<<pbc_param_str<<" to pbc param";
            pbc_param_init_set_str(param_ptr, pbc_param_str.c_str());
            ret_code = RetCode::CommonStatusCode::SUCCESS;
            return param_ptr;
        }

        virtual pbc_param_ptr get_pbc_param()
        { return param_ptr;}

        virtual pbc_cm_ptr get_pbc_cm()
        {
            assert(p_cm);
            return p_cm;
        }
       
       //初始化pbc_param
       virtual pbc_param_ptr init_pbc_param(){
           LOG(DEBUG)<<"INIT INTERFACE PBC_PARAM";
           return get_pbc_param();
       };
      
    };

    /*A类型线性对参数
     * r = 2^exp2 + sign1 * 2^exp1 + sign0 * 1 (Solinas prime)
     * q, h: r * h = q + 1
     * q is a prime, h is a multiple of 12 (thus q = -1 mod 12)
     */
    class ALinearPair: public LinearPair
    {
     public:
        //r和q位数
        int r_bits;
        int q_bits;
        string _q_key;
        string _r_key;
        //调用PBC库生成参数
        ALinearPair(shared_ptr<ConfigParserInterface<JsonConfigParser>>& config,
                string q_key="q_bits_len", string r_key="r_bits_len"):
                LinearPair(config),
                _q_key(q_key), _r_key(r_key)
        {
            q_bits = DEFAULT_QLEN;
            r_bits = DEFAULT_RLEN;
            //从配置中加载q和r信息
            if(pconfig_interface)
            {
                pconfig_interface->get_value(q_key, q_bits,DEFAULT_QLEN);
                pconfig_interface->get_value(r_key, r_bits, DEFAULT_RLEN);
            }
            LOG(DEBUG)<<"ALinearPair, q_bits="<<q_bits<<" r_bits="<<r_bits;
        }
            
        //初始化pbc param
        pbc_param_ptr init_pbc_param() override
        {
            LOG(DEBUG)<<"INIT PBC PARAM";
            pbc_param_init_a_gen(get_pbc_param(), r_bits, q_bits);
            return get_pbc_param(); 
        }
    };
    
    //A1类型线性对参数
    class AOneLinearPair: public LinearPair
    {
     public:
        int order;
        string _order_key;
        AOneLinearPair(shared_ptr<ConfigParserInterface<JsonConfigParser>>& config,
                string order_key="order"):LinearPair(config), _order_key(order_key)
            
        {
            order = DEFAULT_ORDER;
            if(pconfig_interface)
            {
                pconfig_interface->get_value<int>(_order_key, order);
            }
            LOG(DEBUG)<<"AOneLinearPair, order_str="<<order;
            
        }
        
        //初始化pbc参数
        pbc_param_ptr init_pbc_param() override
        {
            LOG(DEBUG)<<"INIT PBC PARAM";
            mpz_t p, q, N;
            mpz_init(p);
            mpz_init(q);
            mpz_init(N);
            pbc_mpz_randomb(p, order);
            pbc_mpz_randomb(q, order);
            mpz_nextprime(p, p);
            mpz_nextprime(q, q);
            mpz_mul(N, p, q);
            pbc_param_init_a1_gen(get_pbc_param(), N);
            mpz_clear(p);
            mpz_clear(q);
            mpz_clear(N);
            return get_pbc_param();
        }

    };
    
    /* D类型线性对参数
     * q: F_q is the base field
     * n: # of points in E(F_q)
     * r: large prime dividing n
     * h: n = h * r
     * a: E: y^2 = x^3 + ax + b
     * nk: # of points in E(F_q^k)
     * hk: nk = hk * r * r
     * coeff0: coefficients of a monic cubic irreducible over F_q
     * nqr: quadratic nonresidue in F_q
     */
    class DLinearPair: public LinearPair
    {
     public:
        //cm info
        //pbc_cm_ptr p_cm;
        //cm info(q, n, h, r, D, k)
        //d_param_s:q, n, h, r, a, b, k, n_k, h_k, coeff_0
        //coeff_1, coeff_2 
        int D;
        DLinearPair(shared_ptr<ConfigParserInterface<JsonConfigParser>>& config,
                    int D_value = DEFAULT_D, string D_key="D"):
                    LinearPair(config),D(D_value)
        {
            if(pconfig_interface)
            {
                pconfig_interface->get_value<int>(D_key, D);
            }
            LOG(DEBUG)<<"D VALUE:"<<D;
            int m = D % 4;
            if(D<=0 || m==1 || m == 2)
            {
                LOG(ERROR)<<"D must be  0 or 3 mod 4 and positive, reset TO DEFAULT";
                D = DEFAULT_D;
            }
        }

        /*int generate(pbc_cm_t cm, void *data){
            pbc_param_init_d_gen(param, cm);

        }*/
        //初始化pbc参数
        pbc_param_ptr init_pbc_param() override
        {
            LOG(DEBUG)<<"INIT D PARAM";
           /*if (!pbc_cm_search_d(generate_d, NULL, D, 500)){
               pbc_die("no suitable curves for this D");
            }*/
            return get_pbc_param();
        }
    };
    
    /* E类型线性对参数
     * r = 2^exp2 + sign1 * 2^exp1 + sign0 * 1 (Solinas prime)
     * q = h r^2 + 1 where r is prime, and h is 28 times a perfect square
     * E: y^2 = x^3 + ax + b
     */ 
    class ELinearPair: public LinearPair
    {
     public:
        int r_bits;
        int q_bits;
        string _r_key;
        string _q_key;
        ELinearPair(shared_ptr<ConfigParserInterface<JsonConfigParser>>& config,
                string r_key="r_bits_len",
                string q_key="q_bits_len"):
                LinearPair(config),
                _r_key(r_key), _q_key(q_key)
        {
            LOG(DEBUG)<<"create E LINEAR PAIR";
            r_bits = DEFAULT_RLEN;
            q_bits = DEFAULT_QLEN;
            if(pconfig_interface)
            {
                LOG(DEBUG)<<"set rkey and q_key";
                pconfig_interface->get_value(_r_key, r_bits);
                pconfig_interface->get_value(_q_key, q_bits);
            }
            LOG(DEBUG)<<"ELinear pair, r_bits="<<r_bits<<" q_bits="<<q_bits;
        }
        
        //初始化pbc参数
        pbc_param_ptr init_pbc_param() override
        {
            pbc_param_init_e_gen(get_pbc_param(), r_bits, q_bits);
            return get_pbc_param();
        }
    };

    /* F类型线性对参数
     * q: The curve is defined over Fq
     * r: The order of the curve.
     * b: y^2= x^3 + b
     * beta: A quadratic nonresidue in Fq: used in quadratic extension.
     * alpha0, alpha1:
     * x^6 + alpha0 + alpha1 sqrt(beta) is irreducible: used in sextic extension.
     */
    class FLinearPair: public LinearPair
    {
     public:
        int bits;
        string _bits_key;
        FLinearPair(shared_ptr<ConfigParserInterface<JsonConfigParser>>& config,
                  string bits_key = "bit_len"): LinearPair(config), _bits_key(bits_key)
        {
            bits=DEFAULT_BITSLEN;
            if(pconfig_interface)
            {
                pconfig_interface->get_value(_bits_key, bits);
            }
            LOG(DEBUG)<<"FLinearPair, bits="<<bits;
        }
        
        pbc_param_ptr init_pbc_param() override
        {
            pbc_param_init_f_gen(get_pbc_param(), bits);
            return get_pbc_param();
        }
    };

    /* G类型线性对参数
     */
    class GLinearPair: public LinearPair
    {
     public:
        GLinearPair(shared_ptr<ConfigParserInterface<JsonConfigParser>>& config):
                LinearPair(config)
        {}

        pbc_param_ptr init_pbc_param() override
        {
            //初始化线性对
            pbc_param_init_g_gen(get_pbc_param(), get_pbc_cm());
            return get_pbc_param();
        }
    };

    /* I类型线性对参数
     * m, t: The ternary extension field is F(3)[x]/(x^m^ + x^t^ + 2).
     * n: the order of G1
     * n2: n * n2 = number of points in E(F_{3^m^})
     */ 
    class ILinearPair: public LinearPair
    {
     public:
        int group_size;
        string _group_key;
        ILinearPair(shared_ptr<ConfigParserInterface<JsonConfigParser>>& config,
                std::string group_key="group_size"):LinearPair(config),_group_key(group_key)
        {
            group_size = DEFAULT_GROUP_SIZE;
            if(pconfig_interface)
                pconfig_interface->get_value(_group_key, group_size);
            LOG(DEBUG)<<"default group size = "<<group_size;
        }

        pbc_param_ptr init_pbc_param()override
        {
           pbc_param_init_i_gen(get_pbc_param(), group_size);
           return get_pbc_param();
        }
    };


    class LinearPairFactory
    {
      public:
       static shared_ptr<LinearPair> create(
             shared_ptr<ConfigParserInterface<JsonConfigParser>>& pconfig,
             const string linear_type_key="linear_type")
         {
           string linear_type_str; 
           if(pconfig)
               pconfig->get_value(linear_type_key, linear_type_str); //默认的线性对类型            
           LOG(DEBUG)<<"LINEAR TYPE IS:"<<linear_type_str;
           LinearPairType linear_type;
           trans_to_linear_type(linear_type, linear_type_str);
           LinearPair* linear_obj = NULL;

           switch(linear_type)
           {
               case A_LINEAR:
                   linear_obj = new ALinearPair(pconfig);
                   break;
               case A_ONE_LINEAR:
                   linear_obj = new AOneLinearPair(pconfig);
                   break;
               case D_LINEAR:
                   LOG(DEBUG)<<"CREATE D TPYPE LINEAR";
                   linear_obj = new DLinearPair(pconfig);
                   break;
               case E_LINEAR:
                   LOG(DEBUG)<<"CREATE E LINEAR PAIR";
                   linear_obj = new ELinearPair(pconfig);
                   break;
               case F_LINEAR:
                   linear_obj = new FLinearPair(pconfig);
                   break;
               case G_LINEAR:
                   linear_obj = new GLinearPair(pconfig);
                   break;
               default:
                   LOG(ERROR)<<"UNKOWN LINEAR TYPE";
                   linear_obj = NULL;
           }
           return shared_ptr<LinearPair>(linear_obj);
       }
    };
}
}
