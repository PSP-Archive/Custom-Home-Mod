#ifndef LIBMENU_H
#define LIBMENU_H

#include <stdbool.h>
#include <pspctrl.h>


#ifdef __cplusplus
extern "C" {
#endif



// used by libmInitBuffers.
enum
{
	LIBM_DRAW_INIT8888	= 0x01 ,
	LIBM_DRAW_INIT4444	= 0x02 ,
	LIBM_DRAW_INIT5650	= 0x04 ,
	LIBM_DRAW_INIT5551	= 0x08 ,
	LIBM_DRAW_BLEND		= 0x10 ,
	LIBM_DRAW_RETURN	= 0x20 ,
};

#define	LIBM_FMT_MASK			(LIBM_DRAW_INIT8888 | LIBM_DRAW_INIT4444 | LIBM_DRAW_INIT5650 | LIBM_DRAW_INIT5551)


#define	LIBM_DRAW_INIT			(LIBM_FMT_MASK | LIBM_DRAW_RETURN)

// Max X and Y. used libmDebugScreenSetXY.
#define	LIBM_SETX_MAX			59
#define	LIBM_SETY_MAX			33

// font size
#define LIBM_CHAR_WIDTH			8
#define LIBM_CHAR_HEIGHT		8

#define	LIBM_NO_DRAW			0

#define	LIBM_VIEW_ALL			false
#define	LIBM_VIEW_ACTIVE		true

// font load
#define LIBM_FONT_CG 1
#define LIBM_FONT_HANKAKU_KANA 2
#define LIBM_FONT_SJIS 3
#define LIBM_FONT_ICON 4

typedef enum
{
	MenuContainer,
	RadioButton,
	ToggleButton,
	CheckBox,
	Spacer,
	SelectBox,
	UpDownCtrl,
	TriggerButton,
	RadioButtonGroup
} MenuItemType;



// State of the menu which get from MenuParams.Action after calling the libmGetHandle.
typedef enum
{
	Menu_None = 0,	// none
	Menu_Show,		// The menu was displayed.
	Menu_Up,		// The MenuContext.HotKey.Up was pushed.
	Menu_Down,		// The MenuContext.HotKey.Down was pushed.
	Menu_Left,		// The MenuContext.HotKey.Left was pushed.
	Menu_Right,		// The MenuContext.HotKey.Right was pushed.
	Menu_Select,	// The MenuContext.HotKey.Select was pushed.
	Menu_Back,		// The MenuContext.HotKey.Back was pushed.
	Menu_Close		// The menu was closed.
} MenuResult;


// libmOpt, type
typedef enum
{
	LIBM_OPT_NONE,	//Disable
	LIBM_OPT_LIST,	//Stack
	LIBM_OPT_FUNC,	//Heap
} optType;



typedef struct MenuItem
{
	struct MenuItem			*Parent;		// parent of the item (NULL = Root)
	struct MenuItem			*Next;			// next item
	struct MenuItem			*Prev;			// prev item
	struct MenuItem			*Children;		// children of the item
	
	
	const char				*Name;			// item name (be displayed)
	int						Type;			// type of the item
	int						actionID;		// actionID
	int						Data;			// value of the item
	
	bool					Visible;		// visible of the item (true, false)
	bool					Disable;		// state of Enable/Disable of the item
	
	
	// color of inactive items
	struct
	{
		u32 				Font;			// font
		u32 				Back;			// background
		u32					Line;			// underline
	}Color;
	
	
	union
	{
		// RadioButton
		struct MenuItem		*RB_Group;		// pointer of group
		
		// RadioButtonGroup
		struct MenuItem		*Group_Sel;		// pointer of selected item
		
		// Container
		struct
		{
			bool			IsOpen;			// state of open/close
			struct MenuItem	*Disp;			// pointer of associated item
		}CNT;
		
		// CheckBox
		bool				CB_Checked; 	// state of select
		
		// SelectBox
		struct
		{
			const char		**List;			// pointer to list
			int				Index;			// selected index
		}SB;
		
		// UpDownControl
		struct
		{
			float			Now;			// now value
			float			Step;			// step value (+/-)
			float			Min;			// minimum value
			float			Max;			// maximum value
			bool			type;			// type (true = integer, false = decimal
		}UD;
		
	}Ctrl;
	
} MenuItem;


typedef struct MenuAction
{
	MenuResult	Action;
	MenuItem	*Item;
} MenuParams;


typedef struct
{
	struct
	{
		// case of using heap
		void* (*malloc_p)(SceSize size) ;
		void  (*free_p) (void *ptr) ;
	}func;
	
	struct
	{
		// case of using stack
		MenuItem *val;
		int size;
	} list;
	
	// type of allocating mamory for items
	optType type;	//LIBM_OPT_LIST -> stack, LIBM_OPT_FUNC -> heap
	
	int count;
	
} libmOpt;



typedef struct MenuContext
{
	// control key
	struct
	{
		u32 				Show;			// show key
		u32					Back;			// back key
		u32					Select;			// select key
		u32					Up;				// up key
		u32					Down;			// down key
		u32					Left;			// left key
		u32					Right;			// right key
	}HotKey;
	
	struct
	{
		u32					Active_Font;	// font color of active item
		u32					Active_Back;	// background color of active item
		
		u32					Normal_Font;	// font color of inactive item
		u32					Normal_Back;	// background color of inactive item
		
		u32					Disable_Font;	// font color of disable item
		u32					Disable_Back;	// background color of disable item
	}Color;
	
	// settings of menu display
	struct
	{
		bool				Type;			// menu type (true = display only active item, false = all )
		bool				AutoReturn;		// auto return on top and bottom
		u32					Lines;			// between the lines (default = 1)
	}MenuInfo;
	
	MenuParams				Params;
	
	bool 					IsOpen;			// open/close
	SceCtrlData				LastState;		// holds last pad data
	MenuItem				*Root;			// pointer to root item
	MenuItem				*Active;		// pointer to active item
	
	libmOpt *opt;
	
} MenuContext;




typedef struct
{
	void	*buffer;	// vram address
	
	int	width;			// display width
	int	height;			// display height
	
	int	format;			// pixel format
	int	lineWidth;		// buffer width
	
	int	frameSize;		// size per frame (byte)
	int	lineSize;		// size per line (byte)
	u8	pixelSize;		// pixel size
	int opt;
	
} libm_vram_info;

typedef struct{
    libm_vram_info *vinfo;
    
    u32 (*convert)(u32);
    u32 (*blend)(u8, u32, u32);
    
    int psx;
    int psy;
    
} libm_draw_info;

/*	#########################################################
    #				        Menu       						#
	#########################################################
*/


/*	libmSetActiveItem
    Set active item.
    
    @param: *Context
    target MenuContext
    
    @param: *Item
    target MenuItem
 */
#define	libmSetActive(Context,Item)		(Context)->Active = Item

/*	libmGetActiveItem
    Get active item.
    
    @param: *Context
    target MenuContext
    
    @return: active item (MenuItem*)
 */
#define	libmGetActive(Context)			(Context)->Active


/*	libmIsOpen
    Get show/hide status of menu.
    
    @param: *Context
    target MenuContext
    
    @return : true = show, false = hide
 */
#define libmIsOpen(Context)				(Context)->IsOpen



#define	libmSetLineColor(item,color)	(item)->Color.Line = color

/*	libmIsInvalidItem
    Check the item is invalid(spacer, hide, disable).
    
    @param: *Item
    target MenuItem
    
    @return : false = invalid item, true = valid item
 */
bool libmIsInvalidItem(MenuItem *Item);

/*	libmGetPrevItem
    Get prev item.
    
    @param: *Item
    base item
    
    @param: Invalid_Skip
    Whether to enable Invalid_Skip.
    
    @return : valid item. or NULL.
 */
MenuItem* libmGetPrevItem( MenuItem *Item , bool Invalid_Skip );


/*	libmGetNextItem
    Get next item.
    
    @param: *Item
    base item
    
    @param: Invalid_Skip
    Whether to enable Invalid_Skip.
    
    @return : valid item. or NULL.
 */
MenuItem* libmGetNextItem( MenuItem *Item , bool Invalid_Skip );



/*	libmGetBottomItem
    Get bottom item.
    
    @param: *Item
    base item
    
    @param: Invalid_Skip
    Whether to enable Invalid_Skip.
    
    @return : valid item. or NULL.
 */
MenuItem* libmGetBottomItem( MenuItem *Item , bool Invalid_Skip );
	

/*	libmGetTopItem
    Get top item.
    
    @param: *Item
    base item
    
    @param: Invalid_Skip
    Whether to enable Invalid_Skip.
    
    @return : valid item. or NULL.
 */
MenuItem* libmGetTopItem( MenuItem *Item , bool Invalid_Skip );

/*	libmSetOpt
    Set option
    
    @param: *Context
    target MenuContext
    
    @param: *opt
    option
    
    ex.)
    
    - use stack -
	opt.type = LIBM_OPT_LIST;
	opt.list.val	= item_list;			(item_list is array of MenuItem*)
	opt.list.size 	= sizeof(item_list);
	libmSetOpt(&opt);
		
	- use heap -
	opt.type = LIBM_OPT_FUNC;
	opt.func.malloc_p	= (alloc memory function);
	opt.func.free_p		= (free memory function);
	libmSetOpt(&opt);
    
    @return : true = success, false = failed
 */

bool libmSetOpt(MenuContext *Context , libmOpt *opt );




/*	libmCreateContext
    Create MenuContext.
    
    @param: *opt
    target option
    
    @return: created MenuContext
    
    * The following values are set automatically
    
	context->HotKey.Show			= PSP_CTRL_NOTE ;
	context->HotKey.Back			= PSP_CTRL_CIRCLE;
	context->HotKey.Select			= PSP_CTRL_CROSS;
	context->HotKey.Up				= PSP_CTRL_UP;
	context->HotKey.Down			= PSP_CTRL_DOWN;
	context->HotKey.Left			= PSP_CTRL_LEFT;
	context->HotKey.Right			= PSP_CTRL_RIGHT;
	
	context->MenuInfo.Type			= false;
	
	context->MenuInfo.AutoReturn	= true;
	
	context->Color.Active_Font		= GREEN;
	context->Color.Normal_Font		= WHITE;
	
	context->Color.Active_Back		= BLACK;
	context->Color.Normal_Back		= Invalid;
	
	context->Color.Disable_Font		= GRAY;
	context->Color.Disable_Back		= Invalid;
	
	context->MenuInfo.Lines		 	= 1;
 */
MenuContext* libmCreateContext(libmOpt *opt);

/*	libmRemoveContext
    Remove MenuContext.
    
    @param: *Context
    target MenuContext
 */
void libmRemoveContext(MenuContext *Context);

/*  libmAddItem
    Add item to context.
    
    @param: *Context
    target MenuContext
    
    @param: *Parent
    target MenuItem
	
    @param: *Item
    add  item
    
    @param: font_color
    font color
    
    @param: back_color
    back color
    
    @param: actionID
    actionID
    
    @param: Data
    value of the item
    
    @return: created item
 */
MenuItem* libmAddItem(MenuContext *Context, MenuItem *Parent, MenuItem *Item,u32 font_color,u32 back_color, int actionID, int Data);

/*	libmRemoveItem
    Remove item.
    
    @param: *Context
    target MenuContext
    
    @param: *Item
    target MenuItem
    
    If target item is container, remove all item in it.
 */
void libmRemoveItem( MenuContext *context , MenuItem *Item );

/*	libmGetHandle
    Get action on menu.
    
    @param: *Context
    target MenuContext
    
    @param: *Input
    Key
    
    @return: MenuParams
 */

MenuParams* libmGetHandle(MenuContext *Context, SceCtrlData *Input);

/*  libmRender
    Draw menu.
    
    @param: *Context
    target MenuContext
    
    @param: PosX
    Position X
    
    @param: PosY
    Position Y
 */
void libmRender(MenuContext* Context,int PosX,int PosY ,char *buf ,int bufLen, libm_draw_info *dinfo);




/*  libmCreateContainer
    Create Container.
    
    @param: *opt
    target option
    
    @param: *Name
    Container name
    
    @return : created item
 */
MenuItem* libmCreateContainer(libmOpt *opt ,const char *Name);

/*  libmCreateRadioButton
    Create RadioButton.
    
    @param: *opt
    target option
    
    @param: *Name
    RadioButton name
    
    @param: State
    state
    
    @param: *Group
    RadioButtonGroup which associate with. (created by libmCreateRadioButtonGroup)
    
	@return : created item
 */
MenuItem* libmCreateRadioButton(libmOpt *opt, const char *Name, bool State, MenuItem *Group);

/*  libmCreateRadioButtonGroup
    Create RadioButtonGroup.
    
    @param: *opt
    target option
    
    @return : created item
 */
MenuItem* libmCreateRadioButtonGroup(libmOpt *opt);



/*  libmCreateCheckBox
    Create CheckBox.
    
    @param: *opt
    target option
    
    @param: *Name
    CheckBox name
    
    @param: State
    state
    
	@return : created item
 */
MenuItem* libmCreateCheckBox(libmOpt *opt ,const char *Name, bool State);

/*  libmCreateSelectBox
    Create SelectBox.
    
    @param: *opt
    target option
    
    @param: *Name
    SelectBox name
    
    @param: *List[]
    show list. (the last is NULL)
    
    @param: Selected
    initial position
    
    @return : created item
 */
MenuItem* libmCreateSelectBox(libmOpt *opt ,const char *Name, const char *List[],int Selected);

/*  libmCreateUpDownCtrl
    Create UpDownControl
    
    @param: *opt
    target option
    
    @param: *Name
    UpDownControl name
    
    @param: Now
    now value
    
    @param: Step
    step value
    
    @param: Min
    minimum value
    
    @param: Max
    maximum value
    
    @param: type
    false = display in decimal
    true = display in integer
    
    @return : created item
 */
MenuItem* libmCreateUpDownCtrl(libmOpt *opt ,const char *Name,float Now,float Step,float Min, float Max,bool type);

/*  libmCreateSpacer
    Create Spacer
    
    @param: *opt
    target option
    
    @param: *Name
    Spacer name
    
    @return : created item
 */
MenuItem* libmCreateSpacer(libmOpt *opt ,const char *Name);

/*  libmCreateTriggerButton
    Create TriggerButton
    
    @param: *opt
    target option
    
	@param: *Name
    TriggerButton name
    
    @return : created item
 */
MenuItem* libmCreateTriggerButton(libmOpt *opt ,const char *Name);



/*	#############################################################
	#							Draw							#
	#############################################################
*/




/*  libmInitBuffers
    To prepare the drawing.
    
    @param: opt
	option
	
	LIBM_DRAW_INIT8888		init pixelformat8888
	LIBM_DRAW_INIT4444		init pixelformat4444
	LIBM_DRAW_INIT5650		init pixelformat5650
	LIBM_DRAW_INIT5551		init pixelformat5551
	LIBM_DRAW_BLEND			enable alpha blending
	LIBM_DRAW_RETURN		enable fold back
	
	LIBM_DRAW_INIT			init pixelformat8888 & enable fold back
    
    @param: sync
    sync
    
	PSP_DISPLAY_SETBUF_IMMEDIATE or PSP_DISPLAY_SETBUF_NEXTFRAME
	
	* If opt = LIBM_DRAW_INIT, sync = PSP_DISPLAY_SETBUF_NEXTFRAME.
    
    @param: *dinfo
    Pointer of libm_draw_info.
    
    @return: true = success, false = failed
 */
bool libmInitBuffers( int opt ,int sync, libm_draw_info *dinfo );



/*  libmSwapBuffers
    Swap buffers.
    
    @param: *dinfo
    Pointer of libm_draw_info.
	
	* Before calling this function, you must beforehand call the libmInitBuffers.
 */
void libmSwapBuffers(libm_draw_info *dinfo);

/*  libmClearBuffers
    Clear buffers.
    
    @param: *dinfo
    Pointer of libm_draw_info.
    
	* Before calling this function, you must beforehand call the libmInitBuffers.
 */
void libmClearBuffers(libm_draw_info *dinfo);


/*  libmPrintXY
	Draw string.
	
	@param: x
	Position X.
	
	@param: y
	Position Y.
	
	@param: fg
	font color. 0 is no drawing.
	
	@param: bg
	back color. 0 is no drawing.
	
	@param: *str
    string
    
    @param: *dinfo
    Pointer of libm_draw_info.
    
    @return: Length of drawn string
    
	* Before calling this function, you must beforehand call the libmInitBuffers.
 */
inline int libmPrintXY( int x, int y, u32 fg, u32 bg, const char *str, libm_draw_info *dinfo );



/*  libmPrintfXY
	Draw string with format.
	
	@param: x
	Position X.
	
	@param: y
	Position Y.
	
	@param: fg
	font color. 0 is no drawing.
	
	@param: bg
	back color. 0 is no drawing.
	
	@param: *buf
	Buffer used in this function.
	
	@param: bufLen
	Size of buf.
	
	@param: *dinfo
    Pointer of libm_draw_info.
	
	@params : format, ...
    string with format.
    
    @return: Length of drawn string.
    
	* Before calling this function, you must beforehand call the libmInitBuffers.
 */
inline int libmPrintfXY( int x, int y, u32 fg, u32 bg, char *buf ,int bufLen , libm_draw_info *dinfo, const char *format, ... );



/*  libmPutCharXY
	Draw a character.
	
	@param: x
	Position X.
	
	@param: y
	Position Y.
	
	@param: fg
	font color. 0 is no drawing.
	
	@param: bg
	back color. 0 is no drawing.
	
	@param: chr
    character
    
    @param: *dinfo
    Pointer of libm_draw_info.
    
    @return: Length of drawn string.
    
	* Before calling this function, you must beforehand call the libmInitBuffers.
 */
inline int libmPutCharXY( int x, int y, u32 fg, u32 bg, const char chr, libm_draw_info *dinfo );




/*  libmDebugScreenInit
	Init screen. Like pspDebugScreenInit.
	
	@param: *dinfo
    Pointer of libm_draw_info.
    
    @return: true = success, false = failed
 */
#define	libmDebugScreenInit(dinfo)		libmInitBuffers(LIBM_DRAW_INIT,PSP_DISPLAY_SETBUF_NEXTFRAME, dinfo)




/*  libmDebugScreenPrint
	Draw string. Like pspDebugScreenPrintf.
	
	@param: fg
	font color. 0 is no drawing.
	
	@param: bg
	back color. 0 is no drawing.
	
	@param: *str
    string
    
    @param: *dinfo
    Pointer of libm_draw_info.
    
    @return: Length of drawn string.
    
	* Before calling this function, you must beforehand call the libmInitBuffers.
 */
#define	libmDebugScreenPrint(fg,bg,str, dinfo)							libmPrintXY(-1,-1,fg,bg,str, dinfo)



/*  libmDebugScreenPrintf
	Draw string with format. Like pspDebugScreenPrintf.
	
	@param: fg
	font color. 0 is no drawing.
	
	@param: bg
	back color. 0 is no drawing.
	
	@param: *buf
	Buffer used in this function.
	
	@param: bufLen
	Size of buf.
	
	@param: *dinfo
    Pointer of libm_draw_info.
	
	@params : format, ...
    string with format.
    
    @return: Length of drawn string.
    
	* Before calling this function, you must beforehand call the libmInitBuffers.
 */
#define	libmDebugScreenPrintf(fg,bg,buf,bufLen, dinfo, format,...)		libmPrintfXY(-1,-1,fg,bg,buf,bufLen, dinfo, format, __VA_ARGS__)


/*  libmDebugScreenPutChar
	Draw a character. Like pspDebugScreenPrintf.
	
	@param: fg
	font color. 0 is no drawing.
	
	@param: bg
	back color. 0 is no drawing.
	
	@param: chr
    character
    
    @param: *dinfo
    Pointer of libm_draw_info.
    
    @return: Length of drawn string.
    
	* Before calling this function, you must beforehand call the libmInitBuffers.
 */
#define	libmDebugScreenPutChar(fg,bg,chr, dinfo)							libmPrintfXY(-1,-1,fg,bg,chr, dinfo)


/*  libmDebugScreenSetXY
	Set global XY position.
	
	@param:x
	Psition X. (0 - 59)
	
	@param: y
	Position Y. (0 - 33)
    
    @param: *dinfo
    Pointer of libm_draw_info.
    
    @return: true = success, false = failed
 */
bool libmDebugScreenSetXY( int x ,int y, libm_draw_info *dinfo );


/*	libmLine
    Draw line.
	
	@param: sx
	Start position X.
	
	@params : int sy
	Start position Y.
	
	@params : int ex
	End position X.
	
	@params : int ey
	End position Y.
    
	@params : u32 color
	color
	
	@param: *dinfo
    Pointer of libm_draw_info.
	
	* Before calling this function, you must beforehand call the libmInitBuffers.
*/
inline void libmLine( int sx, int sy, int ex, int ey, u32 color, libm_draw_info *dinfo );


/*	libmPoint
    Draw point.
	
	@param: *adr
	Draw address. (use libmMakeDrawAddr)
	
	@param: src
	color
    
    @param: *dinfo
    Pointer of libm_draw_info.
    
    ex.)
    
    if( libmInitBuffers(false,PSP_DISPLAY_SETBUF_NEXTFRAME) )
    {
 		// X=50, Y=100
 		void *adr = libmMakeDrawAddr(50,100, &dinfo);
 		u32 color = libmMake32bitColor(255,0,0,255);
    	libmPoint(adr,color, &dinfo);
    }
    
	* Before calling this function, you must beforehand call the libmInitBuffers.
*/
inline void libmPoint( void *adr, u32 src, libm_draw_info *dinfo );


/*	libmPointEx
	Draw point.
	
	@param: *adr
	Draw address. (use libmMakeDrawAddr)
	
	@param: src
	color
    
    @param: *dinfo
    Pointer of libm_draw_info.
    
	* Before calling this function, you must beforehand call the libmInitBuffers.
*/
inline void libmPointEx( void *adr, u32 src, libm_draw_info *dinfo );

/*	libmFillRect
	Fill the range.
	
	@param: sx
	Start position X. (0 - 480)
	
	@param: sy
	Start position Y. (0 - 272)
	
	@param: ex
	End position X. (0 - 480)
	
	@param: ey
	End position Y. (0 - 272)
    
	@param: color
	color
	
	@param: *dinfo
    Pointer of libm_draw_info.
	
	* Before calling this function, you must beforehand call the libmInitBuffers.
*/
inline void libmFillRect( int sx, int sy, int ex, int ey, u32 color, libm_draw_info *dinfo );



/*	libmCircle
    Draw circle.
	
	@param: x
	Position X.
	
	@param: y
	Position Y.
	
	@param: radius
	radius
	
	@param: color
	color
	
	@param: *dinfo
    Pointer of libm_draw_info.
	
	* Before calling this function, you must beforehand call the libmInitBuffers.
*/
inline void libmCircle( int x, int y, u32 radius, u32 color, libm_draw_info *dinfo );



/*	libmFrame
    Draw frame.

	@param: sx
	Start position X. (0 - 480)
	
	@param: sy
	Start position Y. (0 - 272)
	
	@param: ex
	End position X. (0 - 480)
	
	@param: ey
	End position Y. (0 - 272)
    
	@param: color
	color
	
	@param: *dinfo
    Pointer of libm_draw_info.
	
	* Before calling this function, you must beforehand call the libmInitBuffers.
*/
inline void libmFrame( int sx, int sy, int ex, int ey, u32 color, libm_draw_info *dinfo );


/*	libmMakeDrawAddr
	Convert position -> vram address.
	
	@param: x
	Position X. (0 - 480)
	
	@param: y
	Position Y. (0 - 272)
	
	@param: *dinfo
    Pointer of libm_draw_info.
	
	* Before calling this function, you must beforehand call the libmInitBuffers.
*/
inline void* libmMakeDrawAddr( int x, int y, libm_draw_info *dinfo );

/*	libmConvert8888_XXXX
	Convert color. (8888 -> XXXX)
	
	@param: src
	color
	
    @return: Converted color.
*/
inline u32 libmConvert8888_5650( u32 src );
inline u32 libmConvert8888_5551( u32 src );
inline u32 libmConvert8888_4444( u32 src );

/*	libmConvertXXXX_8888
	Convert color. (XXXX -> 8888)
	
	@param: src
	color
	
    @return: Converted color.
*/
inline u32 libmConvert4444_8888(u32 src);
inline u32 libmConvert5551_8888(u32 src);
inline u32 libmConvert5650_8888(u32 src);




/*	libmAlphaBlendXXXX
	Alpha blending.
	
	@param: alpha
	alpha (0-255)
	
	@param: src
	color (source)
	
	@param: dst
	coor (destination)
	
    @return: blended color.
*/
inline u32 libmAlphaBlend8888( u8 alpha, u32 src, u32 dst );
inline u32 libmAlphaBlend4444( u8 alpha, u32 src, u32 dst );
inline u32 libmAlphaBlend5551( u8 alpha, u32 src, u32 dst );
inline u32 libmAlphaBlend5650( u8 alpha, u32 src, u32 dst );



/*	libmMake32bitColor
	Make 32bit color.
	
	@param: R
	Color Red (0-255)
	
	@param: G
	Color Green (0-255)
	
	@param: B
	Color Blue (0-255)
	
	@param: A
	Alpha (0-255)
	
	@return : 32bit color
*/
#define	libmMake32bitColor(R,G,B,A)	 ((R & 255) | ((G & 255) << 8) | ((B & 255) << 16) | ((A & 255) << 24))


/*	libmGetColor
	Get color from address.
	
	@param: addr
	address
	
	@return : color
	
	* Before calling this function, you must beforehand call the libmInitBuffers.
*/
inline u32 libmGetColor(void *addr, libm_draw_info *dinfo);


/*	libmSetCurVInfo
    Set vram info.
	
	@param: format
	pixel format
	
	@param: buf
	vram address.
	
	@params : width
	width
*/
void libmSetCurVInfo(int format, void *buf, int width, libm_draw_info *dinfo);



/*	#############################################################
	#							Extension						#
	#############################################################

*/

/*
    libmLoadFont
    load font.
    
    @param: flag
    which font
    
    LIBM_FONT_CG             "/seplugins/lib/font/cg.bin" (ascii)
    LIBM_FONT_HANKAKU_KANA   "/seplugins/lib/font/hankaku_kana.bin" (japanese hankaku kana)
    LIBM_FONT_SJIS           "/seplugins/lib/font/sjis.bin" (japanese)
    LIBM_FONT_ICON           "/seplugins/lib/font/icon.bin" (2bit color icon)
    
    @return : 0 = success, -1 = failed
*/
int libmLoadFont(int flag);


/*
    libmLen
    Get length of string.
    
    @param : str
    string
    
    @return : Length of string
*/
int libmLen(const char *str);


/*
    libmPrintXY16
    Draw string. (zoom 16x16)
*/
inline int libmPrintXY16( int x, int y, u32 fg, u32 bg, const char *str, libm_draw_info *dinfo );


/*
    libmPrintfXY16
    Draw string with format. (zoom 16x16)
*/
inline int libmPrintfXY16( int x, int y, u32 fg, u32 bg, char *buf ,int bufLen , libm_draw_info *dinfo, const char *format, ... );


/*	libmGetIdxItem
    Get item.
    
    @param: *Item
    base item
    
    @param: Invalid_Skip
    Whether to enable Invalid_Skip.
    
    @param: Point_Idx
    number
    
    @return : Valid item or NULL.
 */
MenuItem* libmGetIdxItem( MenuItem *Item , bool Invalid_Skip , int Point_Idx );


/*
    libmPrintSymbolXY
    Draw icon.
    
    @param: x
	Position X.
	
	@param: y
	Position Y.
	
	@param: color1
	color1
	
	@param: color2
	color2
	
	@param: color3
	color3
	
	@param: color0
	color0
	
	@param: *str
    string
    
    @return: Length of drawn string
    
	* Before calling this function, you must beforehand call the libmInitBuffers.
*/
inline int libmPrintSymbolXY( int x, int y, u32 color1, u32 color2, u32 color3, u32 color0, const char *str, libm_draw_info *dinfo );


/*
    libmPrintSymbolXY16
    Draw icon. (zoom 16x16)
*/
inline int libmPrintSymbolXY16( int x, int y, u32 color1, u32 color2, u32 color3, u32 color0, const char *str, libm_draw_info *dinfo );


/*
    libmCloseAllContainer
    Close all container.
    
    @param: *Context
    target Context
    
    @return : 0 on success, -1 on already closed.
*/
int libmCloseAllContainer(MenuContext *Context);


/*	libmGetActiveNum
	check the current item is Nth of the specified item.
	
	@param: *Context
	target MenuContext
	
	@param: *Item
	target MenuItem (if NULL, the target becomes the top)
	
	@param: Invalid_Skip
	Whether to enable Invalid_Skip.
	
	@return : 0 = Active is target MenuItem, -1 = failed
 */
int libmGetActiveNum(MenuContext *Context, MenuItem *Item , bool Invalid_Skip );


/*	libmInitBuffers_ForHook
	This function is like libmInitBuffers. But, only use in the function which was registered by libmHookDisplayHandler.
	
    @param: opt
	option
	
	LIBM_DRAW_INIT8888		init pixelformat8888(cannot use)
	LIBM_DRAW_INIT4444		init pixelformat4444(cannot use)
	LIBM_DRAW_INIT5650		init pixelformat5650(cannot use)
	LIBM_DRAW_INIT5551		init pixelformat5551(cannot use)
	LIBM_DRAW_BLEND			enable alpha blending
	LIBM_DRAW_RETURN		enable fold back
	
	LIBM_DRAW_INIT			init pixelformat8888 & enable fold back(cannot use)
    
    @param: topaddr, bufferwidth, pixelformat, sync
    arguments of sceDisplaySetFrameBuf_Patched

    @param: *dinfo
    Pointer of libm_draw_info.
    
    @return: true = success, false = failed
 */
bool libmInitBuffers_ForHook( int opt, void *topaddr, int bufferwidth, int pixelformat, int sync, libm_draw_info *dinfo );


/*	libmHookDisplayHandler
	Hook the sceDisplaySetFrameBuf, and the func interrupt to it.
	
	@param: (*func)(void *topaddr, int bufferwidth, int pixelformat, int sync)
	target Function

	@return : previous

	@example

	int (*previous)(void *topaddr, int bufferwidth, int pixelformat, int sync);

	int sceDisplaySetFrameBuf_Patched(void *topaddr, int bufferwidth, int pixelformat, int sync)
	{
		libmInitBuffers_ForHook( 0, topaddr, bufferwidth, pixelformat, sync, &dinfo )
		{
			Drow(&dinfo);
		}
		return previous ? previous(topaddr, bufferwidth, pixelformat, sync) : 0;
	}

	int module_start(SceSize args, void *argp)
	{
		previous = libmHookDisplayHandler(sceDisplaySetFrameBuf_Patched);

		return 0;
	}

 */
void* libmHookDisplayHandler(int (*func)(void *topaddr, int bufferwidth, int pixelformat, int sync));


/*
	libmGetFontAddr
	Get the top address of font data.
	
	@param : flag
	which font. can use the following.
	
	LIBM_FONT_CG
    LIBM_FONT_HANKAKU_KANA
    LIBM_FONT_SJIS
    LIBM_FONT_ICON
    
    @retern top address
*/
char* libmGetFontAddr(int flag);


/*
	libmPutCharXY16
	Draw char. (zoom 16x16)
*/
inline int libmPutCharXY16( int x, int y, u32 fg, u32 bg, const char chr, libm_draw_info *dinfo );


/*
	libmPrintMyIcon
	Draw icon.
	
	@param : x
	Position X.
	
	@param : y
	Position Y.
	
	@param : fg
	Fore color.
	
	@param : bg
	Back color.
	
	@param : *icon
	Pointer of icon data.
	
	@param : size
	Icon size. (ex. 12x12 -> size = 12)
	
	@param : *dinfo
	Pointer of libm_draw_info.
*/
void libmPrintMyIcon(int x, int y, u32 fg, u32 bg, const char *icon, u32 size, libm_draw_info *dinfo);


#ifdef __cplusplus
}
#endif

#endif

