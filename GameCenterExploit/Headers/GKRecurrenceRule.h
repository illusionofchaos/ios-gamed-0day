
@interface GKRecurrenceRule : NSObject <NSCopying, NSSecureCoding> {
    long long  _frequency;
    unsigned long long  _interval;
}

@property long long frequency;
@property unsigned long long interval;

+ (bool)supportsSecureCoding;

- (id)copyWithZone:(struct _NSZone { }*)arg1;
- (void)encodeWithCoder:(id)arg1;
- (long long)frequency;
- (id)initWithCoder:(id)arg1;
- (id)initWithInterval:(unsigned long long)arg1 frequency:(long long)arg2;
- (unsigned long long)interval;
- (void)setFrequency:(long long)arg1;
- (void)setInterval:(unsigned long long)arg1;

@end
