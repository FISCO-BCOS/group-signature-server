# Group-Signature


![](https://github.com/FISCO-BCOS/FISCO-BCOS/raw/master/docs/images/FISCO_BCOS_Logo.svg?sanitize=true)

[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-brightgreen.svg?style=flat-square)](http://makeapullrequest.com)
[![GitHub issues](https://img.shields.io/github/issues/FISCO-BCOS/Group-Signature.svg)](https://github.com/FISCO-BCOS/Group-Signature/issues)
![GitHub All Releases](https://img.shields.io/github/downloads/FISCO-BCOS/Group-Signature/total.svg)
[![GitHub license](https://img.shields.io/github/license/FISCO-BCOS/Group-Signature.svg)](https://github.com/FISCO-BCOS/Group-Signature/blob/master/LICENSE)

签名库基于[PBC Library](https://crypto.stanford.edu/pbc/)分别实现了[BBS04](http://crypto.stanford.edu/~dabo/abstracts/groupsigs.html)群签名方案和[LSAG](https://www.semanticscholar.org/paper/Linkable-Spontaneous-Anonymous-Group-Signature-for-Liu-Wei/3c63f7c90d79593fadfce16d54078ec1850bedc9)环签名方案。群/环签名，是一种能隐藏签名者身份的签名算法，在具有匿名性要求的场景中有着广泛的应用前景，例如投标、拍卖、投票等等。

## 算法特性

### 群签名

- 匿名性：群成员用群参数产生签名，其他人仅可验证签名的有效性，并通过签名知道签名者所属群组，却无法获取签名者身份信息；
- 不可伪造性：只有群成员才能生成有效可被验证的群签名；
- 不可链接性：给定两个签名，无法判断它们是否来自同一个签名者；
- 可追踪性：在监管介入的场景中，群主可通过签名获取签名者身份。

### 环签名

- 不可伪造性：环中其他成员不能伪造真实签名者签名；
- 完全匿名性：没有群主，只有环成员，其他人仅可验证环签名的有效性，但没有人可以获取签名者身份信息。

## 算法流程

### 群签名

- 创建群：群主执行，生成群公钥，群主私钥和群参数(可用不同线性对参数生成群，支持A， A1， E 和 F类型线性对，默认使用A类型线性对)；
- 加入群：群主执行，群主为群成员生成私钥和证书，其中证书用户证明成员身份；
- 生成群签名：群成员通过私钥和证书对信息签名；
- 验证群签名：验证者可通过群公钥和群参数验证签名的有效性；
- 打开群签名：群主可通过签名信息可获取签名者证书，从而追踪到签名者身份；
- 撤销群成员：群主可移除群成员，并更新群公钥；
- 更新私钥：群成员可根据撤销历史更新自己的私钥。

### 环签名

- 初始化环：由环成员执行，生成环参数，环参数就好比微信面对面建群的密码，任何知道该参数的成员都可以加入该环；
- 加入环：由环成员执行，通过环参数，获得公私钥对；
- 生成环签名：环成员使用私钥和其他环成员公钥产生匿名签名，环大小可由用户根据性能和安全性需求自定义指定；
- 验证环签名：验证者可通过环参数和产生环签名的公钥列表，验证环签名的有效性。

## 签名库使用

### 安装依赖

签名库基于PBC Library密码库实现，该密码库依赖了高精度数学计算库GMP，因此需要先安装GMP。

```
// Centos
sudo yum install gmp-static
// Ubuntu
apt-get install libgmp-dev
// Mac Os
brew install gmp
```

### 编译安装

```
# 下载源码
git clone https://github.com/FISCO-BCOS/Group-Signature.git
# 生成Makefile，CentOS请使用cmake3
mkdir build && cd build && cmake ..
# 编译，可使用-j4加速
make 
# 安装库到系统目录
make install
```

## 接口说明

### 群签名接口

群签名算法接口定义在`group_sig/algorithm/GroupSig.h`文件中，各个接口说明如下：

- 接口名称：**create_group_default**

- 接口功能说明：使用默认参数创建群

- 参数说明

  | 输入参数     | 类型      | 说明               |
  | ------------ | --------- | ------------------ |
  | 无           |           |                    |
  | **输出参数** | **类型**  | **说明**           |
  | 返回值       | GroupInfo | 结构体，记录群信息 |

  ```
  // 结构体信息
  struct GroupInfo
  {
  	string gpk;    // 群公钥
  	string gmsk;   // 群主私钥
  	string gamma;  // 群私钥
  	string param;  // 群参数
  	GroupInfo() {}
  	GroupInfo(string &in1, string &in2, string &in3, string &in4):gpk(in1), gmsk(in2), gamma(in3), param(in4) {}
  };
  ```

- 接口名称：**create_group**

- 接口功能说明：通过指定参数创建群

- 参数说明

  | 输入参数      | 类型      | 说明                      |
  | ------------- | --------- | ------------------------- |
  | pbc_param_str | string    | 支持A, A1, E和F类型线性对 |
  | **输出参数**  | **类型**  | **说明**                  |
  | 返回值        | GroupInfo | 结构体，记录群信息        |

  示例如下：

  - A类型线性对： "{"linear_type": "a", "r_bits_len": 256, "q_bits_len":256}"
  - A1类型线性对："{"linear_type":"a_one", "order":512}"
  - E类型线性对："{"linear_type":"e", "q_bits_len": 512, "r_bits_len":512}"
  - F类型线性对："{"linear_type":"f", "bit_len": 256}"

- 接口名称：**group_member_join**

- 接口功能说明：群主为群成员生成公私钥对

- 参数说明

  | 输入参数     | 类型     | 说明           |
  | ------------ | -------- | -------------- |
  | param_info   | string   | 群参数         |
  | gmsk_info    | string   | 群主私钥       |
  | gpk_info     | string   | 群公钥         |
  | gamma_info   | string   | 群私钥         |
  | **输出参数** | **类型** | **说明**       |
  | 返回值       | string   | 成员私钥和证书 |

- 接口名称：**group_sig**

- 接口功能说明：群成员签名

- 参数说明

  | 输入参数     | 类型     | 说明           |
  | ------------ | -------- | -------------- |
  | gpk_info     | string   | 群公钥         |
  | sk_info      | string   | 成员私钥和证书 |
  | param_info   | string   | 群参数         |
  | message      | string   | 消息           |
  | **输出参数** | **类型** | **说明**       |
  | 返回值       | string   | 群签名         |

- 接口名称：**group_verify**

- 接口功能说明：群成员签名

- 参数说明

  | 输入参数     | 类型     | 说明     |
  | ------------ | -------- | -------- |
  | sig          | string   | 群签名   |
  | message      | string   | 消息     |
  | gpk_info     | string   | 群公钥   |
  | param_info   | string   | 群参数   |
  | **输出参数** | **类型** | **说明** |
  | 返回值       | bool     | 验证结果 |

- 接口名称：**open_cert**

- 接口功能说明：群成员签名

- 参数说明

  | 输入参数     | 类型     | 说明     |
  | ------------ | -------- | -------- |
  | sig          | string   | 群签名   |
  | message      | string   | 消息     |
  | gpk_info     | string   | 群公钥   |
  | gmsk_info    | string   | 群主私钥 |
  | param_info   | string   | 群参数   |
  | **输出参数** | **类型** | **说明** |
  | 返回值       | string   | 证书     |

- 接口名称：**revoke_member**

- 接口功能说明：撤销群成员

- 参数说明

  | 输入参数     | 类型     | 说明                   |
  | ------------ | -------- | ---------------------- |
  | gpk_info     | string   | 群公钥                 |
  | param_info   | string   | 消息                   |
  | revoke_info  | string   | 被撤销成员的私钥和证书 |
  | gamma_info   | string   | 群私钥                 |
  | **输出参数** | **类型** | **说明**               |
  | 返回值       | string   | 新的群公钥             |

- 接口名称：**revoke_update_private_key**

- 接口功能说明：群成员更新私钥

- 参数说明

  | 输入参数     | 类型     | 说明                                 |
  | ------------ | -------- | ------------------------------------ |
  | sk_info      | string   | 成员私钥和证书                       |
  | param_info   | string   | 群参数                               |
  | revoke_list  | string   | 撤销列表                             |
  | g1_list      | string   | 撤销历史中所有更新后的群公钥的g1列表 |
  | gpk_info     | string   | 成员所属群的群公钥                   |
  | **输出参数** | **类型** | **说明**                             |
  | 返回值       | string   | 新的成员私钥和证书                   |

### 环签名接口

群签名算法接口定义在`group_sig/algorithm/RingSig.h`文件中，各个接口说明如下：

- 接口名称：**setup_ring**

- 接口功能说明：通过指定参数创建环

- 参数说明

  | 输入参数     | 类型     | 说明           |
  | ------------ | -------- | -------------- |
  | bit_len      | int      | 指定环参数长度 |
  | **输出参数** | **类型** | **说明**       |
  | 返回值       | string   | 环参数         |

- 接口名称：**join_ring**

- 接口功能说明：环成员加入群

- 参数说明

  | 输入参数     | 类型     | 说明                        |
  | ------------ | -------- | --------------------------- |
  | param_info   | string   | 环参数                      |
  | pos          | string   | 成员在环中的位置，从“0”开始 |
  | **输出参数** | **类型** | **说明**                    |
  | 返回值       | pair     | 公私钥对                    |

- 接口名称：**ring_sig**

- 接口功能说明：环成员加入群

- 参数说明

  | 输入参数         | 类型             | 说明     |
  | ---------------- | ---------------- | -------- |
  | message          | string           | 消息     |
  | public_key_list  | vector\<string\> | 公钥列表 |
  | private_key_info | string           | 私钥     |
  | param_info       | string           | 环参数   |
  | **输出参数**     | **类型**         | **说明** |
  | 返回值           | pair             | 公私钥对 |

- 接口名称：**ring_verify**

- 接口功能说明：环成员加入群

- 参数说明

  | 输入参数     | 类型     | 说明     |
  | ------------ | -------- | -------- |
  | sig          | string   | 签名     |
  | message      | string   | 消息     |
  | param_info   | string   | 环参数   |
  | **输出参数** | **类型** | **说明** |
  | 返回值       | bool     | 验证结果 |

## 贡献代码

- 我们欢迎并非常感谢您的贡献，请参阅[代码贡献流程](https://mp.weixin.qq.com/s/hEn2rxqnqp0dF6OKH6Ua-A
  )。
- 如项目对您有帮助，欢迎star支持！
- 如果发现代码存在安全漏洞，请在[这里](https://security.webank.com)上报。

## 加入社区

**FISCO BCOS开源社区**是国内活跃的开源社区，社区长期为机构和个人开发者提供各类支持与帮助。已有来自各行业的数千名技术爱好者在研究和使用FISCO BCOS。如您对FISCO BCOS开源技术及应用感兴趣，欢迎加入社区获得更多支持与帮助。

![](https://media.githubusercontent.com/media/FISCO-BCOS/LargeFiles/master/images/QR_image.png)

## License

![license](https://img.shields.io/badge/license-Apache%20v2-blue.svg)



Group-Signature的开源协议为[APACHE LICENSE 2.0](http://www.apache.org/licenses/). 详情参考[LICENSE](./LICENSE)。
