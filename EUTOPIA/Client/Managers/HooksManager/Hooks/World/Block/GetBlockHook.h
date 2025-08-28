#pragma once
#include <iostream>
#include <string>

#include "../../../../ModuleManager/Modules/Category/Render/OreESP.h"
#include "../../FuncHook.h"

class GetBlockHook : public FuncHook {
   private:
    using func_t = Block*(__thiscall*)(BlockSource*, const BlockPos&);
    static inline func_t oFunc;

    static Block* BlockSourceGetBlockCallback(BlockSource* _this, const BlockPos& blockPos) {
        Block* result = oFunc(_this, blockPos);
        static PortalESP* portalEsp = ModuleManager::getModule<PortalESP>();
        static StorageESP* storageEsp = ModuleManager::getModule<StorageESP>();
        static OreESP* oreEsp = ModuleManager::getModule<OreESP>();

        if(storageEsp && storageEsp->isEnabled()) {
            int blockId = result->blockLegacy->blockId;
            std::string blockName = result->blockLegacy->blockName;

            if(result->blockLegacy && !blockName.empty()) {
                if(storageEsp->chest && (blockId == 54 || blockId == 146)) {  // Chest
                    storageEsp->blockList[blockPos] = UIColor(255, 165, 0);
                } else if(storageEsp->enderChest && blockId == 130) {  // Ender Chest
                    storageEsp->blockList[blockPos] = UIColor(255, 0, 255);
                } else if(storageEsp->shulkerBox &&
                          blockName.find("shulker") != std::string::npos) {  // Shulker Box
                    storageEsp->blockList[blockPos] = UIColor(255, 255, 255);
                } else if(storageEsp->barrel && blockId == 458) {  // Barrel
                    storageEsp->blockList[blockPos] = UIColor(155, 75, 0);
                } else if(storageEsp->hopper && blockId == 154) {  // Hopper
                    storageEsp->blockList[blockPos] = UIColor(128, 128, 128);
                } else if(storageEsp->furnace &&
                          (blockId == 61 || blockId == 451 || blockId == 453)) {  // Furnace
                    storageEsp->blockList[blockPos] = UIColor(70, 70, 70);
                } else if(storageEsp->spawner && blockId == 52) {  // Spawner
                    storageEsp->blockList[blockPos] = UIColor(0, 0, 0);
                } else if(storageEsp->bookshelf && blockId == 47) {  // Bookshelf
                    storageEsp->blockList[blockPos] = UIColor(139, 69, 19);
                } else if(storageEsp->enchantingTable && blockId == 116) {  // Enchanting Table
                    storageEsp->blockList[blockPos] = UIColor(0, 0, 255);
                } else if(storageEsp->decoratedPot &&
                          blockName.find("decorated_pot") != std::string::npos) {  // Decorated Pot
                    storageEsp->blockList[blockPos] = UIColor(255, 0, 0);
                } else if(storageEsp->dropper &&
                          blockName.find("dropper") != std::string::npos) {  // Dropper
                    storageEsp->blockList[blockPos] = UIColor(192, 192, 192);
                } else if(storageEsp->dispenser && blockId == 23) {  // Dispenser
                    storageEsp->blockList[blockPos] = UIColor(169, 169, 169);
                } else if(storageEsp->anvil && blockId == 145) {  // Anvil
                    storageEsp->blockList[blockPos] = UIColor(105, 105, 105);
                }
            }
        }

        if(oreEsp && oreEsp->isEnabled()) {
            int blockId = result->blockLegacy->blockId;
            std::string blockName = result->blockLegacy->blockName;

            if(result->blockLegacy && !blockName.empty()) {
                if(oreEsp->diamondOre && (blockId == 56)) {  // Diamond Ore
                    oreEsp->blockList[blockPos] = UIColor(0, 255, 255);
                } else if(oreEsp->goldOre && (blockId == 14)) {  // Gold Ore
                    oreEsp->blockList[blockPos] = UIColor(255, 215, 0);
                } else if(oreEsp->ironOre && (blockId == 15)) {  // Iron Ore
                    oreEsp->blockList[blockPos] = UIColor(184, 134, 11);
                } else if(oreEsp->coalOre && (blockId == 16)) {  // Coal Ore
                    oreEsp->blockList[blockPos] = UIColor(54, 54, 54);
                } else if(oreEsp->redstoneOre &&
                          (blockId == 73 || blockId == 74)) {  // Redstone Ore (lit/unlit)
                    oreEsp->blockList[blockPos] = UIColor(255, 0, 0);
                } else if(oreEsp->lapisOre && (blockId == 21)) {  // Lapis Lazuli Ore
                    oreEsp->blockList[blockPos] = UIColor(0, 0, 255);
                } else if(oreEsp->emeraldOre && (blockId == 129)) {  // Emerald Ore
                    oreEsp->blockList[blockPos] = UIColor(0, 255, 0);
                } else if(oreEsp->quartzOre && (blockId == 153)) {  // Nether Quartz Ore
                    oreEsp->blockList[blockPos] = UIColor(255, 255, 255);
                }
            }
        }

        if(portalEsp && portalEsp->isEnabled()) {
            if(result->blockLegacy && !result->blockLegacy->blockName.empty()) {
                if(result->blockLegacy->blockName.find("portal") != std::string::npos) {  // Portal
                    portalEsp->blockList[blockPos] = UIColor(200, 0, 255);
                }
            }
        }
        return result;
    }

   public:
    GetBlockHook() {
        OriginFunc = (void*)&oFunc;
        func = (void*)&BlockSourceGetBlockCallback;
    }
};
