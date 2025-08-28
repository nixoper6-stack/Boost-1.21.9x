#include "NameTag.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <vector>
#include <unordered_map>

#include<FriendUtil.h>
#include "../../../../../../SDK/GlobalInstance.h"
#include "../../../../../../SDK/World/Actor/Actor.h"
#include "../../../../../../SDK/World/Actor/LocalPlayer.h"
#include "../../../../../../Utils/ColorUtil.h"
#include "../../../../../../Utils/Minecraft/TargetUtil.h"
#include "../../../../../../Utils/RenderUtil.h"
#include "../../../../../../SDK/Render/Matrix.h"
#include <Minecraft/InvUtil.h>

// 物品图片映射表
static std::unordered_map<std::string, std::string> itemImageMap = {
    // 工具类
    {"wooden_sword", "https://minecraft.wiki/images/0/0c/Wooden_Sword_JE2_BE2.png"},
    {"stone_sword", "https://minecraft.wiki/images/3/39/Stone_Sword_JE2_BE2.png"},
    {"iron_sword", "https://minecraft.wiki/images/8/8e/Iron_Sword_JE2_BE2.png"},
    {"golden_sword", "https://minecraft.wiki/images/9/9a/Golden_Sword_JE2_BE2.png"},
    {"diamond_sword", "https://minecraft.wiki/images/4/44/Diamond_Sword_JE3_BE3.png"},
    {"netherite_sword", "https://minecraft.wiki/images/4/44/Netherite_Sword_JE1_BE1.png"},
    
    {"wooden_pickaxe", "https://minecraft.wiki/images/7/74/Wooden_Pickaxe_JE3_BE3.png"},
    {"stone_pickaxe", "https://minecraft.wiki/images/e/e1/Stone_Pickaxe_JE3_BE3.png"},
    {"iron_pickaxe", "https://minecraft.wiki/images/d/d1/Iron_Pickaxe_JE3_BE3.png"},
    {"golden_pickaxe", "https://minecraft.wiki/images/3/3b/Golden_Pickaxe_JE3_BE3.png"},
    {"diamond_pickaxe", "https://minecraft.wiki/images/e/e7/Diamond_Pickaxe_JE3_BE3.png"},
    {"netherite_pickaxe", "https://minecraft.wiki/images/9/9b/Netherite_Pickaxe_JE1_BE1.png"},
    
    {"wooden_axe", "https://minecraft.wiki/images/d/dc/Wooden_Axe_JE3_BE3.png"},
    {"stone_axe", "https://minecraft.wiki/images/4/4c/Stone_Axe_JE3_BE3.png"},
    {"iron_axe", "https://minecraft.wiki/images/a/a0/Iron_Axe_JE3_BE3.png"},
    {"golden_axe", "https://minecraft.wiki/images/f/fa/Golden_Axe_JE3_BE3.png"},
    {"diamond_axe", "https://minecraft.wiki/images/e/e0/Diamond_Axe_JE3_BE3.png"},
    {"netherite_axe", "https://minecraft.wiki/images/1/17/Netherite_Axe_JE1_BE1.png"},
    
    {"wooden_shovel", "https://minecraft.wiki/images/4/43/Wooden_Shovel_JE2_BE2.png"},
    {"stone_shovel", "https://minecraft.wiki/images/1/13/Stone_Shovel_JE2_BE2.png"},
    {"iron_shovel", "https://minecraft.wiki/images/1/19/Iron_Shovel_JE2_BE2.png"},
    {"golden_shovel", "https://minecraft.wiki/images/6/69/Golden_Shovel_JE2_BE2.png"},
    {"diamond_shovel", "https://minecraft.wiki/images/c/c9/Diamond_Shovel_JE3_BE3.png"},
    {"netherite_shovel", "https://minecraft.wiki/images/1/1a/Netherite_Shovel_JE1_BE1.png"},
    
    // 武器类
    {"bow", "https://minecraft.wiki/images/a/ab/Bow_%28Pullback_0%29_JE1_BE1.png"},
    {"crossbow", "https://minecraft.wiki/images/b/bc/Crossbow_%28Pullback_0%29_JE1_BE1.png"},
    {"trident", "https://minecraft.wiki/images/f/ff/Trident_JE2_BE2.png"},
    
    // 食物类
    {"apple", "https://minecraft.wiki/images/5/59/Apple_JE3_BE3.png"},
    {"golden_apple", "https://minecraft.wiki/images/8/8d/Golden_Apple_JE2_BE2.png"},
    {"enchanted_golden_apple", "https://minecraft.wiki/images/7/76/Enchanted_Golden_Apple_JE2_BE2.png"},
    {"bread", "https://minecraft.wiki/images/f/f7/Bread_JE3_BE3.png"},
    {"cooked_beef", "https://minecraft.wiki/images/5/54/Cooked_Beef_JE3_BE3.png"},
    {"cooked_porkchop", "https://minecraft.wiki/images/6/6a/Cooked_Porkchop_JE4_BE3.png"},
    {"cooked_chicken", "https://minecraft.wiki/images/c/c9/Cooked_Chicken_JE3_BE3.png"},
    {"steak", "https://minecraft.wiki/images/5/54/Cooked_Beef_JE3_BE3.png"},
    
    // 方块类
    {"cobblestone", "https://minecraft.wiki/images/f/f0/Cobblestone_JE5_BE3.png"},
    {"stone", "https://minecraft.wiki/images/1/14/Stone_JE5_BE4.png"},
    {"dirt", "https://minecraft.wiki/images/c/c5/Dirt_JE2_BE2.png"},
    {"grass_block", "https://minecraft.wiki/images/f/fb/Grass_Block_JE7_BE6.png"},
    {"oak_planks", "https://minecraft.wiki/images/d/da/Oak_Planks_JE6_BE4.png"},
    {"oak_wood", "https://minecraft.wiki/images/7/7b/Oak_Log_%28UD%29_JE5_BE3.png"},
    {"obsidian", "https://minecraft.wiki/images/c/c0/Obsidian_JE3_BE2.png"},
    {"end_crystal", "https://minecraft.wiki/images/5/53/End_Crystal_JE2_BE2.png"},
    
    // 药水类
    {"potion", "https://minecraft.wiki/images/d/d5/Potion_JE2_BE2.png"},
    {"splash_potion", "https://minecraft.wiki/images/7/75/Splash_Potion_JE2_BE2.png"},
    {"lingering_potion", "https://minecraft.wiki/images/7/76/Lingering_Potion_JE2_BE2.png"},
    
    // 其他常用物品
    {"ender_pearl", "https://minecraft.wiki/images/f/f6/Ender_Pearl_JE3_BE2.png"},
    {"experience_bottle", "https://minecraft.wiki/images/c/c5/Bottle_o%27_Enchanting_JE2_BE2.png"},
    {"shield", "https://minecraft.wiki/images/c/c6/Shield_JE2_BE1.png"},
    {"totem_of_undying", "https://minecraft.wiki/images/b/b5/Totem_of_Undying_JE2_BE2.png"},
    {"firework_rocket", "https://minecraft.wiki/images/0/05/Firework_Rocket_JE2_BE2.png"},
    
    // 默认占位符
    {"unknown", "https://tc.z.wiki/autoupload/f/ZSGXuRzxvs5sdqwcFgzTDn-qmx38a93-GGH8Rwwrjjmyl5f0KlZfm6UsKj-HyTuv/20250724/P7dQ/1006X1052/1c63927e81b1adb640c6d8c513d8f69.jpg"}
};

NameTag::NameTag() : Module("NameTag", "Better nametags using RenderUtil.", Category::RENDER) {
    registerSetting(
        new SliderSetting<float>("Opacity", "Background opacity", &opacity, 0.8f, 0.f, 1.f));
    registerSetting(
        new BoolSetting("RenderLocalPlayer", "Render your own nametag", &renderLocalPlayer, true));
    registerSetting(new SliderSetting<float>("ItemSpacing", "Space between elements",
                                             &itemSpacing, 4.f, 2.f, 10.f));
    registerSetting(new BoolSetting("ShowHealth", "Show health value", &showHealth, true));
    registerSetting(new BoolSetting("ShowHeldItem", "Show held item", &showHeldItem, true));

    // 缩放设置
    registerSetting(new SliderSetting<float>("MinScale", "Minimum scale for distant tags", &minScale, 0.4f, 0.1f, 1.0f));
    registerSetting(new SliderSetting<float>("MaxScale", "Maximum scale for close tags", &maxScale, 1.5f, 1.0f, 3.0f));
    registerSetting(new SliderSetting<float>("ScaleFactor", "Scale calculation factor", &scaleFactor, 6.0f, 1.0f, 15.0f));
    
    // 健康条设置
    registerSetting(new SliderSetting<float>("HealthBarHeight", "Height of health bar", &healthBarHeight, 3.0f, 2.0f, 8.0f));
    registerSetting(new BoolSetting("ShowAbsorption", "Show absorption hearts (golden)", &showAbsorption, true));
}

static bool isInvalidChar(char c) {
    return !(c >= 0 && *reinterpret_cast<unsigned char*>(&c) < 128);
}

std::string NameTag::sanitizeText(const std::string& text) {
    std::string out;
    bool wasValid = true;
    for(char c : text) {
        bool isValid = !isInvalidChar(c);
        if(wasValid) {
            if(!isValid) {
                wasValid = false;
            } else {
                out += c;
            }
        } else {
            wasValid = isValid;
        }
    }
    return out;
}

std::string NameTag::getItemImageUrl(const std::string& itemName) {
    auto it = itemImageMap.find(itemName);
    if(it != itemImageMap.end()) {
        return it->second;
    }
    
    // 如果找不到确切匹配，尝试模糊匹配
    for(const auto& pair : itemImageMap) {
        if(itemName.find(pair.first) != std::string::npos || 
           pair.first.find(itemName) != std::string::npos) {
            return pair.second;
        }
    }
    
    // 返回默认占位符
    return itemImageMap["unknown"];
}

void NameTag::onD2DRender() {


    LocalPlayer* localPlayer = GI::getLocalPlayer();
    if(!localPlayer)
        return;
    if(!GI::canUseMoveKeys())
        return;
    if(!GI::isInHudScreen())
        return;
    // 获取所有actor并渲染nametag
    for(Actor* actor : localPlayer->level->getRuntimeActorList()) {
        bool isLocal = (actor == localPlayer);
        if(!renderLocalPlayer && isLocal)
            continue;

        if(!isLocal && !TargetUtil::isTargetValid(actor, false))
            continue;
        if(!actor || !actor->isAlive())
            continue;

        renderPlayerNameTag(actor, isLocal);
    }
}

float NameTag::calculateScale(float distance) {
    // 改进的缩放算法：使用对数缩放来保证远距离可见性
    if(distance <= 1.0f) {
        return maxScale;
    }
    
    // 使用对数函数进行平滑缩放
    float logScale = scaleFactor / std::log(distance * 1.5f + 1.0f);
    
    // 确保缩放值在指定范围内
    return std::clamp(logScale, minScale, maxScale);
}

void NameTag::renderPlayerNameTag(Actor* actor, bool isLocal) {
    // 使用Matrix::WorldToScreen
    Vec2<float> screenPos;
    Vec3<float> worldPos = actor->getEyePos().add(Vec3<float>(0.f, 0.75f, 0.f));
    
    if(!Matrix::WorldToScreen(worldPos, screenPos))
        return;

    // 检查屏幕边界
    Vec2<float> windowSize = GI::getGuiData()->windowSizeReal;
    if(screenPos.x < -200 || screenPos.y < -200 || screenPos.x > windowSize.x + 200 || screenPos.y > windowSize.y + 200) {
        return;
    }

    // 获取玩家名称
    std::string tagPtr = actor->getNameTag();
    if(!&tagPtr)
        return;
    std::string rawName = sanitizeText(tagPtr);
    if(rawName.empty())
        rawName = "Player";

    // 计算距离和缩放
    LocalPlayer* localPlayer = GI::getLocalPlayer();
    float dist = actor->getEyePos().dist(localPlayer->getEyePos());
    float baseScale = calculateScale(dist);

    // 新的布局设计
    renderNewLayout(actor, isLocal, screenPos, rawName, dist, baseScale);
}

void NameTag::renderNewLayout(Actor* actor, bool isLocal, const Vec2<float>& screenPos, 
                              const std::string& playerName, float distance, float scale) {
    float padding = 6.f * scale;
    float textSize = 0.45f * scale;
    float itemSize = 12.f * scale;
    float avatarSize = 20.f * scale;
    
    // 计算组件尺寸
    float nameWidth = RenderUtil::getTextWidth(playerName, textSize);
    float textHeight = RenderUtil::getTextHeight(playerName, textSize);
    
    // 移除距离显示相关代码
    
    // 计算右侧内容宽度（名字+物品间距）
    float rightContentWidth = nameWidth;
    if(showHeldItem) {
        // 为物品预留空间，确保不与名字重叠
        rightContentWidth += itemSize + itemSpacing * 2;
    }
    
    // 计算总布局尺寸
    float totalWidth = padding * 3 + (showAvatar ? avatarSize + itemSpacing : 0) + rightContentWidth;
    float totalHeight = padding * 2 + std::max(avatarSize, textHeight + (showHealth ? healthBarHeight + itemSpacing : 0));
    
    // 背景位置（居中）
    float bgLeft = screenPos.x - totalWidth / 2.f;
    float bgTop = screenPos.y - totalHeight;
    Vec4<float> bgRect(bgLeft, bgTop, bgLeft + totalWidth, bgTop + totalHeight);
    
    // 绘制现代化背景
    UIColor bgColor = UIColor(0, 0, 0, (int)(opacity * 200));
    float cornerRadius = 6.f * scale;
    RenderUtil::fillRoundedRectangle(bgRect, bgColor, cornerRadius);
    
    // 绘制边框
    UIColor borderColor = UIColor(255, 255, 255, 40);
    RenderUtil::drawRoundedRectangle(bgRect, borderColor, cornerRadius, 1.0f);
    
    float currentX = bgLeft + padding;
    float currentY = bgTop + padding;
    
    // 1. 绘制头像（左侧）
    
    
    // 2. 右侧内容区域
    float rightStartX = currentX;
    
    // 2.1 绘制玩家名字（右上）
    UIColor nameColor = FriendManager::isFriend(playerName) ? 
                        UIColor(0, 255, 0, 255) : UIColor(255, 255, 255, 255);
    
    Vec2<float> namePos(rightStartX, currentY);
    RenderUtil::drawText(namePos, playerName, nameColor, textSize);
    
    // 2.2 绘制手持物品（名字右边，增加间距避免重叠）
    if(showHeldItem) {
        ItemStack* heldItem = InvUtil::getItem(actor->supplies->mSelectedSlot);
        if(heldItem && InvUtil::isVaildItem(heldItem) && heldItem->mItem.get()) {
            std::string itemName = heldItem->mItem->mName;
            std::string imageUrl = getItemImageUrl(itemName);
            
            // 物品位置：名字右边 + 更多间距
            float itemX = rightStartX + nameWidth + itemSpacing * 1.5f;
            Vec2<float> itemPos(itemX, currentY);
            Vec4<float> itemRect(itemPos.x, itemPos.y, itemPos.x + itemSize, itemPos.y + itemSize);
            
            RenderUtil::drawImageFromUrl(itemRect, imageUrl, "held_item_" + itemName, 1.0f);
            
            // 显示物品数量（右下角）
            int itemCount = static_cast<int>(heldItem->mCount);
            if(itemCount > 1) {
                std::string countText = std::to_string(itemCount);
                float countTextSize = textSize * 0.4f;
                float countWidth = RenderUtil::getTextWidth(countText, countTextSize);
                float countHeight = RenderUtil::getTextHeight(countText, countTextSize);
                
                // 数量显示位置：物品右下角
                Vec2<float> countPos(itemPos.x + itemSize - countWidth - 1.0f, 
                                   itemPos.y + itemSize - countHeight + 1.0f);

                // 数量文字
                UIColor countColor = UIColor(0, 255, 0, 255);
                RenderUtil::drawText(countPos, countText, countColor, countTextSize);
                
                // 物品名称显示在物品图标正上方中间
                float itemNameTextSize = textSize * 0.35f;
                float itemNameWidth = RenderUtil::getTextWidth(itemName, itemNameTextSize);
                
                // 物品名称位置：物品图标水平中心，垂直在图标上方
                Vec2<float> itemNamePos(itemPos.x + (itemSize - itemNameWidth) / 2.0f, // 水平居中
                                     itemPos.y - itemNameTextSize - 15.0f);          // 垂直在上方

                UIColor itemNameColor =ColorUtil::getAquaThemeColor(2.f);
                RenderUtil::drawText(itemNamePos, itemName, itemNameColor, itemNameTextSize);
            }
        }
    }
    
    // 2.3 绘制血量条（右下）
    if(showHealth) {
        float healthBarY = currentY + textHeight + itemSpacing;
        renderHealthBar(actor, Vec2<float>(rightStartX, healthBarY), nameWidth, scale);
    }
}

void NameTag::renderHealthBar(Actor* actor, const Vec2<float>& position, float width, float scale) {
    float barHeight = healthBarHeight * scale;
    float health = actor->getHealth();
    float absorption = actor->getAbsorption();
    float maxHealth = 20.0f; // 默认最大血量
    
    // 血量条背景
    Vec4<float> bgRect(position.x, position.y, position.x + width, position.y + barHeight);
    UIColor bgColor = UIColor(0, 0, 0, 150);
    RenderUtil::fillRoundedRectangle(bgRect, bgColor, barHeight / 2.0f);
    
    // 血量条边框
    UIColor borderColor = UIColor(100, 100, 100, 200);
    RenderUtil::drawRoundedRectangle(bgRect, borderColor, barHeight / 2.0f, 1.0f);
    
    // 计算血量百分比
    float healthPercent = std::clamp(health / maxHealth, 0.0f, 1.0f);
    
    // 选择血量颜色
    UIColor healthColor;
    if(healthPercent > 0.7f) {
        healthColor = UIColor(0, 255, 0, 255); // 绿色 - 高血量
    } else if(healthPercent > 0.3f) {
        healthColor = UIColor(255, 255, 0, 255); // 黄色 - 中血量
    } else {
        healthColor = UIColor(255, 0, 0, 255); // 红色 - 低血量
    }
    
    // 绘制主血量条
    if(healthPercent > 0) {
        float healthWidth = width * healthPercent;
        Vec4<float> healthRect(position.x + 1, position.y + 1, 
                              position.x + healthWidth - 1, position.y + barHeight - 1);
        RenderUtil::fillRoundedRectangle(healthRect, healthColor, (barHeight - 2) / 2.0f);
    }
    
    // 绘制吸收血量（金色心）
    if(showAbsorption && absorption > 0) {
        float absorptionPercent = std::clamp(absorption / maxHealth, 0.0f, 1.0f);
        float absorptionWidth = width * absorptionPercent;
        
        // 金色吸收条（叠加在血量条上方）
        Vec4<float> absorptionRect(position.x + 1, position.y - barHeight * 0.3f, 
                                  position.x + absorptionWidth - 1, position.y);
        UIColor absorptionColor = UIColor(255, 215, 0, 200); // 金色
        RenderUtil::fillRoundedRectangle(absorptionRect, absorptionColor, barHeight / 4.0f);
        
        // 吸收条边框
        UIColor absorptionBorderColor = UIColor(255, 255, 0, 150);
        RenderUtil::drawRoundedRectangle(absorptionRect, absorptionBorderColor, barHeight / 4.0f, 0.5f);
    }
    
    // 血量数值文本（移到血量条右边）
    if(scale > 0.6f) { // 降低显示门槛
        std::string healthText = std::to_string((int)health);
        if(absorption > 0) {
            healthText += "+" + std::to_string((int)absorption);
        }
        
        float textScale = 0.3f * scale;
        float textWidth = RenderUtil::getTextWidth(healthText, textScale);
        float textHeight = RenderUtil::getTextHeight(healthText, textScale);
        
        // 数值位置：血量条右边 + 小间距
        Vec2<float> textPos(position.x + width + 4.0f * scale, 
                           position.y + (barHeight - textHeight) / 2.0f);
        
        UIColor textColor = UIColor(255, 255, 255, 255);
        RenderUtil::drawText(textPos, healthText, textColor, textScale);
    }
}