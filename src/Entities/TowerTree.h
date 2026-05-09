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
    std::string towerType_; // Tower type
    float baseRange_; // Base attack range
    float baseDamage_; // Base damage
    float baseAs_; // Base attack speed
    float baseRs_; // Base rotation speed
    float baseArmorPiercing_; // Base armor piercing percentage
    float cone_angle_; // Base firing cone angle
    std::unique_ptr<UpgradeNode> rootUpgrade_; // Root of the upgrade tree
    std::vector<float> shapes_; // Shape descriptor for custom sprite


public:
    TowerTree();
    ~TowerTree();

    /**
     * @brief Factory for instantiating Augment classes by their string names
     * 
     * @param augmentName 
     * @return std::unique_ptr<Augment> 
     */
    static std::unique_ptr<Augment> createAugment(const std::string& augmentName);

    /**
     * @brief Loads the tower blueprint from a JSON configuration file
     * 
     * @param filepath 
     * @return std::unique_ptr<TowerTree> 
     */
    static std::unique_ptr<TowerTree> loadFromFile(const std::string& filepath);

    /**
     * @brief Create Tower using blueprint
     * 
     * @param position 
     * @param proj 
     * @return std::unique_ptr<Tower> 
     */
    std::unique_ptr<Tower> instantiateTower(Point position, Projectile& proj) const;
    
    /**
     * @brief Returns to the root upgrade node of the tower's upgrade tree
     * 
     *
     * @return 
     */
    const UpgradeNode* getRootUpgrade() const { return rootUpgrade_.get(); }
    const std::string getTowerType() const {return towerType_;}


};

#endif