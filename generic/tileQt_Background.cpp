/*
 *  tileQt_Background.cpp
 * -----------------------
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

/*
 * Map between Tk/Tile & Qt/KDE state flags.
 */
static TTK_StateTable background_statemap[] =
{
};

typedef struct {
} BackgroundElement;

static TTK_ElementOptionSpec BackgroundElementOptions[] = {
    {NULL}
};

static void BackgroundElementGeometry(
    void *clientData, void *elementRecord, Tk_Window tkwin,
    int *widthPtr, int *heightPtr, TTK_Padding *paddingPtr)
{
    // int padding = qApp->style().pixelMetric(QStyle::PM_DefaultFrameWidth);
    *paddingPtr = TTK_UniformPadding(0);
}

static void BackgroundElementDraw(
    void *clientData, void *elementRecord, Tk_Window tkwin,
    Drawable d, TTK_Box b, unsigned state)
{
    QPixmap      pixmap(b.width, b.height);
    QPainter     painter(&pixmap);
    if (TileQt_QPixmap_BackgroundTile &&
        !(TileQt_QPixmap_BackgroundTile->isNull())) {
        painter.fillRect(0, 0, b.width, b.height,
                         QBrush(QColor(255,255,255),
                         *TileQt_QPixmap_BackgroundTile));
    } else {
        painter.fillRect(0, 0, b.width, b.height,
                         qApp->palette().active().background());
    }
    TileQt_CopyQtPixmapOnToDrawable(pixmap, d, tkwin,
                                    0, 0, b.width, b.height, b.x, b.y);
}

static TTK_ElementSpec BackgroundElementSpec = {
    TK_STYLE_VERSION_2,
    sizeof(BackgroundElement),
    BackgroundElementOptions,
    BackgroundElementGeometry,
    BackgroundElementDraw
};

/*------------------------------------------------------------------------
 * +++ Border element.
 */

typedef struct {
    Tcl_Obj 	*borderWidthObj;
    Tcl_Obj 	*reliefObj;
} BorderElement;

static TTK_ElementOptionSpec BorderElementOptions[] = {
    { "-borderwidth", TK_OPTION_PIXELS,
	Tk_Offset(BorderElement,borderWidthObj), BorderBorderWidth },
    { "-relief", TK_OPTION_RELIEF,
	Tk_Offset(BorderElement,reliefObj), "flat" },
    {NULL}
};

static void BorderElementGeometry(
    void *clientData, void *elementRecord, Tk_Window tkwin,
    int *widthPtr, int *heightPtr, TTK_Padding *paddingPtr)
{
    BorderElement *border = (BorderElement*) elementRecord;
    int borderWidth = 0;
    Tcl_GetIntFromObj(NULL, border->borderWidthObj, &borderWidth);
    *paddingPtr = TTK_UniformPadding((short)borderWidth+2);
}

static void BorderElementDraw(
    void *clientData, void *elementRecord, Tk_Window tkwin,
    Drawable d, TTK_Box b, unsigned state)
{
    BorderElement *border = (BorderElement*) elementRecord;
    int relief = TK_RELIEF_FLAT;
    int borderWidth = 0;

    Tcl_GetIntFromObj(NULL, border->borderWidthObj, &borderWidth);
    Tk_GetReliefFromObj(NULL, border->reliefObj, &relief);
    QStyle::SFlags sflags;

    switch (relief) {
	case TK_RELIEF_GROOVE :
	case TK_RELIEF_RIDGE :
	case TK_RELIEF_RAISED :
            sflags |= QStyle::Style_Raised;
	    break;
	case TK_RELIEF_SUNKEN :
            sflags |= QStyle::Style_Sunken;
	    break;
	case TK_RELIEF_FLAT :
	    break;
	case TK_RELIEF_SOLID :
	    break;
    }

    QPixmap      pixmap(b.width, b.height);
    QPainter     painter(&pixmap);
    if (TileQt_QPixmap_BackgroundTile &&
        !(TileQt_QPixmap_BackgroundTile->isNull())) {
        painter.fillRect(0, 0, b.width, b.height,
                         QBrush(QColor(255,255,255),
                         *TileQt_QPixmap_BackgroundTile));
    } else {
        painter.fillRect(0, 0, b.width, b.height,
                         qApp->palette().active().background());
    }
    if (borderWidth) {
      qApp->style().drawPrimitive(QStyle::PE_GroupBoxFrame, &painter,
            QRect(0, 0, b.width, b.height), qApp->palette().active(), sflags,
            QStyleOption(borderWidth, 0));
    }
    TileQt_CopyQtPixmapOnToDrawable(pixmap, d, tkwin,
                                    0, 0, b.width, b.height, b.x, b.y);
}

static TTK_ElementSpec BorderElementSpec = {
    TK_STYLE_VERSION_2,
    sizeof(BorderElement),
    BorderElementOptions,
    BorderElementGeometry,
    BorderElementDraw
};


/*------------------------------------------------------------------------
 * +++ Widget layout.
 */

int TileQt_Init_Background(Tcl_Interp *interp, TTK_Theme themePtr)
{
    /*
     * Register elements:
     */
    TTK_RegisterElementSpec(themePtr, "background",
            &BackgroundElementSpec, NULL);
    //TTK_RegisterElementSpec(themePtr, "border",
    //        &BorderElementSpec, NULL);
    /*
     * Register layouts:
     */

    return TCL_OK;
}; /* TileQt_Init_Background */