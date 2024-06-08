#pragma once

#include "simulation/components/component.h"

struct CompositeComponent : public Component
{
    virtual ~CompositeComponent() = default;
};
