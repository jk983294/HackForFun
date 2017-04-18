#ifndef MK_ACTION_MERGER_H
#define MK_ACTION_MERGER_H

#include "HookData.h"
#include <vector>

using namespace std;

class MKActionMerger
{
public:
    MKActionMerger();
    ~MKActionMerger();

    void merge(vector<MouseKeyboardAction>& rawActions);

public:
    vector<MouseKeyboardAction> actions;
};

#endif