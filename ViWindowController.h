#import <Cocoa/Cocoa.h>
#import "ViTagStack.h"

@class PSMTabBarControl;
@class ViDocument;
@class ViDocumentView;
@class ProjectDelegate;
@class ViResizeView;
@class ViJumpList;

@interface ViWindowController : NSWindowController
{
	IBOutlet PSMTabBarControl *tabBar;
	IBOutlet NSSplitView *splitView;
	IBOutlet NSView *documentView;
	IBOutlet NSToolbar *toolbar;

	ViTagStack *tagStack;
	BOOL isLoaded;
	ViDocument *initialDocument;
	ViDocument *lastDocument;
	ViDocumentView *lastDocumentView;
	ViDocument *selectedDocument;
	ViDocument *mostRecentDocument;
	ViDocumentView *mostRecentView;

	// command output view
	IBOutlet NSTextField *statusbar;
	IBOutlet NSSplitView *commandSplit;
	IBOutlet NSTextView *commandOutput;

	// project list
	IBOutlet NSOutlineView *projectOutline;
	IBOutlet ProjectDelegate *projectDelegate;
	IBOutlet NSImageView *projectResizeView;

	// symbol list
	IBOutlet NSImageView *symbolsResizeView;
	IBOutlet NSView *symbolsView;
	IBOutlet NSSearchField *symbolFilterField;
	IBOutlet NSOutlineView *symbolsOutline;
	NSCell *separatorCell;
	NSMutableArray *documents;
	NSMutableArray *filteredDocuments;
	NSMutableDictionary *symbolFilterCache;
	BOOL closeSymbolListAfterUse;

	ViJumpList *jumpList;
}

@property(readwrite, assign) NSMutableArray *documents;
@property(readwrite, assign) ViDocument *selectedDocument;
@property(readonly) NSTextField *statusbar;

+ (id)currentWindowController;
+ (NSWindow *)currentMainWindow;

- (void)setMostRecentDocument:(ViDocument *)document view:(ViDocumentView *)docView;
- (void)selectDocument:(ViDocument *)aDocument;
- (void)closeDocumentViews:(ViDocument *)aDocument;
- (void)addNewTab:(ViDocument *)document;
- (ViDocument *)currentDocument;

- (IBAction)saveProject:(id)sender;

- (ViTagStack *)sharedTagStack;

- (IBAction)selectNextTab:(id)sender;
- (IBAction)selectPreviousTab:(id)sender;

- (void)switchToLastFile;
- (void)gotoURL:(NSURL *)url line:(NSUInteger)line column:(NSUInteger)column;

- (IBAction)searchSymbol:(id)sender;
- (IBAction)filterSymbols:(id)sender;
- (IBAction)toggleSymbolList:(id)sender;
- (IBAction)splitViewHorizontally:(id)sender;

- (void)updateSelectedSymbolForLocation:(NSUInteger)aLocation;

@end

