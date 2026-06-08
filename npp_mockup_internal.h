#ifndef NPP_MOCKUP_INTERNAL_H
#define NPP_MOCKUP_INTERNAL_H

#include "win_compat.h"

#include <commctrl.h>
#include <richedit.h>
#include <shellapi.h>
#include <stdio.h>
#include <stdlib.h>

#include "npp_mockup_window.h"
#include "debug_window.h"

#ifndef MSFTEDIT_CLASSA
#define MSFTEDIT_CLASSA "RICHEDIT50A"
#endif

#ifndef EM_SETBKGNDCOLOR
#define EM_SETBKGNDCOLOR (WM_USER + 67)
#endif

#ifndef EM_SETEVENTMASK
#define EM_SETEVENTMASK (WM_USER + 69)
#endif

#ifndef EM_GETEVENTMASK
#define EM_GETEVENTMASK (WM_USER + 59)
#endif

#ifndef ENM_SELCHANGE
#define ENM_SELCHANGE 0x00080000
#endif

#ifndef EM_EXLINEFROMCHAR
#define EM_EXLINEFROMCHAR (WM_USER + 54)
#endif

#ifndef EM_GETFIRSTVISIBLELINE
#define EM_GETFIRSTVISIBLELINE 0x00CE
#endif

#ifndef EM_LINESCROLL
#define EM_LINESCROLL 0x00B6
#endif

#ifndef WS_EX_ACCEPTFILES
#define WS_EX_ACCEPTFILES 0x00000010L
#endif

#define NPP_MOCKUP_WINDOW_CLASS_NAME "NppMockupWindowClass"
#define NPP_MOCKUP_WINDOW_TITLE "NP++ Mockup"

#define NPP_MAX_DOCS 64
#define NPP_MAX_TOOL_BUTTONS 64

#define NPP_TIMER_ID 1
#define NPP_TIMER_MS 250

#define NPP_TOOLBAR_HEIGHT 30
#define NPP_TAB_HEIGHT 28
#define NPP_STATUS_HEIGHT 24
#define NPP_SPLITTER_WIDTH 6
#define NPP_MIN_PANE_WIDTH 160

#define ID_NPP_LEFT_TAB 3001
#define ID_NPP_RIGHT_TAB 3002
#define ID_NPP_LEFT_EDIT 3003
#define ID_NPP_RIGHT_EDIT 3004

typedef struct NppDocument
{
    char title[MAX_PATH];
    char path[MAX_PATH];
    char *text;
    DWORD selStart;
    DWORD selEnd;
    int firstVisibleLine;
} NppDocument;

typedef struct NppPane
{
    HWND tab;
    HWND edit;
    WNDPROC oldEditProc;

    NppDocument docs[NPP_MAX_DOCS];
    int docCount;
    int activeDoc;
    int paneIndex;
} NppPane;

typedef UINT (WINAPI *NppDragQueryFileProc)(HDROP, UINT, LPSTR, UINT);
typedef void (WINAPI *NppDragFinishProc)(HDROP);

extern HINSTANCE g_npp_hInstance;
extern HWND g_npp_window;
extern HWND g_npp_status;

extern HWND g_npp_toolbarButtons[NPP_MAX_TOOL_BUTTONS];
extern int g_npp_toolbarButtonCount;

extern NppPane g_npp_panes[2];
extern int g_npp_activePane;

extern int g_npp_splitPercent;
extern int g_npp_draggingSplitter;
extern RECT g_npp_splitterRect;

extern HMODULE g_npp_richEditModule;
extern const char *g_npp_editClassName;
extern int g_npp_usingRichEdit;

extern HBRUSH g_npp_statusBrush;
extern NppMockupWindowClosedCallback g_npp_onClosed;

extern HMODULE g_npp_shell32;
extern NppDragQueryFileProc g_npp_dragQueryFile;
extern NppDragFinishProc g_npp_dragFinish;

#define g_hInstance g_npp_hInstance
#define g_window g_npp_window
#define g_status g_npp_status
#define g_toolbarButtons g_npp_toolbarButtons
#define g_toolbarButtonCount g_npp_toolbarButtonCount
#define g_panes g_npp_panes
#define g_activePane g_npp_activePane
#define g_splitPercent g_npp_splitPercent
#define g_draggingSplitter g_npp_draggingSplitter
#define g_splitterRect g_npp_splitterRect
#define g_richEditModule g_npp_richEditModule
#define g_editClassName g_npp_editClassName
#define g_usingRichEdit g_npp_usingRichEdit
#define g_statusBrush g_npp_statusBrush
#define g_onClosed g_npp_onClosed
#define g_shell32 g_npp_shell32
#define g_dragQueryFile g_npp_dragQueryFile
#define g_dragFinish g_npp_dragFinish

void Npp_CopyText(
    char *dest,
    int destSize,
    const char *src
);

char *Npp_DuplicateText(
    const char *text
);

void Npp_FreeDocument(
    NppDocument *doc
);

void Npp_FreePane(
    NppPane *pane
);

void Npp_LoadRichEditClass(void);

void Npp_SetEditTheme(
    HWND edit
);

void Npp_GetEditSelection(
    HWND edit,
    DWORD *start,
    DWORD *end
);

void Npp_SetEditSelection(
    HWND edit,
    DWORD start,
    DWORD end
);

int Npp_GetLineFromChar(
    HWND edit,
    int charIndex
);

int Npp_GetLineIndex(
    HWND edit,
    int line
);

int Npp_GetLineCount(
    HWND edit
);

HWND Npp_CreateEditor(
    HWND parent,
    int id
);

void Npp_SubclassEdit(
    NppPane *pane
);

HWND Npp_CreateTabControl(
    HWND parent,
    int id
);

void Npp_PaneSaveActiveDocument(
    NppPane *pane
);

void Npp_PaneLoadActiveDocument(
    NppPane *pane
);

int Npp_PaneAddDocumentFromText(
    NppPane *pane,
    const char *title,
    const char *path,
    const char *text,
    int selectNew
);

void Npp_PaneSelectDocument(
    NppPane *pane,
    int index
);

void Npp_HandleTabNotify(
    NMHDR *hdr
);

void Npp_CreateInitialDocuments(void);

void Npp_HandleDropFiles(
    HDROP drop,
    int paneIndex
);

void Npp_UpdateStatus(void);

#endif