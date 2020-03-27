// AFHTTPSessionManager.h


#import <Foundation/Foundation.h>
#if !TARGET_OS_WATCH
#import <SystemConfiguration/SystemConfiguration.h>
#endif
#import <TargetConditionals.h>

#if TARGET_OS_IOS || TARGET_OS_WATCH || TARGET_OS_TV
#import <MobileCoreServices/MobileCoreServices.h>
#else
#import <CoreServices/CoreServices.h>
#endif

#import "AFURLSessionManager.h"

/**
 `AFHTTPSessionManager` 是`AFURLSessionManager`用于简化HTTP请求的一个子类，当提供了`baseURL`，通过`GET` / `POST` /等进行方式进行请求，构造方法可以得到一个相对路径。

 ## 补充点

 开发iOS7或者Mac OSX 10.9以后工程将广泛使用`AFHTTPSessionManager`进行网络请求，提供一个类方法返回一个具有认证和其他配置项的共享单例对象
 而对于开发 iOS 6 或者 Mac OS X 10.8 及以前的工程，使用AFHTTPRequestOperationManager 可能达到更简单的效果

 ## 方法重写
 使用GET` / `POST` 等构造方法更改所有数据任务操作构造的行为，需要重写`dataTaskWithRequest:uploadProgress:downloadProgress:completionHandler:`.
 
 ## 序列化
 创建一个HTTP客户端请求将包含一个遵循‘<AFURLRequestSerialization>’的默认请求头及‘requestSerializer’编码属性
 从服务端获取到的响应将自动转化成一个遵守`<AFURLResponseSerialization>`的序列化‘`responseSerializers`属性
 
 ## URL 使用相对路径构成

 对于HTTP的方便方法，当提供了 ‘baseURL’ ，request serializer 通过`NSURL +URLWithString:relativeToURL:`,方法构造URLs，如果‘baseURL’为nil.那么请求路径需要使用`NSURL +URLWithString:`.来提供。

 下面是使用baseURL来构建相对路径的例子：

    NSURL *baseURL = [NSURL URLWithString:@"http://example.com/v1/"];
    [NSURL URLWithString:@"foo" relativeToURL:baseURL];                  // http://example.com/v1/foo
    [NSURL URLWithString:@"foo?bar=baz" relativeToURL:baseURL];          // http://example.com/v1/foo?bar=baz
    [NSURL URLWithString:@"/foo" relativeToURL:baseURL];                 // http://example.com/foo
    [NSURL URLWithString:@"foo/" relativeToURL:baseURL];                 // http://example.com/v1/foo
    [NSURL URLWithString:@"/foo/" relativeToURL:baseURL];                // http://example.com/foo/
    [NSURL URLWithString:@"http://example2.com/" relativeToURL:baseURL]; // http://example2.com/

 还需要注意的是，一个反斜杠会默认添加到'baseURL'的后面（如果没有的话），否则的如果URLs后面没有自己添加斜杠将会造成一些难以预测的行为
 @warning 注意，在使用期间，必须拥有后台会话的管理者，这可以通过创建应用程序共享的单例实例来实现
 */

NS_ASSUME_NONNULL_BEGIN

@interface AFHTTPSessionManager : AFURLSessionManager <NSSecureCoding, NSCopying>

/**
 构建相对路径用于`GET` / `POST` / 等模式来请求的简便方法如`requestWithMethod:URLString:parameters:`
 */
@property (readonly, nonatomic, strong, nullable) NSURL *baseURL;

/**
 通过 `requestWithMethod:URLString:parameters:` & `multipartFormRequestWithMethod:URLString:parameters:constructingBodyWithBlock:`等构造的请求将携带一些默认的请求头中的指定参数序列化。‘’，默认情况下，将其设置为“ AFHTTPRequestSerializer”的实例，该实例将“ GET”，“ HEAD”和“ DELETE”请求的查询字符串参数序列化，或者以其他方式对HTTP消息主体进行URL形式编码。
 @warning `requestSerializer` 不能为nil
 */
@property (nonatomic, strong) AFHTTPRequestSerializer <AFURLRequestSerialization> * requestSerializer;

/**
 在使用使用`GET` /`POST` /等模式通过`dataTaskWithRequest：success：failure：`创建的数据任务中，返回服务器的响应。 便捷方法由响应序列化程序自动验证并序列化。 默认情况下，此属性设置为“ AFJSONResponseSerializer”的实例。

 @warning `responseSerializer` 不能为nil
 */
@property (nonatomic, strong) AFHTTPResponseSerializer <AFURLResponseSerialization> * responseSerializer;

///-------------------------------
/// @name Managing Security Policy
///-------------------------------

/**
 创建的会话用来评估服务器对安全连接的信任度的安全策略。 除非另有说明，否则AFURLSessionManager会使用defaultPolicy。 使用“ AFSSLPinningModePublicKey”或“ AFSSLPinningModeCertificate”配置的安全策略只能应用于以安全URL（即https）初始化的session manager。 在不安全的session manager上启用了应用安全策略会引发“无效的安全策略”异常。
 */
@property (nonatomic, strong) AFSecurityPolicy *securityPolicy;

///---------------------
/// @name Initialization
///---------------------

/**
 创建并返回`AFHTTPSessionManager` 对象
 */
+ (instancetype)manager;

/**
 通过指定的base URL来初始化 `AFHTTPSessionManager` 对象

 @param url 初始的url

 @return 返回一个AFHTTPSessionManager实例
 */
- (instancetype)initWithBaseURL:(nullable NSURL *)url;

/**
 通过指定的base URL来初始化 `AFHTTPSessionManager` 对象

 这是指定的初始化方法

 @param url The base URL for the HTTP client.
 @param configuration The configuration used to create the managed session.

 @return The newly-initialized HTTP client
 */
- (instancetype)initWithBaseURL:(nullable NSURL *)url
           sessionConfiguration:(nullable NSURLSessionConfiguration *)configuration NS_DESIGNATED_INITIALIZER;

///---------------------------
/// @name Making HTTP Requests
///---------------------------

/**
 通过‘GET’请求创建并执行一个NSURLSessionDataTask

 @param URLString 请求的URL.
 @param parameters 请求参数
 @param success 请求成功返回的回调
 @param failure 请求失败，或者请求成功，但是返回数据异常

 @see -dataTaskWithRequest:completionHandler:
 */
- (nullable NSURLSessionDataTask *)GET:(NSString *)URLString
                   parameters:(nullable id)parameters
                      success:(nullable void (^)(NSURLSessionDataTask *task, id _Nullable responseObject))success
                      failure:(nullable void (^)(NSURLSessionDataTask * _Nullable task, NSError *error))failure DEPRECATED_ATTRIBUTE;


/**
 通过‘GET’请求创建并执行一个NSURLSessionDataTask

 @param URLString 请求的URL.
 @param parameters 请求参数
 @param downloadProgress 下载进度更新时进行的一个block回调。这个block是在session的队列进行响应的，而不是在主队列.
 @param success 请求成功返回的回调
 @param failure 请求失败，或者请求成功，但是返回数据异常

 @see -dataTaskWithRequest:uploadProgress:downloadProgress:completionHandler:
 */
- (nullable NSURLSessionDataTask *)GET:(NSString *)URLString
                            parameters:(nullable id)parameters
                              progress:(nullable void (^)(NSProgress *downloadProgress))downloadProgress
                               success:(nullable void (^)(NSURLSessionDataTask *task, id _Nullable responseObject))success
                               failure:(nullable void (^)(NSURLSessionDataTask * _Nullable task, NSError *error))failure DEPRECATED_ATTRIBUTE;

/**
 通过‘GET’请求创建并执行一个NSURLSessionDataTask
 @param URLString 请求的URL.
 @param parameters 请求参数
 @param headers 请求头将拼接到默认请求头上
 @param downloadProgress 下载进度更新时进行的一个block回调。这个block是在session的队列进行响应的，而不是在主队列.
 @param success 请求成功返回的回调
 @param failure 请求失败，或者请求成功，但是返回数据异常
 
 @see -dataTaskWithRequest:uploadProgress:downloadProgress:completionHandler:
 */
- (nullable NSURLSessionDataTask *)GET:(NSString *)URLString
                            parameters:(nullable id)parameters
                               headers:(nullable NSDictionary <NSString *, NSString *> *)headers
                              progress:(nullable void (^)(NSProgress *downloadProgress))downloadProgress
                               success:(nullable void (^)(NSURLSessionDataTask *task, id _Nullable responseObject))success
                               failure:(nullable void (^)(NSURLSessionDataTask * _Nullable task, NSError *error))failure;

/**
 通过‘HEAD’请求创建并执行一个NSURLSessionDataTask

 @param URLString 请求的URL.
 @param parameters 请求参数
 @param success 请求成功返回的回调
 @param failure 请求失败，或者请求成功，但是返回数据异常

 @see -dataTaskWithRequest:completionHandler:
 */
- (nullable NSURLSessionDataTask *)HEAD:(NSString *)URLString
                    parameters:(nullable id)parameters
                       success:(nullable void (^)(NSURLSessionDataTask *task))success
                       failure:(nullable void (^)(NSURLSessionDataTask * _Nullable task, NSError *error))failure DEPRECATED_ATTRIBUTE;

/**
 通过‘HEAD’请求创建并执行一个NSURLSessionDataTask

 @param URLString 请求的URL.
 @param parameters 请求参数
 @param headers 请求头将拼接到默认请求头上
 @param success 请求成功返回的回调
 @param failure 请求失败，或者请求成功，但是返回数据异常
 
 @see -dataTaskWithRequest:completionHandler:
 */
- (nullable NSURLSessionDataTask *)HEAD:(NSString *)URLString
                             parameters:(nullable id)parameters
                                headers:(nullable NSDictionary <NSString *, NSString *> *)headers
                                success:(nullable void (^)(NSURLSessionDataTask *task))success
                                failure:(nullable void (^)(NSURLSessionDataTask * _Nullable task, NSError *error))failure;

/**
 通过‘POST’请求创建并执行一个NSURLSessionDataTask

 @param URLString 请求的URL.
 @param parameters 请求参数
 @param success 请求成功返回的回调
 @param failure 请求失败，或者请求成功，但是返回数据异常

 @see -dataTaskWithRequest:completionHandler:
 */
- (nullable NSURLSessionDataTask *)POST:(NSString *)URLString
                    parameters:(nullable id)parameters
                       success:(nullable void (^)(NSURLSessionDataTask *task, id _Nullable responseObject))success
                       failure:(nullable void (^)(NSURLSessionDataTask * _Nullable task, NSError *error))failure DEPRECATED_ATTRIBUTE;

/**
 通过‘POST’请求创建并执行一个NSURLSessionDataTask

 @param URLString 请求的URL.
 @param parameters 请求参数
 @param uploadProgress 上传进度更新时进行的一个block回调。这个block是在session的队列进行响应的，而不是在主队列.
 @param success 请求成功返回的回调
 @param failure 请求失败，或者请求成功，但是返回数据异常
 

 @see -dataTaskWithRequest:uploadProgress:downloadProgress:completionHandler:
 */
- (nullable NSURLSessionDataTask *)POST:(NSString *)URLString
                             parameters:(nullable id)parameters
                               progress:(nullable void (^)(NSProgress *uploadProgress))uploadProgress
                                success:(nullable void (^)(NSURLSessionDataTask *task, id _Nullable responseObject))success
                                failure:(nullable void (^)(NSURLSessionDataTask * _Nullable task, NSError *error))failure DEPRECATED_ATTRIBUTE;

/**
 通过‘POST’请求创建并执行一个NSURLSessionDataTask

 @param URLString 请求的URL.
 @param parameters 请求参数
 @param headers 请求头将拼接到默认请求头上
 @param uploadProgress 上传进度更新时进行的一个block回调。这个block是在session的队列进行响应的，而不是在主队列.
 @param success 请求成功返回的回调
 @param failure 请求失败，或者请求成功，但是返回数据异常
 
 @see -dataTaskWithRequest:uploadProgress:downloadProgress:completionHandler:
 */
- (nullable NSURLSessionDataTask *)POST:(NSString *)URLString
                             parameters:(nullable id)parameters
                                headers:(nullable NSDictionary <NSString *, NSString *> *)headers
                               progress:(nullable void (^)(NSProgress *uploadProgress))uploadProgress
                                success:(nullable void (^)(NSURLSessionDataTask *task, id _Nullable responseObject))success
                                failure:(nullable void (^)(NSURLSessionDataTask * _Nullable task, NSError *error))failure;

/**
 通过‘POST’请求创建并执行一个NSURLSessionDataTask

 @param URLString 请求的URL.
 @param parameters 请求参数
 @param block  接受单个参数并将数据添加到HTTP body。 block参数是遵循了“ AFMultipartFormData”协议的对象。
 @param success 请求成功返回的回调
 @param failure 请求失败，或者请求成功，但是返回数据异常
 

 @see -dataTaskWithRequest:completionHandler:
 */
- (nullable NSURLSessionDataTask *)POST:(NSString *)URLString
                    parameters:(nullable id)parameters
     constructingBodyWithBlock:(nullable void (^)(id <AFMultipartFormData> formData))block
                       success:(nullable void (^)(NSURLSessionDataTask *task, id _Nullable responseObject))success
                       failure:(nullable void (^)(NSURLSessionDataTask * _Nullable task, NSError *error))failure DEPRECATED_ATTRIBUTE;

/**
 通过‘POST’请求创建并执行一个NSURLSessionDataTask

 @param URLString 请求的URL.
 @param parameters 请求参数
 @param block  接受单个参数并将数据添加到HTTP body。 block参数是遵循了“ AFMultipartFormData”协议的对象。
 @param uploadProgress 上传进度更新时进行的一个block回调。这个block是在session的队列进行响应的，而不是在主队列.
 @param success 请求成功返回的回调
 @param failure 请求失败，或者请求成功，但是返回数据异常
 @see -dataTaskWithRequest:uploadProgress:downloadProgress:completionHandler:
 */
- (nullable NSURLSessionDataTask *)POST:(NSString *)URLString
                             parameters:(nullable id)parameters
              constructingBodyWithBlock:(nullable void (^)(id <AFMultipartFormData> formData))block
                               progress:(nullable void (^)(NSProgress *uploadProgress))uploadProgress
                                success:(nullable void (^)(NSURLSessionDataTask *task, id _Nullable responseObject))success
                                failure:(nullable void (^)(NSURLSessionDataTask * _Nullable task, NSError *error))failure DEPRECATED_ATTRIBUTE;
/**
 通过‘POST’请求创建并执行一个NSURLSessionDataTask

 @param URLString 请求的URL.
 @param parameters 请求参数
 @param headers 请求头将拼接到默认请求头上
 @param block  接受单个参数并将数据添加到HTTP body。 block参数是遵循了“ AFMultipartFormData”协议的对象。
 @param uploadProgress 上传进度更新时进行的一个block回调。这个block是在session的队列进行响应的，而不是在主队列.
 @param success 请求成功返回的回调
 @param failure 请求失败，或者请求成功，但是返回数据异常
 
 @see -dataTaskWithRequest:uploadProgress:downloadProgress:completionHandler:
 */
- (nullable NSURLSessionDataTask *)POST:(NSString *)URLString
                             parameters:(nullable id)parameters
                                headers:(nullable NSDictionary <NSString *, NSString *> *)headers
              constructingBodyWithBlock:(nullable void (^)(id <AFMultipartFormData> formData))block
                               progress:(nullable void (^)(NSProgress *uploadProgress))uploadProgress
                                success:(nullable void (^)(NSURLSessionDataTask *task, id _Nullable responseObject))success
                                failure:(nullable void (^)(NSURLSessionDataTask * _Nullable task, NSError *error))failure;

/**
  通过‘PUT’请求创建并执行一个NSURLSessionDataTask

 @param URLString 请求的URL.
 @param parameters 请求参数
 @param success 请求成功返回的回调
 @param failure 请求失败，或者请求成功，但是返回数据异常

 @see -dataTaskWithRequest:completionHandler:
 */
- (nullable NSURLSessionDataTask *)PUT:(NSString *)URLString
                   parameters:(nullable id)parameters
                      success:(nullable void (^)(NSURLSessionDataTask *task, id _Nullable responseObject))success
                      failure:(nullable void (^)(NSURLSessionDataTask * _Nullable task, NSError *error))failure DEPRECATED_ATTRIBUTE;

/**
 通过‘PUT’请求创建并执行一个NSURLSessionDataTask

 @param URLString 请求的URL.
 @param parameters 请求参数
 @param headers 请求头将拼接到默认请求头上
 @param success 请求成功返回的回调
 @param failure 请求失败，或者请求成功，但是返回数据异常
 
 @see -dataTaskWithRequest:completionHandler:
 */
- (nullable NSURLSessionDataTask *)PUT:(NSString *)URLString
                            parameters:(nullable id)parameters
                               headers:(nullable NSDictionary <NSString *, NSString *> *)headers
                               success:(nullable void (^)(NSURLSessionDataTask *task, id _Nullable responseObject))success
                               failure:(nullable void (^)(NSURLSessionDataTask * _Nullable task, NSError *error))failure;

/**
 通过‘PATCH’请求创建并执行一个NSURLSessionDataTask

 @param URLString 请求的URL.
 @param parameters 请求参数
 @param success 请求成功返回的回调
 @param failure 请求失败，或者请求成功，但是返回数据异常

 @see -dataTaskWithRequest:completionHandler:
 */
- (nullable NSURLSessionDataTask *)PATCH:(NSString *)URLString
                     parameters:(nullable id)parameters
                        success:(nullable void (^)(NSURLSessionDataTask *task, id _Nullable responseObject))success
                        failure:(nullable void (^)(NSURLSessionDataTask * _Nullable task, NSError *error))failure DEPRECATED_ATTRIBUTE;

/**
 通过‘PATCH’请求创建并执行一个NSURLSessionDataTask

 @param URLString 请求的URL.
 @param parameters 请求参数
 @param headers 请求头将拼接到默认请求头上
 @param success 请求成功返回的回调
 @param failure 请求失败，或者请求成功，但是返回数据异常
 
 @see -dataTaskWithRequest:completionHandler:
 */
- (nullable NSURLSessionDataTask *)PATCH:(NSString *)URLString
                              parameters:(nullable id)parameters
                                 headers:(nullable NSDictionary <NSString *, NSString *> *)headers
                                 success:(nullable void (^)(NSURLSessionDataTask *task, id _Nullable responseObject))success
                                 failure:(nullable void (^)(NSURLSessionDataTask * _Nullable task, NSError *error))failure;

/**
 通过‘DELETE’请求创建并执行一个NSURLSessionDataTask

 @param URLString 请求的URL.
 @param parameters 请求参数
 @param success 请求成功返回的回调
 @param failure 请求失败，或者请求成功，但是返回数据异常

 @see -dataTaskWithRequest:completionHandler:
 */
- (nullable NSURLSessionDataTask *)DELETE:(NSString *)URLString
                      parameters:(nullable id)parameters
                         success:(nullable void (^)(NSURLSessionDataTask *task, id _Nullable responseObject))success
                         failure:(nullable void (^)(NSURLSessionDataTask * _Nullable task, NSError *error))failure DEPRECATED_ATTRIBUTE;

/**
 通过‘DELETE’请求创建并执行一个NSURLSessionDataTask

 @param URLString 请求的URL.
 @param parameters 请求参数
 @param headers 请求头将拼接到默认请求头上
 @param success 请求成功返回的回调
 @param failure 请求失败，或者请求成功，但是返回数据异常
 
 @see -dataTaskWithRequest:completionHandler:
 */
- (nullable NSURLSessionDataTask *)DELETE:(NSString *)URLString
                               parameters:(nullable id)parameters
                                  headers:(nullable NSDictionary <NSString *, NSString *> *)headers
                                  success:(nullable void (^)(NSURLSessionDataTask *task, id _Nullable responseObject))success
                                  failure:(nullable void (^)(NSURLSessionDataTask * _Nullable task, NSError *error))failure;

@end

NS_ASSUME_NONNULL_END
