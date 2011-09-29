#import "ViFileURLHandler.h"
#import "ViError.h"
#import "ViFile.h"
#import "NSURL-additions.h"
#include "logging.h"

@interface ViFileDeferred : NSObject <ViDeferred>
{
	id<ViDeferredDelegate> delegate;
}
@end

@implementation ViFileDeferred
@synthesize delegate;
+ (ViFileDeferred *)sharedDeferred
{
	static ViFileDeferred *__sharedDeferred = nil;
	if (__sharedDeferred == nil)
		__sharedDeferred = [[ViFileDeferred alloc] init];
	return __sharedDeferred;
}
- (void)cancel
{
}
- (void)wait
{
}
@end

@implementation ViFileURLHandler

- (id)init
{
	if ((self = [super init]) != nil) {
		_fm = [[NSFileManager alloc] init];
	}

	return self;
}

- (void)dealloc
{
	[_fm release];
	[super dealloc];
}

- (BOOL)respondsToURL:(NSURL *)aURL
{
	return [aURL isFileURL];
}

- (id<ViDeferred>)contentsOfDirectoryAtURL:(NSURL *)aURL
			      onCompletion:(void (^)(NSArray *contents, NSError *error))aBlock
{
	DEBUG(@"url = %@", aURL);

	if (aURL == nil) {
		aBlock(nil, [ViError errorWithFormat:@"Invalid argument."]);
		return nil;
	}

	/*
	 * The aBlock argument is typically a stack block literal and
	 * can't be automatically retained without moving it to the
	 * heap.
	 */
	void (^blockCopy)(NSArray *, NSError *) = [[aBlock copy] autorelease];

	dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_LOW, 0), ^{
		NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
		NSFileManager *fileman = [[NSFileManager alloc] init];
		NSError *error = nil;
		NSArray *files = [fileman contentsOfDirectoryAtPath:[aURL path] error:&error];
		NSMutableArray *contents = [NSMutableArray array];
		for (NSString *filename in files) {
			NSDictionary *attrs, *symattrs = nil;
			NSURL *symurl = nil;
			NSURL *u = [aURL URLByAppendingPathComponent:filename];
			NSString *p = [u path];

			attrs = [fileman attributesOfItemAtPath:p error:&error];
			BOOL isAlias = NO;
			symurl = [u URLByResolvingSymlinksAndAliases:&isAlias];
			if (attrs && (isAlias || [[attrs fileType] isEqualToString:NSFileTypeSymbolicLink])) {
				if (symurl)
					symattrs = [fileman attributesOfItemAtPath:[symurl path] error:&error];
			}

			if (attrs)
				[contents addObject:[ViFile fileWithURL:u
							     attributes:attrs
							   symbolicLink:symurl
						     symbolicAttributes:symattrs]];
			else if (error)
				break;
		}
		[fileman release];

		/* Schedule completion block on main queue. */
		dispatch_sync(dispatch_get_main_queue(), ^{
			blockCopy(contents, error);
		});
		[pool drain];
	});

	return [ViFileDeferred sharedDeferred];
}

- (id<ViDeferred>)createDirectoryAtURL:(NSURL *)aURL
			  onCompletion:(void (^)(NSError *error))aBlock
{
	DEBUG(@"url = %@", aURL);
	NSError *error = nil;
	[_fm createDirectoryAtPath:[aURL path]
       withIntermediateDirectories:YES
			attributes:nil
			     error:&error];
	aBlock(error);
	return nil;
}

- (NSURL *)normalizeURL:(NSURL *)aURL
{
	NSString *path = [aURL relativePath];
	if ([path length] == 0)
		path = NSHomeDirectory();
	else if ([path hasPrefix:@"~"])
		path = [NSHomeDirectory() stringByAppendingPathComponent:[path substringFromIndex:1]];
	else if ([path hasPrefix:@"/~"])
		path = [NSHomeDirectory() stringByAppendingPathComponent:[path substringFromIndex:2]];
	else
		return [[aURL URLByStandardizingPath] absoluteURL];
	return [[[NSURL fileURLWithPath:path] URLByStandardizingPath] absoluteURL];
}

- (NSString *)stringByAbbreviatingWithTildeInPath:(NSURL *)aURL
{
	return [[aURL path] stringByAbbreviatingWithTildeInPath];
}

- (id<ViDeferred>)attributesOfItemAtURL:(NSURL *)aURL
			   onCompletion:(void (^)(NSURL *, NSDictionary *, NSError *))aBlock
{
	DEBUG(@"url = %@", aURL);
	NSError *error = nil;
	NSURL *url = [self normalizeURL:aURL];
	NSDictionary *attributes = [_fm attributesOfItemAtPath:[url path] error:&error];
	if (error || attributes == nil)
		aBlock(nil, nil, error);
	else
		aBlock(url, attributes, nil);
	return nil;
}

- (id<ViDeferred>)fileExistsAtURL:(NSURL *)aURL
		     onCompletion:(void (^)(NSURL *, BOOL, NSError *))aBlock
{
	DEBUG(@"url = %@", aURL);
	BOOL result, isDirectory;
	NSURL *url = [self normalizeURL:aURL];
	result = [_fm fileExistsAtPath:[url path] isDirectory:&isDirectory];
	aBlock(result ? url : nil, isDirectory, nil);
	return nil;
}

- (id<ViDeferred>)moveItemAtURL:(NSURL *)srcURL
			  toURL:(NSURL *)dstURL
		   onCompletion:(void (^)(NSURL *, NSError *))aBlock
{
	DEBUG(@"%@ -> %@", srcURL, dstURL);
	NSError *error = nil;
	[_fm moveItemAtURL:srcURL toURL:dstURL error:&error];
	aBlock([dstURL URLByResolvingSymlinksInPath], error);
	return nil;
}

- (id<ViDeferred>)removeItemsAtURLs:(NSArray *)urls
		       onCompletion:(void (^)(NSError *))aBlock
{
	NSWorkspace *workspace = [NSWorkspace sharedWorkspace];
	[workspace recycleURLs:urls completionHandler:^(NSDictionary *newURLs, NSError *error) {
		aBlock(error);
	}];
	return nil;
}

#if 0
- (id<ViDeferred>)dataWithContentsOfURL:(NSURL *)aURL
			   onCompletion:(void (^)(NSData *data, NSError *error))aBlock
{
	DEBUG(@"url = %@", aURL);
	NSError *error = nil;
	NSData *data;
	data = [NSData dataWithContentsOfFile:[aURL path]
				      options:0
					error:&error];
	aBlock(data, error);
	return nil;
}
#endif

- (id<ViDeferred>)writeDataSafely:(NSData *)data
			    toURL:(NSURL *)aURL
		     onCompletion:(void (^)(NSURL *, NSDictionary *, NSError *))aBlock
{
	DEBUG(@"url = %@", aURL);
	NSError *error = nil;
	[data writeToURL:aURL options:NSDataWritingAtomic error:&error];
	NSURL *normalizedURL = [self normalizeURL:aURL];
	NSDictionary *attributes = [_fm attributesOfItemAtPath:[normalizedURL path] error:&error];
	aBlock(normalizedURL, attributes, error);
	return nil;
}

@end
