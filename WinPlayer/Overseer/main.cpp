#include <vector>
#include "HookData.h"
#include "MKActionMerger.h"
#include "IoUtils.h"

//#define _WIN32_WINNT 0x050

using namespace std;

vector<MouseKeyboardAction> actions;
bool recordMode = false;                        // press F7 enter record mode
bool hookMode = false;
HHOOK hhkLowLevelKbd, hhkLowLevelMouse;

void save_actions();

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    BOOL fEatKeystroke = FALSE;

    if (nCode == HC_ACTION)
    {
        DWORD timestamp = ((KBDLLHOOKSTRUCT*)lParam)->time;
        DWORD vkCode = ((KBDLLHOOKSTRUCT*)lParam)->vkCode;
        string key = HookConstants::instance().virtualKeys.right.at(vkCode);
        
        switch (wParam)
        {
        case WM_KEYDOWN:
            cout << "WM_KEYDOWN " << vkCode << " " << key << endl;
            if (recordMode)
                actions.push_back({ timestamp, KEYBOARD_KEY_DOWN, key, vkCode });
            break;
        case WM_KEYUP:
            if (vkCode == VK_F7)
            {
                recordMode = !recordMode;

                if (recordMode)         // first time enter record mode, don't record this F7 press
                {
                    cout << "start record mode... " << endl;
                    return CallNextHookEx(NULL, nCode, wParam, lParam);
                }
                else
                {
                    cout << "finish record mode... " << endl;
                    save_actions();
                }

            }

            cout << "WM_KEYUP " << vkCode << " " << key << endl;
            if (recordMode)
                actions.push_back({ timestamp, KEYBOARD_KEY_UP, key, vkCode });
            break;
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{

    if (nCode == HC_ACTION)
    {
        int x = ((PMSLLHOOKSTRUCT)lParam)->pt.x;
        int y = ((PMSLLHOOKSTRUCT)lParam)->pt.y;
        DWORD timestamp = ((PMSLLHOOKSTRUCT)lParam)->time;

        switch (wParam)
        {
        case WM_LBUTTONUP:
            cout << "WM_LBUTTONUP ( " << x << ", " << y << " )" << endl;
            if (recordMode)
                actions.push_back({ timestamp, MOUSE_CLICK_LEFT, x, y });
            break;
        case WM_RBUTTONUP:
            cout << "WM_RBUTTONUP ( " << x << ", " << y << " )" << endl;
            if (recordMode)
                actions.push_back({ timestamp, MOUSE_CLICK_RIGHT, x, y });
            break;
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

void toggle_hook()
{
    hookMode = !hookMode;
    if (hookMode)
    {
        hhkLowLevelKbd = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, 0, 0);
        hhkLowLevelMouse = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, 0, 0);
    }
    else {
        UnhookWindowsHookEx(hhkLowLevelKbd);
        UnhookWindowsHookEx(hhkLowLevelMouse);
    }
}

void save_actions()
{
    toggle_hook();
    cout << "------------------------" << endl;
    for (MouseKeyboardAction& action : actions)
    {
        cout << action << endl;
    }
    cout << "------------------------" << endl;

    MKActionMerger merger;
    merger.merge(actions);

    for (MouseKeyboardAction& action : merger.actions)
    {
        cout << action << endl;
    }
    cout << "------------------------" << endl;

    IoUtils::toFile(string("actions.txt"), merger.actions);

    actions.clear();
    toggle_hook();
}

int main()
{
    // Install the low-level keyboard & mouse hooks
    toggle_hook();

    // Keep this app running until Ctrl + C
    MSG msg;
    while (!GetMessage(&msg, NULL, NULL, NULL)) {    //this while loop keeps the hook
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    toggle_hook();

    return(0);
}