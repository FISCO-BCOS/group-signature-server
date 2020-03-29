# group-signature-server

![](https://github.com/FISCO-BCOS/FISCO-BCOS/raw/master/docs/images/FISCO_BCOS_Logo.svg?sanitize=true)

[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-brightgreen.svg?style=flat-square)](http://makeapullrequest.com)
[![GitHub issues](https://img.shields.io/github/issues/FISCO-BCOS/group-signature-server.svg)](https://github.com/FISCO-BCOS/group-signature-server/issues)
[![All releases](https://img.shields.io/github/release/FISCO-BCOS/group-signature-server.svg)](https://github.com/FISCO-BCOS/group-signature-server/releases/latest)
![](https://img.shields.io/github/license/FISCO-BCOS/group-signature-server) 

群/环签名服务端基于[群/环签名库](https://github.com/FISCO-BCOS/group-signature-lib)，为群/环签名[客户端](https://github.com/FISCO-BCOS/group-signature-client)提供签名RPC服务。服务端可部署在机构内，支持[BBS04](http://crypto.stanford.edu/~dabo/abstracts/groupsigs.html)群签名方案和[LSAG](https://www.semanticscholar.org/paper/Linkable-Spontaneous-Anonymous-Group-Signature-for-Liu-Wei/3c63f7c90d79593fadfce16d54078ec1850bedc9)环签名方案，为机构成员提供群签名和环签名服务。

群/环签名客户端和服务端是专门提供给社区用户的FISCO BCOS[隐私保护模块](https://fisco-bcos-documentation.readthedocs.io/zh_CN/latest/docs/manual/privacy.html)关于群/环签名的开发示例，架构如下：

![](image/demo.jpg)


## 代码结构

| <div align = left>目录</div> | <div align = left>说明</div>                     |
| ---------------------------------------- | ----------------------------------- |
| easylog                     | 打印服务端日志                                      |
| httpserver                  | 群/环签名RPC服务|
| database                    | 存储签名参数以及群成员信息                            |

## 服务部署

### 安装依赖

- Ubuntu

推荐Ubuntu 16.04以上版本，16.04以下的版本没有经过测试。

```bash
$ sudo apt install -y flex patch bison libgmp-dev byacc
```

- CentOS

推荐使用CentOS7以上版本。

```bash
$ sudo yum install -y flex patch bison gmp-static byacc
```

- macOS

推荐xcode10以上版本。macOS依赖包安装依赖于[Homebrew](https://brew.sh/)。

```bash
$ brew install flex bison gmp byacc
```

### 源码编译

```bash
# 从拉取git代码
git clone https://github.com/FISCO-BCOS/group-signature-server.git
# 创建目录
cd group-signature-server && mkdir -p build && cd build
# Centos请使用cmake3
cmake .. 
# 编译，可使用-j4加速
make
# 编译后，会在build目录下生成RPC服务程序server
```
### 启动服务

```bash
# 假设当前在group-signature-server/build目录下
./server 
# 若要把server放到后台执行,则可借助nohup工具，命令示例：
nohup ./server &
```

**服务帮助**

```bash
./server -h
# 输出如下
group sig and ring sig RPC:
  -p [ --port ] arg            listen port of group sig and ring sig RPC
  -n [ --http_thread_num ] arg thread num of http server used to deal with 
                               requests
  -l [ --log_path ] arg        path of log configuration
  -s [ --ssl_cert ] arg        path of ssl certification
  -k [ --ssl_key ] arg         key to access ssl certification
  -h [ --help ]                help of group sig and ring sig rpc
```

- 命令示例

./server -p 8005 -n 10 -l ../log.conf 

- 参数含义

| <div align = left>参数</div>  | <div align = left>说明</div>  | <div align = left>默认参数</div>   |
| --------------------- | --------------------- | ---------- |
| -p | rpc服务的监听端口             | 8005      |
| -n | rpc服务启动的线程数目      | 10         |
| -l    | 日志配置文件路径                | ../log.conf |
| -s     | 证书路径（针对https）    |         |
| -k     | 私钥路径（针对https） |       |
| -h    | 显示帮助信息                |          |


**配置日志**

group-signature-server目录下存着一个日志配置文件示例log.conf，用户使用时需要做如下配置：

- 根据需求，修改日志打印选项，生产环境中，一般WARNING/ERROR/FATAL打开，其他选项关闭；
- 测试环境中，可打开DEBUG和TRACE。
- 配置文件生效需要重启服务

## 接口说明

详见[接口文档](doc/rpc_interface.md)。

## 贡献代码

- 我们欢迎并非常感谢您的贡献，请参阅[代码贡献流程](CONTRIBUTING.md)。
- 如项目对您有帮助，欢迎star支持！

## 加入社区

**FISCO BCOS开源社区**是国内活跃的开源社区，社区长期为机构和个人开发者提供各类支持与帮助。已有来自各行业的数千名技术爱好者在研究和使用FISCO BCOS。如您对FISCO BCOS开源技术及应用感兴趣，欢迎加入社区获得更多支持与帮助。

![](https://raw.githubusercontent.com/FISCO-BCOS/LargeFiles/master/images/QR_image.png)

## License

![license](https://img.shields.io/github/license/FISCO-BCOS/group-signature-server.svg)

group-signature-server的开源协议为[GNU GENERAL PUBLIC LICENSE](http://www.gnu.org/licenses/gpl-3.0.en.html). 详情参考[LICENSE](./LICENSE)。