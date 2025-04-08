# SSL/TLS 
## SSL/TLS基本框架
数据安全传输需要
1. 机密性：加密非通信方无法知道内容
2. 完整性：传输过程中，数据不缺失不改变
3. 不可抵赖性：通信方身份得到认证，不可抵赖
对称加密加密速度快，但密钥交换时容易被获取
非对称加密加密速度慢，但安全，且不可抵赖
为了平衡性能
1. 客户端浏览器通常会预装一组受信任的证书签发机构（Certificate Authorities，CA）的公钥
2. 客户端发起连接（Handshake Protocol: Client Hello）
3. 服务端通过发送证书将服务端公钥给客户端（Handshake Protocol: Certificate）
4. 客户端通过服务端证书与CA公钥验证服务端真实性并保证不可抵赖性，验证通过则继续后续步骤
5. 双方通过复杂的协商过程获得一个连接状态（包括一个压缩算法、一个对称加密算法以及对应的对称密钥（Session Key）保证机密性和一个摘要（MAC）算法保证完整性）
6. 双方发送TLS 密码切换协议信息（Change Cipher Spec Protocol: Change Cipher Spec）作为TLS record level 对数据是否进行加密的分界线（tls1.3标准中没有，但为了兼容可能会有）
7. 后续通信数据（Application Data Protocol）组织为TLSPlaintext并进一步转化为中间产物(tls1.2压缩为TLSCompressed/tls1.3填充成为TLSInnerPlaintext)
8. 将中间产物的fragment添加MAC值（包含消息序列号的消息摘要）并将中间产物使用Session Key加密（通常是一组条目用于块加密、流加密或AEAD加密）成为TLSCiphertext
## 握手阶段如何获得Session Key
获得Session Key主要在于TLS协议中的Handshake阶段，而 TLS 1.2 和 TLS 1.3 在握手协议的实现上有很大的不同
在handshake阶段，客户端和服务端会填充如下安全参数
```C
struct {
    ConnectionEnd          entity;
    PRFAlgorithm           prf_algorithm;
    BulkCipherAlgorithm    bulk_cipher_algorithm;
    CipherType             cipher_type;
    uint8                  enc_key_length;
    uint8                  block_length;
    uint8                  fixed_iv_length;
    uint8                  record_iv_length;
    MACAlgorithm           mac_algorithm;  /*mac 算法*/
    uint8                  mac_length;     /*mac 值的长度*/
    uint8                  mac_key_length; /*mac 算法密钥的长度*/
    CompressionMethod      compression_algorithm;
    opaque                 master_secret[48];
    opaque                 client_random[32];
    opaque                 server_random[32];
} SecurityParameters;
```
并根据各自的安全参数各自生成以下六个条目（通信双方各自维护自己的六个条目）
```
client write MAC key
server write MAC key
client write encryption key
server write encryption key
client write IV // 初始化向量（IV）应为随机产生，无法预测的
server write IV
```
当服务端接收处理记录时会使用 client write条目，反之亦然。例如：客户端使用 client write MAC key、client write encryption key、client write IV 密钥块加密消息，服务端接收到消息以后，也需要使用其维护的client write MAC key、client write encryption key、client write IV 的密钥进行解密。
### tls 1.2
使用prf算法将预备主密钥生成主密钥，
```C
prf(ssl_session, &ssl_session->pre_master_secret, "extended master secret",
    &handshake_hashed_data,
    NULL, 
    &ssl_session->master_secret,
    SSL_MASTER_SECRET_LENGTH)
```
```C
prf(ssl_session, &ssl_session->pre_master_secret, "master secret",
    &ssl_session->client_random,
    &ssl_session->server_random, 
    &ssl_session->master_secret,
    SSL_MASTER_SECRET_LENGTH)
```
再使用prf算法将主密钥生成session key
```C
prf(ssl_session, &ssl_session->master_secret, "key expansion",
    &ssl_session->server_random,
    &ssl_session->client_random,
    &key_block, 
    needed)
```
从key_block中分割出六个条目
```C
ptr=key_block.data;
    /* client/server write MAC key (for non-AEAD ciphers) */
    if (cipher_suite->mode == MODE_STREAM || cipher_suite->mode == MODE_CBC) {
        c_mk=ptr; ptr+=ssl_cipher_suite_dig(cipher_suite)->len;
        s_mk=ptr; ptr+=ssl_cipher_suite_dig(cipher_suite)->len;
    }
    /* client/server write encryption key */
    c_wk=ptr; ptr += encr_key_len;
    s_wk=ptr; ptr += encr_key_len;
    /* client/server write IV (used as IV (for CBC) or salt (for AEAD)) */
    if (write_iv_len > 0) {
        c_iv=ptr; ptr += write_iv_len;
        s_iv=ptr; /* ptr += write_iv_len; */
    }
```
### tls 1.3
HKDF 算法得出密钥
```RFC
   Figure 1 below shows the basic full TLS handshake:

       Client                                           Server

Key  ^ ClientHello
Exch | + key_share*
     | + signature_algorithms*
     | + psk_key_exchange_modes*
     v + pre_shared_key*       -------->
                                                  ServerHello  ^ Key
                                                 + key_share*  | Exch
                                            + pre_shared_key*  v
                                        {EncryptedExtensions}  ^  Server
                                        {CertificateRequest*}  v  Params
                                               {Certificate*}  ^
                                         {CertificateVerify*}  | Auth
                                                   {Finished}  v
                               <--------  [Application Data*]
     ^ {Certificate*}
Auth | {CertificateVerify*}
     v {Finished}              -------->
       [Application Data]      <------->  [Application Data]

              +  Indicates noteworthy extensions sent in the
                 previously noted message.

              *  Indicates optional or situation-dependent
                 messages/extensions that are not always sent.

              {} Indicates messages protected using keys
                 derived from a [sender]_handshake_traffic_secret.

              [] Indicates messages protected using keys
                 derived from [sender]_application_traffic_secret_N.

               Figure 1: Message Flow for Full TLS Handshake
```
##### 1. 客户端->服务端：Handshake Protocol: Client Hello
包含random：32字节随机数
包含cipher_suites：客户端支持的加密套件列表。密码套件里面中能体现出AEAD算法或者HKDF哈希对
包含如下extension
1. supported_versions：tls1.3必须包含，决定了是否协商 TLS 1.3 
2. supported_groups：表明客户端支持的用于(EC)DHE密钥交换的命名组。按照优先级从高到低。
3. key_share：包含客户端所能支持的(EC)DHE算法的密钥参数
##### 2. 服务端->客户端：Handshake Protocol: Server Hello
包含random：32字节随机数
包含cipher_suite：从 ClientHello 中的 cipher_suites 列表中选择的一个加密套件
包含如下extension
1. supported_versions：依据Client Hello协商出的版本
2. key_share：依据Client Hello协商出的椭圆曲线及对应的密钥协商所需参数（Diffie-Hellman参数或ECDHE参数）
##### 3. 服务端->客户端：Handshake Protocol: Encrypted Extensions
这条消息是由server_handshake_traffic_secret中派生的密钥加密的第一条消息
包含应该被保护的扩展，即任何不需要建立加密上下文但不与各个证书相互关联的扩展
#### 方法三

## 会话恢复阶段如何获得Session Key