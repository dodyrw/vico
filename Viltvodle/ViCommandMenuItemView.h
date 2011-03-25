@interface ViCommandMenuItemView : NSView
{
	NSMutableDictionary	*attributes;
	NSString		*title;
	NSString		*command;
	NSSize			 titleSize, commandSize;
	NSColor			*disabledColor;
	NSColor			*highlightColor;
	NSColor			*normalColor;
}

@property (readonly) NSString *command;
@property (readonly) NSString *title;

- (void)setCommand:(NSString *)aCommand;
- (void)setTabTrigger:(NSString *)aTabTrigger;
- (void)setTitle:(NSString *)aTitle;
- (id)initWithTitle:(NSString *)aTitle command:(NSString *)aCommand font:(NSFont *)aFont;
- (id)initWithTitle:(NSString *)aTitle tabTrigger:(NSString *)aTabTrigger font:(NSFont *)aFont;

@end