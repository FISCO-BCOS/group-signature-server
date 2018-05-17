# 群签名&&环签名RPC服务操作手册
## 目录
<!-- TOC -->

- [1. 基本介绍](#1-基本介绍)
    - [1.1 群签名&&环签名rpc服务主要模块](#11-群签名环签名rpc服务主要模块)
    - [1.2 群签名&&环签名主要特性和流程](#12-群签名环签名主要特性和流程)
    - [1.3 场景](#13-场景)
    - [1.4 群签名 && 环签名RPC接口](#14-群签名环签名rpc接口)
- [2 群签名 && 环签名RPC服务部署](#2-群签名环签名rpc服务部署)
    - [2.1 部署依赖软件包](#21-部署依赖软件包)
    - [2.2 编译安装群签名&&环签名RPC服务](#22-编译安装群签名环签名rpc服务)
    - [2.3 使用说明](#23-使用说明)

<!-- /TOC -->

<br>
<br>

## 1. 基本介绍

群签名&&环签名rpc调用[群签名&&环签名算法库](doc/群签名_环签名算法接口.md)，实现了[BBS04群签名算法](http://crypto.stanford.edu/~dabo/abstracts/groupsigs.html)和[可连接的环签名算法](http://pdfs.semanticscholar.org/3c63/f7c90d79593fadfce16d54078ec1850bedc9.pdf)， 部署于机构内，用于提供群签名和环签名服务。

本节首先简单介绍群签名&&环签名rpc的主要模块及相关代码目录；接着介绍群签名&&环签名算法主要特性和流程，让大家对这两种算法有基本的认识；之后，简单介绍几种应用场景。

### 1.1 群签名&&环签名rpc服务主要模块

| <div align = left>模块 && 代码目录</div>                               | <div align = left>说明</div>                                       |
| ---------------------------------------- | ---------------------------------------- |
| 常用数据结构模块(devcore)                        | 定义了其他模块经常使用的基本数据结构，如配置解析、错误码、日志等         |
| 群签名算法模块(algorithm/bbs04/)<br>(群签名算法接口：algorithm/GroupSig.h) | BBS04群签名算法实现模块，bbs04是基于线性对的群签名算法，sig-service提供了多种线性对支持，用户可根据安全性需求自定义线性对 |
| 环签名算法模块(algorithm/ring-sig/)<br>(环签名算法接口：algorithm/RingSig.h) | 可连接的环签名算法实现模块，用户可根据安全性需求，自定义环大小          |
| rpc服务模块(httpserver)                      | 实现了群签名&&环签名rpc服务<br>(1) 群签名RPC服务主要在httpserver/GroupSigRpc.*中实现;<br>(2) 环签名RPC服务主要通过httpserver/RingSig.\*实现 |
| 数据库模块(database)                          | 存储签名参数信息，并提供秘钥托管服务，目前支持levelDB，但可方便地扩展到其他类型数据库 |
| 依赖软件安装脚本(script)                         | 调用该目录下的install_deps.sh和install.sh部署群签名&&环签名RPC的依赖软件 |

<br>

[返回目录](#目录)

<br>


### 1.2 群签名&&环签名主要特性和流程

**(1) 群签名和环签名主要特性**

| <div align = left>算法</div>      | <div align = left>特性</div>                                       |
| ------- | ---------------------------------------- |
| **群签名** | 1. **匿名性**：群成员用群参数产生签名，其他人仅可验证签名的有效性，并通过签名知道签名者所属群组，却无法获取签名者身份信息；<br>2. **可追踪性**: 在监管介入的场景中，群主可通过签名获取签名者身份. |
| **环签名** | 1. **完全匿名性**：其他人仅可验证环签名的有效性，无法获取签名者身份信息；<br>2. **不可追踪性**：无法追踪签名对应的签名者信息. |

<br>

**(2) 群签名主要流程**

| <div align = left>流程</div>      | <div align = left>说明</div>                                       |
| ------- | ---------------------------------------- |
| 生成群     | 生成群公钥(gpk)，群主私钥(gmsk)和群参数(可用不同线性对参数生成群，sig-service支持A， A1， E 和 F类型线性对，默认使用A类型线性对) |
| 加入群     | 群主为群成员产生私钥(gsk)和证书(cert)                 |
| 生成群签名   | 群成员用私钥和证书产生群签名                           |
| 群签名验证   | 其他人通过群公钥、群参数验证群签名信息的有效性（此时其他人仅知道签名者属于哪个群，但无法获取签名者身份信息） |
| 追踪签名者信息 | 在监管介入场景中，群主通过签名信息可获取签名者证书，从而追踪到签名者身份     |

<br>

**(3) 环签名主要流程**

| <div align = left>流程</div>         | <div align = left>说明</div>                                       |
| ---------- | ---------------------------------------- |
| 初始化环       | 生成环参数                                    |
| 为环成员产生公私钥对 | 成员加入环时，rpc服务为环成员产生公私钥对                   |
| 生成环签名      | 环成员使用私钥和其他环成员公钥产生匿名签名，环大小可由用户根据性能和安全性需求自定义指定（环越大，安全性越高，性能越低；环越小，安全性越低，性能越低，sig-service默认环大小为32） |
| 环签名验证      | 其他人通过环参数和产生环签名的公钥列表，验证环签名的有效性            |

<br>

[返回目录](#目录)

<br>



### 1.3 场景

群签名&&环签名RPC服务部署于可信机构内，提供群签名和环签名服务，该服务可与[群签名&&环签名客户端](https://github.com/FISCO-BCOS/sig-service-client) 结合使用，应用于区块链场景，也可以作为机构的签名服务，应用于其他场景。

下面列举群签名&&环签名在区块链中的应用场景：

**(1) 群签名场景**

**场景1( ToC )：拍卖、匿名存证等场景**

机构内成员（C端用户）或机构内下属机构通过机构将群签名信息上链，其他人在链上验证签名时，仅可获知签名所属的群组，却无法获取签名者身份，保证成员的匿名性和签名的不可篡改性；

**场景2(ToB)：竞标、对账、匿名征信等场景**

B端用户将生成的群签名通过AMOP发送给上链结构（如agency），上链机构将收集到的群签名信息统一上链，其他人验证签名时，无法获取签名者身份，保证成员的匿名性，监管可通过可信第三方追踪签名者信息，保证签名的可追踪性

<br>

**(2) 环签名场景**

**场景1：（匿名投票）**

机构内成员（C端用户）对投票信息进行环签名，并通过可信机构（如agency）将签名信息和投票结果写到链上，其他人可在链上验证签名时，仅可获取发布投票到链上的机构，却无法获取投票者身份信息

**场景2：（如匿名存证、征信）**

与群签名匿名存证、征信场景类似，唯一的区别是任何人都无法追踪签名者身份

**场景3：匿名交易**

在UTXO模型下，可将环签名算法应用于匿名交易，任何人都无法追踪转账交易双方；

<br>

[返回目录](#目录)

<br>
<br>


### 1.4 群签名 && 环签名RPC接口

群签名&&环签名RPC详细接口可参考[群签名&&环签名RPC接口文档](doc/rpc_interface.md)

<br>

[返回目录](#目录)

<br>
<br>

## 2. 群签名&&环签名RPC服务部署

本章主要介绍了如何部署和运行群签名&&环签名RPC服务。

### 2.1 部署依赖软件包

**(1) 安装基础依赖软件** 

部署群签名&&环签名RPC服务之前，要安装git, dos2unxi, lsof依赖软件：

- git：用于拉取最新代码
- dos2unix && lsof: 用于处理windows文件上传到linux服务器时，可执行文件无法被linux正确解析的问题；

可用如下命令安装这些基础依赖软件：

```bash
[centos]
sudo yum -y install git
sudo yum -y install dos2unix
sudo yum -y install java
sudo yum -y install lsof

[ubuntu]
sudo apt install git
sudo apt install lsof
sudo apt install tofrodos
ln -s /usr/bin/todos /usr/bin/unxi2dos
ln -s /usr/bin/fromdos /usr/bin/dos2unix
```

部署dos2unix后，调用format.sh脚本格式化可执行文件，使其可被linux系统正确解析：

```bash
# 格式化format.sh脚本
dos2unix format.sh
# 执行format.sh脚本格式化其他可执行文件，使其可被正确解析执行
bash format.sh
```

<br>

**(2) 安装levelDB、gmp等依赖软件**


群签名&&环签名rpc服务，需要安装levelDB, gmp等依赖软件，sig-service在script目录下提供了install_deps.sh脚本，执行以下命令安装这些依赖软件：

```bash
# 进入script目录 && 执行install_deps.sh脚本
cd script && sudo bash install_deps.sh
```
<br>

**(3) 安装群签名算法依赖软件pbc和pbc-sig**

群签名算法依赖pbc库和pbc-sig库，部署群签名&&环签名RPC服务前，首先要安装pbc和pbc-sig库，sig-service在script目录下提供了pbc和pbc-sig一键安装脚本install.sh，执行以下命令安装pbc和pbc-sig:

```bash
# 进入script目录，执行install.sh脚本安装pbc和pbc-sig
cd script && sudo bash install.sh
#或者 （注：执行下面命令前，需要先保证install.sh脚本可执行 ：chmod +x install.sh可使其可执行）
cd script && sudo ./install.sh
```

<br>

[返回目录](#目录)

<br>
<br>

### 2.2 编译安装群签名&&环签名RPC服务

```bash
# 编译sig-service
# 方法一: 使用compile脚本编译
cd sig-service && bash compile.sh
#或者: （注：执行下面命令前，需要先保证compile.sh脚本可执行 ：chmod +x compile.sh可使其可执行）
cd sig-service && ./compile.sh

# 方法二： 手动编译, 其中-j4表示用4个线程并发编译，用户可根据机器实际配置动态调整编译线程数
##（1）【Centos系统】编译后，会在build目录下生成rpc服务程序server
cd sig-service && mkdir -p build && cd build && cmake3 .. && make -j4

###（2）【Ubuntu系统】编译后，会在build目录下生成rpc服务程序server
cd sig-service && mkdir -p build && cd build && cmake .. && make -j4
```

<br>

[返回目录](#目录)

<br>
<br>


### 2.3 使用说明
**(1) 启动群签名&&环签名RPC服务**

```bash
#群签名&&环签名RPC服务使用方法：
[app@VM_105_81_centos build]$ ./server -h
group sig and ring sig RPC:
  -p [ --port ] arg            listen port of group sig and ring sig RPC
  -n [ --http_thread_num ] arg thread num of http server used to deal with 
                               requests
  -l [ --log_path ] arg        path of log configuration
  -s [ --ssl_cert ] arg        path of ssl certification
  -k [ --ssl_key ] arg         key to access ssl certification
  -h [ --help ]                help of group sig and ring sig rpc
```

各参数含义如下：

| <div align = left>参数</div>                    | <div align = left>说明</div>               | <div align = left>默认参数</div>       |
| --------------------- | --------------------- | ---------- |
| -p, --port            | rpc服务启动端口             | 8003       |
| -n, --http_thread_num | rpc服务启动的http线程数目      | 50         |
| -l, --log_path        | 配置文件路径                | "log.conf" |
| -s, --ssl_cert        | ssl证书路径（针对https服务）    | “”         |
| -k, --ssl_key         | 访问ssl证书的秘钥（针对https服务） | “”         |
| -h, --help            | 显示帮助信息                | -          |

<br>

**(2) 日志配置 log.conf**

sig-service目录下存着一个日志配置文件示例log.conf，其内容如下。

用户使用时，需要：

- 修改FILENAME，将其修改为用户自己的日志文件路径；
- 根据需求，修改日志打印选项，生产环境中，一般WARNING/ERROR/FATAL打开（设置选项值为true），其他选项关闭（设置选项值为false）；测试环境中，可打开DEBUG和TRACE

```bash
* GLOBAL:  
    ENABLED                 =   true  
    TO_FILE                 =   true  
    TO_STANDARD_OUTPUT      =   false  
    FORMAT                  =   "%level|%datetime{%Y-%M-%d %H:%m:%s:%g}|%msg"   
    FILENAME                =   "/data/sig-service/log/log_%datetime{%Y%M%d}.log"  
    MILLISECONDS_WIDTH      =   3   
    PERFORMANCE_TRACKING    =   false  
    MAX_LOG_FILE_SIZE       =   209715200 ## 200MB - Comment starts with two hashes (##)
    LOG_FLUSH_THRESHOLD     =   100  ## Flush after every 100 logs
    
* TRACE:  
    ENABLED                 =   false
    FILENAME                =   "/data/sig-service/log/trace_log_%datetime{%Y%M%d}.log"  
    
* DEBUG:  
    ENABLED                 =   true
    FILENAME                =   "/data/sig-service/log/debug_log_%datetime{%Y%M%d}.log"  

* FATAL:  
    ENABLED                 =   true  
    FILENAME                =   "/data/sig-service/log/fatal_log_%datetime{%Y%M%d}.log"
    
* ERROR:  
    ENABLED                 =   true
    FILENAME                =   "/data/sig-service/log/error_log_%datetime{%Y%M%d}.log"  
    
* WARNING: 
     ENABLED                 =   true
     FILENAME                =   "/data/sig-service/log/warn_log_%datetime{%Y%M%d}.log"
 
* INFO: 
    ENABLED                 =   true
    FILENAME                =   "/data/sig-service/log/info_log_%datetime{%Y%M%d}.log"  
    
* VERBOSE:  
    ENABLED                 =   true
    FILENAME                =   "/data/sig-service/log/verbose_log_%datetime{%Y%M%d}.log"
```

用户启动群签名&&环签名RPC服务时，用-l或--log_path选项设置日志路径，或者直接将log.conf拷贝到编译生成的可执行文件同一路径，不指定-l或--log_path，一个简单的启动例子如下：

```bash
#在8005端口启动群签名&&环签名RPC服务，日志配置文件路径是bak/log.conf； 开启的http线程数目是1000
[app@VM_105_81_centos sig-service]$ chmod +x build/server && ./build/server -p 8005 -n 1000 -l bak/log.conf 
port:8005 thread:1000
ADD HTTP CONNECTOR TO test_server
start listening on port 8005
###若要把server放到后台执行,则可借助screen, tmux, nohup等工具，用nohup将程序放到后台执行的命令示例：
chmod +x build/server && nohup  ./build/server -p 8005 -n 1000 -l bak/log.conf > result.log 2>&1 &
```

<br>

[返回目录](#目录)

<br>
<br>
