#include "Arraylist.h"

#include "../../../../../../Utils/TimerUtil.h"
#include "../../../ModuleManager.h"
#include "ColorsMod.h"

Arraylist::Arraylist() : Module("Arraylist", "Display module enabled", Category::CLIENT) {
    registerSetting(new BoolSetting("Bottom", "Flip to bottom", &bottom, true));

    registerSetting(new BoolSetting("Show Modes", "Show Modes", &showModes, true));
    registerSetting(new SliderSetting<float>("Spacing X", "Adjust spacing X", &spacing, -3, -5, 5));
    registerSetting(
        new SliderSetting<float>("Spacing Y", "Adjust spacing Y", &spacing2, -3, -5, 5));
    registerSetting(
        new SliderSetting<float>("Size", "Control the size of modules", &size, 1.f, 0.1f, 2.0f));
    registerSetting(new BoolSetting("Outline", "NULL", &outline, false));
    registerSetting(new EnumSetting("Outline Mode", "NULL", {"Normal", "SideLine"}, &Enum2, 0));
    registerSetting(new SliderSetting<int>("Opacity", "NULL", &opacity, 130, 0, 255));
    registerSetting(new SliderSetting<int>("Offset", "NULL", &offset, 10, 0, 30));
}

bool Arraylist::sortByLength(Module* lhs, Module* rhs) {
    float textSize = 0.7f;
    float textWidth1 = RenderUtil::getTextWidth(lhs->getModuleName(), textSize);
    float textWidth2 = RenderUtil::getTextWidth(rhs->getModuleName(), textSize);

    static Arraylist* arralistMod = ModuleManager::getModule<Arraylist>();
    if(arralistMod->showModes) {
        if(lhs->getModeText() != "NULL")
            textWidth1 += RenderUtil::getTextWidth(" " + lhs->getModeText(), textSize);
        if(rhs->getModeText() != "NULL")
            textWidth2 += RenderUtil::getTextWidth(" " + rhs->getModeText(), textSize);
    }

    std::string keybindText1 = "[" + std::string(KeyNames[lhs->getKeybind()]) + "]";
    std::string keybindText2 = "[" + std::string(KeyNames[rhs->getKeybind()]) + "]";
    if(keybindText1 != "[Unknown]")
        textWidth1 += RenderUtil::getTextWidth(" " + keybindText1, textSize);
    if(keybindText2 != "[Unknown]")
        textWidth2 += RenderUtil::getTextWidth(" " + keybindText2, textSize);

    return (textWidth1 > textWidth2);
}

void Arraylist::onD2DRender() {
    LocalPlayer* localPlayer = GI::getLocalPlayer();
    if(localPlayer == nullptr)
        return;

    Vec2<float> windowSize = GI::getClientInstance()->guiData->windowSizeReal;
    float textSize = size;
    float textPaddingX = std::max(1.f, spacing + 3.f) * textSize;
    float textPaddingY = spacing2 * textSize;
    float textHeight = RenderUtil::getTextHeight("", textSize);
    float lineWidth = 1.5f * textSize;

    Vec2<float> Pos = Vec2<float>(windowSize.x - offset, offset);
    if(bottom)
        Pos.y = windowSize.y - (textHeight + textPaddingY * 2.f) - offset;

    static std::vector<Module*> moduleList = ModuleManager::moduleList;
    std::sort(moduleList.begin(), moduleList.end(), sortByLength);

    UIColor mainColor;
    Vec4<float> lastRect;
    index = 0;

    Colors* colorsModule = ModuleManager::getModule<Colors>();

    for(auto& mod : moduleList) {
        std::string moduleName = mod->getModuleName();
        std::string modeText = mod->getModeText();
        std::string keybindText = "[" + std::string(KeyNames[mod->getKeybind()]) + "]";
        bool shouldRender = mod->isEnabled() && mod->isVisible();

        if(!showModes)
            modeText = "NULL";
        if(keybindText == "[Unknown]")
            keybindText = "";

        if(useRainbow) {
            mainColor = ColorUtil::getRainbowColor2(5.f, 1.f, 1.f, index);
        } else if(colorsModule != nullptr && colorsModule->isEnabled()) {
            mainColor = colorsModule->getColor(index);  // Use getColor to get dynamic colors!
        } else {
            mainColor = UIColor(255, 255, 255);
        }

        UIColor modeColor = UIColor(mainColor);

        mod->arraylistAnim = Math::moveTowards(mod->arraylistAnim, shouldRender ? 1.05f : -0.1f,
                                               RenderUtil::deltaTime * 10.f);
        mod->arraylistAnim = std::clamp(mod->arraylistAnim, 0.f, 1.f);

        if(mod->arraylistAnim > 0.f) {
            float fullTextWidth = RenderUtil::getTextWidth(moduleName, textSize);
            if(modeText != "NULL")
                fullTextWidth += RenderUtil::getTextWidth(" " + modeText, textSize);
            if(!keybindText.empty())
                fullTextWidth += RenderUtil::getTextWidth(" " + keybindText, textSize);

            Vec4<float> rectPos =
                Vec4<float>(Pos.x - textPaddingX - fullTextWidth - textPaddingX, Pos.y, Pos.x,
                            Pos.y + textPaddingY + textHeight + textPaddingY);

            float animRect = (windowSize.x + 25.f - rectPos.x) * (1.f - mod->arraylistAnim);
            rectPos.x += animRect;
            rectPos.z += animRect;

            float offsetY = 2.0f;
            float offsetX = 0.8f;

            float rectWidth = rectPos.z - rectPos.x;
            float textStartX = rectPos.x + (rectWidth - fullTextWidth) / 2.f;

            Vec2<float> textPos(textStartX + offsetX, rectPos.y + textPaddingY - offsetY);

            RenderUtil::fillRectangle(rectPos, UIColor(0, 0, 0, opacity));

            if(outline) {
                float increasedLineWidth = lineWidth * 1.7f;
                if(Enum2 == 1) {
                    float sideLineWidth = lineWidth * 1.7f;
                    RenderUtil::fillRectangle(
                        Vec4<float>(rectPos.z, rectPos.y, rectPos.z + sideLineWidth, rectPos.w),
                        mainColor);
                } else {
                    RenderUtil::fillRectangle(Vec4<float>(rectPos.x - increasedLineWidth, rectPos.y,
                                                          rectPos.x, rectPos.w),
                                              mainColor);
                    RenderUtil::fillRectangle(
                        Vec4<float>(rectPos.z, rectPos.y, rectPos.z + increasedLineWidth,
                                    rectPos.w),
                        mainColor);

                    if(index == 0) {
                        if(bottom)
                            RenderUtil::fillRectangle(
                                Vec4<float>(rectPos.x - increasedLineWidth, rectPos.w,
                                            rectPos.z + increasedLineWidth,
                                            rectPos.w + increasedLineWidth),
                                mainColor);
                        else
                            RenderUtil::fillRectangle(
                                Vec4<float>(rectPos.x - increasedLineWidth,
                                            rectPos.y - increasedLineWidth,
                                            rectPos.z + increasedLineWidth, rectPos.y),
                                mainColor);
                    } else {
                        if(bottom)
                            RenderUtil::fillRectangle(
                                Vec4<float>(lastRect.x - increasedLineWidth,
                                            rectPos.w - increasedLineWidth, rectPos.x, rectPos.w),
                                mainColor);
                        else
                            RenderUtil::fillRectangle(
                                Vec4<float>(lastRect.x - increasedLineWidth, rectPos.y, rectPos.x,
                                            rectPos.y + increasedLineWidth),
                                mainColor);
                    }
                }
            }

            UIColor grayColor(175, 175, 175, 255);

            RenderUtil::drawText(textPos, moduleName, mainColor, textSize, true);
            textPos.x += RenderUtil::getTextWidth(moduleName, textSize);

            if(modeText != "NULL") {
                textPos.x += RenderUtil::getTextWidth(" ", textSize);
                RenderUtil::drawText(textPos, modeText, grayColor, textSize, true);
                textPos.x += RenderUtil::getTextWidth(modeText, textSize);
            }

            if(!keybindText.empty()) {
                textPos.x += RenderUtil::getTextWidth(" ", textSize);
                RenderUtil::drawText(textPos, keybindText, grayColor, textSize, true);
            }

            lastRect = rectPos;
        }

        float yAnim = mod->arraylistAnim * 979.25f;
        if(yAnim > 1.f)
            yAnim = 1.f;
        Pos.y += (textHeight + textPaddingY * 2.f) * yAnim * (bottom ? -1.f : 1.f);

        if(shouldRender)
            index++;
    }

    if(Enum2 == 0 && outline) {
        float increasedLineWidth = lineWidth * 1.7f;
        if(bottom)
            RenderUtil::fillRectangle(
                Vec4<float>(lastRect.x - increasedLineWidth, lastRect.y - increasedLineWidth,
                            lastRect.z + increasedLineWidth, lastRect.y),
                mainColor);
        else
            RenderUtil::fillRectangle(
                Vec4<float>(lastRect.x - increasedLineWidth, lastRect.w,
                            lastRect.z + increasedLineWidth, lastRect.w + increasedLineWidth),
                mainColor);
    }
}
