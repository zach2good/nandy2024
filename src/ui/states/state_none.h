#pragma once

#include "uistate.h"

class StateNone : public UIState
{
public:
    StateNone()
    {
        m_Name = "None";
    }

    void draw() override
    {
    }
};
