#include "MKActionMerger.h"

MKActionMerger::MKActionMerger()
{
}

MKActionMerger::~MKActionMerger()
{
}

void MKActionMerger::merge(vector<MouseKeyboardAction>& rawActions)
{
    bool isCtrlPressed = false;
    int previousX = 0, previousY = 0;
    MouseKeyboardAction previouAction{ SLEEP, 0 };

    for (auto& rawAction : rawActions)
    {
        if (actions.size() > 0)
        {
            previouAction = actions[actions.size() - 1];
            if (previouAction.type >= MOUSE_MOVE && previouAction.type <= MOUSE_DOUBLE_CLICK_LEFT)
            {
                previousX = previouAction.x;
                previousY = previouAction.y;
            }
        }

        switch (rawAction.type)
        {
        case KEYBOARD_KEY_DOWN:
            if (rawAction.vkey == VK_LCONTROL)
            {
                isCtrlPressed = true;
            }
            else if (isNormalInput(rawAction.vkey))
            {
            }
            break;
        case KEYBOARD_KEY_UP:
            if (rawAction.vkey == VK_LCONTROL)
            {
                isCtrlPressed = false;
            }
            else if (isCtrlPressed)
            {
                actions.push_back({ rawAction.timestamp, KEYBOARD_CTRL, rawAction.key, rawAction.vkey });
            }
            else if (isNormalInput(rawAction.vkey))
            {
                if (previouAction.type == KEYBOARD && isNormalInput(previouAction.vkey))
                {
                    actions[actions.size() - 1].key += rawAction.key;
                }
                else
                {
                    actions.push_back({ rawAction.timestamp, KEYBOARD, rawAction.key, rawAction.vkey });
                }
            }
            else
            {
                actions.push_back({ rawAction.timestamp, KEYBOARD, rawAction.key, rawAction.vkey });
            }
            break;
        case MOUSE_CLICK_LEFT:
            // it will be considered double click when position equals and time elapse is less than 800 ms
            if (previouAction.type == MOUSE_CLICK_LEFT && (rawAction.sleepTime - previouAction.sleepTime < 800)
                && previousX == rawAction.x && previousY == rawAction.y)
            {
                actions[actions.size() - 1].type = MOUSE_DOUBLE_CLICK_LEFT;
            }
            else if (previousX == rawAction.x && previousY == rawAction.y)
            {
                actions.push_back(rawAction);
            }
            else
            {
                actions.push_back({ rawAction.timestamp, MOUSE_MOVE, rawAction.x, rawAction.y });
                actions.push_back(rawAction);
            }
            break;
        case MOUSE_CLICK_RIGHT:
            if (previousX == rawAction.x && previousY == rawAction.y)
            {
                actions.push_back(rawAction);
            }
            else
            {
                actions.push_back({ rawAction.timestamp, MOUSE_MOVE, rawAction.x, rawAction.y });
                actions.push_back(rawAction);
            }
            break;
        }
    }
}