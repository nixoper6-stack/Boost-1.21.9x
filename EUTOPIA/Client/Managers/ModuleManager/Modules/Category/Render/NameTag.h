#pragma once
#include "../../ModuleBase/Module.h"

class NameTag : public Module {
   public:
    NameTag();

    void onD2DRender() override;

   private:
    // 设置项
    float opacity = 0.8f;
    bool renderLocalPlayer = true;
    float itemSpacing = 4.0f;
    bool showHealth = true;
    bool showHeldItem = true;
    bool showAvatar = true;

    // 缩放设置
    float minScale = 0.4f;
    float maxScale = 1.5f;
    float scaleFactor = 6.0f;

    // 血量条设置
    float healthBarHeight = 3.0f;
    bool showAbsorption = true;

    // 内部方法
    std::string sanitizeText(const std::string& text);
    std::string getItemImageUrl(const std::string& itemName);
    void renderPlayerNameTag(Actor* actor, bool isLocal);
    void renderNewLayout(Actor* actor, bool isLocal, const Vec2<float>& screenPos,
                         const std::string& playerName, float distance, float scale);
    void renderHealthBar(Actor* actor, const Vec2<float>& position, float width, float scale);
    float calculateScale(float distance);
};