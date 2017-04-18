#include "HookData.h"
#include "IoUtils.h"
#include "StringUtils.h"
#include <boost/program_options.hpp>

namespace po = boost::program_options;

void generate_default_actions(vector<MouseKeyboardAction> &actions);
void generate_actions_from_file(vector<MouseKeyboardAction> &actions, string path);

int main(int argc, char *argv[])
{
    vector<MouseKeyboardAction> actions;

    po::options_description desc("Program options");
    desc.add_options()("filename,F", po::value<string>(), "file to process");

    po::variables_map vm;
    auto parsed = po::parse_command_line(argc, argv, desc);
    po::store(parsed, vm);
    po::notify(vm);

    if (vm.count("filename"))
    {
        generate_actions_from_file(actions, vm["filename"].as<string>());
    }    
    else
    {
        generate_default_actions(actions);
    }

    for (MouseKeyboardAction& action : actions)
    {
        cout << action << endl;
    }

    MKActionPlayer player;
    player.play(actions);

    return 0;
}

void generate_default_actions(vector<MouseKeyboardAction> &actions)
{
    actions.push_back({ OPEN_WEB, wstring{ L"www.baidu.com" } });
    actions.push_back({ SLEEP, 1000 });
    actions.push_back({ MOUSE_MOVE, 782, 48 });

    actions.push_back({ MOUSE_CLICK_LEFT });
    actions.push_back({ KEYBOARD, "www.baidu.com" });
    actions.push_back({ SLEEP, 5000 });
    actions.push_back({ KEYBOARD, "enter" });
    actions.push_back({ SLEEP, 5000 });
    actions.push_back({ MOUSE_MOVE, 782, 148 });
    actions.push_back({ MOUSE_CLICK_LEFT });
    actions.push_back({ KEYBOARD_CTRL, "a" });

    actions.push_back({ MOUSE_CLICK_RIGHT });
}
void generate_actions_from_file(vector<MouseKeyboardAction> &actions, string path)
{
    string actionStr, cmd;
    int x, y;
    DWORD sleepTime;
    MKActionType type;
    vector<string> lines = IoUtils::fromFile(path);
    for (auto itr = lines.begin(); itr != lines.end(); ++itr)
    {
        istringstream i(*itr);
        i >> actionStr;
        type = HookConstants::instance().mkActionTypes.left.at(actionStr);

        switch (type)
        {
        case MOUSE_MOVE:
            i >> x >> y;
            actions.push_back({ MOUSE_MOVE, x, y });
            break;
        case MOUSE_CLICK_LEFT:
        case MOUSE_CLICK_RIGHT:      
        case MOUSE_DOUBLE_CLICK_LEFT:
            actions.push_back({ type });
            break;
        case KEYBOARD:
        case KEYBOARD_CTRL:
            i >> cmd;
            actions.push_back({ type, cmd });
            break;
        case OPEN_WEB:
        case OPEN_APP:
            ++itr;
            actions.push_back({ type, StringUtils::string2wstring(*itr) });
            break;
        case SLEEP:
            i >> sleepTime;
            actions.push_back({ type, sleepTime });
            break;
        }
    }
}