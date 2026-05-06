#ifndef TOWERTREE_H
#define TOWERTREE_H

#include <string>
#include <vector>
#include <memory>
#include "Entities/Tower.h"
#include "Entities/Augment.h"

struct UpgradeNode {
    std::string name;
    float cost;
    std::vector<std::string> augments;
    std::vector<std::unique_ptr<UpgradeNode>> children;
};

class TowerTree {
private:
    std::string towerType_;
    float baseRange_;
    float baseDamage_;
    float baseAs_;
    float baseRs_;
    std::unique_ptr<UpgradeNode> rootUpgrade_;

public:
    TowerTree();
    ~TowerTree();

    // Factory for instantiating Augment classes by their string names
    static std::unique_ptr<Augment> createAugment(const std::string& augmentName);

    // Loads the tower blueprint from a JSON configuration file
    static std::unique_ptr<TowerTree> loadFromFile(const std::string& filepath);

    // Creates a Tower based on this blueprint, ready for the map
    std::unique_ptr<Tower> instantiateTower(Point position, Projectile& proj) const;
    
    const UpgradeNode* getRootUpgrade() const { return rootUpgrade_.get(); }
    const std::string getTowerType() const {return towerType_;}
};

#endif