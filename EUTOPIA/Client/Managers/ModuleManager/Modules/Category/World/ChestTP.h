#pragma once
#include <set>

#include "../../ModuleBase/Module.h"

struct BlockPosComparator {
    bool operator()(const BlockPos& lhs, const BlockPos& rhs) const {
        if(lhs.x != rhs.x)
            return lhs.x < rhs.x;
        if(lhs.y != rhs.y)
            return lhs.y < rhs.y;
        return lhs.z < rhs.z;
    }
};

class ChestTP : public Module {
   private:
    std::set<BlockPos, BlockPosComparator> visitedChests;
    bool works = false;
    bool tpdone = false;
    Vec3<float> goal;
    float closestDistance = 0;
    int range = 50;
    bool customY = false;  
    int customYLevel = 60; 

   public:
    ChestTP();
    virtual void onEnable() override;
    virtual void onNormalTick(LocalPlayer* actor) override;
    virtual void onDisable() override;
};
