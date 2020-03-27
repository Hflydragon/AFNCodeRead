// AFNetworkReachabilityManager.h

#import <Foundation/Foundation.h>

#if !TARGET_OS_WATCH
#import <SystemConfiguration/SystemConfiguration.h>

typedef NS_ENUM(NSInteger, AFNetworkReachabilityStatus) {
    AFNetworkReachabilityStatusUnknown          = -1,//未知网络
    AFNetworkReachabilityStatusNotReachable     = 0,//没有网络
    AFNetworkReachabilityStatusReachableViaWWAN = 1,//蜂窝数据（自己的流量）
    AFNetworkReachabilityStatusReachableViaWiFi = 2,//WiFi
};

NS_ASSUME_NONNULL_BEGIN

/**
 `AFNetworkReachabilityManager` 检测网络WWAN 和 WiFi 域名，地址是否可用

 可行性将用于确定网络接连失败的原因，或者在建立网络连接时进行网络请求重试，这不能阻止用户发起一个网络请求，因为可能用户重新发起请求才能确定网络是否可用

 See Apple's Reachability Sample Code ( https://developer.apple.com/library/ios/samplecode/reachability/ )

 @warning Instances of `AFNetworkReachabilityManager` must be started with `-startMonitoring` before reachability status can be determined.
 */
@interface AFNetworkReachabilityManager : NSObject

/**
 当前的网络状态
 */
@property (readonly, nonatomic, assign) AFNetworkReachabilityStatus networkReachabilityStatus;

/**
 当前的网络状态是否可用
 */
@property (readonly, nonatomic, assign, getter = isReachable) BOOL reachable;

/**
 当前的网络状态是否为可用的蜂窝数据
 */
@property (readonly, nonatomic, assign, getter = isReachableViaWWAN) BOOL reachableViaWWAN;

/**
 当前的网络状态是否为可用的WiFi
 */
@property (readonly, nonatomic, assign, getter = isReachableViaWiFi) BOOL reachableViaWiFi;

///---------------------
/// @name Initialization
///---------------------

/**
 返回网络监测管理类，这是一个单例
 */
+ (instancetype)sharedManager;

/**
 返回一个包含默认socket地址的网络监测类
 
 */
+ (instancetype)manager;

/**
 创建并返回指定域的网络监测管理器。

 @param domain 这个作用域将用于监测网络可用性
 */
+ (instancetype)managerForDomain:(NSString *)domain;

/**
 返回一个特定socket地址的网络监测类

 @param address (`sockaddr_in6`) 特定的socket地址
 */
+ (instancetype)managerForAddress:(const void *)address;

/**
 通过特定的reachability对象返回一个可用性管理类

 @param reachability The reachability object to monitor.

 @return An initialized network reachability manager, actively monitoring the specified reachability.
 */
- (instancetype)initWithReachability:(SCNetworkReachabilityRef)reachability NS_DESIGNATED_INITIALIZER;

/**
 *  不可用初始化
 */
+ (instancetype)new NS_UNAVAILABLE;

/**
 * 不可用初始化
 */
- (instancetype)init NS_UNAVAILABLE;

///--------------------------------------------------
/// @name Starting & Stopping Reachability Monitoring
///--------------------------------------------------

/**
 开始监测
 */
- (void)startMonitoring;

/**
 停止监测
 */
- (void)stopMonitoring;

///-------------------------------------------------
/// @name Getting Localized Reachability Description
///-------------------------------------------------

/**
 返回一个描述当前网状态的字符串
 */
- (NSString *)localizedNetworkReachabilityStatusString;

///---------------------------------------------------
/// @name 网络状态改变的回调
///---------------------------------------------------

/**
 当网络状态发生改变时，将主动返回回调

 @param block A block object to be executed when the network availability of the `baseURL` host changes.. This block has no return value and takes a single argument which represents the various reachability states from the device to the `baseURL`.
 */
- (void)setReachabilityStatusChangeBlock:(nullable void (^)(AFNetworkReachabilityStatus status))block;

@end

///----------------
/// @name Constants
///----------------

/**
 ## Network Reachability

 The following constants are provided by `AFNetworkReachabilityManager` as possible network reachability statuses.

 enum {
 AFNetworkReachabilityStatusUnknown,
 AFNetworkReachabilityStatusNotReachable,
 AFNetworkReachabilityStatusReachableViaWWAN,
 AFNetworkReachabilityStatusReachableViaWiFi,
 }

 `AFNetworkReachabilityStatusUnknown`
 The `baseURL` host reachability is not known.

 `AFNetworkReachabilityStatusNotReachable`
 The `baseURL` host cannot be reached.

 `AFNetworkReachabilityStatusReachableViaWWAN`
 The `baseURL` host can be reached via a cellular connection, such as EDGE or GPRS.

 `AFNetworkReachabilityStatusReachableViaWiFi`
 The `baseURL` host can be reached via a Wi-Fi connection.

 ### Keys for Notification UserInfo Dictionary

 Strings that are used as keys in a `userInfo` dictionary in a network reachability status change notification.

 `AFNetworkingReachabilityNotificationStatusItem`
 A key in the userInfo dictionary in a `AFNetworkingReachabilityDidChangeNotification` notification.
 The corresponding value is an `NSNumber` object representing the `AFNetworkReachabilityStatus` value for the current reachability status.
 */

///--------------------
/// @name Notifications
///--------------------

/**
 Posted when network reachability changes.
 This notification assigns no notification object. The `userInfo` dictionary contains an `NSNumber` object under the `AFNetworkingReachabilityNotificationStatusItem` key, representing the `AFNetworkReachabilityStatus` value for the current network reachability.

 @warning In order for network reachability to be monitored, include the `SystemConfiguration` framework in the active target's "Link Binary With Library" build phase, and add `#import <SystemConfiguration/SystemConfiguration.h>` to the header prefix of the project (`Prefix.pch`).
 */
FOUNDATION_EXPORT NSString * const AFNetworkingReachabilityDidChangeNotification;
FOUNDATION_EXPORT NSString * const AFNetworkingReachabilityNotificationStatusItem;

///--------------------
/// @name Functions
///--------------------

/**
 Returns a localized string representation of an `AFNetworkReachabilityStatus` value.
 */
FOUNDATION_EXPORT NSString * AFStringFromNetworkReachabilityStatus(AFNetworkReachabilityStatus status);

NS_ASSUME_NONNULL_END
#endif
