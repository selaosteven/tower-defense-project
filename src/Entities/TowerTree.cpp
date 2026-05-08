#include "Entities/TowerTree.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

TowerTree::TowerTree() : baseRange_(0), baseDamage_(0), baseAs_(0), baseRs_(0) {}
TowerTree::~TowerTree() {}

std::unique_ptr<Augment> TowerTree::createAugment(const std::string& augmentName) {
    // Mapping string names to their actual C++ class definitions
    if (augmentName == "CritAugment") { // crit damage
        return std::make_unique<CritAugment>(0.2f, 2.0f); 
    } else if (augmentName == "RangeAugment") { // la range
        return std::make_unique<RangeAugment>(25.0f);
    } else if (augmentName == "AntiAirAugment") { // able to hit flying enemy
        return std::make_unique<TargetingAugment>(false, true); // Targets ONLY air
    } else if (augmentName == "AllTargetAugment") { // targeting everyone
        return std::make_unique<TargetingAugment>(true, true); // Targets both ground and air
    } else if(augmentName == "SlowAugment") { // freezing / slow
        return std::make_unique<SlownessAugment>(0.8f);
    } else if(augmentName == "SplashAugment") { // AOE => degat de zone
        return std::make_unique<AoeAugment>(0.5f);
    } else if(augmentName == "DamageAugment") {
        return std::make_unique<DamageAugment>();
    } else if(augmentName == "ProjectileSpeedAugment") {
        return std::make_unique<ProjectileSpeedAugment>(1.5f); // 50% faster projectiles
    } else if(augmentName == "RotationSpeedAugment") {
        return std::make_unique<RotationSpeedAugment>(90.0f); // Adds 90 degrees/sec rotation
    } else if(augmentName == "AttackSpeedAugment") {
        return std::make_unique<AttackSpeedAugment>(1.2f); // 20% faster Attack Speed
    } else if(augmentName == "SplashRadiusAugment") {
        return std::make_unique<SplashRadiusAugment>(0.5f); // Increase splash radius by 0.5 tiles
    } else if(augmentName == "SlowStrongerAugment") {
        return std::make_unique<SlownessAugment>(0.4f); // Harsher slow multiplier than the standard one
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

    if (j.contains("formes") && j["formes"].is_array()) {
        for (const auto& shape : j["formes"]) {

            std::string type = shape.value("type", "");

            SDL_Color col = {255, 255, 255, 255};
            if (shape.contains("color") && shape["color"].is_array() && shape["color"].size() == 4) {
                col.r = shape["color"][0];
                col.g = shape["color"][1];
                col.b = shape["color"][2];
                col.a = shape["color"][3];
            }

            if (type == "rectangle") {
                tree->shapes_.push_back(0);
                tree->shapes_.push_back(shape.value("width", 1.0f));
                tree->shapes_.push_back(shape.value("height", 1.0f));
                tree->shapes_.push_back(col.r);
                tree->shapes_.push_back(col.g);
                tree->shapes_.push_back(col.b);
                tree->shapes_.push_back(col.a);
            }
            else if (type == "circle") {
                tree->shapes_.push_back(1);
                tree->shapes_.push_back(shape.value("radius", 0.5f));
                tree->shapes_.push_back(col.r);
                tree->shapes_.push_back(col.g);
                tree->shapes_.push_back(col.b);
                tree->shapes_.push_back(col.a);
            }
            else if (type == "triangle") {
                tree->shapes_.push_back(2);
                tree->shapes_.push_back(shape.value("size", 1.0f));
                tree->shapes_.push_back(shape.value("orientation", 0.0f));
                tree->shapes_.push_back(col.r);
                tree->shapes_.push_back(col.g);
                tree->shapes_.push_back(col.b);
                tree->shapes_.push_back(col.a);
            }
            else if (type == "octogone") {
                tree->shapes_.push_back(3);
                tree->shapes_.push_back(shape.value("size", 1.0f));
                tree->shapes_.push_back(col.r);
                tree->shapes_.push_back(col.g);
                tree->shapes_.push_back(col.b);
                tree->shapes_.push_back(col.a);
            }
            else if (type == "carre") {
                tree->shapes_.push_back(4);
                tree->shapes_.push_back(shape.value("side", 1.0f));
                tree->shapes_.push_back(col.r);
                tree->shapes_.push_back(col.g);
                tree->shapes_.push_back(col.b);
                tree->shapes_.push_back(col.a);
            }
        }
    }

    return tree;
}

std::unique_ptr<Tower> TowerTree::instantiateTower(Point position, Projectile& proj) const {
    auto tower = std::make_unique<Tower>(
        baseRange_, baseDamage_, baseAs_, baseRs_, proj, towerType_,shapes_,0,0
    );
    tower->setPosition(position);
    
    // Apply root node augments (Base mechanics for this tower type)
    if (rootUpgrade_) {
        tower->applyUpgrade(rootUpgrade_.get());
    }
    
    return tower;
}