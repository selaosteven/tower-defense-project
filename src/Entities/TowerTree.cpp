#include "Entities/TowerTree.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

TowerTree::TowerTree() : baseRange_(0), baseDamage_(0), baseAs_(0), baseRs_(0) {}
TowerTree::~TowerTree() {}

std::unique_ptr<Augment> TowerTree::createAugment(const std::string& augmentName) {
    // Mapping string names to their actual C++ class definitions
    if (augmentName == "CritAugment") {
        return std::make_unique<CritAugment>(0.2f, 2.0f); 
    } else if (augmentName == "RangeAugment") {
        return std::make_unique<RangeAugment>(25.0f);
    } else if (augmentName == "AntiAirAugment") {
        return std::make_unique<TargetingAugment>(false, true); // Targets ONLY air
    } else if (augmentName == "AllTargetAugment") {
        return std::make_unique<TargetingAugment>(true, true); // Targets both ground and air
    }
    // If the augment name doesn't exist, we skip it
    std::cerr << "Warning: Augment '" << augmentName << "' not found. Skipping." << std::endl;
    return nullptr;
}

std::unique_ptr<UpgradeNode> parseUpgradeNode(const json& j) {
    auto node = std::make_unique<UpgradeNode>();
    node->name = j.value("name", "Unknown Upgrade");
    node->cost = j.value("cost", 0.0f);
    
    if (j.contains("augments") && j["augments"].is_array()) {
        for (const auto& aug : j["augments"]) {
            node->augments.push_back(aug.get<std::string>());
        }
    }

    if (j.contains("children") && j["children"].is_array()) {
        for (const auto& childJson : j["children"]) {
            node->children.push_back(parseUpgradeNode(childJson));
        }
    }
    return node;
}

std::unique_ptr<TowerTree> TowerTree::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open " << filepath << std::endl;
        return nullptr;
    }

    json j;
    try {
        file >> j;
    } catch (const json::parse_error& e) {
        std::cerr << "JSON parsing error in " << filepath << ": " << e.what() << std::endl;
        return nullptr;
    }

    auto tree = std::make_unique<TowerTree>();
    tree->towerType_ = j.value("type", "DefaultTower");
    tree->baseRange_ = j.value("baseRange", 100.0f);
    tree->baseDamage_ = j.value("baseDamage", 10.0f);
    tree->baseAs_ = j.value("baseAs", 1.0f);
    tree->baseRs_ = j.value("baseRs", 1.0f);

    if (j.contains("upgrades")) {
        tree->rootUpgrade_ = parseUpgradeNode(j["upgrades"]);
    }

    return tree;
}

std::unique_ptr<Tower> TowerTree::instantiateTower(Point position, Projectile& proj) const {
    auto tower = std::make_unique<Tower>(
        baseRange_, baseDamage_, baseAs_, baseRs_, proj, towerType_
    );
    tower->setPosition(position);
    
    // Apply root node augments (Base mechanics for this tower type)
    if (rootUpgrade_) {
        tower->applyUpgrade(rootUpgrade_.get());
    }
    
    return tower;
}