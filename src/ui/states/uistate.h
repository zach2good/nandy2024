#pragma once

#include <string>

class UIState
{
public:
    virtual ~UIState() = default;

    virtual void draw() = 0;

    auto getName() const -> std::string const& { return m_Name; }

protected:
    std::string m_Name;
};
