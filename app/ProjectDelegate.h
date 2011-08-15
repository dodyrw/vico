#import "ViRegexp.h"
#import "ViToolbarPopUpButtonCell.h"
#import "ViOutlineView.h"
#import "ViJumpList.h"

#include <CoreServices/CoreServices.h>

@class ViWindowController;
@class ExEnvironment;

@interface ProjectFile : NSObject
{
	NSURL *url;
	NSDictionary *attributes;
	double score;
	NSMutableArray *children;
	NSString *name;
	NSImage *icon;
	BOOL nameIsDirty, iconIsDirty;
	BOOL isDirectory;
}
@property(nonatomic,readwrite, assign) double score;
@property(nonatomic,readonly) NSURL *url;
@property(nonatomic,readwrite, assign) NSMutableArray *children;
@property(nonatomic,readonly) BOOL isDirectory;
- (void)setURL:(NSURL *)aURL;
- (BOOL)hasCachedChildren;
- (NSString *)name;
- (NSImage *)icon;
@end

@interface ProjectDelegate : NSObject <NSOutlineViewDataSource, NSOutlineViewDelegate, ViJumpListDelegate, ViKeyManagerTarget>
{
	IBOutlet NSWindow *window;
	IBOutlet ExEnvironment *environment;
	IBOutlet ViWindowController *windowController;
	IBOutlet ViOutlineView *explorer;
	IBOutlet NSMenu *actionMenu;
	IBOutlet NSSearchField *filterField;
	IBOutlet NSSplitView *splitView;
	IBOutlet NSView *explorerView;
	IBOutlet NSWindow *sftpConnectView;
	IBOutlet NSForm *sftpConnectForm;
	IBOutlet NSScrollView *scrollView;
	IBOutlet ViToolbarPopUpButtonCell *actionButtonCell;
	IBOutlet NSPopUpButton *actionButton;
	IBOutlet NSProgressIndicator *progressIndicator;
	IBOutlet NSToolbarItem *searchToolbarItem;

	NSURL *rootURL;
	CGFloat width;
	NSFont *font;

	// remembering expanded state
	NSMutableSet *expandedSet;
	BOOL isExpandingTree;

	// incremental file filtering
	NSMutableArray *filteredItems;
	NSMutableArray *itemsToFilter;
	ViRegexp *rx;

	BOOL closeExplorerAfterUse;
	BOOL hideToolbarAfterUse;
	IBOutlet id delegate;
	NSMutableArray *rootItems;
	ViRegexp *skipRegex;

	BOOL isFiltered;
	BOOL isFiltering;

	ViJumpList *history;

        /*
         * Since we can't pass an object through a void* contextInfo and
         * expect the object to survive garbage collection, store a strong
         * reference here.
         */
	NSMutableSet *contextObjects;
}

@property(nonatomic,readwrite,assign) id delegate;
@property(nonatomic,readonly) ViOutlineView *outlineView;

- (void)browseURL:(NSURL *)aURL andDisplay:(BOOL)display;
- (void)browseURL:(NSURL *)aURL;
- (IBAction)addSFTPLocation:(id)sender;
- (IBAction)actionMenu:(id)sender;

- (IBAction)openInTab:(id)sender;
- (IBAction)openInCurrentView:(id)sender;
- (IBAction)openInSplit:(id)sender;
- (IBAction)openInVerticalSplit:(id)sender;
- (IBAction)renameFile:(id)sender;
- (IBAction)removeFiles:(id)sender;
- (IBAction)rescan:(id)sender;
- (IBAction)revealInFinder:(id)sender;
- (IBAction)openWithFinder:(id)sender;
- (IBAction)newFolder:(id)sender;
- (IBAction)newDocument:(id)sender;
- (IBAction)bookmarkFolder:(id)sender;
- (IBAction)gotoBookmark:(id)sender;

- (IBAction)acceptSftpSheet:(id)sender;
- (IBAction)cancelSftpSheet:(id)sender;

- (IBAction)filterFiles:(id)sender;
- (IBAction)searchFiles:(id)sender;
- (BOOL)explorerIsOpen;
- (void)openExplorerTemporarily:(BOOL)temporarily;
- (void)closeExplorer;
- (IBAction)focusExplorer:(id)sender;
- (IBAction)toggleExplorer:(id)sender;
- (void)cancelExplorer;
- (BOOL)isEditing;
- (BOOL)displaysURL:(NSURL *)aURL;

@end
