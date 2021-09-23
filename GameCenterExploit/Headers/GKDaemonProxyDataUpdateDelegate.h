
@protocol GKDaemonProxyDataUpdateDelegate <NSObject>

@optional

- (void)friendRequestSelected:(GKFriendRequestInternal *)arg1;
- (void)refreshContentsForDataType:(unsigned int)arg1 userInfo:(NSDictionary *)arg2;
- (void)setBadgeCount:(unsigned long long)arg1 forType:(unsigned long long)arg2;

@end
