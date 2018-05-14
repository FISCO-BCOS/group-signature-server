# 群签名&&环签名RPC服务接口

## 目录
<!-- TOC -->
- [1. 返回码说明](#1-返回码说明)
- [2. 群签名RPC服务接口](#2-群签名rpc服务接口)
    - [2.1 群签名RPC服务基础接口](#21-群签名rpc服务基础接口)
        - [创建群签名](#创建群签名)
        - [加入群](#加入群)
        - [生成群签名](#生成群签名)
        - [群签名验证](#群签名验证)
        - [追踪签名者信息](#追踪签名者信息)
    - [2.2 群签名RPC服务get接口](#22-群签名rpc服务get接口)
        - [获取群公钥](#获取群公钥)
        - [群主获取私钥](#群主获取私钥)
        - [群成员获取私钥](#群成员获取私钥)
- [3. 环签名RPC服务接口](#3-环签名rpc服务接口)
    - [3.1 环签名RPC服务基础接口](#31-环签名rpc服务基础接口)
        - [初始化环](#初始化环)
        - [加入环,获取公私钥对](#加入环获取公私钥对)
        - [环签名](#环签名)
        - [环签名验证](#环签名验证)
        
    - [3.2 环签名RPC服务get接口](#32-环签名rpc服务get接口)
        - [获取环参数](#获取环参数)
        - [获取指定公钥](#获取指定公钥)
        - [获取私钥](#获取私钥)
   

<!-- /TOC -->

<br>
<br>

## 1. 返回码说明

RPC的返回码定义在devcore/StatusCode.h, 该RPC的返回码如下：

| <div align = left>返回码</div>| <div align = left>返回码含义</div>|
| ---- | ---- |
| 0     | RPC方法调用成功|
| 10001     | UNKOWN_ALGORITHM，RPC requests参数指定的群算法类型未知（目前只支持BBS04算法，以后会支持更多算法）     |
| 10002     | OBTAIN_ALGORITHM_FAILED， 获取群签名算法类型失败     |
| 10004     | DB_ACCESS_FAILED，访问秘钥托管数据库失败     |
| 10005     | INTERNAL_PARAM_PARSE_FAILED，群签名算法内部返回码，传入的参数无效     |
| 10006     | INTERNAL_PARAM_INIT_FAILED，群签名算法内部返回码，初始化群签名系统参数失败     |
| 10007     | INTERNAL_GPK_INIT_FAILED，群签名算法内部返回码，初始化群公钥信息失败     |
| 10008     | INTERNAL_GMSK_INIT_FAILED，群签名算法内部返回码，初始化群主私钥失败     |
| 10009     | INTERNAL_GSK_INIT_FAILED，群签名算法内部返回码，初始化群成员私钥失败     |
| 10010    |  INTERNAL_GPK_FREE_FAILED，群签名算法内部返回码，算法调用结束时，释放群公钥数据所占内存资源失败    |
| 10011     | INTERNAL_GMSK_FREE_FAILED，群签名算法内部返回码，算法调用结束时，释放群主私钥数据所占内存资源失败     |
| 10012     | INTERNAL_GSK_FREE_FAILED，群签名算法内部返回码，算法调用结束时，释放群成员私钥数据所占内存资源失败     |
| 10013     | OPEN_CERT_FAILED，群签名算法内部返回码，群主追踪签名者信息失败     |
| 10014     | LOAD_KEY_FAILED，群签名算法内部返回码，加载秘钥信息失败     |
| 10015     | STORE_KEY_FAILED，群签名算法内部返回码，将秘钥结构转换成字符串失败     |
| 10016     | INTERNAL_INVALID_PBC_PARAM，群签名算法内部返回码，传入的pbc_param参数无效     |
| 10200     | INVALID_MEMBER_POS，环签名时，传入的环成员位置无效（必须>1）     |
| 10201     | INIT_PARAM_INVALID_PRIME, 环签名算法内部返回码, 初始化环参数时，生成无效的大素数     |
| 10202     | CORRUPTED_PARAM，环签名算法内部返回码，调用环签名算法时，传入的环参数无效     |
| 10203     | CORRUPTED_KEY，环签名算法内部返回码，调用环签名算法时，传入的秘钥无效     |
| 10204     | STORE_PARAM_FAILED，存储环参数失败     |
| 10205     | STORE_KEY_FAILED，存储公钥或私钥信息失败    |
| 10206     | PK_NUM_MISMATCH，环签名算法内部返回码，验证环签名时，S参数个数与公钥参数数目不匹配，即：环签名无效     |
| 10207     | KEY_STORE_FAILED，环签名算法内部返回码，存储环签名信息失败     |
| 10208     | KEY_LOAD_FAILED，环签名算法内部返回码，环签名验证时，加载签名信息失败     |
| 10209     | CORRUPTED_SIG， 环签名算法内部返回码，环签名无效     |
| 10210     | RING_SIZE_TOO_BIG，设定的环过大，超过总的环成员数目     |
| 10211     | RING_SIG_VERIFY_FAILED, 环签名算法内部返回码，环签名验证失败     |
| 10212     | INVALID_RING_SIZE, 环签名算法内部返回码，设定的环大小无效     |
| 10301     | KEY_EXIST, 数据库内部返回码，表明关键字已经存在     |
| 10302     | KEY_NOT_EXIST，数据库内部返回码，表明关键字不存在     |
| 10303     | DB_CORRUPTION，数据内部返回码，表明数据库数据被损坏     |

<br>

[返回目录](#目录)

<br>


## 2. 群签名RPC服务接口

### 2.1 群签名RPC服务基础接口

### 创建群签名

- **Requests**

```bash
{
    "jsonrpc": "2.0",
    "params": {"group_name":"name of group", 
        "gm_pass": "password of group manager",
        "pbc_param": "optional, specified type of pbc linear type, default is A type linear pair"
    },
    "id": 1,
    "method": "create_group"

}

#example of pbc_param:
A type linear pair: {\"linear_type\":\"a\", \"q_bits_len\": 256, \"r_bits_len\":256}
A1 type linear pair: {\"linear_type\":\"a_one\", \"order\":512}
E type linear pair: {\"linear_type\":\"e\", \"q_bits_len\": 512, \"r_bits_len\":512}
F typer linear pair: {\"linear_type\":\"f\", \"bit_len\": 256}
```
- **请求方法：create_group**

- **请求参数**

| <div align = left>参数</div>         | <div align = left>说明</div>   |
| ---------- | ---- |
| group_name | required，群名称      |
| gm_pass    | required, 群主访问RPC服务的口令     |
| pbc_param  | optional，PBC参数，用于指定创建群的线性对类型，默认是A类型线性对; <br> pbc_param示例: <br> A类型线性对: {\"linear_type\":\"a\", \"q_bits_len\": 256, \"r_bits_len\":256} <br> A1类型线性对:{\"linear_type\":\"a_one\", \"order\":512} <br> E类型线性对:{\"linear_type\":\"e\", \"q_bits_len\": 512, \"r_bits_len\":512} <br> F类型线性对: {\"linear_type\":\"f\", \"bit_len\": 256}     |
| algorithm_method | optional, 群签名采用的算法，默认是[bbs04](#)群签名算法，目前仅支持bbs04群签名算法，以后会增加更多群签名算法支持 |

- **Response Result**

```bash
#case1：创建群成功
{
    "id":1,
    "jsonrpc": "2.0", 
    "result": "gpk_info"
}
#case2: 创建群失败: 返回错误码和出错原因
{
    "id": 1,
    "jsonrpc": "2.0",
    "result": {
        "details": "failed reason of create group",
        "ret_code": "return code"
    }
}
```

- **返回参数**

result: 创建生成的群公钥信息

<br>

[返回目录](#目录)

<br>


### 加入群

- **Requests**

```bash
{
    "id": 1,
    "jsonrpc": "2.0",
    "method": "join_group"
    "params": {
        "group_name": "required, name of group to join",
        "member_name": "required, specified id/name of joined member",
        "pass": "optional, passwd of member to access RPC(reserved now, without any login authentication)"
    }
}
```

- **请求方法: join_group**
- **请求参数**

| 参数         | 说明   |
| ---------- | ---- |
| group_name | required，要加入的群名称 |
| member_name | required, 加入的成员名称 |
| pass | optional, 成员访问RPC服务的口令（目前RPC没有做身份认证，可基于该版本添加口令认证和访问控制功能） |

- **Response Result**

```bash
#case1: 加入群成功
{
    "id": 1,
    "jsonrpc": "2.0",
    "result" : {
        "details": "success", 
        "result": "private key of the member joined",
        "ret_code": 0
    }
}
#case2: 加入群失败
{
    "id": 1,
    "jsonrpc": "2.0",
    "result": {
        "details": "reasons of joining group failed",
        "ret_code": "return code"
    }
}
```

- **返回参数**

| <div align = left>参数</div>         | <div align = left>说明</div>   |
| ---------- | ---- |
| details | 说明信息，若成员加入成功，返回success；否则，返回具体的出错信息 |
| result | 若成员加入成功，返回新加入成员私钥；若成员加入失败，没有该字段 |
| ret_code| 返回码，0表示成员加入成功，其他返回码对应含义参考[1. 返回码说明](#1-返回码说明) |

<br>

[返回目录](#目录)

<br>
<br>

### 生成群签名

- **Requests**
```bash
{
    "id": 1,
    "jsonrpc": "2.0",
    "method": "group_sig",
    "params": {
        "group_name": "required, group name of the generated signature belongs to",
        "member_name": "required, name of the member that requests for group signature",
        "message": "required, plain text to generate group signature"
    }
}
```

- **请求方法: group_sig**
 

- **请求参数**
 
| <div align = left>参数</div>         | <div align = left>说明</div>   |
| ---------- | ---- |
| group_name | required, 产生的群签名所属的群  |
| member_name | required, 请求RPC服务生成群签名的群成员 |
| message | required, 群签名对应的明文 |


- **Response Result**

```bash
#case1: 产生群签名成功
{
    "id": 1,
    "jsonrpc": "2.0",
    "result": {
        "sig": "group signature",
        "gpk": "public key of group",
        "pbc_param": "pbc param information of the group signature",
        "message": "plain text of the group signature",
        "ret_code": 0
    }
}

#case2: 产生群签名失败
{
    "id": 1,
    "jsonrpc": "2.0",
    "result": {
        "details": "detailed reason of generating group signature failed",
        "ret_code": "return code"
    }
}
```

- **返回参数**
| <div align = left>参数</div>         | <div align = left>说明</div>   |
| ---------- | ---- |
| details | 若产生群签名成功，返回success；否则返回具体失败原因|
| sig |产生群签名成功情况下，存在该字段，存储群签名信息|
| gpk |产生群签名成功情况下，存在该字段，存储群公钥信息|
| pbc_param |产生群签名成功情况下，存在该字段，存储群签名所在组的pbc参数信息|
| message |产生群签名成功情况下，存在该字段，存储签名对应的明文信息|
| ret_code|返回码，产生群签名成功返回0；否则返回其他错误码，各个返回码详细说明参考[1. 返回码说明](#1-返回码说明)|
 
<br>

[返回目录](#目录)

<br>
<br>

### 群签名验证

- **Requests**

```bash
{
    "id": 1,
    "jsonrpc": "2.0",
    "method": "group_verify",
    "params":{
        "group_name": "group of the signature belongs to",
        "group_sig" : "group signature to be verified",
        "message": "plain-text of the group signature"
    }
}
```

- **请求方法: group_verify**

- **请求参数**

| <div align = left>参数</div>         | <div align = left>说明</div>   |
| ---------- | ---- |
| group_name | required, 群签名所属的群|
| group_sig |required, 群签名信息|
| message |required, 群签名对应的明文信息|

- **Response Result**

```bash
#case1: 调用群签名验证接口成功
{
    "id": 1,
    "jsonrpc": "2.0",
    "result": {
        "ret_code": 0, 
        "details": "success",
        "result": "true or false"
    }
}
#case2：调用群签名验证接口失败
{
    "id": 1,
    "jsonrpc": "2.0",
    "result": {
        "ret_code": "return code",
        "details": "failed reason of callback group verify failed"
    }
}
```

- **返回参数**

| <div align = left>参数</div>         | <div align = left>说明</div>   |
| ---------- | ---- |
| ret_code | 返回码，0表示调用群签名验证接口成功；其他值表明调用群签名验证接口失败，返回码详细说明参考[1. 返回码说明](#1-返回码说明) |
| details | 若调用群签名验证接口成功，返回success; 否则返回调用失败的原因 |
| result | 群签名验证通过，返回true，否则返回false |

<br>

[返回目录](#目录)

<br>
<br>

### 追踪签名者信息

- **Requests**

```bash
{
    "id": 1,
    "jsonrpc": "2.0",
    "method":"open_cert",
    "params":{
        "group_name": "group of the signature belongs to",
        "group_sig":"signature to be opened by the group manager",
        "message":"plain-text of the group signature",
        "gm_pass":"password of the group manager"
    }
}
```

- **请求方法: open_cert**
- **请求参数**

| <div align = left>参数</div>         | <div align = left>说明</div>   |
| ---------- | ---- |
| group_name | required, 要追踪的签名所属的群 |
| group_sig | required, 要追踪签名者的群签名信息 |
| message |  required, 要追踪签名者的群签名对应的明文信息 |
| gm_pass |  required, 群主访问RPC服务的口令，可随意填写（当前版本没有对该字段做校验，该字段预留，可基于本版本完善该功能）|


- **Response Result**

```bash
# case1: 调用open_cert接口成功
{
    "id": 1,
    "jsonrpc": "2.0",
    "result": 
    {
        "ret_code":"0",
        "details": "success",
        "result":"cert of the owner of the signature"
    }
}

# case2: 调用open_cert接口失败
{
    "id": 1,
    "jsonrpc": "2.0",
    "result": 
    {
        "ret_code": "return code",
        "details": "failed reason of tracing the owner of the specified signature"
    }
}
```

- **返回参数**

| <div align = left>参数</div>         | <div align = left>说明</div>   |
| ---------- | ---- |
| ret_code | 返回码，若获取签名者证书成功，返回0；否则返回其他返回码，返回码具体含义参考[1. 返回码说明](#1-返回码说明)|
| details | 返回信息，若获取签名者证书信息成功，返回success；否则返回调用open_cert接口失败的原因 |
| result |若获取签名者证书成功，返回证书值；否则不含该字段|

<br>

[返回目录](#目录)

<br>
<br>

### 2.2 群签名RPC服务get接口

### 获取群公钥

- **Requests**

```bash
{
    "id": 1,
    "jsonrpc": "2.0",
    "method": "get_public_info",
    "params": {"group_name": "group of the required public key belongs to"}
}
```

- **请求方法: get_public_info**
- **请求参数**

| <div align = left>参数</div>         | <div align = left>说明</div>   |
| ---------- | ---- |
|  group_name | required，要查询的群公钥所属的群  |

- **Response Result**

```bash
#case 1: 调用获取群公钥信息接口get_public_info成功
{
    "id": 1,
    "jsonrpc": "2.0",
    "result":
    {
        "ret_code": 0,
        "details": "success",
        "result": "public key of the specified group"
    }
}

#case 2: 调用获取群公钥信息接口get_public_info失败
{
    "id": 1,
    "jsonrpc": "2.0",
    "result": 
    {
        "ret_code": 0,
        "details": "failed reason of obtaining public key of specified group"
    }
}
```

- **返回参数**

| <div align = left>参数</div>         | <div align = left>说明</div>   |
| ---------- | ---- |
| ret_code | 返回码，若获取指定群公钥信息成功，返回0；否则返回其他返回码，返回码具体含义可参考[1. 返回码说明](#1-返回码说明)|
| details | 说明信息，若获取群公钥信息成功，为"success"；否则为具体的访问出错信息 |
| result | 若获取群公钥信息成功，存储群公钥信息(16进制字符串)；否则，response中不含该字段 |

<br>

[返回目录](#目录)

<br>
<br>

### 群主获取私钥

- **Requests**

```bash
{
    "id": 1,
    "jsonrpc": "2.0",
    "method": "get_gmsk_info",
    "params": {"group_name": "group of the group manager belongs to",
        "gm_pass": "password of the group manager to access rpc service"
    }
}
```

- **请求方法: get_gmsk_info**
- **请求参数**

| <div align = left>参数</div>         | <div align = left>说明</div>   |
| ---------- | ---- |
| group_name | required, 群主所属的群 |
| gm_pass |  required, 群主访问RPC服务的口令，可随意填写（当前版本没有对该字段做校验，该字段预留，可基于本版本完善该功能）|


- **Response Result**

```bash
# case1: 群主获取私钥信息成功
{
    "id": 1,
    "jsonrpc": "2.0",
    "result":
    {
        "ret_code": "0",
        "details": "success",
        "result": "private key of the group manager"
    }
}

# case2: 群主获取私钥信息失败
{
    "id": 1,
    "jsonrpc": "2.0",
    "result": 
    {
        "ret_code": "return code",
        "details": "reasons of obtaining private key for the group manager failed"
    }
}
```

- **返回参数**

| <div align = left>参数</div>         | <div align = left>说明</div>   |
| ---------- | ---- |
| ret_code | 返回码，若为0表明群主获取私钥成功；其他返回码表明群主获取私钥失败，返回码具体含义可参考[1. 返回码说明](#1-返回码说明)|
| details | 说明信息，若群主获取私钥成功，为"success"；否则说明具体出错原因|
| result | 若群主获取私钥成功，返回私钥信息；否则无该字段|

<br>

[返回目录](#目录)

<br>
<br>

### 群成员获取私钥

- **Requests**

```bash
{
    "id": 1,
    "jsonrpc": "2.0",
    "method": "get_gsk_info",
    "params": {"group_name":"group of the member belongs to",
        "member_name":"id of the member",
        "pass":"password of the member to access rpc service"
    }
}
```

- **请求方法: get_gsk_info**
- **请求参数**

| <div align = left>参数</div>         | <div align = left>说明</div>   |
| ---------- | ---- |
| group_name | required, 群成员所属的群 |
| member_name | required, 群成员名 |
| gm_pass |  required, 群成员访问RPC服务的口令，可随意填写（当前版本没有对该字段做校验，该字段预留，可基于本版本完善该功能）|

- **Response Result**
 
```bash
#case 1: 群成员获取私钥信息成功
{
    "id": 1,
    "jsonrpc": "2.0",
    "result":
    {
        "ret_code": "0",
        "details": "success",
        "result": "private key of the group member"
    }
}

#case 2: 群成员获取私钥信息失败
{
    "id": 1,
    "jsonrpc": "2.0",
    "result": 
    {
        "ret_code": "return code", 
        "details": "failed reason of obtaining the private key for the group member"
    }
}
```

- **返回参数**

| <div align = left>参数</div>         | <div align = left>说明</div>   |
| ---------- | ---- |
| ret_code | 返回码，若群成员获取私钥信息成功，返回0；否则返回其他非0错误码，具体错误码信息参考 [1. 返回码说明](#1-返回码说明)|
| details | 说明信息，若群成员获取私钥成功，为"success"；否则为具体错误信息 |
| result | 若群成员获取私钥成功，返回私钥；否则，response中无该字段|

<br>

[返回目录](#目录)

<br>
<br>

## 3. 环签名RPC服务接口

### 3.1 环签名RPC服务基础接口

### 初始化环

- **Requests**

```bash
{
    "id": 1,
    "jsonrpc": "2.0",
    "method": "setup_ring",
    "params": "{"ring_name": "ring to be set up",
        "bit_len": "param length of the ring(related to security)"
    }"
}
```

- **请求方法: setup_ring**
- **请求参数**

| <div align = left>参数</div>         | <div align = left>说明</div>  |
| ---------- | ---- |
| ring_name | required, 要初始化的环名称 |
| bit_len | optional, 环参数 && 环成员公私钥对长度，与签名安全性紧密相关，默认位1024|

- **Response Result**

```bash
# case1: 初始化环成功
{
    "id": 1,
    "jsonrpc": "2.0",
    "result":
    {
        "ret_code": "0",
        "details": "success",
        "result": "param information of the ring"
    }
}

# case2: 初始化环失败
{
    "id": 1,
    "jsonrpc": "2.0",
    "result": 
    {
        "ret_code": "return code",
        "details":
    }
}
```

- **返回参数**

| <div align = left>参数</div>         | <div align = left>说明</div>   |
| ---------- | ---- |
|ret_code| 返回码，0表明初始化环成功；其他返回码表明初始化环参数失败，返回码具体含义可参考[1. 返回码说明](#1-返回码说明)|
|details| 详细说明信息，若初始化环参数成功，返回"success"；否则返回具体出错原因|
| result | 若初始化环成功，返回环参数信息；否则不返回该字段|

<br>

[返回目录](#目录)

<br> 
<br>

### 加入环,获取公私钥对

- **Requests**

```bash
{
    "id": 1,
    "jsonrpc": "2.0",
    "method": "join_ring",
    params: {"ring_name": "ring to join"}
}
```

- **请求方法: join_ring**
- **请求参数**

| <div align = left>参数</div>         | <div align = left>说明</div>   |
| ---------- | ---- |
|  ring_name | required, 环成员要加入的环大小 |

- **Response Result**

```bash
#case1: 加入环成功
{
    "id": 1,
    "jsonrpc": "2.0",
    "result":
    {
        "ret_code": "0",
        "details": "success",
        "private_key": "generated private key for the joined member",
        "public_key": "generated public key for the joined member"
    }
}

#case2: 加入环失败
{
    "id": 1,
    "jsonrpc": "2.0",
    "result": 
    {
        "ret_code": "return code",
        "details": "failed reason of joining ring"
    }
}
```

- **返回参数**

| <div align = left>参数</div>         | <div align = left>说明</div>   |
| ---------- | ---- |
| ret_code | 返回码，若成员加入环成功，返回0；否则返回其他返回码，返回码具体含义可参考[1. 返回码说明](#1-返回码说明)|
| details | 说明信息，若成员加入环成功，输出success; 否则输出出错信息 |
| private_key | 新加入的环成员私钥 |
| public_key | 新加入的环成员的公钥 |

<br>

[返回目录](#目录)

<br>
<br>

### 环签名

- **Requests**

```bash
{
    "id": 1,
    "jsonrpc": "2.0",
    "method": "linkable_ring_sig",
    "params": {"ring_name": "ring of the generated signature belongs to" ,
        "message": "plain-text of the ring signature"
    }
}
```

- **请求方法: linkable_ring_sig**
- **请求参数**

| <div align = left>参数</div>         | <div align = left>说明</div>   |
| ---------- | ---- |
| ring_name | required, 产生环签名的环 |
| message | required, 环签名对应的明文 |

- **Response Result**
 
```bash
#case1: 为指定位置的环成员生成环签名成功
{
    "id": 1,
    "jsonrpc": "2.0",
    "result":
    {
        "ret_code": "0",
        "param_info": "param information of the specified ring",
        "sig":"generated ring signature",
        "message": "plan-text of the generated ring signature"
    }
}

#case2: 为指定位置的环成员生成环签名失败
{
    "id": 1,
    "jsonrpc": "2.0",
    "result": 
    {
        "ret_code": "return code",
        "details": "failed reason of generating ring signature"
    }
}
```

- **返回参数**

| <div align = left>参数</div>         | <div align = left>说明</div>   |
| ---------- | ---- |
| ret_code | 返回码，若为指定成员生成环签名成功，返回0；否则返回其他返回码，返回码具体含义可参考[1. 返回码说明](#1-返回码说明)|
| details | 生成环签名失败时，包含该字段，输出生成环签名失败的原因|
| param_info |环参数信息|
| sig |生成的环签名信息|
| message|环签名对应的明文信息|

<br>

[返回目录](#目录)

<br>
<br>

### 环签名验证

- **Requests**

```bash
{
    "id": 1,
    "jsonrpc": "2.0",
    "method": "linkable_ring_verify",
    "params": {"ring_name": "ring of the signature belongs to",
        "sig": "ring signature",
        "message": "plain-text of the signature"
    }
}
```

- **请求方法: linkable_ring_verify**
- **请求参数**

| <div align = left>参数</div>         | <div align = left>说明</div>   |
| ---------- | ---- |
| ring_name | required, 签名所属的环  |
| sig | required, 环签名信息|
| message | required, 环签名对应的明文|

- **Response Result**

```bash
#case1: 调用环签名验证接口成功
{
    "id": 1,
    "jsonrpc": "2.0",
    "result":
    {
        "ret_code": "0",
        "details": "success",
        "result": "0 or 1, 0 represents that verify invalid; 1 represents that verify valid"
    }
}

#case2: 调用环签名验证接口失败
{
    "id": 1,
    "jsonrpc": "2.0",
    "result": 
    {
        "ret_code": "return code",
        "details": "failed reasons of callback verify interface of ring signature"
    }
}
```

- **返回参数**

| <div align = left>参数</div>         | <div align = left>说明</div>   |
| ---------- | ---- |
| ret_code | 返回码，若调用环签名验证接口成功，返回0；否则返回其他返回码，返回码具体含义可参考[1. 返回码说明](#1-返回码说明) |
|details | 调用环签名验证接口成功，返回success; 否则返回出错原因|
| result | 调用环签名验证接口成功时，含有该字段: <br> 返回字符串0，表明环签名验证无效；返回字符串1表明环签名验证有效；|

<br>

[返回目录](#目录)

<br>
<br>

### 3.2 环签名RPC服务get接口

### 获取环参数

- **Requests**
 
```bash
{
    "id": 1,
    "jsonrpc": "2.0",
    "method": "get_ring_param",
    "params": { "ring_name": "ring of the param belongs to"}
}
```

- **请求方法: get_ring_param**
- **请求参数**

| <div align = left>参数</div>         | <div align = left>说明</div>   |
| ---------- | ---- |
|  ring_name | required, 要获取的参数所属的环名称  |

- **Response Result**

```bash
#case1: 获取指定环参数成功
{
    "id": 1,
    "jsonrpc": "2.0",
    "result":
    {
        "ret_code": "0",
        "details": "success",
        "result": "param information of specified ring"
    }
}

#case2: 获取指定环参数失败
{
    "id": 1,
    "jsonrpc": "2.0",
    "result": 
    {
        "ret_code": "return code",
        "details": "failed reasons of obtaining param information for specified ring"
    }
}
```

- **返回参数**

| <div align = left>参数</div>         | <div align = left>说明</div>   |
| ---------- | ---- |
| ret_code | 返回码:若获取环参数成功，返回0；否则返回其他返回码，具体返回码含义可参考[1. 返回码说明](#1-返回码说明) |
| details |描述信息: 若获取环参数成功，输出success; 否则返回获取环参数失败的原因 |
| result | 获取环参数成功时，含有该字段，存储环参数信息 |

<br>

[返回目录](#目录)

<br>
<br>

### 获取指定公钥

- **Requests**

```bash
{
    "id": 1,
    "jsonrpc": "2.0",
    "method": "get_public_key",
    "params": { "ring_name": "ring of which the public key belongs to", 
        "id":"position of the public key" }
}
```

- **请求方法: get_public_key**
- **请求参数**

| <div align = left>参数</div>         | <div align = left>说明</div>   |
| ---------- | ---- |
| ring_name |  要获取的公钥所属的环|
| id | 要获取的公钥在环中的位置 |


- **Response Result**

```bash
#case1: 环成员获取公钥信息成功
{
    "id": 1,
    "jsonrpc": "2.0",
    "result":
    {
        "ret_code": "0",
        "details": "success",
        "result": "public key of specified position"
    }
}
#case2: 环成员获取公钥信息失败
{
    "id": 1,
    "jsonrpc": "2.0",
    "result": 
    {
        "ret_code": "return code"
        "details": "failed reason of obtaining specified public key"
    }
}
```

- **返回参数**

| <div align = left>参数</div>         | <div align = left>说明</div>   |
| ---------- | ---- |
| ret_code | 返回码：若环成员获取公钥信息成功，返回0；否则返回其他非零错误码，错误码详细含义可参考[1. 返回码说明](#1-返回码说明)|
| details | 说明信息：若环成员获取公钥信息成功，返回success；否则返回环成员获取公钥信息失败的原因 |
| result | 若环成员获取公钥信息成功，返回指定环公钥；否则不含该字段 |

<br>

[返回目录](#目录)

<br>
<br>

### 获取私钥

- **Requests**
 
```bash
{
    "id": 1,
    "jsonrpc": "2.0",
    "method": "get_private_key",
    "params": {
        "ring_name": "ring of which the private key belongs to",
        "id": "position of the private key"}
    }
}
```

- **请求方法: get_private_key**
- **请求参数**

| <div align = left>参数</div>         | <div align = left>说明</div>   |
| ---------- | ---- |
| ring_name | 要获取的私钥所属的环 |
| id | 要获取的私钥在环中的位置 |
(注：当前版本没有做访问控制，为了更强安全性，在生产环境中使用该接口时，一定要加上口令验证功能，由于本项目是第一版，没有加口令验证功能，后续版本会考虑支持该特性)

- **Response Result**

```bash
#case1: 环成员获取私钥成功
{
    "id": 1,
    "jsonrpc": "2.0",
    "result":
    {
        "ret_code": "0",
        "details": "success",
        "result": "private key of the specified ring member"
    }
}

{
    "id": 1,
    "jsonrpc": "2.0",
    "result": 
    {
        "ret_code": "return code",
        "details": "failed reason of obtaining private key for the specified ring member failed"
    }
}
```

- **返回参数**

| <div align = left>参数</div>         | <div align = left>说明</div>   |
| ---------- | ---- |
| ret_code | 返回码：0表示环成员获取私钥成功；其他返回码表明环成员获取私钥失败，返回码具体含义可参考[1. 返回码说明](#1-返回码说明)|
| details | 说明信息：若环成员获取私钥成功，返回success；否则返回获取环成员私钥信息失败的原因 |
| result | 获取的环成员私钥信息|

 
<br>

[返回目录](#目录)

<br>
