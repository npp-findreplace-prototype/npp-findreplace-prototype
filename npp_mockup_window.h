#ifndef NPP_MOCKUP_WINDOW_H
#define NPP_MOCKUP_WINDOW_H

#include <windows.h>

typedef void (*NppMockupWindowClosedCallback)(void);

typedef struct NppMockupTextInfo
{
    int characterCount;
    int lineCount;

    int caretCharacter;
    int caretLine;
    int caretColumn;

    int selectionStartCharacter;
    int selectionEndCharacter;
    int selectionStartLine;
    int selectionEndLine;

    int selectedCharacterCount;
    int selectedLineCount;
    int hasSelection;
} NppMockupTextInfo;

HWND NppMockupWindow_Show(
    HINSTANCE hInstance,
    NppMockupWindowClosedCallback onClosed
);

void NppMockupWindow_Close(void);
HWND NppMockupWindow_GetHwnd(void);
int NppMockupWindow_IsOpen(void);

HWND NppMockupWindow_GetActiveRichEdit(void);
int NppMockupWindow_GetActiveTextInfo(NppMockupTextInfo *info);

char *NppMockupWindow_AllocActiveText(int *length);
void NppMockupWindow_FreeText(char *text);

#endif