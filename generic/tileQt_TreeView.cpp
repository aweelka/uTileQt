/*
 *  tileQt_TreeView.cpp
 * ---------------------
 *
 * This file is part of the Tile-Qt package, a Tk/Tile based theme that uses
 * Qt/KDE for drawing.
 *
 * Copyright (C) 2004-2005 by:
 * Georgios Petasis, petasis@iit.demokritos.gr,
 * Software and Knowledge Engineering Laboratory,
 * Institute of Informatics and Telecommunications,
 * National Centre for Scientific Research (NCSR) "Demokritos",
 * Aghia Paraskevi, 153 10, Athens, Greece.
 */

#include "tileQt_Utilities.h"
#include <tk.h>
#include "tkTheme.h"
#include "tileQt_WidgetDefaults.h"

#define ITEM_LAYOUT		"Item"
#define CELL_LAYOUT		"Cell"
#define HEADING_LAYOUT		"Heading"
#define ROW_LAYOUT		"Row"

/*------------------------------------------------------------------------
 * +++ TreeView Field Element.
 */
 
/*
 * Map between Tk/Tile & Qt/KDE state flags.
 */
static TTK_StateTable treeview_field_statemap[] =
{
    {QStyle::Style_Enabled|QStyle::Style_Down,      TTK_STATE_PRESSED, 0},
    {QStyle::Style_Enabled,                         TTK_STATE_ACTIVE, 0},
    {QStyle::Style_Enabled,                         0,0}
};

typedef struct {
} TreeViewFieldElement;


static TTK_ElementOptionSpec TreeViewFieldElementOptions[] = {
    {NULL}
};

static void TreeViewFieldElementGeometry(
    void *clientData, void *elementRecord, Tk_Window tkwin,
    int *widthPtr, int *heightPtr, TTK_Padding *paddingPtr)
{
    *paddingPtr = TTK_UniformPadding(0);
}

static void TreeViewFieldElementDraw(
    void *clientData, void *elementRecord, Tk_Window tkwin,
    Drawable d, TTK_Box b, unsigned state)
{
    QPixmap      pixmap(b.width, b.height);
    QPainter     painter(&pixmap);
    QStyle::SFlags sflags =TTK_StateTableLookup(treeview_field_statemap, state);
    sflags |= QStyle::Style_Horizontal;
    if (TileQt_QPixmap_BackgroundTile &&
        !(TileQt_QPixmap_BackgroundTile->isNull())) {
        painter.fillRect(0, 0, b.width, b.height,
                         QBrush(QColor(255,255,255),
                         *TileQt_QPixmap_BackgroundTile));
    } else {
        painter.fillRect(0, 0, b.width, b.height,
                         qApp->palette().active().background());
    }
    // printf("x=%d, y=%d, w=%d, h=%d\n", b.x, b.y, b.width, b.height);
    qApp->style().drawPrimitive(QStyle::PE_GroupBoxFrame, &painter,
          QRect(0, 0, b.width, b.height), qApp->palette().active(), sflags,
          QStyleOption(/*lineWidth*/1, /*midLineWidth*/0,
                       /*frameShape*/QFrame::StyledPanel,
                       /*frameShadow*/QFrame::Sunken));
    TileQt_CopyQtPixmapOnToDrawable(pixmap, d, tkwin,
                                    0, 0, b.width, b.height, b.x, b.y);
}

static TTK_ElementSpec TreeViewFieldElementSpec = {
    TK_STYLE_VERSION_2,
    sizeof(TreeViewFieldElement),
    TreeViewFieldElementOptions,
    TreeViewFieldElementGeometry,
    TreeViewFieldElementDraw
};

/*------------------------------------------------------------------------
 * +++ TreeView Client Element.
 */
 
/*
 * Map between Tk/Tile & Qt/KDE state flags.
 */
static TTK_StateTable treeview_client_statemap[] =
{
    {QStyle::Style_Enabled|QStyle::Style_Down,      TTK_STATE_PRESSED, 0},
    {QStyle::Style_Enabled,                         TTK_STATE_ACTIVE, 0},
    {QStyle::Style_Enabled,                         0,0}
};

typedef struct {
} TreeViewClientElement;


static TTK_ElementOptionSpec TreeViewClientElementOptions[] = {
    {NULL}
};

static void TreeViewClientElementGeometry(
    void *clientData, void *elementRecord, Tk_Window tkwin,
    int *widthPtr, int *heightPtr, TTK_Padding *paddingPtr)
{
    *paddingPtr = TTK_UniformPadding(0);
}

static void TreeViewClientElementDraw(
    void *clientData, void *elementRecord, Tk_Window tkwin,
    Drawable d, TTK_Box b, unsigned state)
{
    QPixmap      pixmap(b.width, b.height);
    QPainter     painter(&pixmap);
    QStyle::SFlags sflags =TTK_StateTableLookup(treeview_client_statemap,state);
    sflags |= QStyle::Style_Horizontal;
    if (TileQt_QPixmap_BackgroundTile &&
        !(TileQt_QPixmap_BackgroundTile->isNull())) {
        painter.fillRect(0, 0, b.width, b.height,
                         QBrush(QColor(255,255,255),
                         *TileQt_QPixmap_BackgroundTile));
    } else {
        painter.fillRect(0, 0, b.width, b.height,
                         qApp->palette().active().background());
    }
    // printf("x=%d, y=%d, w=%d, h=%d\n", b.x, b.y, b.width, b.height);
    qApp->style().drawPrimitive(QStyle::PE_GroupBoxFrame, &painter,
          QRect(0, 0, b.width, b.height), qApp->palette().active(), sflags,
          QStyleOption(/*lineWidth*/1, /*midLineWidth*/0,
                       /*frameShape*/QFrame::StyledPanel,
                       /*frameShadow*/QFrame::Sunken));
    TileQt_CopyQtPixmapOnToDrawable(pixmap, d, tkwin,
                                    0, 0, b.width, b.height, b.x, b.y);
}

static TTK_ElementSpec TreeViewClientElementSpec = {
    TK_STYLE_VERSION_2,
    sizeof(TreeViewClientElement),
    TreeViewClientElementOptions,
    TreeViewClientElementGeometry,
    TreeViewClientElementDraw
};

/*------------------------------------------------------------------------
 * +++ TreeView Header Border Element.
 */
 
/*
 * Map between Tk/Tile & Qt/KDE state flags.
 */
static TTK_StateTable treeheading_border_statemap[] =
{
    {QStyle::Style_Enabled|QStyle::Style_Down,      TTK_STATE_PRESSED, 0},
    {QStyle::Style_Enabled,                         TTK_STATE_ACTIVE, 0},
    {QStyle::Style_Enabled,                         0,0}
};

typedef struct {
} TreeHeadingBorderElement;


static TTK_ElementOptionSpec TreeHeadingBorderElementOptions[] = {
    {NULL}
};

static void TreeHeadingBorderElementGeometry(
    void *clientData, void *elementRecord, Tk_Window tkwin,
    int *widthPtr, int *heightPtr, TTK_Padding *paddingPtr)
{
    *paddingPtr = TTK_UniformPadding(TreeViewUniformPadding);
}

static void TreeHeadingBorderElementDraw(
    void *clientData, void *elementRecord, Tk_Window tkwin,
    Drawable d, TTK_Box b, unsigned state)
{
    QPixmap      pixmap(b.width, b.height);
    QPainter     painter(&pixmap);
    QStyle::SFlags sflags = TTK_StateTableLookup(treeheading_border_statemap,
                                                 state);
    sflags |= QStyle::Style_Horizontal;
    if (TileQt_QPixmap_BackgroundTile &&
        !(TileQt_QPixmap_BackgroundTile->isNull())) {
        painter.fillRect(0, 0, b.width, b.height,
                         QBrush(QColor(255,255,255),
                         *TileQt_QPixmap_BackgroundTile));
    } else {
        painter.fillRect(0, 0, b.width, b.height,
                         qApp->palette().active().background());
    }
    // printf("x=%d, y=%d, w=%d, h=%d\n", b.x, b.y, b.width, b.height);
    qApp->style().drawPrimitive(QStyle::PE_HeaderSection, &painter,
          QRect(0, 0, b.width, b.height), qApp->palette().active(), sflags);
    TileQt_CopyQtPixmapOnToDrawable(pixmap, d, tkwin,
                                    0, 0, b.width, b.height, b.x, b.y);
}

static TTK_ElementSpec TreeHeadingBorderElementSpec = {
    TK_STYLE_VERSION_2,
    sizeof(TreeHeadingBorderElement),
    TreeHeadingBorderElementOptions,
    TreeHeadingBorderElementGeometry,
    TreeHeadingBorderElementDraw
};


/*------------------------------------------------------------------------
 * +++ Widget layout.
 */

TTK_BEGIN_LAYOUT(TreeviewLayout)
    TTK_GROUP("Treeview.field", TTK_FILL_BOTH|TTK_BORDER,
	TTK_GROUP("Treeview.padding", TTK_FILL_BOTH,
	    TTK_NODE("Treeview.client", TTK_FILL_BOTH)))
TTK_END_LAYOUT

TTK_BEGIN_LAYOUT(ItemLayout)
    TTK_GROUP("Treeitem.padding", TTK_FILL_BOTH,
	TTK_NODE("Treeitem.indicator", TTK_PACK_LEFT)
	TTK_NODE("Treeitem.image", TTK_PACK_LEFT)
	TTK_GROUP("Treeitem.focus", TTK_PACK_LEFT,
	    TTK_NODE("Treeitem.text", TTK_PACK_LEFT)))
TTK_END_LAYOUT

TTK_BEGIN_LAYOUT(CellLayout)
    TTK_GROUP("Treedata.padding", TTK_FILL_BOTH,
	TTK_NODE("Treeitem.label", TTK_FILL_BOTH))
TTK_END_LAYOUT

TTK_BEGIN_LAYOUT(HeadingLayout)
    TTK_NODE("Treeheading.cell", TTK_FILL_BOTH)
    TTK_GROUP("Treeheading.border", TTK_FILL_BOTH,
	TTK_NODE("Treeheading.image", TTK_PACK_RIGHT)
	TTK_NODE("Treeheading.text", 0))
TTK_END_LAYOUT

TTK_BEGIN_LAYOUT(RowLayout)
    TTK_NODE("Treeitem.row", TTK_FILL_BOTH)
TTK_END_LAYOUT

int TileQt_Init_TreeView(Tcl_Interp *interp, TTK_Theme themePtr)
{
    /*
     * Register elements:
     */
    TTK_RegisterElementSpec(themePtr, "Treeview.field",
            &TreeViewFieldElementSpec, NULL);
    TTK_RegisterElementSpec(themePtr, "Treeview.client",
            &TreeViewClientElementSpec, NULL);
    TTK_RegisterElementSpec(themePtr, "Treeheading.border",
            &TreeHeadingBorderElementSpec, NULL);
    
    /*
     * Register layouts:
     */
    TTK_RegisterLayout(themePtr, ITEM_LAYOUT, ItemLayout);
    TTK_RegisterLayout(themePtr, CELL_LAYOUT, CellLayout);
    TTK_RegisterLayout(themePtr, HEADING_LAYOUT, HeadingLayout);
    TTK_RegisterLayout(themePtr, ROW_LAYOUT, RowLayout);

    return TCL_OK;
}; /* TileQt_Init_TreeView */