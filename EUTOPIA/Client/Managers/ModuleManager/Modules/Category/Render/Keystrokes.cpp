#include "Keystrokes.h"

Keystrokes::Keystrokes() : Module("Keystrokes", "Displays keystrokes", Category::RENDER) {}

void Keystrokes::onLevelRender() {
    auto player = Game.getLocalPlayer();
    if(!player)
        return;

    if(Game.getClientInstance()->getScreenName() != "hud_screen")
        return;

    auto moveCo = player->getMoveInputComponent();

    bool forward = moveCo->mForward;
    bool back = moveCo->mBackward;
    bool left = moveCo->mLeft;
    bool right = moveCo->mRight;
    bool jump = moveCo->mIsJumping;

    float delta = ImGui::GetIO().DeltaTime;
    float rounding = 8.0f;

    ImVec2 boxSize = ImVec2(140, 140);
    ImVec2 renderPos = ImVec2(10, 100);  // fixed position

    auto drawList = ImGui::GetBackgroundDrawList();
    ImVec2 keySize = ImVec2(40, 40);
    ImVec2 keyPadding = ImVec2(5, 5);

    ImVec2 spaceSize =
        ImVec2(boxSize.x - keyPadding.x * 2, boxSize.y - keySize.y * 2 - keyPadding.y * 4);

    static float animSpeed = 10.0f;
    static std::map<int, float> keyAnimation = {{0, 0.f}, {1, 0.f}, {2, 0.f}, {3, 0.f}, {4, 0.f}};

    keyAnimation[0] = std::lerp(keyAnimation[0], forward ? 1.f : 0.f, delta * animSpeed);
    keyAnimation[1] = std::lerp(keyAnimation[1], back ? 1.f : 0.f, delta * animSpeed);
    keyAnimation[2] = std::lerp(keyAnimation[2], left ? 1.f : 0.f, delta * animSpeed);
    keyAnimation[3] = std::lerp(keyAnimation[3], right ? 1.f : 0.f, delta * animSpeed);
    keyAnimation[4] = std::lerp(keyAnimation[4], jump ? 1.f : 0.f, delta * animSpeed);

    float textSize = 20.0f;
    ImU32 textColor = IM_COL32(255, 255, 255, 255);
    ImU32 bgColor = IM_COL32(0, 0, 0, 150);
    ImColor keyColor = ImColor(0, 120, 255, 200);

    auto drawKey = [&](const char* label, ImVec2 pos, int animIndex, ImVec2 size) {
        ImVec2 center = ImVec2(pos.x + size.x / 2, pos.y + size.y / 2);
        ImVec2 scale = ImVec2(size.x * keyAnimation[animIndex], size.y * keyAnimation[animIndex]);

        ImColor animatedColor = keyColor;
        animatedColor.Value.w *= keyAnimation[animIndex];

        drawList->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), bgColor, rounding);
        drawList->AddRectFilled(ImVec2(center.x - scale.x / 2, center.y - scale.y / 2),
                                ImVec2(center.x + scale.x / 2, center.y + scale.y / 2),
                                animatedColor, rounding * keyAnimation[animIndex]);

        ImVec2 labelSize = ImGui::GetFont()->CalcTextSizeA(textSize, FLT_MAX, 0, label);
        drawList->AddText(
            ImGui::GetFont(), textSize,
            ImVec2(pos.x + size.x / 2 - labelSize.x / 2, pos.y + size.y / 2 - labelSize.y / 2),
            textColor, label);
    };

    drawKey("W", ImVec2(renderPos.x + boxSize.x / 2 - keySize.x / 2, renderPos.y + keyPadding.y), 0,
            keySize);
    drawKey("A", ImVec2(renderPos.x + keyPadding.x, renderPos.y + keySize.y + keyPadding.y * 2), 2,
            keySize);
    drawKey("S",
            ImVec2(renderPos.x + boxSize.x / 2 - keySize.x / 2,
                   renderPos.y + keySize.y + keyPadding.y * 2),
            1, keySize);
    drawKey("D",
            ImVec2(renderPos.x + boxSize.x - keySize.x - keyPadding.x,
                   renderPos.y + keySize.y + keyPadding.y * 2),
            3, keySize);
    drawKey("SPACE",
            ImVec2(renderPos.x + boxSize.x / 2 - spaceSize.x / 2,
                   renderPos.y + keySize.y * 2 + keyPadding.y * 3),
            4, spaceSize);
}
