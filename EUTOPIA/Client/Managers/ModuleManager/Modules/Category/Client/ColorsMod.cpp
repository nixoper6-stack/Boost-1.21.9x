#include "ColorsMod.h"

#include "../../Utils/TimerUtil.h"

Colors* g_Colors = new Colors();

Colors::Colors() : Module("Colors", "change the color of the client.", Category::CLIENT) {
    registerSetting(new EnumSetting("Mode", "Color mode", {"Single", "RGB"}, &this->modeValue, 0));
    registerSetting(new SliderSetting<float>("Brightness", "controls rainbow brightness",
                                             &this->brightness, 1.f, 0.f, 1.f));
    registerSetting(
        new SliderSetting<int>("Separation", "changes separation.", &separation, 125, 0, 255));
    this->mainColor = UIColor(50, 205, 50, 255);
    this->midColor = UIColor(0, 255, 255, 255);
    registerSetting(new ColorSetting("Start color", "used in Single mode", &this->mainColor,
                                     this->mainColor, true));
}
bool Colors::isEnabled() {
    return true;
}
void Colors::setEnabled(bool enabled) {}
bool Colors::isVisible() {
    return false;
}
UIColor Colors::getColor(int index) const {
    float time = TimerUtil::getCurrentMs() / 1000.0f;
    switch(this->modeValue) {
        case 0:
            return this->mainColor;
        case 1:
            return ColorUtil::getRainbowColor2(5.f, 1.f, brightness, index);
        default:
            return this->mainColor;
    }
}
