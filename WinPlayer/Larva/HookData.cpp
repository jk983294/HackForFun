#include "HookData.h"
#include "StringUtils.h"
#include <shellapi.h>

const wstring webBrowser(L"C:/Program Files (x86)/Google/Chrome/Application/chrome.exe");
const double fScreenWidth = ::GetSystemMetrics(SM_CXSCREEN) - 1;
const double fScreenHeight = ::GetSystemMetrics(SM_CYSCREEN) - 1;

MouseKeyboardAction::MouseKeyboardAction(MKActionType type, DWORD sleepTime) :
type(type), sleepTime(sleepTime)
{
}

MouseKeyboardAction::MouseKeyboardAction(DWORD timestamp, MKActionType type, int x, int y) :
timestamp(timestamp), type(type), x(x), y(y)
{
}

MouseKeyboardAction::MouseKeyboardAction(DWORD timestamp, MKActionType type, string key, DWORD vkey) :
timestamp(timestamp), type(type), key(key), vkey(vkey)
{
}

MouseKeyboardAction::MouseKeyboardAction(MKActionType type, wstring executable) :
type(type), executable(executable)
{
}

MouseKeyboardAction::MouseKeyboardAction(MKActionType type, string cmd) :
type(type), key(cmd)
{
    if (type == KEYBOARD_CTRL)
    {
        vkey = get_key(key);
    }
    else if (type == KEYBOARD)
    {
        vkey = get_key(key);
        if (!vkey)
            get_decomposed_keys();
    }
}

MouseKeyboardAction::MouseKeyboardAction(MKActionType type, int x, int y) :
type(type), x(x), y(y)
{
}

MouseKeyboardAction::MouseKeyboardAction(MKActionType type) :
type(type), x(0), y(0)
{
}

BYTE MouseKeyboardAction::get_key(string key_name)
{
    if (HookConstants::instance().containVirtualKey(key_name))
        return static_cast<BYTE>(HookConstants::instance().virtualKeys.left.at(key_name));
    else return 0;
}

void MouseKeyboardAction::get_decomposed_keys()
{
    auto itr = key.begin();
    while (itr != key.end())
    {
        string str(itr, itr + 1);
        if (HookConstants::instance().containVirtualKey(str))
        {
            decomposedVkeys.push_back(get_key(str));
        }
        else if (itr + 1 != key.end())
        {
            string str1(itr, itr + 2);
            if (HookConstants::instance().containVirtualKey(str1))
            {
                decomposedVkeys.push_back(get_key(str1));
                ++itr;
            }
        }
        ++itr;
    }
}

ostream& operator<<(ostream& s, const MouseKeyboardAction& action){
    string actionTypeStr = HookConstants::instance().mkActionTypes.right.at(action.type);
    switch (action.type)
    {
    case MOUSE_MOVE:
        s << actionTypeStr << " " << action.x << " " << action.y;
        break;
    case MOUSE_CLICK_LEFT:
    case MOUSE_CLICK_RIGHT:
    case MOUSE_DOUBLE_CLICK_LEFT:
        s << actionTypeStr;
        break;
    case KEYBOARD:
    case KEYBOARD_KEY_DOWN:
    case KEYBOARD_KEY_UP:
    case KEYBOARD_CTRL:
        s << actionTypeStr << " " << action.key;
        break;
    case OPEN_WEB:
    case OPEN_APP:
        s << actionTypeStr << endl << StringUtils::wstring2string(action.executable);
        break;
    case SLEEP:
        s << actionTypeStr << " " << action.sleepTime;
        break;
    }
    return s;
}

// 0 - 9, 'a' - 'z', 0 - 9 of number pad
bool isNormalInput(DWORD vkey)
{
    return (vkey >= 0x30 && vkey <= (0x30 + 9))
        || (vkey >= 0x41 && vkey <= (0x41 + ('z' - 'a')))
        || (vkey >= VK_NUMPAD0 && vkey <= VK_NUMPAD9)
        || (vkey >= VK_MULTIPLY && vkey <= VK_DIVIDE);
}

void HookConstants::addVirtualKeyDescriptionPair(const string &&description, DWORD &&key){
    virtualKeys.insert(bimap<string, DWORD>::value_type(description, key));
}

bool HookConstants::containVirtualKey(string key)
{
    return HookConstants::instance().virtualKeys.left.find(key) != HookConstants::instance().virtualKeys.left.end();
}

bool HookConstants::containVirtualKeyValue(DWORD value)
{
    return HookConstants::instance().virtualKeys.right.find(value) != HookConstants::instance().virtualKeys.right.end();
}

HookConstants::HookConstants(){
    addVirtualKeyDescriptionPair("l_mouse", VK_LBUTTON);
    addVirtualKeyDescriptionPair("r_mouse", VK_RBUTTON);

    addVirtualKeyDescriptionPair("enter", VK_RETURN);
    addVirtualKeyDescriptionPair("space", VK_SPACE);
    addVirtualKeyDescriptionPair("l_shift", VK_LSHIFT);
    addVirtualKeyDescriptionPair("r_shift", VK_RSHIFT);
    addVirtualKeyDescriptionPair("l_ctrl", VK_LCONTROL);
    addVirtualKeyDescriptionPair("r_ctrl", VK_RCONTROL);
    addVirtualKeyDescriptionPair("l_alt", VK_LMENU);
    addVirtualKeyDescriptionPair("r_alt", VK_RMENU);
    addVirtualKeyDescriptionPair("lwin", VK_LWIN);
    addVirtualKeyDescriptionPair("rwin", VK_RWIN);
    addVirtualKeyDescriptionPair("app", VK_APPS);

    addVirtualKeyDescriptionPair("left", VK_LEFT);
    addVirtualKeyDescriptionPair("up", VK_UP);
    addVirtualKeyDescriptionPair("right", VK_RIGHT);
    addVirtualKeyDescriptionPair("down", VK_DOWN);

    addVirtualKeyDescriptionPair("tab", VK_TAB);
    addVirtualKeyDescriptionPair("esc", VK_ESCAPE);
    addVirtualKeyDescriptionPair("backspace", VK_BACK);
    addVirtualKeyDescriptionPair("insert", VK_INSERT);
    addVirtualKeyDescriptionPair("delete", VK_DELETE);
    addVirtualKeyDescriptionPair("print", VK_SNAPSHOT);

    addVirtualKeyDescriptionPair("page_up", VK_PRIOR);
    addVirtualKeyDescriptionPair("page_down", VK_NEXT);
    addVirtualKeyDescriptionPair("end", VK_END);
    addVirtualKeyDescriptionPair("home", VK_HOME);

    addVirtualKeyDescriptionPair("numlock", VK_NUMLOCK);
    addVirtualKeyDescriptionPair("scrolllock", VK_SCROLL);
    addVirtualKeyDescriptionPair("capslock", VK_CAPITAL);

    addVirtualKeyDescriptionPair("f1", VK_F1);
    addVirtualKeyDescriptionPair("f2", VK_F2);
    addVirtualKeyDescriptionPair("f3", VK_F3);
    addVirtualKeyDescriptionPair("f4", VK_F4);
    addVirtualKeyDescriptionPair("f5", VK_F5);
    addVirtualKeyDescriptionPair("f6", VK_F6);
    addVirtualKeyDescriptionPair("f7", VK_F7);
    addVirtualKeyDescriptionPair("f8", VK_F8);
    addVirtualKeyDescriptionPair("f9", VK_F9);
    addVirtualKeyDescriptionPair("f10", VK_F10);
    addVirtualKeyDescriptionPair("f11", VK_F11);
    addVirtualKeyDescriptionPair("f12", VK_F12);

    addVirtualKeyDescriptionPair("0", VK_NUMPAD0);
    addVirtualKeyDescriptionPair("1", VK_NUMPAD1);
    addVirtualKeyDescriptionPair("2", VK_NUMPAD2);
    addVirtualKeyDescriptionPair("3", VK_NUMPAD3);
    addVirtualKeyDescriptionPair("4", VK_NUMPAD4);
    addVirtualKeyDescriptionPair("5", VK_NUMPAD5);
    addVirtualKeyDescriptionPair("6", VK_NUMPAD6);
    addVirtualKeyDescriptionPair("7", VK_NUMPAD7);
    addVirtualKeyDescriptionPair("8", VK_NUMPAD8);
    addVirtualKeyDescriptionPair("9", VK_NUMPAD9);

    addVirtualKeyDescriptionPair("+", VK_ADD);
    addVirtualKeyDescriptionPair("-", VK_SUBTRACT);
    addVirtualKeyDescriptionPair("*", VK_MULTIPLY);
    addVirtualKeyDescriptionPair("/", VK_DIVIDE);
    addVirtualKeyDescriptionPair(".", VK_DECIMAL);

    addVirtualKeyDescriptionPair(",", VK_OEM_COMMA);
    addVirtualKeyDescriptionPair("k.", VK_OEM_PERIOD);
    addVirtualKeyDescriptionPair("k/", VK_OEM_2);
    addVirtualKeyDescriptionPair(";", VK_OEM_1);
    addVirtualKeyDescriptionPair("'", VK_OEM_7);
    addVirtualKeyDescriptionPair("[", VK_OEM_4);
    addVirtualKeyDescriptionPair("]", VK_OEM_6);
    addVirtualKeyDescriptionPair("\\", VK_OEM_5);
    addVirtualKeyDescriptionPair("k-", VK_OEM_MINUS);
    addVirtualKeyDescriptionPair("k=", VK_OEM_PLUS);

    for (char i = 'a'; i <= 'z'; i++)
        addVirtualKeyDescriptionPair(StringUtils::convert(i), 0x41 + (i - 'a'));

    for (int i = 0; i < 10; i++)
        addVirtualKeyDescriptionPair("k" + StringUtils::convert(i), 0x30 + i);

    mkActionTypes.insert(bimap<string, MKActionType>::value_type("MOUSE_MOVE", MOUSE_MOVE));
    mkActionTypes.insert(bimap<string, MKActionType>::value_type("MOUSE_CLICK_LEFT", MOUSE_CLICK_LEFT));
    mkActionTypes.insert(bimap<string, MKActionType>::value_type("MOUSE_CLICK_RIGHT", MOUSE_CLICK_RIGHT));
    mkActionTypes.insert(bimap<string, MKActionType>::value_type("MOUSE_DOUBLE_CLICK_LEFT", MOUSE_DOUBLE_CLICK_LEFT));
    mkActionTypes.insert(bimap<string, MKActionType>::value_type("KEYBOARD", KEYBOARD));
    mkActionTypes.insert(bimap<string, MKActionType>::value_type("KEYBOARD_CTRL", KEYBOARD_CTRL));
    mkActionTypes.insert(bimap<string, MKActionType>::value_type("KEYBOARD_KEY_DOWN", KEYBOARD_KEY_DOWN));
    mkActionTypes.insert(bimap<string, MKActionType>::value_type("KEYBOARD_KEY_UP", KEYBOARD_KEY_UP));
    mkActionTypes.insert(bimap<string, MKActionType>::value_type("OPEN_WEB", OPEN_WEB));
    mkActionTypes.insert(bimap<string, MKActionType>::value_type("OPEN_APP", OPEN_APP));
    mkActionTypes.insert(bimap<string, MKActionType>::value_type("SLEEP", SLEEP));
}

void MKActionPlayer::play(vector<MouseKeyboardAction>& actions)
{
    for (MouseKeyboardAction& action : actions)
    {
        switch (action.type)
        {
        case SLEEP:
            sleep(action.sleepTime);
            break;
        case MOUSE_MOVE:
            mouseMove(action.x, action.y);
            break;
        case MOUSE_CLICK_LEFT:
            mouseLeftClick();
            break;
        case MOUSE_CLICK_RIGHT:
            mouseRightClick();
            break;
        case MOUSE_DOUBLE_CLICK_LEFT:
            mouseLeftDoubleClick();
            break;
        case KEYBOARD:
            // first try whole string as one key stroke, if failed, then try decompose the key strokes
            if (action.vkey)
            {
                keyboard(action.vkey);
            }    
            else
            {
                for (BYTE key : action.decomposedVkeys)
                    keyboard(key);
            }
            break;
        case KEYBOARD_CTRL:
            ctrlKeyboard(action.vkey);
            break;
        case OPEN_WEB:
            openWebsite(action.executable);
            break;
        case OPEN_APP:
            openApp(action.executable);
            break;
        }
        Sleep(100);                             // operation sleep interval
    }
}

void MKActionPlayer::sleep(DWORD time)
{
    Sleep(time);
}

void MKActionPlayer::mouseMove(int x, int y)
{
    long fx = static_cast<long>(x * (65535.0f / fScreenWidth));
    long fy = static_cast<long>(y * (65535.0f / fScreenHeight));
    INPUT  Input = { 0 };
    Input.type = INPUT_MOUSE;
    Input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
    Input.mi.dx = fx;
    Input.mi.dy = fy;
    ::SendInput(1, &Input, sizeof(INPUT));
}

void MKActionPlayer::mouseLeftClick()
{
    INPUT Input = { 0 };
    // left down
    Input.type = INPUT_MOUSE;
    Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    ::SendInput(1, &Input, sizeof(INPUT));
    // left up
    ::ZeroMemory(&Input, sizeof(INPUT));
    Input.type = INPUT_MOUSE;
    Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
    ::SendInput(1, &Input, sizeof(INPUT));
}

void MKActionPlayer::mouseLeftDoubleClick()
{
    mouseLeftClick();
    mouseLeftClick();
}

void MKActionPlayer::mouseRightClick()
{
    INPUT Input = { 0 };
    // left down
    Input.type = INPUT_MOUSE;
    Input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
    ::SendInput(1, &Input, sizeof(INPUT));
    // left up
    ::ZeroMemory(&Input, sizeof(INPUT));
    Input.type = INPUT_MOUSE;
    Input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
    ::SendInput(1, &Input, sizeof(INPUT));
}

void MKActionPlayer::keyboard(DWORD key)
{
    BYTE byteKey = static_cast<BYTE>(key);
    keybd_event(byteKey, 0, 0, 0);
    keybd_event(byteKey, 0, KEYEVENTF_KEYUP, 0);
}

void MKActionPlayer::ctrlKeyboard(DWORD key)
{
    BYTE byteKey = static_cast<BYTE>(key);
    keybd_event(VK_CONTROL, 0, 0, 0);
    keybd_event(byteKey, 0, 0, 0);
    keybd_event(byteKey, 0, KEYEVENTF_KEYUP, 0);
    keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
}

void MKActionPlayer::openWebsite(wstring& website)
{
    ShellExecute(0, L"open", webBrowser.c_str(), website.c_str(), 0, SW_SHOW);
}

void MKActionPlayer::openApp(wstring& appAddr)
{
    ShellExecute(0, L"open", appAddr.c_str(), 0, 0, SW_SHOW);
}

