#pragma once

#include "../../ModuleBase/Module.h"
#include "../../Utils/ColorUtil.h"
#include "../../Utils/TimerUtil.h"

class Colors : public Module {
   public:
    UIColor mainColor = UIColor(50, 205, 50, 255);
    UIColor midColor = UIColor(0, 255, 255, 255);

    int modeValue = 0;
    float brightness = 1.f;
    float saturation = 1.f;
    float seconds = 3.f;

    int separation = 125;

    Colors();

    int getSeparationValue() const {
        return this->separation;
    }

    UIColor getColor(int index = 0) const;

    bool isEnabled() override;
    void setEnabled(bool enabled) override;
    bool isVisible() override;
};

extern Colors* g_Colors;
