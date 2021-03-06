/*
 * $Id: tileQt_Init.cpp,v 1.2 2008/08/20 12:22:52 petasis Exp $
 *
 * Copyright (C) 2004-2008 Georgios Petasis
 *
 * The Tile-Qt theme is a Tk/Tile theme that uses Qt/KDE for drawing.
 */

#include "tileQt_QtHeaders.h"
#include "tileQt_Utilities.h"
#include "tileQt_Elements.h"
#include "tileQt_TkHeaders.h"
#include <string.h>

static int TileQt_QAppCreated = 0;

/*
 * Override QWidget::hasFocus, to return our values, as non-visible windows
 * never get focus, so hasFocus always returns false.
 */
static bool TileQt_HasFocus = false;
bool QWidget::hasFocus() const {
  return TileQt_HasFocus;
}
void TileQt_SetFocus(bool focus) {
  Tcl_MutexLock(&tileqtMutex);
  TileQt_HasFocus = focus;
  Tcl_MutexUnlock(&tileqtMutex);
}; /* TileQt_SetFocus */

extern TileQt_WidgetCache **TileQt_CreateQApp(Tcl_Interp *interp);
extern void TileQt_DestroyQApp(void);

static char initScript[] =
    "namespace eval tileqt { };"
    "namespace eval ttk::theme::tileqt { variable version "
                                                   PACKAGE_VERSION " };"
    "source [file join $::tileqtdir tileqt.tcl];"
    "unset ::tileqtdir;";

/*
 * Exit Handler.
 */
static void TileQt_ExitProc(ClientData data) {
  //Tcl_MutexLock(&tileqtMutex);
  //TileQt_DestroyQApp();
  //Tcl_MutexUnlock(&tileqtMutex);
  Tcl_MutexLock(&tileqtMutex);
  // printf("TileQt_ExitProc: %d\n", TileQt_QAppCreated); fflush(NULL);
  if (TileQt_QAppCreated < 0) {
    Tcl_MutexUnlock(&tileqtMutex);
    return;
  }
  --TileQt_QAppCreated;
  if (TileQt_QAppCreated == 0) {
    // printf("TileQt_ExitProc: %d <- TileQt_DestroyQApp();\n",
    //       TileQt_QAppCreated); fflush(NULL);
    TileQt_DestroyQApp();
  }
  Tcl_MutexUnlock(&tileqtMutex);
  return;
}; /* TileQt_ExitProc */

/*
 * Helper Functions
 */
int Tileqt_ThemeName(ClientData clientData, Tcl_Interp *interp,
                                 int objc, Tcl_Obj *const objv[]) {
  if (objc != 1) {Tcl_WrongNumArgs(interp, 1, objv, ""); return TCL_ERROR;}
  TileQt_WidgetCache **wc_array = (TileQt_WidgetCache **) clientData;
  TileQt_WidgetCache *wc = wc_array[0];
  Tcl_MutexLock(&tileqtMutex);
  if (qApp) {
    Tcl_SetResult(interp,
#ifdef TILEQT_QT_VERSION_4                    
                    (char *) wc->TileQt_Style->objectName().toUtf8().data()
#endif /* TILEQT_QT_VERSION_4 */
                    , TCL_VOLATILE);
  } else {
    Tcl_SetResult(interp, (char *) "", TCL_STATIC);
  }
  Tcl_MutexUnlock(&tileqtMutex);
  return TCL_OK;
}; /* Tileqt_ThemeName */

int Tileqt_ThemeColour(ClientData clientData, Tcl_Interp *interp,
                                 int objc, Tcl_Obj *const objv[]) {
  static const char *Methods[] = {
    "-active",     "-disabled",        "-inactive",   "-normal",
    "-background", "-foreground",      "-button",     "-light",
    "-dark",       "-mid",             "-text",       "-base",
    "-midlight",   "-brightText",      "-buttonText", "-shadow",
    "-highlight",  "-highlightedText", "-link",       "-linkVisited",
    "-alternatebase",
    (char *) NULL
  };
  enum methods {
    STT_ACTIVE,     STT_DISABLED,        STT_INACTIVE,   STT_NORMAL,
    CLR_background, CLR_foreground,      CLR_button,     CLR_light,
    CLR_dark,       CLR_mid,             CLR_text,       CLR_base,
    CLR_midlight,   CLR_brightText,      CLR_buttonText, CLR_shadow,
    CLR_highlight,  CLR_highlightedText, CLR_link,       CLR_linkVisited,
    CLR_alternatebase
  };
  int index;
  if (objc != 2 && objc != 3) {
    Tcl_WrongNumArgs(interp, 1, objv, "?-active|-disabled|-inactive? colour");
    return TCL_ERROR;
  }
  if (!qApp) {
    Tcl_SetResult(interp, (char *) "", TCL_STATIC);
    return TCL_OK;
  }
  Tcl_MutexLock(&tileqtMutex);
  QPalette palette = qApp->palette();
#ifdef TILEQT_QT_VERSION_4
  QPalette::ColorGroup colours = QPalette::Normal;
  QPalette::ColorRole  role    = QPalette::Window;
#endif /* TILEQT_QT_VERSION_4 */
  QColor colour;
  for (int i = 1; i < objc; ++i) {
    if (Tcl_GetIndexFromObj(interp, objv[i], (const char **) Methods,
                            "method", 0, &index) != TCL_OK) {
      Tcl_MutexUnlock(&tileqtMutex); return TCL_ERROR;
    }
    switch ((enum methods) index) {
#ifdef TILEQT_QT_VERSION_4
      case STT_NORMAL:          {colours = QPalette::Normal;           break;}
      case STT_ACTIVE:          {colours = QPalette::Active;           break;}
      case STT_DISABLED:        {colours = QPalette::Disabled;         break;}
      case STT_INACTIVE:        {colours = QPalette::Inactive;         break;}
      case CLR_background:      {role = QPalette::Window;              break;}
      case CLR_foreground:      {role = QPalette::WindowText;          break;}
      case CLR_button:          {role = QPalette::Button;              break;}
      case CLR_light:           {role = QPalette::Light;               break;}
      case CLR_dark:            {role = QPalette::Dark;                break;}
      case CLR_mid:             {role = QPalette::Mid;                 break;}
      case CLR_text:            {role = QPalette::Text;                break;}
      case CLR_base:            {role = QPalette::Base;                break;}
      case CLR_alternatebase:   {role = QPalette::AlternateBase;       break;}
      case CLR_midlight:        {role = QPalette::Midlight;            break;}
      case CLR_brightText:      {role = QPalette::BrightText;          break;}
      case CLR_buttonText:      {role = QPalette::ButtonText;          break;}
      case CLR_shadow:          {role = QPalette::Shadow;              break;}
      case CLR_highlight:       {role = QPalette::Highlight;           break;}
      case CLR_highlightedText: {role = QPalette::HighlightedText;     break;}
      case CLR_link:            {role = QPalette::Link;                break;}
      case CLR_linkVisited:     {role = QPalette::LinkVisited;         break;}
#endif /* TILEQT_QT_VERSION_4 */
    }
  }
#ifdef TILEQT_QT_VERSION_4
  colour = palette.color(colours, role);
  Tcl_SetResult(interp, (char *) colour.name().toUtf8().data(), TCL_VOLATILE);
#endif /* TILEQT_QT_VERSION_4 */
  Tcl_MutexUnlock(&tileqtMutex);
  return TCL_OK;
}; /* Tileqt_ThemeColour */

void setPalette (Tcl_Interp *interp)
{
    int rBG, gBG, bBG, rFG, gFG, bFG,
        rABG, gABG, bABG, rSC, gSC, bSC,
        rAFG, gAFG, bAFG, rHLBG, gHLBG, bHLBG,
        rSBG, gSBG, bSBG, rHLC, gHLC, bHLC,
        rSFG, gSFG, bSFG, rDFG, gDFG, bDFG,
        rIBG, gIBG, bIBG, rTC, gTC, bTC;

    // Active colors (they are synonyms for normal in Qt 4)
    // For Tk they mean what happens when a button is under mouse
    qApp->palette().setCurrentColorGroup(QPalette::Active);
    //activeBackground
    qApp->palette().window().color().getRgb(&rABG, &gABG, &bABG);
    //activeForeground
    qApp->palette().windowText().color().getRgb(&rAFG, &gAFG, &bAFG);

    // Inactive colors
    qApp->palette().setCurrentColorGroup(QPalette::Inactive);

    // Disabled colors
    qApp->palette().setCurrentColorGroup(QPalette::Disabled);
    //disabledForeground
    qApp->palette().windowText().color().getRgb(&rDFG, &gDFG, &bDFG);

    // Normal colors
    qApp->palette().setCurrentColorGroup(QPalette::Normal);
    //background
    qApp->palette().window().color().getRgb(&rBG, &gBG, &bBG);
    //foreground
    qApp->palette().windowText().color().getRgb(&rFG, &gFG, &bFG);
    //highlightBackground (in fact the border around inactive buttons/fields)
    qApp->palette().dark().color().getRgb(&rHLBG, &gHLBG, &bHLBG);
    //highlightColor (border around active buttons)
    qApp->palette().highlight().color().getRgb(&rHLC, &gHLC, &bHLC);
    //selectForeground
    qApp->palette().highlightedText().color().getRgb(&rSFG, &gSFG, &bSFG);
    //selectBackground
    qApp->palette().highlight().color().getRgb(&rSBG, &gSBG, &bSBG);
    //troughColor (comboboxes, scales, arrow backgrounds)
    qApp->palette().button().color().getRgb(&rTC, &gTC, &bTC);
    //insertBackground (cursor)
    qApp->palette().windowText().color().getRgb(&rIBG, &gIBG, &bIBG);

    //selectColor - no idea what this is
    //qApp->palette().highlightedText().color().getRgb(&rSC, &gSC, &bSC);
    rSC=255; gSC=0; bSC=0;

    char cBuf[512];
    sprintf(cBuf, "tk_setPalette background #%02x%02x%02x "
            "foreground #%02x%02x%02x activeBackground #%02x%02x%02x "
            "selectColor #%02x%02x%02x activeForeground #%02x%02x%02x "
            "highlightBackground #%02x%02x%02x selectBackground #%02x%02x%02x "
            "highlightColor #%02x%02x%02x selectForeground #%02x%02x%02x "
            "disabledForeground #%02x%02x%02x insertBackground #%02x%02x%02x "
            "troughColor #%02x%02x%02x", rBG, gBG, bBG, rFG, gFG, bFG,
            rABG, gABG, bABG, rSC, gSC, bSC,
            rAFG, gAFG, bAFG, rHLBG, gHLBG, bHLBG,
            rSBG, gSBG, bSBG, rHLC, gHLC, bHLC,
            rSFG, gSFG, bSFG, rDFG, gDFG, bDFG,
            rIBG, gIBG, bIBG, rTC, gTC, bTC);

    Tcl_Eval(interp, cBuf);
} /* setPalette*/


int Tileqt_SetPalette(ClientData clientData, Tcl_Interp *interp,
                                 int objc, Tcl_Obj *const objv[]) 
{
    setPalette(interp);
    return TCL_OK;
} /* Tileqt_SetPalette */

int Tileqt_AvailableStyles(ClientData clientData, Tcl_Interp *interp,
                                 int objc, Tcl_Obj *const objv[]) {
  if (objc != 1) {Tcl_WrongNumArgs(interp, 1, objv, ""); return TCL_ERROR;}
  Tcl_MutexLock(&tileqtMutex);
  if (qApp) {
    QStringList styles = QStyleFactory::keys();
    Tcl_Obj* stylesObj = Tcl_NewListObj(0, NULL);
    for (QStringList::Iterator it = styles.begin(); it != styles.end(); ++it ) {
        Tcl_ListObjAppendElement(interp, stylesObj, Tcl_NewStringObj(
#ifdef TILEQT_QT_VERSION_4
                                 (*it).toUtf8().data()
#endif /* TILEQT_QT_VERSION_4 */
                                  , -1));
    }
    Tcl_SetObjResult(interp, stylesObj);
  } else {
    Tcl_SetResult(interp, (char *) "", TCL_STATIC);
  }
  Tcl_MutexUnlock(&tileqtMutex);
  return TCL_OK;
}; /* Tileqt_AvailableStyles */

int Tileqt_SetStyle(ClientData clientData, Tcl_Interp *interp,
                                 int objc, Tcl_Obj *const objv[]) {
  if (objc != 2) {Tcl_WrongNumArgs(interp, 1, objv, "style"); return TCL_ERROR;}
  Tcl_MutexLock(&tileqtMutex);
  TileQt_WidgetCache **wc_array = (TileQt_WidgetCache **) clientData;
  TileQt_WidgetCache *wc = wc_array[0];
  if (qApp) {
    int len;
    const char* str = Tcl_GetStringFromObj(objv[1], &len);
    QString style = QString::fromUtf8(str, len);
    QStyle *new_style = QStyleFactory::create(style);
    QStyle *todelete = NULL;
    if (new_style == NULL) {
      Tcl_SetResult(interp, (char *) "unknwon style: \"", TCL_STATIC);
      Tcl_AppendResult(interp, str, "\"", NULL);
      Tcl_MutexUnlock(&tileqtMutex);
      return TCL_ERROR;
    }
    //qApp->setStyle(style);
    /* Is this style the qApp style? */
    if (wc->TileQt_Style_Owner) todelete = wc->TileQt_Style;

#ifdef TILEQT_QT_VERSION_4                    
    if (qApp->style()->objectName() == style) {
      wc->TileQt_Style = qApp->style();
#endif /* TILEQT_QT_VERSION_4 */
      wc->TileQt_Style_Owner = false;
    } else {
      wc->TileQt_Style = QStyleFactory::create(style);
      wc->TileQt_Style_Owner = true;
    }
    TileQt_StoreStyleNameLowers(wc);
    wc->TileQt_QScrollBar_Widget->setStyle(wc->TileQt_Style);
    wc->TileQt_QComboBox_RW_Widget->setStyle(wc->TileQt_Style);
    wc->TileQt_QComboBox_RO_Widget->setStyle(wc->TileQt_Style);
    wc->TileQt_QWidget_WidgetParent->setStyle(wc->TileQt_Style);
    wc->TileQt_QWidget_Widget->setStyle(wc->TileQt_Style);
    wc->TileQt_QSlider_Hor_Widget->setStyle(wc->TileQt_Style);
    wc->TileQt_QSlider_Ver_Widget->setStyle(wc->TileQt_Style);
    wc->TileQt_QProgressBar_Hor_Widget->setStyle(wc->TileQt_Style);
    wc->TileQt_QTabWidget_Widget->setStyle(wc->TileQt_Style);
    wc->TileQt_QPixmap_BackgroundTile = 
#ifdef TILEQT_QT_VERSION_4
                     (wc->TileQt_QWidget_Widget)->palette().window().texture();
#endif /* TILEQT_QT_VERSION_4 */
    wc->TileQt_Style->polish(wc->TileQt_QWidget_Widget);
    if (todelete) delete todelete;
#if 0
    // Print Scrollbar statistics...
#define SC_PRINT_INFO(subcontrol) \
    wc->TileQt_Style->querySubControlMetrics(QStyle::CC_ScrollBar,\
        wc->TileQt_QScrollBar_Widget, subcontrol).x(),\
    wc->TileQt_Style->querySubControlMetrics(QStyle::CC_ScrollBar,\
        wc->TileQt_QScrollBar_Widget, subcontrol).y(),\
    wc->TileQt_Style->querySubControlMetrics(QStyle::CC_ScrollBar,\
        wc->TileQt_QScrollBar_Widget, subcontrol).width(),\
    wc->TileQt_Style->querySubControlMetrics(QStyle::CC_ScrollBar,\
        wc->TileQt_QScrollBar_Widget, subcontrol).height()
    printf("SC_ScrollBarAddLine: x=%d, y=%d, w=%d, h=%d\n", SC_PRINT_INFO(QStyle::SC_ScrollBarAddLine));
    printf("SC_ScrollBarSubLine: x=%d, y=%d, w=%d, h=%d\n", SC_PRINT_INFO(QStyle::SC_ScrollBarSubLine));
    printf("SC_ScrollBarAddPage: x=%d, y=%d, w=%d, h=%d\n", SC_PRINT_INFO(QStyle::SC_ScrollBarAddPage));
    printf("SC_ScrollBarSubPage: x=%d, y=%d, w=%d, h=%d\n", SC_PRINT_INFO(QStyle::SC_ScrollBarSubPage));
    printf("SC_ScrollBarFirst: x=%d, y=%d, w=%d, h=%d\n", SC_PRINT_INFO(QStyle::SC_ScrollBarFirst));
    printf("SC_ScrollBarLast: x=%d, y=%d, w=%d, h=%d\n", SC_PRINT_INFO(QStyle::SC_ScrollBarLast));
#endif
  }
  memcpy(wc_array[1], wc_array[0], sizeof(TileQt_WidgetCache));
  wc_array[0]->orientation = TTK_ORIENT_HORIZONTAL;
  wc_array[1]->orientation = TTK_ORIENT_VERTICAL;
  /* Save the name of the current theme... */
  Tcl_SetVar(interp, "ttk::theme::tileqt::theme",
#ifdef TILEQT_QT_VERSION_4                    
             wc->TileQt_Style->objectName().toUtf8().data(), TCL_GLOBAL_ONLY);
#endif /* TILEQT_QT_VERSION_4 */
  Tcl_MutexUnlock(&tileqtMutex);
  return TCL_OK;
}; /* Tileqt_SetStyle */

int Tileqt_GetPixelMetric(ClientData clientData, Tcl_Interp *interp,
                                 int objc, Tcl_Obj *const objv[]) {
/*
  PM_TabBarTabOverlap         - number of pixels the tabs should overlap.
  PM_TabBarTabHSpace          - extra space added to the tab width.
  PM_TabBarTabVSpace          - extra space added to the tab height.
  PM_TabBarBaseHeight         - height of the area between the tab bar and the
                                tab pages.
  PM_TabBarBaseOverlap        - number of pixels the tab bar overlaps the
                                tab bar base.
  PM_TabBarScrollButtonWidth
  PM_TabBarTabShiftHorizontal - horizontal pixel shift when a tab is selected.
  PM_TabBarTabShiftVertical   - vertical pixel shift when a tab is selected.
*/
  static const char *Methods[] = {
    "-PM_TabBarTabOverlap",       "-PM_TabBarTabHSpace",
    "-PM_TabBarTabVSpace",        "-PM_TabBarBaseHeight",
    "-PM_TabBarBaseOverlap",      "-PM_TabBarTabShiftHorizontal",
    "-PM_TabBarTabShiftVertical", "-PM_TabBarScrollButtonWidth",
    "-PM_DefaultFrameWidth",
    (char *) NULL
  };
  enum methods {
    PM_TabBarTabOverlap,       PM_TabBarTabHSpace,
    PM_TabBarTabVSpace,        PM_TabBarBaseHeight,
    PM_TabBarBaseOverlap,      PM_TabBarTabShiftHorizontal,
    PM_TabBarTabShiftVertical, PM_TabBarScrollButtonWidth,
    PM_DefaultFrameWidth
  };
  int index, pixels = 0;
  TileQt_WidgetCache **wc_array = (TileQt_WidgetCache **) clientData;
  TileQt_WidgetCache *wc = wc_array[0];
  if (objc != 2) {
    Tcl_WrongNumArgs(interp, 1, objv, "pixel_metric_identifier");
    return TCL_ERROR;
  }
  if (!qApp) {
    Tcl_SetResult(interp, (char *) "", TCL_STATIC);
    return TCL_OK;
  }
  if (Tcl_GetIndexFromObj(interp, objv[1], (const char **) Methods,
                            "method", 0, &index) != TCL_OK) {
    return TCL_ERROR;
  }
#ifdef TILEQT_QT_VERSION_4
#ifndef QStyleOptionTabV2
#define QStyleOptionTabV2 QStyleOptionTab
#endif /* QStyleOptionTabV2 */
#define PM(pm) (wc->TileQt_Style->pixelMetric(QStyle::pm, &option, \
                                              wc->TileQt_QTabBar_Widget))
#define PM2(pm) (wc->TileQt_Style->pixelMetric(QStyle::pm, &option, \
                                              wc->TileQt_QTabWidget_Widget))
#endif /* TILEQT_QT_VERSION_4 */
  Tcl_MutexLock(&tileqtMutex);
#ifdef TILEQT_QT_VERSION_4
  QStyleOptionTabV2 option;
  option.initFrom(wc->TileQt_QTabBar_Widget);
#endif /* TILEQT_QT_VERSION_4 */
  switch ((enum methods) index) {
    case PM_TabBarTabOverlap:  {pixels = PM(PM_TabBarTabOverlap);  break;}
    case PM_TabBarTabHSpace:   {pixels = PM(PM_TabBarTabHSpace);   break;}
    case PM_TabBarTabVSpace:   {pixels = PM(PM_TabBarTabVSpace);   break;}
    case PM_TabBarBaseOverlap: {pixels = PM(PM_TabBarBaseOverlap); break;}
    case PM_TabBarBaseHeight:  {pixels = PM2(PM_TabBarBaseHeight); break;}
    case PM_TabBarTabShiftHorizontal: {pixels = PM(PM_TabBarTabShiftHorizontal);
                                       break;}
    case PM_TabBarTabShiftVertical:   {pixels = PM(PM_TabBarTabShiftVertical);
                                       break;}
    case PM_TabBarScrollButtonWidth:  {pixels = PM(PM_TabBarScrollButtonWidth);
                                       break;}
    case PM_DefaultFrameWidth:        {pixels = PM(PM_DefaultFrameWidth);
                                       break;}
  }
  Tcl_MutexUnlock(&tileqtMutex);
  Tcl_SetObjResult(interp, Tcl_NewIntObj(pixels));
  return TCL_OK;
}; /* Tileqt_GetPixelMetric */

int Tileqt_GetStyleHint(ClientData clientData, Tcl_Interp *interp,
                                 int objc, Tcl_Obj *const objv[]) {
/*
  SH_TabBar_Alignment         - The alignment for tabs in a QTabBar.
                                Possible values are Qt::AlignLeft,
                                Qt::AlignCenter and Qt::AlignRight.
*/
  static const char *Methods[] = {
    "-SH_TabBar_Alignment",
    (char *) NULL
  };
  enum methods {
    SH_TabBar_Alignment
  };
  int index, hint = 0;
  QStyle::StyleHint stylehint;
  const char *pstr = "";
  TileQt_WidgetCache **wc_array = (TileQt_WidgetCache **) clientData;
  TileQt_WidgetCache *wc = wc_array[0];
  if (objc != 2) {
    Tcl_WrongNumArgs(interp, 1, objv, "style_hint_identifier");
    return TCL_ERROR;
  }
  if (!qApp) {
    Tcl_SetResult(interp, (char *) "", TCL_STATIC);
    return TCL_OK;
  }
  if (Tcl_GetIndexFromObj(interp, objv[1], (const char **) Methods,
                            "method", 0, &index) != TCL_OK) {
    return TCL_ERROR;
  }
#ifdef TILEQT_QT_VERSION_4
#endif /* TILEQT_QT_VERSION_4 */
  switch ((enum methods) index) {
    case SH_TabBar_Alignment:  {stylehint = QStyle::SH_TabBar_Alignment; break;}
  }
  Tcl_MutexLock(&tileqtMutex);
  hint = wc->TileQt_Style->styleHint(stylehint);
  Tcl_MutexUnlock(&tileqtMutex);
  switch (hint) {
    case Qt::AlignLeft:    {pstr = "Qt::AlignLeft";    break;}
    case Qt::AlignRight:   {pstr = "Qt::AlignRight";   break;}
    case Qt::AlignCenter:  {pstr = "Qt::AlignCenter";  break;}
    case Qt::AlignJustify: {pstr = "Qt::AlignJustify"; break;}
    case Qt::AlignTop:     {pstr = "Qt::AlignTop";     break;}
    case Qt::AlignBottom:  {pstr = "Qt::AlignBottom";  break;}
    case Qt::AlignVCenter: {pstr = "Qt::AlignVCenter"; break;}
    case Qt::AlignHCenter: {pstr = "Qt::AlignHCenter"; break;}
    default: {
      Tcl_SetObjResult(interp, Tcl_NewIntObj(hint));
      return TCL_OK;
    }
  }
  Tcl_SetResult(interp, (char *) pstr, TCL_STATIC);
  return TCL_OK;
}; /* Tileqt_GetStyleHint */

int Tileqt_GetSubControlMetrics(ClientData clientData, Tcl_Interp *interp,
                                 int objc, Tcl_Obj *const objv[]) {
  /*
   * QStyle::SC_ScrollBarAddLine - scrollbar add line (i.e. down/right arrow). 
   * QStyle::SC_ScrollBarSubLine - scrollbar sub line (i.e. up/left arrow). 
   * QStyle::SC_ScrollBarAddPage - scrollbar add page (i.e. page down). 
   * QStyle::SC_ScrollBarSubPage - scrollbar sub page (i.e. page up). 
   * QStyle::SC_ScrollBarFirst   - scrollbar first line (i.e. home). 
   * QStyle::SC_ScrollBarLast    - scrollbar last line (i.e. end). 
   * QStyle::SC_ScrollBarSlider  - scrollbar slider handle. 
   * QStyle::SC_ScrollBarGroove  - special subcontrol which contains the area
   *                               in which the slider handle may move.
   */
  static const char *Methods[] = {
    "-SC_ScrollBarAddLine",       "-SC_ScrollBarSubLine",
    "-SC_ScrollBarAddPage",       "-SC_ScrollBarSubPage",
    "-SC_ScrollBarFirst",         "-SC_ScrollBarLast",
    "-SC_ScrollBarSlider",        "-SC_ScrollBarGroove",
    (char *) NULL
  };
  enum methods {
    SC_ScrollBarAddLine,       SC_ScrollBarSubLine,
    SC_ScrollBarAddPage,       SC_ScrollBarSubPage,
    SC_ScrollBarFirst,         SC_ScrollBarLast,
    SC_ScrollBarSlider,        SC_ScrollBarGroove
  };
  int index;
  Tcl_Obj *result;
  TileQt_WidgetCache **wc_array = (TileQt_WidgetCache **) clientData;
  TileQt_WidgetCache *wc = wc_array[0];
  if (objc != 2) {
    Tcl_WrongNumArgs(interp, 1, objv, "sub_control_identifier");
    return TCL_ERROR;
  }
  if (!qApp) {
    Tcl_SetResult(interp, (char *) "", TCL_STATIC);
    return TCL_OK;
  }
  if (Tcl_GetIndexFromObj(interp, objv[1], (const char **) Methods,
                            "method", 0, &index) != TCL_OK) {
    return TCL_ERROR;
  }
  QStyle::ComplexControl control = QStyle::CC_ScrollBar;
  QWidget *widget = 0;
  QStyle::SubControl subcontrol = QStyle::SC_None;
#ifdef TILEQT_QT_VERSION_4
  QStyleOptionComplex *option = NULL;
#endif /* TILEQT_QT_VERSION_4 */

  if ((enum methods) index >= SC_ScrollBarAddLine &&
      (enum methods) index <= SC_ScrollBarGroove) {
    widget  = wc->TileQt_QScrollBar_Widget;
    control = QStyle::CC_ScrollBar;
#ifdef TILEQT_QT_VERSION_4
    option  = new QStyleOptionComplex();
    if (option) option->initFrom(widget);
#endif /* TILEQT_QT_VERSION_4 */
  }
  switch ((enum methods) index) {
    case SC_ScrollBarAddLine: {
      subcontrol = QStyle::SC_ScrollBarAddLine; break;
    }
    case SC_ScrollBarSubLine: {
      subcontrol = QStyle::SC_ScrollBarSubLine; break;
    }
    case SC_ScrollBarAddPage: {
      subcontrol = QStyle::SC_ScrollBarAddPage; break;
    }
    case SC_ScrollBarSubPage: {
      subcontrol = QStyle::SC_ScrollBarSubPage; break;
    }
    case SC_ScrollBarFirst: {
      subcontrol = QStyle::SC_ScrollBarFirst;   break;
    }
    case SC_ScrollBarLast: {
      subcontrol = QStyle::SC_ScrollBarLast;    break;
    }
    case SC_ScrollBarSlider: {
      subcontrol = QStyle::SC_ScrollBarSlider;  break;
    }
    case SC_ScrollBarGroove: {
      subcontrol = QStyle::SC_ScrollBarGroove;  break;
    }
  }
  Tcl_MutexLock(&tileqtMutex);
#ifdef TILEQT_QT_VERSION_4
  QRect rc = wc->TileQt_Style->
    subControlRect(control, option, subcontrol, widget);
  if (option) delete option;
#endif /* TILEQT_QT_VERSION_4 */
  Tcl_MutexUnlock(&tileqtMutex);
  result = Tcl_NewListObj(0, NULL);
  Tcl_ListObjAppendElement(interp, result, Tcl_NewIntObj(rc.x()));
  Tcl_ListObjAppendElement(interp, result, Tcl_NewIntObj(rc.y()));
  Tcl_ListObjAppendElement(interp, result, Tcl_NewIntObj(rc.width()));
  Tcl_ListObjAppendElement(interp, result, Tcl_NewIntObj(rc.height()));
  Tcl_SetObjResult(interp, result);
  return TCL_OK;
}; /* Tileqt_GetSubControlMetrics */

extern "C" int DLLEXPORT
Tileqt_Init(Tcl_Interp *interp)
{
    Ttk_Theme themePtr;
    Tk_Window tkwin;
    char tmpScript[1024];
    TileQt_WidgetCache **wc = NULL;

    if (Tcl_InitStubs(interp, TCL_VERSION, 0) == NULL)
        return TCL_ERROR;
    if (Tk_InitStubs(interp,  TK_VERSION,  0) == NULL)
        return TCL_ERROR;
    if (Ttk_InitStubs(interp) == NULL)
        return TCL_ERROR;

    /* The first thing we must do, is to retrieve a valid display. */
    // Tcl_MutexLock(&tileqtMutex);
    // if (TileQt_QAppCreated == 0) qApp = NULL;
    // Tcl_MutexUnlock(&tileqtMutex);
    tkwin = Tk_MainWindow(interp);
    if (tkwin == NULL) return TCL_ERROR;

    themePtr  = Ttk_CreateTheme(interp, "tileqt", NULL);
    if (!themePtr) return TCL_ERROR;
    /*
     * Initialise Qt:
     */
    Tcl_MutexLock(&tileqtMutex);
    wc = TileQt_CreateQApp(interp);
    ++TileQt_QAppCreated;

    /*
     * Register the various widgets...
     */
    TileQt_Init_Background(interp, wc, themePtr);
    TileQt_Init_Button(interp, wc, themePtr);
    TileQt_Init_CheckButton(interp, wc, themePtr);
    TileQt_Init_RadioButton(interp, wc, themePtr);
    TileQt_Init_Menubutton(interp, wc, themePtr);
    TileQt_Init_ToolButton(interp, wc, themePtr);
    TileQt_Init_Entry(interp, wc, themePtr);
    TileQt_Init_Combobox(interp, wc, themePtr);
    TileQt_Init_Labelframe(interp, wc, themePtr);
    TileQt_Init_Notebook(interp, wc, themePtr);
    TileQt_Init_TreeView(interp, wc, themePtr);
    TileQt_Init_Progress(interp, wc, themePtr);
    TileQt_Init_Paned(interp, wc, themePtr);
    TileQt_Init_SizeGrip(interp, wc, themePtr);
    //TileQt_Init_Separator(interp, wc, themePtr);
    //TileQt_Init_Arrows(interp, wc, themePtr);
    Tcl_CreateExitHandler(&TileQt_ExitProc, 0);
    //Tcl_CreateThreadExitHandler(&TileQt_ExitProc, 0);
    
    /*
     * Register the TileQt package...
     */
    Tcl_CreateObjCommand(interp, "ttk::theme::tileqt::currentThemeName",
                         Tileqt_ThemeName, (ClientData) wc, NULL);
    Tcl_CreateObjCommand(interp,
                         "ttk::theme::tileqt::currentThemeColour",
                         Tileqt_ThemeColour, (ClientData) wc, NULL);
    Tcl_CreateObjCommand(interp,
                         "ttk::theme::tileqt::availableStyles_AsReturned",
                         Tileqt_AvailableStyles, (ClientData) wc, NULL);
    Tcl_CreateObjCommand(interp,
                         "ttk::theme::tileqt::setStyle",
                         Tileqt_SetStyle, (ClientData) wc, NULL);
    Tcl_CreateObjCommand(interp,
                         "ttk::theme::tileqt::setPalette",
                         Tileqt_SetPalette, (ClientData) wc, NULL);
    Tcl_CreateObjCommand(interp,
                         "ttk::theme::tileqt::getPixelMetric",
                         Tileqt_GetPixelMetric, (ClientData) wc, NULL);
    Tcl_CreateObjCommand(interp,
                         "ttk::theme::tileqt::getStyleHint",
                         Tileqt_GetStyleHint, (ClientData) wc, NULL);
    Tcl_CreateObjCommand(interp,
                         "ttk::theme::tileqt::getSubControlMetrics",
                         Tileqt_GetSubControlMetrics, (ClientData) wc, NULL);
    /* Save the name of the current theme... */
    strcpy(tmpScript, "namespace eval ttk::theme::tileqt { variable theme ");
    if (qApp) {
#ifdef TILEQT_QT_VERSION_4                    
      strcat(tmpScript, qApp->style()->objectName().toUtf8().data());
#endif /* TILEQT_QT_VERSION_4 */
    } else {
      strcat(tmpScript, "{}");
    }
    strcat(tmpScript, " };");
    Tcl_MutexUnlock(&tileqtMutex);
    /* This will run library/tileqt.tcl */
    if (Tcl_Eval(interp, tmpScript) != TCL_OK) {
      return TCL_ERROR;
    }
    if (Tcl_Eval(interp, initScript) != TCL_OK) {
      return TCL_ERROR;
    }
    Tcl_PkgProvide(interp, "ttk::theme::tileqt", PACKAGE_VERSION);
    Tcl_PkgProvide(interp, PACKAGE_NAME, PACKAGE_VERSION);
    
    setPalette(interp);
    return TCL_OK;
}; /* TileQt_Init */

int DLLEXPORT
TileQt_Finish(Tcl_Interp *interp)
{
    Tcl_MutexLock(&tileqtMutex);
    if (TileQt_QAppCreated < 0) {Tcl_MutexUnlock(&tileqtMutex); return 0;}
    --TileQt_QAppCreated;
    if (TileQt_QAppCreated == 0) TileQt_DestroyQApp();
    Tcl_MutexUnlock(&tileqtMutex);
    return 0;
}; /* TileQt_Finish */
