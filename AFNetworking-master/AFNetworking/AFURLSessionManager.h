// AFURLSessionManager.h
// Copyright (c) 2011–2016 Alamofire Software Foundation ( http://alamofire.org/ )
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.


#import <Foundation/Foundation.h>

#import "AFURLResponseSerialization.h"
#import "AFURLRequestSerialization.h"
#import "AFSecurityPolicy.h"
#import "AFCompatibilityMacros.h"
#if !TARGET_OS_WATCH
#import "AFNetworkReachabilityManager.h"
#endif

/**
 AFURLSessionManager 创建和管理了一个NSURLSession对象，并且持有了一个遵循``<NSURLSessionTaskDelegate>`, `<NSURLSessionDataDelegate>`, `<NSURLSessionDownloadDelegate>`, and `<NSURLSessionDelegate>`，等代理的NSURLSessionConfiguration类

 ##  补充笔记
 这是添加了部分特定属性的网络请求类，是AFHTTPSessionManager的基类，如果你想对AFURLSessionManager进行网络请求扩展，可以考虑一下使用子类AFHTTPSessionManager
 
 ## NSURLSession & NSURLSessionTask 代理方法

 `AFURLSessionManager` 实现了以下代理方法:

 ### `NSURLSessionDelegate`

 - `URLSession:didBecomeInvalidWithError:`
 - `URLSession:didReceiveChallenge:completionHandler:`
 - `URLSessionDidFinishEventsForBackgroundURLSession:`

 ### `NSURLSessionTaskDelegate`

 - `URLSession:willPerformHTTPRedirection:newRequest:completionHandler:`
 - `URLSession:task:didReceiveChallenge:completionHandler:`
 - `URLSession:task:didSendBodyData:totalBytesSent:totalBytesExpectedToSend:`
 - `URLSession:task:needNewBodyStream:`
 - `URLSession:task:didCompleteWithError:`

 ### `NSURLSessionDataDelegate`

 - `URLSession:dataTask:didReceiveResponse:completionHandler:`
 - `URLSession:dataTask:didBecomeDownloadTask:`
 - `URLSession:dataTask:didReceiveData:`
 - `URLSession:dataTask:willCacheResponse:completionHandler:`

 ### `NSURLSessionDownloadDelegate`

 - `URLSession:downloadTask:didFinishDownloadingToURL:`
 - `URLSession:downloadTask:didWriteData:totalBytesWritten:totalBytesExpectedToWrite:`
 - `URLSession:downloadTask:didResumeAtOffset:expectedTotalBytes:`

 如果这些方法被子类重写，那么一定要先使用‘super’进行调用。

 ## 网络状态监测

可以通过‘reachabilityManager’的属性监测网络可用性状态和是否有网络。应用程序可以通过监测网络是否可用来决定是继续还是暂停已经正在进行的网络请求。查看‘AFNetworkReachabilityManager’了解详情。

 ## NSCoding 警告

Encoded managers 没有包含任何的block属性。当使用initWithCoder或者NSKeyedUnarchiver时，在blocks回调时请确保设置了代理。

 ## NSCopying 警告

 - `-copy` 和 `-copyWithZone:` 基于原始的manager属性返回了一个新的NSURLSessionManager.
 - Operation copies do not include any delegate callback blocks, as they often strongly captures a reference to `self`, which would otherwise have the unintuitive side-effect of pointing to the _original_ session manager when copied.

 @warning Managers for background sessions must be owned for the duration of their use. This can be accomplished by creating an application-wide or shared singleton instance.
 */

NS_ASSUME_NONNULL_BEGIN

@interface AFURLSessionManager : NSObject <NSURLSessionDelegate, NSURLSessionTaskDelegate, NSURLSessionDataDelegate, NSURLSessionDownloadDelegate, NSSecureCoding, NSCopying>

/**
  session管理.
 */
@property (readonly, nonatomic, strong) NSURLSession *session;

/**
  代理回调和执行时的队列
 */
@property (readonly, nonatomic, strong) NSOperationQueue *operationQueue;

/**
 通过`dataTaskWithRequest:success:failure:`方法创建了`GET` / `POST`的dataTask请求响应，构建方法由respon自动验证和序列化，通常给‘AFJSONResponseSerializer’为默认值。
 @warning `responseSerializer` 不能为 `nil`.
 */
@property (nonatomic, strong) id <AFURLResponseSerialization> responseSerializer;

///-------------------------------
/// @name 管理隐私策略
///-------------------------------

/**
隐私策略通常用于创建session来通过隐私请求来访问自己的服务，`AFURLSessionManager`使用`defaultPolicy` 除非另有说明
 */
@property (nonatomic, strong) AFSecurityPolicy *securityPolicy;

#if !TARGET_OS_WATCH
///--------------------------------------
/// @name 网络可用性检测
///--------------------------------------

/**
 网络可用性管理类. `AFURLSessionManager`默认使用`sharedManager`,网络状态初始化默认为AFNetworkReachabilityStatusUnknown
 */
@property (readwrite, nonatomic, strong) AFNetworkReachabilityManager *reachabilityManager;
#endif

///----------------------------
/// @name 获取 Session Tasks 数组
///----------------------------

/**
session创建的正在执行的所有请求数据，上传和下载任务
 */
@property (readonly, nonatomic, strong) NSArray <NSURLSessionTask *> *tasks;

/**
session创建的正在执行的所有请求数据任务
 */
@property (readonly, nonatomic, strong) NSArray <NSURLSessionDataTask *> *dataTasks;

/**
session创建的正在执行的所有上传任务
 */
@property (readonly, nonatomic, strong) NSArray <NSURLSessionUploadTask *> *uploadTasks;

/**
session创建的正在执行的所有下载任务
 */
@property (readonly, nonatomic, strong) NSArray <NSURLSessionDownloadTask *> *downloadTasks;

///-------------------------------
/// @name Managing 回调队列
///-------------------------------

/**
这是‘completionBlock’的队列，如果没有设置或者为‘NULL’,那么就是主队列。
 */
@property (nonatomic, strong, nullable) dispatch_queue_t completionQueue;

/**
这是‘completionBlock’的队列组，如果没有设置或者为‘NULL’,那么就是正在使用私有的队列组
 */
@property (nonatomic, strong, nullable) dispatch_group_t completionGroup;

///---------------------------------
/// @name 解决系统错误
///---------------------------------

/**
在后台上传任务返回为nil时是否试图重新请求，默认是NO
 @bug 在iOS7.0,有个在后台创建的一个上传任务有时会返回nil的bug，未解决这件事情，可以通过设置这个属性为YES，来尝试重新进行创建这个task.

 @see https://github.com/AFNetworking/AFNetworking/issues/1675
 */
@property (nonatomic, assign) BOOL attemptsToRecreateUploadTasksForBackgroundSessions;

///---------------------
/// @name 初始化
///---------------------

/**
 创建特别定制configuration并返回一个session的manager实例,这是一个指定的实例化构造器

 @param configuration 创建managed session的配置configuration.

 @return 返回 session的管理manager实例.
 */
- (instancetype)initWithSessionConfiguration:(nullable NSURLSessionConfiguration *)configuration NS_DESIGNATED_INITIALIZER;

/**
使当前会话无效， 通过取消请求任务

 @param cancelPendingTasks 是否取消请求任务.
 */
- (void)invalidateSessionCancelingTasks:(BOOL)cancelPendingTasks DEPRECATED_ATTRIBUTE;

/**
使当前会话无效，是否取消请求任务和是否重新设置另一个会话
 
 @param cancelPendingTasks  Whether or not to cancel pending tasks.
 @param resetSession        Whether or not to reset the session of the manager.
 */
- (void)invalidateSessionCancelingTasks:(BOOL)cancelPendingTasks resetSession:(BOOL)resetSession;

///-------------------------
/// @name Data Tasks
///-------------------------

/**
通过特定的请求创建一个‘NSURLSessionDataTask’
 @param request 网络请求的request.
 @param completionHandler 当任务结束时的一个block回调，回调中有三个参数: 服务器响应, 序列化处理后的响应，如果有错误发生将返回的错误.
 */
- (NSURLSessionDataTask *)dataTaskWithRequest:(NSURLRequest *)request
                            completionHandler:(nullable void (^)(NSURLResponse *response, id _Nullable responseObject,  NSError * _Nullable error))completionHandler DEPRECATED_ATTRIBUTE;

/**
 通过特定的请求创建一个‘NSURLSessionDataTask’

 @param request 网络请求的request.
 @param uploadProgressBlock 上传进度更新时进行的一个block回调。这个block是在session的队列进行响应的，而不是在主队列.
 @param downloadProgressBlock 下载进度更新时进行的一个block回调。这个block是在session的队列进行响应的，而不是在主队列.
 @param completionHandler 任务完成时的一个block回调  回调中有三个参数: 服务器响应, 序列化处理后的响应，如果有错误发生将返回的错误.
 */
- (NSURLSessionDataTask *)dataTaskWithRequest:(NSURLRequest *)request
                               uploadProgress:(nullable void (^)(NSProgress *uploadProgress))uploadProgressBlock
                             downloadProgress:(nullable void (^)(NSProgress *downloadProgress))downloadProgressBlock
                            completionHandler:(nullable void (^)(NSURLResponse *response, id _Nullable responseObject,  NSError * _Nullable error))completionHandler;

///---------------------------
/// @name 上传任务
///---------------------------

/**
 通过特定上传本地文件的请求创建一个‘NSURLSessionUploadTask’

 @param request 网络请求的request.
 @param fileURL 本地上传文件路径URL
 @param uploadProgressBlock 上传进度更新时进行的一个block回调。这个block是在session的队列进行响应的，而不是在主队列.
 @param completionHandler 任务完成时的一个block回调  回调中有三个参数: 服务器响应, 序列化处理后的响应，如果有错误发生将返回的错误.

 @see `attemptsToRecreateUploadTasksForBackgroundSessions`
 */
- (NSURLSessionUploadTask *)uploadTaskWithRequest:(NSURLRequest *)request
                                         fromFile:(NSURL *)fileURL
                                         progress:(nullable void (^)(NSProgress *uploadProgress))uploadProgressBlock
                                completionHandler:(nullable void (^)(NSURLResponse *response, id _Nullable responseObject, NSError  * _Nullable error))completionHandler;

/**
 通过特定HTTP body请求创建一个‘NSURLSessionUploadTask’

 @param request 网络请求的request.
 @param bodyData 将要上传的包含了HTTP body的数据类
 @param uploadProgressBlock 上传进度更新时进行的一个block回调。这个block是在session的队列进行响应的，而不是在主队列.
 @param completionHandler 任务完成时的一个block回调  回调中有三个参数: 服务器响应, 序列化处理后的响应，如果有错误发生将返回的错误.
 */
- (NSURLSessionUploadTask *)uploadTaskWithRequest:(NSURLRequest *)request
                                         fromData:(nullable NSData *)bodyData
                                         progress:(nullable void (^)(NSProgress *uploadProgress))uploadProgressBlock
                                completionHandler:(nullable void (^)(NSURLResponse *response, id _Nullable responseObject, NSError * _Nullable error))completionHandler;

/**
 通过特定流请求创建一个‘NSURLSessionUploadTask’

 @param request 网络请求的request.
 @param uploadProgressBlock 上传进度更新时进行的一个block回调。这个block是在session的队列进行响应的，而不是在主队列.
 @param completionHandler 任务完成时的一个block回调  回调中有三个参数: 服务器响应, 序列化处理后的响应，如果有错误发生将返回的错误.
 */
- (NSURLSessionUploadTask *)uploadTaskWithStreamedRequest:(NSURLRequest *)request
                                                 progress:(nullable void (^)(NSProgress *uploadProgress))uploadProgressBlock
                                        completionHandler:(nullable void (^)(NSURLResponse *response, id _Nullable responseObject, NSError * _Nullable error))completionHandler;

///-----------------------------
/// @name Running Download Tasks
///-----------------------------

/**
 通过特定请求创建一个‘NSURLSessionDownloadTask’

 @param request 网络请求的request..
 @param downloadProgressBlock 下载进度更新时进行的一个block回调。这个block是在session的队列进行响应的，而不是在主队列.
 @param destination 返回了包含待确定存储路径的下载文件的一个block回调，这个block有两个返回参数，存储路径和服务响应，返回一个下载结果的待确定临时target路径，如果确定了具体路径filePath并且移动过去，临时的路径下的文件将会被删除。
 @param completionHandler 任务完成时的一个block回调  回调中有三个参数: 服务器响应, 下载的存储路径，如果有错误发生将返回的错误. 

 @warning 如果在iOS中使用了后台的configuration下载配置，那么app退出这些回调将丢失，更推荐后台会话使用‘setDownloadTaskDidFinishDownloadingBlock’设置路径存储下载文件. 
 */
- (NSURLSessionDownloadTask *)downloadTaskWithRequest:(NSURLRequest *)request
                                             progress:(nullable void (^)(NSProgress *downloadProgress))downloadProgressBlock
                                          destination:(nullable NSURL * (^)(NSURL *targetPath, NSURLResponse *response))destination
                                    completionHandler:(nullable void (^)(NSURLResponse *response, NSURL * _Nullable filePath, NSError * _Nullable error))completionHandler;

/**
 通过特定流请求创建一个`NSURLSessionDownloadTask`.

 @param resumeData 网络请求的request..
 @param downloadProgressBlock 下载进度更新时进行的一个block回调。这个block是在session的队列进行响应的，而不是在主队列.
 @param destination 返回了包含待确定存储路径的下载文件的一个block回调，这个block有两个返回参数，存储路径和服务响应，返回一个下载结果的待确定临时target路径，如果确定了具体路径filePath并且移动过去，临时的路径下的文件将会被删除。
 @param completionHandler 任务完成时的一个block回调  回调中有三个参数: 服务器响应, 下载的存储路径，如果有错误发生将返回的错误. 
 */
- (NSURLSessionDownloadTask *)downloadTaskWithResumeData:(NSData *)resumeData
                                                progress:(nullable void (^)(NSProgress *downloadProgress))downloadProgressBlock
                                             destination:(nullable NSURL * (^)(NSURL *targetPath, NSURLResponse *response))destination
                                       completionHandler:(nullable void (^)(NSURLResponse *response, NSURL * _Nullable filePath, NSError * _Nullable error))completionHandler;

///---------------------------------
/// @name 获取任务进度
///---------------------------------

/**
 返回一个上传任务的进度

 @param task 上传的会话任务，不能为nil

 @return 返回 `NSProgress` 对象包含了上传任务的进度，如果为‘nil’那么这个进度将无效
 */
- (nullable NSProgress *)uploadProgressForTask:(NSURLSessionTask *)task;

/**
 返回一个下载任务的进度

 @param task 下载的会话任务，不能为nil

 @return 返回 `NSProgress` 对象包含了下载任务的进度，如果为‘nil’那么这个进度将无效
 */
- (nullable NSProgress *)downloadProgressForTask:(NSURLSessionTask *)task;

///-----------------------------------------
/// @name 设置会话代理的回调
///-----------------------------------------

/**
 设置一个block来捕捉会话的退出，等效于‘NSURLSessionDelegate’中的‘URLSession:didBecomeInvalidWithError:’方法

 @param block 来捕捉会话的退出的block,这个block无返回值，有两个参数，当前会话以及造成会话退出的错误信息
 */
- (void)setSessionDidBecomeInvalidBlock:(nullable void (^)(NSURLSession *session, NSError *error))block;

/**
 设置一个block来捕捉当一个网络连接的身份验证过程，等效于 `NSURLSessionDelegate` 的方法 `URLSession:didReceiveChallenge:completionHandler:`

 @param block 来捕捉网络连接的身份验证过程发生改变的block. 这个blocK返回了改变的身份验证节点，并且有三个参数，分别为会话，身份验证挑战，解决挑战的一个凭证
 */
- (void)setSessionDidReceiveAuthenticationChallengeBlock:(nullable NSURLSessionAuthChallengeDisposition (^)(NSURLSession *session, NSURLAuthenticationChallenge *challenge, NSURLCredential * _Nullable __autoreleasing * _Nullable credential))block;

///--------------------------------------
/// @name 设置会话代理的回调
///--------------------------------------

/**
 设置一个block来捕捉一个任务需要发送一个新的请求体给远程服务，等效于 `NSURLSessionTaskDelegate` 方法 `URLSession:task:needNewBodyStream:`.

 @param block 一个任务需要发送一个新的boy流给远程服务
 */
- (void)setTaskNeedNewBodyStreamBlock:(nullable NSInputStream * (^)(NSURLSession *session, NSURLSessionTask *task))block;

/**
 设置一个block来捕捉当一个请求试图重定向到另一个不同的URL，，等效于 `NSURLSessionTaskDelegate` 方法 `URLSession:willPerformHTTPRedirection:newRequest:completionHandler:`.

 @param block 当一个请求试图重定向到另一个不同的URL.返回一个新的重定向request，还有4个参数：会话、任务、返回的重定向结，重定向的请求
 */
- (void)setTaskWillPerformHTTPRedirectionBlock:(nullable NSURLRequest * _Nullable (^)(NSURLSession *session, NSURLSessionTask *task, NSURLResponse *response, NSURLRequest *request))block;

/**
 设置一个block来捕捉当一个网络会话任务接受的身份验证过程，等效于 `NSURLSessionTaskDelegate` 的方法 `URLSession:didReceiveChallenge:completionHandler:`

 @param block 来捕捉网络连接的身份验证过程发生改变的block. 这个blocK返回了改变的身份验证节点，并且有三个参数，分别为会话，身份验证挑战，解决挑战的一个凭证
 */
- (void)setTaskDidReceiveAuthenticationChallengeBlock:(nullable NSURLSessionAuthChallengeDisposition (^)(NSURLSession *session, NSURLSessionTask *task, NSURLAuthenticationChallenge *challenge, NSURLCredential * _Nullable __autoreleasing * _Nullable credential))block;

/**
 设置一个要定期执行的block来跟踪上传进度，，等效于 `NSURLSessionTaskDelegate` 的方法 `URLSession:task:didSendBodyData:totalBytesSent:totalBytesExpectedToSend

 @param block 当不确定的大量字节上传到服务器时将被被调用，这个block没有返回值，有4个参数：第一个为会话、第二个为任务task、第三个为当最后一次上传调用被执行时的字节数，总字节数、在执行期间还剩多少未上传的字节数。作为一个不确定长度的HTTP Body,这个block将可能被调用多次，并在主线程执行。
 */
- (void)setTaskDidSendBodyDataBlock:(nullable void (^)(NSURLSession *session, NSURLSessionTask *task, int64_t bytesSent, int64_t totalBytesSent, int64_t totalBytesExpectedToSend))block;

/**
 设置一个block捕捉特定任务相关的最后一条消息，等效于 `NSURLSessionTaskDelegate` 方法 `URLSession:task:didCompleteWithError:`.

 @param block 当会话任务执行完成时block将会被调用。这个block没有返回值，有3个参数：分别为会话，任务以及错误。
 */
- (void)setTaskDidCompleteBlock:(nullable void (^)(NSURLSession *session, NSURLSessionTask *task, NSError * _Nullable error))block;

/**
 设置一个block来捕捉当特定任务的所有指标全部完成，等效于 `NSURLSessionTaskDelegate` 方法 `URLSession:task:didFinishCollectingMetrics:`.

 @param block 当会话任务执行完成时block将会被调用。这个block没有返回值，有3个参数：分别为会话，任务以及错误。
 */
#if AF_CAN_INCLUDE_SESSION_TASK_METRICS
- (void)setTaskDidFinishCollectingMetricsBlock:(nullable void (^)(NSURLSession *session, NSURLSessionTask *task, NSURLSessionTaskMetrics * _Nullable metrics))block AF_API_AVAILABLE(ios(10), macosx(10.12), watchos(3), tvos(10));
#endif
///-------------------------------------------
/// @name Setting Data Task Delegate Callbacks
///-------------------------------------------

/**
 设置一个block来捕捉当任务请求接受到响应时进行回调，等效于`NSURLSessionDataDelegate` 方法 `URLSession:dataTask:didReceiveResponse:completionHandler:`.

 @param block 当就收到响应时回调
 */
- (void)setDataTaskDidReceiveResponseBlock:(nullable NSURLSessionResponseDisposition (^)(NSURLSession *session, NSURLSessionDataTask *dataTask, NSURLResponse *response))block;

/**
  设置一个数据任务变成一个下载任务，等效于NSURLSessionDataDelegate 方法‘URLSession:dataTask:didBecomeDownloadTask:’
 @param block 数据任务变成一个下载任务
 */
- (void)setDataTaskDidBecomeDownloadTaskBlock:(nullable void (^)(NSURLSession *session, NSURLSessionDataTask *dataTask, NSURLSessionDownloadTask *downloadTask))block;

/**
 设置一个数据任务接收到数据时的响应，等效于`NSURLSessionDataDelegate` 方法 `URLSession:dataTask:didReceiveData:`.
 @param block 数据任务接收到数据时的响应 这里有三个参数：会话、数据任务、接受的数据
 */
- (void)setDataTaskDidReceiveDataBlock:(nullable void (^)(NSURLSession *session, NSURLSessionDataTask *dataTask, NSData *data))block;

/**
  设置一个block用于捕捉即将要进入缓存数据，已确定数据任务的缓存行为。等效于`NSURLSessionDataDelegate` 方法 `URLSession:dataTask:willCacheResponse:completionHandler:`.
 */
- (void)setDataTaskWillCacheResponseBlock:(nullable NSCachedURLResponse * (^)(NSURLSession *session, NSURLSessionDataTask *dataTask, NSCachedURLResponse *proposedResponse))block;

/**
 设置一个block，当会话排队的所有消息完成时，该block被执行, as handled by the `NSURLSessionDataDelegate` method `URLSessionDidFinishEventsForBackgroundURLSession:`.

 @param block A block object to be executed once all messages enqueued for a session have been delivered. The block has no return value and takes a single argument: the session.
 */
- (void)setDidFinishEventsForBackgroundURLSessionBlock:(nullable void (^)(NSURLSession *session))block AF_API_UNAVAILABLE(macos);

///-----------------------------------------------
/// @name Setting Download Task Delegate Callbacks
///-----------------------------------------------

/**
 设置下载任务完成下载后要执行的块方法

 @param block 这个block返回一个下载即将被移入的地址，4个参数分别为：会话、下载任务、临时的下载文件路径。如果文件管理器在尝试移动这个临时文件时发生了错误，那么将会发出一个‘AFURLSessionDownloadTaskDidFailToMoveFileNotification’通知，这个下载任务作为object,userInfo是这个error
 */
- (void)setDownloadTaskDidFinishDownloadingBlock:(nullable NSURL * _Nullable  (^)(NSURLSession *session, NSURLSessionDownloadTask *downloadTask, NSURL *location))block;

/**
 设置一个block来跟踪下载任务，等效于`NSURLSessionDownloadDelegate` method `URLSession:downloadTask:didWriteData:totalBytesWritten:totalBytesExpectedToWrite:`.
 @param block 当不确定数量的字节数从服务器下载下来时，这个block将会被调用。这个block没有返回值，有4个参数：会话、下载任务、这一次block执行时的字节数，总字节数、还未下载的字节数。作为NSHTTPURLResponse明确的大小，这个block将会被执行多次，并且会在会话管理的队列上执行
 */
- (void)setDownloadTaskDidWriteDataBlock:(nullable void (^)(NSURLSession *session, NSURLSessionDownloadTask *downloadTask, int64_t bytesWritten, int64_t totalBytesWritten, int64_t totalBytesExpectedToWrite))block;

/**
 Sets a block to be executed when a download task has been resumed, as handled by the `NSURLSessionDownloadDelegate` method `URLSession:downloadTask:didResumeAtOffset:expectedTotalBytes:`.
设置下载任务恢复后要执行的block
 @param block 包含了4个参数
 */
- (void)setDownloadTaskDidResumeBlock:(nullable void (^)(NSURLSession *session, NSURLSessionDownloadTask *downloadTask, int64_t fileOffset, int64_t expectedTotalBytes))block;

@end

///--------------------
/// @name Notifications
///--------------------

/**
 当任务恢复时
 */
FOUNDATION_EXPORT NSString * const AFNetworkingTaskDidResumeNotification;

/**
 Posted when a task finishes executing. Includes a userInfo dictionary with additional information about the task.
 任务完成时，包含了一个userInfo的字典关于这个任务的信息
 */
FOUNDATION_EXPORT NSString * const AFNetworkingTaskDidCompleteNotification;

/**
 Posted when a task suspends its execution.
 当暂停时
 */
FOUNDATION_EXPORT NSString * const AFNetworkingTaskDidSuspendNotification;

/**
 Posted when a session is invalidated.
 当会话销毁时
 */
FOUNDATION_EXPORT NSString * const AFURLSessionDidInvalidateNotification;

/**
 Posted when a session download task encountered an error when moving the temporary download file to a specified destination.
 下载会话任务在文件管理器移动文件过程中发生了一个错误
 */
FOUNDATION_EXPORT NSString * const AFURLSessionDownloadTaskDidFailToMoveFileNotification;

/**
 The raw response data of the task. Included in the userInfo dictionary of the `AFNetworkingTaskDidCompleteNotification` if response data exists for the task.
 任务响应数据
 */
FOUNDATION_EXPORT NSString * const AFNetworkingTaskDidCompleteResponseDataKey;

/**
 The serialized response object of the task. Included in the userInfo dictionary of the `AFNetworkingTaskDidCompleteNotification` if the response was serialized.
 */
FOUNDATION_EXPORT NSString * const AFNetworkingTaskDidCompleteSerializedResponseKey;

/**
 The response serializer used to serialize the response. Included in the userInfo dictionary of the `AFNetworkingTaskDidCompleteNotification` if the task has an associated response serializer.
 */
FOUNDATION_EXPORT NSString * const AFNetworkingTaskDidCompleteResponseSerializerKey;

/**
 The file path associated with the download task. Included in the userInfo dictionary of the `AFNetworkingTaskDidCompleteNotification` if an the response data has been stored directly to disk.
 */
FOUNDATION_EXPORT NSString * const AFNetworkingTaskDidCompleteAssetPathKey;

/**
 Any error associated with the task, or the serialization of the response. Included in the userInfo dictionary of the `AFNetworkingTaskDidCompleteNotification` if an error exists.
 */
FOUNDATION_EXPORT NSString * const AFNetworkingTaskDidCompleteErrorKey;

/**
 The session task metrics taken from the download task. Included in the userInfo dictionary of the `AFNetworkingTaskDidCompleteSessionTaskMetrics`
 */
FOUNDATION_EXPORT NSString * const AFNetworkingTaskDidCompleteSessionTaskMetrics;

NS_ASSUME_NONNULL_END
