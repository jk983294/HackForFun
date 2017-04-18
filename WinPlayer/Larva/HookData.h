#ifndef HOOK_DATA_H
#define HOOK_DATA_H

#include <windows.h>
#include <string>
#include <iostream>
#include <vector>
#include <boost/bimap.hpp>

using namespace std;
using namespace boost;


bool isNormalInput(DWORD vkey);

enum MKActionType
{
    MOUSE_MOVE,
    MOUSE_CLICK_LEFT,
    MOUSE_CLICK_RIGHT,
    MOUSE_DOUBLE_CLICK_LEFT,
    KEYBOARD,                       // type c
    KEYBOARD_CTRL,                  // ctrl + c
    KEYBOARD_KEY_DOWN,              // low level key down
    KEYBOARD_KEY_UP,                // low level key up
    OPEN_WEB,
    OPEN_APP,
    SLEEP
};

class HookConstants
{
public:
    void addVirtualKeyDescriptionPair(const string &&description, DWORD &&key);

    static HookConstants& instance()
    {
        static HookConstants *instance = new HookConstants;
        return *instance;
    }

    bool containVirtualKey(string key);

    bool containVirtualKeyValue(DWORD value);

private:
    HookConstants();

    HookConstants(const HookConstants &old);
    const HookConstants &operator=(const HookConstants &old);
    ~HookConstants(){}
public:
    bimap<string, DWORD> virtualKeys;
    bimap<string, MKActionType> mkActionTypes;
};

class MouseKeyboardAction
{
public:
    // for track windows action purpose
    MouseKeyboardAction(DWORD timestamp, MKActionType type, int x, int y);

    MouseKeyboardAction(DWORD timestamp, MKActionType type, string key, DWORD vkey);
    
    // for play windows action purpose
    // open web / app
    MouseKeyboardAction(MKActionType type, wstring executable);
    // sleep
    MouseKeyboardAction(MKActionType type, DWORD sleepTime);
    // keyboard
    MouseKeyboardAction(MKActionType type, string cmd);
    // mouse move
    MouseKeyboardAction(MKActionType type, int x, int y);
    // mouse click
    MouseKeyboardAction(MKActionType type);

    BYTE get_key(string key_name);

    void get_decomposed_keys();

    friend ostream& operator<<(ostream& s, const MouseKeyboardAction&);
public:
    DWORD timestamp;
    MKActionType type;
    int x, y;
    DWORD sleepTime;
    string key;
    DWORD vkey;
    wstring executable;
    vector<BYTE> decomposedVkeys;
};

class MKActionPlayer
{
public:    
    void play(vector<MouseKeyboardAction>& actions);

    void sleep(DWORD time);

    void mouseMove(int x, int y);

    void mouseLeftClick();

    void mouseLeftDoubleClick();

    void mouseRightClick();

    void ctrlKeyboard(DWORD key);

    void openWebsite(wstring& website);

    void openApp(wstring& appAddr);

    void keyboard(DWORD key);
};

#endif