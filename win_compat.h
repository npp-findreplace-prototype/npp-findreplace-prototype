#ifndef WIN_COMPAT_H
#define WIN_COMPAT_H

#include <windows.h>

#ifndef WM_DPICHANGED
#define WM_DPICHANGED 0x02E0
#endif

#ifndef WM_MOUSELEAVE
#define WM_MOUSELEAVE 0x02A3
#endif

#ifndef TME_LEAVE
#define TME_LEAVE 0x00000002
#endif

#ifndef GWLP_WNDPROC
#define GWLP_WNDPROC GWL_WNDPROC
#endif

#ifndef GWLP_USERDATA
#define GWLP_USERDATA GWL_USERDATA
#endif

#ifndef GWLP_HINSTANCE
#define GWLP_HINSTANCE GWL_HINSTANCE
#endif

#ifndef GWLP_ID
#define GWLP_ID GWL_ID
#endif

#ifndef GetWindowLongPtr
#define GetWindowLongPtr GetWindowLong
#endif

#ifndef SetWindowLongPtr
#define SetWindowLongPtr SetWindowLong
#endif

#ifndef LONG_PTR
#define LONG_PTR LONG
#endif

#ifndef UINT_PTR
#define UINT_PTR UINT
#endif

#ifndef INT_PTR
#define INT_PTR INT
#endif

#ifndef DWORD_PTR
#define DWORD_PTR DWORD
#endif

#ifndef IDC_HAND
#define IDC_HAND MAKEINTRESOURCE(32649)
#endif

#ifndef WS_EX_CONTROLPARENT
#define WS_EX_CONTROLPARENT 0x00010000L
#endif

#ifndef WS_EX_COMPOSITED
#define WS_EX_COMPOSITED 0x02000000L
#endif

#ifndef WS_CLIPSIBLINGS
#define WS_CLIPSIBLINGS 0x04000000L
#endif

#ifndef WS_CLIPCHILDREN
#define WS_CLIPCHILDREN 0x02000000L
#endif

#ifndef SWP_NOCOPYBITS
#define SWP_NOCOPYBITS 0x0100
#endif

#ifndef RDW_NOERASE
#define RDW_NOERASE 0x0020
#endif

#ifndef RDW_ALLCHILDREN
#define RDW_ALLCHILDREN 0x0080
#endif

#ifndef RDW_NOCHILDREN
#define RDW_NOCHILDREN 0x0040
#endif

#ifndef RDW_UPDATENOW
#define RDW_UPDATENOW 0x0100
#endif

#ifndef RDW_INVALIDATE
#define RDW_INVALIDATE 0x0001
#endif

#ifndef RDW_ERASE
#define RDW_ERASE 0x0004
#endif

#ifndef DLGC_WANTARROWS
#define DLGC_WANTARROWS 0x0001
#endif

#ifndef DLGC_WANTTAB
#define DLGC_WANTTAB 0x0002
#endif

#ifndef DLGC_WANTCHARS
#define DLGC_WANTCHARS 0x0080
#endif

#ifndef VK_RETURN
#define VK_RETURN 0x0D
#endif

#ifndef VK_TAB
#define VK_TAB 0x09
#endif

#ifndef VK_SPACE
#define VK_SPACE 0x20
#endif

#ifndef VK_ESCAPE
#define VK_ESCAPE 0x1B
#endif

#ifndef EM_SETRECT
#define EM_SETRECT 0x00B3
#endif

#ifndef RT_RCDATA
#define RT_RCDATA MAKEINTRESOURCE(10)
#endif

#ifndef INVALID_FILE_ATTRIBUTES
#define INVALID_FILE_ATTRIBUTES ((DWORD)-1)
#endif

#ifndef INVALID_FILE_SIZE
#define INVALID_FILE_SIZE ((DWORD)-1)
#endif

#endif