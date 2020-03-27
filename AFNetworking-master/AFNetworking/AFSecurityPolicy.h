// AFSecurityPolicy.h

#import <Foundation/Foundation.h>
#import <Security/Security.h>

typedef NS_ENUM(NSUInteger, AFSSLPinningMode) {
    AFSSLPinningModeNone,//代表客户端无条件地信任服务器端返回的证书。
    AFSSLPinningModePublicKey,//代表客户端会将服务器端返回的证书与本地保存的证书中，PublicKey的部分进行校验；如果正确，才继续进行。
    AFSSLPinningModeCertificate,//代表客户端会将服务器端返回的证书和本地保存的证书中的所有内容，包括PublicKey和证书部分，全部进行校验；如果正确，才继续进行。
};

/**
“ AFSecurityPolicy” 对支持X.509证书和公钥验证的服务器信任并建立安全连接。
 添加 SSL 安全证书到你的app当中将可以有效的防止中间第三人攻击，强烈建议对数据敏感的客户或财务信息的应用程序配置并启用SSL的HTTPS连接来进行网络通讯。
 */

NS_ASSUME_NONNULL_BEGIN

@interface AFSecurityPolicy : NSObject <NSSecureCoding, NSCopying>

/**
  服务器信任的证书策略，默认为‘AFSSLPinningModeNone’
 */
@property (readonly, nonatomic, assign) AFSSLPinningMode SSLPinningMode;

/**
 用于根据SSL pinning 模式评估服务器信任度的证书。
 注意，如果启用了pinning，则任何pinning证书匹配，`evaluateServerTrust：forDomain：`都将返回true。
 @see policyWithPinningMode:withPinnedCertificates:
 */
@property (nonatomic, strong, nullable) NSSet <NSData *> *pinnedCertificates;

/**
 如果证书无效或者过期了是否信任服务器，默认为‘NO’
 */
@property (nonatomic, assign) BOOL allowInvalidCertificates;

/**
 是否在证书的CN字段中验证域名，默认是YES
 */
@property (nonatomic, assign) BOOL validatesDomainName;

///-----------------------------------------
/// @name 从Bundle中加载证书
///-----------------------------------------

/**
  返回bundle中的所有证书，如果使用了AFNetworking作为一个framework，你必须使用这个方法来加载你应用程序当中的证书，并且通过调用`policyWithPinningMode:withPinnedCertificates`.方法使用这些证书来建立安全策略

 @return 返回bundle中的所有证书
 */
+ (NSSet <NSData *> *)certificatesInBundle:(NSBundle *)bundle;

///-----------------------------------------
/// @name 获取隐私策略
///-----------------------------------------

/**
 返回一个默认的隐私策略，不允许使用无效证书，验证域名以及不对证书或公钥进行验证。
 @return 返回默认的策略.
 */
+ (instancetype)defaultPolicy;

///---------------------
/// @name Initialization
///---------------------

/**
 创建并返回一个 具有指定策略的实例
 在主bundle中的‘.cer’证书是默认读取的，如果你想使用其他的证书，可以使用`policyWithPinningMode:withPinnedCertificates:`方法进行读取

 @param pinningMode The SSL pinning mode.

 @return A new security policy.

 @see -policyWithPinningMode:withPinnedCertificates:
 */
+ (instancetype)policyWithPinningMode:(AFSSLPinningMode)pinningMode;


+ (instancetype)policyWithPinningMode:(AFSSLPinningMode)pinningMode withPinnedCertificates:(NSSet <NSData *> *)pinnedCertificates;

///------------------------------
/// @name Evaluating Server Trust
///------------------------------

/**
 根据安全策略是否接受指定的服务器信任。

 响应来自服务器的身份验证质询时，应使用此方法。
 
 @param serverTrust The X.509 certificate trust of the server.（这里添加了说明）
 @param domain The domain of serverTrust. If `nil`, the domain will not be validated.

 @return Whether or not to trust the server.
 */
- (BOOL)evaluateServerTrust:(SecTrustRef)serverTrust
                  forDomain:(nullable NSString *)domain;

@end

NS_ASSUME_NONNULL_END

///----------------
/// @name Constants
///----------------

/**
 ## SSL Pinning Modes

 The following constants are provided by `AFSSLPinningMode` as possible SSL pinning modes.

 enum {
 AFSSLPinningModeNone,
 AFSSLPinningModePublicKey,
 AFSSLPinningModeCertificate,
 }

 `AFSSLPinningModeNone`
 Do not used pinned certificates to validate servers.

 `AFSSLPinningModePublicKey`
 Validate host certificates against public keys of pinned certificates.

 `AFSSLPinningModeCertificate`
 Validate host certificates against pinned certificates.
*/
