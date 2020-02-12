# group-signature-server




![](https://github.com/FISCO-BCOS/FISCO-BCOS/raw/master/docs/images/FISCO_BCOS_Logo.svg?sanitize=true)

[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-brightgreen.svg?style=flat-square)](http://makeapullrequest.com)
[![GitHub issues](https://img.shields.io/github/issues/FISCO-BCOS/group-signature-server.svg)](https://github.com/FISCO-BCOS/group-signature-server/issues)
![GitHub All Releases](https://img.shields.io/github/downloads/FISCO-BCOS/group-signature-service/total.svg)
[![GitHub license](https://img.shields.io/github/license/FISCO-BCOS/group-signature-server.svg)](https://github.com/FISCO-BCOS/group-signature-server/blob/master/LICENSE)

群/环签名服务端基于[群/环签名库](https://github.com/FISCO-BCOS/Group-Signature)，为[签名客户端](https://github.com/FISCO-BCOS/group-signature-client/tree/dev-2.0)提供签名服务的RPC接口。服务端部署在机构内，支持[BBS04](http://crypto.stanford.edu/~dabo/abstracts/groupsigs.html)群签名方案和[LSAG](https://www.semanticscholar.org/paper/Linkable-Spontaneous-Anonymous-Group-Signature-for-Liu-Wei/3c63f7c90d79593fadfce16d54078ec1850bedc9)环签名方案，为机构成员提供群签名和环签名服务。

## 代码模块

| <div align = left>模块 && 代码目录</div> | <div align = left>说明</div>                                 |
| ---------------------------------------- | ------------------------------------------------------------ |
| 日志模块(easylog)                        | 打印服务端日志                                               |
| RPC服务模块(httpserver)                  | 实现了群/环签名RPC服务<br>(1) 群签名RPC服务主要在httpserver/GroupSigRpc.*中实现;<br>(2) 环签名RPC服务主要通过httpserver/RingSig.\*实现 |
| 数据库模块(database)                     | 存储签名参数信息，并提供秘钥托管服务                         |

## 服务部署

### 安装依赖

**(1) 基础依赖** 

部署群/环签名RPC服务之前，要安装git, dos2unxi, lsof依赖软件：

- git：用于拉取最新代码；
- dos2unix && lsof: 用于处理windows文件上传到linux服务器时，可执行文件无法被linux正确解析的问题。

可用如下命令安装这些基础依赖软件：

```bash
# [Centos]
sudo yum -y install git lsof dos2unix

# [Mac Os]
brew install git lsof dos2unix

# [Ubuntu] 没有dos2unix工具
sudo apt install git lsof tofrodos
ln -s /usr/bin/todos /usr/bin/unxi2dos
ln -s /usr/bin/fromdos /usr/bin/dos2unix
```

**(2) 库依赖**

服务端依赖的签名库基于PBC Library密码库实现，该密码库依赖了高精度数学计算库GMP，因此需要先安装GMP。

```bash
# CentOS
sudo yum install gmp-static

# Ubuntu
apt-get install libgmp-dev

# Mac Os
brew install gmp
```

### 编译源码

```bash
# 从拉取git代码
git clone https://github.com/FISCO-BCOS/group-signature-server.git

# 切换分支
git checkout dev-2.0

# 若是linux/unix环境，安装依赖软件之后，执行format.sh脚本格式化shell脚本和json配置文件，使其可被linux/unix正确解析
dos2unix format.sh

# 格式化shell脚本和json配置文件
bash format.sh

# 方法一: 使用compile脚本编译
cd group-signature-server && bash compile.sh

# 方法二： 手动编译, 其中-j4表示用4个线程并发编译
# 编译后，会在build目录下生成RPC服务程序server
# Centos请使用cmake3
cd group-signature-server && mkdir -p build && cd build && cmake .. && make -j4
```

## 使用说明

**(1) 获取服务帮助说明**

```bash
#群/环签名RPC服务使用帮助：
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
| -p, --port            | rpc服务启动端口             | 8003      |
| -n, --http_thread_num | rpc服务启动的http线程数目      | 50         |
| -l, --log_path        | 配置文件路径                | "log.conf" |
| -s, --ssl_cert        | ssl证书路径（针对https服务）    | “”         |
| -k, --ssl_key         | 访问ssl证书的秘钥（针对https服务） | “”         |
| -h, --help            | 显示帮助信息                | -          |

**(2) 配置日志 **

group-signature-server目录下存着一个日志配置文件示例log.conf，用户使用时需要做如下配置：

- 修改FILENAME，将其修改为用户自己的日志文件路径；
- 根据需求，修改日志打印选项，生产环境中，一般WARNING/ERROR/FATAL打开（设置选项值为true），其他选项关闭（设置选项值为false）；
- 测试环境中，可打开DEBUG和TRACE。

 **(3) 启动服务 **

用户启动群/环签名RPC服务时，用-l或--log_path选项设置日志路径，或者直接将log.conf拷贝到编译生成的可执行文件同一路径，不指定-l或--log_path，一个简单的启动例子：

```bash
# 在8005端口启动群签名&&环签名RPC服务，开启的http线程数目是10
./build/server -p 8005 -n 10 -l log.conf 

# 若要把server放到后台执行,则可借助screen, tmux, nohup等工具，命令示例：
nohup ./build/server -p 8005 -n 10 -l log.conf &
```

## 群/环签名RPC接口

群/环签名RPC详细接口可参考[群/环签名RPC接口文档](doc/rpc_interface.md)。

## 贡献代码

- 我们欢迎并非常感谢您的贡献，请参阅[代码贡献流程](https://mp.weixin.qq.com/s/hEn2rxqnqp0dF6OKH6Ua-A
  )。
- 如项目对您有帮助，欢迎star支持！
- 如果发现代码存在安全漏洞，请在[这里](https://security.webank.com)上报。

## 加入社区

**FISCO BCOS开源社区**是国内活跃的开源社区，社区长期为机构和个人开发者提供各类支持与帮助。已有来自各行业的数千名技术爱好者在研究和使用FISCO BCOS。如您对FISCO BCOS开源技术及应用感兴趣，欢迎加入社区获得更多支持与帮助。

![](https://media.githubusercontent.com/media/FISCO-BCOS/LargeFiles/master/images/QR_image.png)

## License

![license](https://img.shields.io/github/license/FISCO-BCOS/group-signature-server.svg)

Groupsig-Server的开源协议为[GNU GENERAL PUBLIC LICENSE](http://www.gnu.org/licenses/gpl-3.0.en.html). 详情参考[LICENSE](./LICENSE)。