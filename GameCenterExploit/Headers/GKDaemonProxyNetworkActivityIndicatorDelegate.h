
@protocol GKDaemonProxyNetworkActivityIndicatorDelegate <NSObject>

@required

- (void)beginNetworkActivity;
- (void)endNetworkActivity;
- (void)resetNetworkActivity;

@end
