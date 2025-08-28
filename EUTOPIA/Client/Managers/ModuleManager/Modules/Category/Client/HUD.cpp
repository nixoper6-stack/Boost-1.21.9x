#include "HUD.h"

#include "../../../ModuleManager.h"

HUD::HUD() : Module("HUD", "Displays HUD Elements", Category::CLIENT) {
    registerSetting(new BoolSetting("Position", "NULL", &showPosition, true));
    registerSetting(new BoolSetting("Direction", "NULL", &showDirection, false));

    registerSetting(new SliderSetting<int>("Opacity", "NULL", &opacity, 130, 0, 255));
    registerSetting(new SliderSetting<int>("Spacing", "NULL", &spacing, -2, -3, 3));
    registerSetting(new SliderSetting<int>("Offset", "NULL", &offset, 10, 0, 30));
}

HUD::~HUD() {}

void HUD::onD2DRender() {
    LocalPlayer* localPlayer = GI::getLocalPlayer();
    if(localPlayer == nullptr)
        return;

    Vec2<float> windowSize = GI::getClientInstance()->guiData->windowSizeReal;
    Vec2<float> windowSizeScaled = GI::getClientInstance()->guiData->windowSizeScaled;

    float textSize = 1.f;
    float textPaddingX = std::max(1.f, ((float)spacing + 3.f)) * textSize;
    float textPaddingY = (float)spacing * textSize;
    float textHeight = RenderUtil::getTextHeight("", textSize);
    float posX = 0.f + (float)offset;
    float posY = windowSize.y - (textHeight + textPaddingY * 2.f) - (float)offset;

{
        Colors* colorsModule = ModuleManager::getModule<Colors>();
        UIColor mainColor = colorsModule->getColor();

        std::string part1 = "Boost";
        std::string part2 = " v3";
        float x = offset;
        float y = offset;

        float width = 0.f;
        for(char c : part1)
            width += RenderUtil::getTextWidth(std::string(1, c), 1.2f);
        for(char c : part2)
            width += RenderUtil::getTextWidth(std::string(1, c), 1.2f);

        float height = RenderUtil::getTextHeight("", 1.2f);
        RenderUtil::fillRectangle(Vec4<float>(x - 1, y, x + width + 1, y + height),
                                  UIColor(0, 0, 0, opacity));

        for(int i = 0; i < (int)part1.size(); i++) {
            RenderUtil::drawText(Vec2<float>(x, y), std::string(1, part1[i]), mainColor, 1.2f);
            x += RenderUtil::getTextWidth(std::string(1, part1[i]), 1.2f);
        }

        for(int i = 0; i < (int)part2.size(); i++) {
            RenderUtil::drawText(Vec2<float>(x, y), std::string(1, part2[i]), mainColor, 1.2f);
            x += RenderUtil::getTextWidth(std::string(1, part2[i]), 1.2f);
        }
    }


    if(showPosition) {
        Vec3<float> lpPos = localPlayer->getPos();
        int dimensionId = localPlayer->getDimensionTypeComponent()->type;
        if(dimensionId == 0) {
            // Overworld
            char netherPosText[50];
            sprintf_s(netherPosText, 50, "Nether: %.1f, %.1f, %.1f", (lpPos.x / 8.f),
                      (lpPos.y - 1.6f), (lpPos.z / 8.f));
            RenderUtil::fillRectangle(
                Vec4<float>(posX, posY,
                    posX + RenderUtil::getTextWidth(std::string(netherPosText), textSize, false) +
                                textPaddingX * 2.f,
                            posY + textHeight + textPaddingY * 2.f),
                UIColor(0, 0, 0, opacity));
            RenderUtil::drawText(Vec2<float>(posX + textPaddingX, posY + textPaddingY),
                          std::string(netherPosText), UIColor(255, 255, 255), textSize, false);
            posY -= textHeight + (textPaddingY * 2.f);
        } else if(dimensionId == 1) {
            // Nether
            char overworldPosText[50];
            sprintf_s(overworldPosText, 50, "Overworld: %.1f, %.1f, %.1f", (lpPos.x * 8.f),
                      (lpPos.y - 1.6f), (lpPos.z * 8.f));
            RenderUtil::fillRectangle(
                Vec4<float>(posX, posY,
                            posX +
                                               RenderUtil::getTextWidth(
                                                   std::string(overworldPosText), textSize, false) +
                                textPaddingX * 2.f,
                            posY + textHeight + textPaddingY * 2.f),
                UIColor(0, 0, 0, opacity));
            RenderUtil::drawText(Vec2<float>(posX + textPaddingX, posY + textPaddingY),
                          std::string(overworldPosText), UIColor(255, 255, 255), textSize, false);
            posY -= textHeight + (textPaddingY * 2.f);
        }

        char posText[50];
        sprintf_s(posText, 50, "Position: %.1f, %.1f, %.1f", lpPos.x, (lpPos.y - 1.6f), lpPos.z);
        RenderUtil::fillRectangle(
            Vec4<float>(posX, posY,
                        posX + DrawUtil::getTextWidth(std::string(posText), textSize) +
                            textPaddingX * 2.f,
                        posY + textHeight + textPaddingY * 2.f),
            UIColor(0, 0, 0, opacity));
        RenderUtil::drawText(Vec2<float>(posX + textPaddingX, posY + textPaddingY),
                           std::string(posText),
                      UIColor(255, 255, 255), textSize, false);
        posY -= textHeight + (textPaddingY * 2.f);
    }

    if(showDirection) {
        float rotation = localPlayer->getRotation().x;
        if(rotation < 0) {
            rotation += 360.0;
        }
        std::string directionText;
        if((0 <= rotation && rotation < 45) || (315 <= rotation && rotation < 360)) {
            directionText = "South (+Z)";
        } else if(45 <= rotation && rotation < 135) {
            directionText = "West (-X)";
        } else if(135 <= rotation && rotation < 225) {
            directionText = "North (-Z)";
        } else if(225 <= rotation && rotation < 315) {
            directionText = "East (+X)";
        } else {
            directionText = "NULL";
        }
        char finalText[25];
        sprintf_s(finalText, 25, "Direction: %s", directionText.c_str());

        RenderUtil::fillRectangle(
            Vec4<float>(
                posX, posY,
                        posX + RenderUtil::getTextWidth(std::string(finalText), textSize) +
                                           textPaddingX * 2.f,
                                       posY + textHeight + textPaddingY * 2.f),
                           UIColor(0, 0, 0, opacity));
        RenderUtil::drawText(Vec2<float>(posX + textPaddingX, posY + textPaddingY),
                             std::string(finalText),
                      UIColor(255, 255, 255), textSize);

        posY -= textHeight + (textPaddingY * 2.f);
    }
}
