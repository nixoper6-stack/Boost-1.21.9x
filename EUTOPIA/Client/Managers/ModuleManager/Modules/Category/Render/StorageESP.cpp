#include "StorageESP.h"

#include <DrawUtil.h>

StorageESP::StorageESP()
    : Module("StorageESP", "Highlights storage-related blocks", Category::RENDER),
      chest(true),
      enderChest(true),
      shulkerBox(true),
      barrel(true),
      hopper(true),
      furnace(true),
      spawner(true),
      bookshelf(false),
      enchantingTable(false),
      itemFrame(true),
      decoratedPot(false),
      web(false),
      dropper(true),
      dispenser(true),
      anvil(false),
      radius(64),
      alpha(128),
      lineAlpha(255) {
    registerSetting(
        new SliderSetting<int>("Radius", "Scan radius for blocks", &radius, radius, 24, 512));
    registerSetting(new SliderSetting<int>("Alpha", "Box opacity", &alpha, alpha, 0, 255));
    registerSetting(
        new SliderSetting<int>("Line Alpha", "Outline opacity", &lineAlpha, lineAlpha, 0, 255));

    registerSetting(new BoolSetting("Chest", "Highlight Chests", &chest, chest));
    registerSetting(
        new BoolSetting("Ender Chest", "Highlight Ender Chests", &enderChest, enderChest));
    registerSetting(
        new BoolSetting("Shulker Box", "Highlight Shulker Boxes", &shulkerBox, shulkerBox));
    registerSetting(new BoolSetting("Barrel", "Highlight Barrels", &barrel, barrel));
    registerSetting(new BoolSetting("Hopper", "Highlight Hoppers", &hopper, hopper));
    registerSetting(new BoolSetting("Furnace", "Highlight Furnaces", &furnace, furnace));
    registerSetting(new BoolSetting("Spawner", "Highlight Spawners", &spawner, spawner));
    registerSetting(new BoolSetting("Bookshelf", "Highlight Bookshelves", &bookshelf, bookshelf));
    registerSetting(new BoolSetting("Enchanting Table", "Highlight Enchanting Tables",
                                    &enchantingTable, enchantingTable));
    registerSetting(new BoolSetting("Item Frame", "Highlight Item Frames", &itemFrame, itemFrame));
    registerSetting(
        new BoolSetting("Decorated Pot", "Highlight Decorated Pots", &decoratedPot, decoratedPot));
    registerSetting(new BoolSetting("Cobweb", "Highlight Cobwebs", &web, web));
    registerSetting(new BoolSetting("Dropper", "Highlight Droppers", &dropper, dropper));
    registerSetting(new BoolSetting("Dispenser", "Highlight Dispensers", &dispenser, dispenser));
    registerSetting(new BoolSetting("Anvil", "Highlight Anvils", &anvil, anvil));
}

void StorageESP::onLevelRender() {
    auto ci = GI::getClientInstance();
    if(!ci)
        return;

    LocalPlayer* localPlayer = ci->getLocalPlayer();
    if(!localPlayer)
        return;

    AABBShapeComponent* playerAABBShape = localPlayer->getAABBShapeComponent();
    if(!playerAABBShape)
        return;

    AABB playerAABB = playerAABBShape->getAABB();
    Vec3<float> feetPos = playerAABB.getCenter();
    feetPos.y = playerAABB.lower.y;

    const BlockPos& currentPos = feetPos.add(Vec3<float>(0.f, 0.5f, 0.f)).floor().CastTo<int>();

    for(auto it = blockList.begin(); it != blockList.end();) {
        if(it->second == UIColor(0, 0, 0, 0) || abs(currentPos.x - it->first.x) > radius ||
           abs(currentPos.z - it->first.z) > radius || abs(currentPos.y - it->first.y) > radius) {
            it = blockList.erase(it);
        } else {
            ++it;
        }
    }

    auto region = ci->getRegion();
    if(!region)
        return;

    if(!blockList.empty()) {
        for(const auto& pos : blockList) {
            BlockPos blockPos = pos.first;
            Block* block = region->getBlock(blockPos);
            if(!block)
                continue;

            BlockLegacy* blockLegacy = block->blockLegcy;
            if(!blockLegacy)
                continue;

            AABB blockAABB;
            blockLegacy->getOutline(block, region, &blockPos, &blockAABB);

            UIColor boxColor = pos.second;
            DrawUtil::drawBox3dFilled(blockAABB, UIColor(boxColor.r, boxColor.g, boxColor.b, alpha),
                                      UIColor(boxColor.r, boxColor.g, boxColor.b, lineAlpha), 1.f);
        }
    }
}

void StorageESP::updateBlockList(BlockSource* source, const BlockPos& pos) {
    Block* block = source->getBlock(pos);
    if(!block)
        return;

    BlockLegacy* blockLegacy = block->blockLegcy;
    if(!blockLegacy)
        return;

    int blockId = blockLegacy->blockid;
    const std::string& blockName = blockLegacy->blockName;

    if(chest && (blockId == 54 || blockId == 146)) {
        blockList[pos] = UIColor(255, 165, 0);

    } else if(enderChest && blockId == 130) {
        blockList[pos] = UIColor(255, 0, 255);
    } else if(shulkerBox && blockName.find("shulker") != std::string::npos) {
        blockList[pos] = UIColor(255, 255, 0);
    } else if(barrel && blockId == 458) {
        blockList[pos] = UIColor(155, 75, 0);
    } else if(hopper && blockId == 154) {
        blockList[pos] = UIColor(128, 128, 128);
    } else if(furnace && (blockId == 61 || blockId == 451 || blockId == 453)) {
        blockList[pos] = UIColor(70, 70, 70);
    } else if(spawner && blockId == 52) {
        blockList[pos] = UIColor(0, 0, 0);
    } else if(bookshelf && blockId == 47) {
        blockList[pos] = UIColor(139, 69, 19);
    } else if(enchantingTable && blockId == 116) {
        blockList[pos] = UIColor(0, 0, 255);
    } else if(decoratedPot && blockName.find("decorated_pot") != std::string::npos) {
        blockList[pos] = UIColor(210, 105, 30);
    } else if(web && blockName.find("web") != std::string::npos) {
        blockList[pos] = UIColor(128, 128, 128);
    } else if(dropper && blockName.find("dropper") != std::string::npos) {
        blockList[pos] = UIColor(192, 192, 192);
    } else if(dispenser && blockId == 23) {
        blockList[pos] = UIColor(169, 169, 169);
    } else if(anvil && blockId == 145) {
        blockList[pos] = UIColor(105, 105, 105);
    } else {
        blockList.erase(pos);
    }
}
