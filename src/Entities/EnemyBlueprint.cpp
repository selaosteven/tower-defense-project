#include "Entities/EnemyBlueprint.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <cstdlib> // For rand()
#include <ctime>   // For time()
#include <cmath>

using json = nlohmann::json;

EnemyBlueprint::EnemyBlueprint() : baseLp_(0), baseSpeed_(0), baseResistance_(0), isFlying_(false) {
    // Seed random number generator once
    static bool seeded = false;
    if (!seeded) {
        srand(time(nullptr));
        seeded = true;
    }
}
EnemyBlueprint::~EnemyBlueprint() {}

std::unique_ptr<EnemyBlueprint> EnemyBlueprint::loadFromFile(const std::string& filepath) {
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

    auto blueprint = std::make_unique<EnemyBlueprint>();
    blueprint->enemyType_ = j.value("type", "DefaultEnemy");
    blueprint->baseLp_ = j.value("lp", 10.0f);
    blueprint->baseSpeed_ = j.value("speed", 1.0f);
    blueprint->baseResistance_ = j.value("resistance", 0.0f);
    blueprint->isFlying_ = j.value("isFlying", false);
    blueprint->possibleChars_ = j.value("chars", "X");
    
    if (j.contains("color") && j["color"].is_array() && j["color"].size() >= 3) {
        blueprint->color_.r = j["color"][0].get<uint8_t>();
        blueprint->color_.g = j["color"][1].get<uint8_t>();
        blueprint->color_.b = j["color"][2].get<uint8_t>();
        blueprint->color_.a = j["color"].size() >= 4 ? j["color"][3].get<uint8_t>() : 255;
    } else {
        if (blueprint->isFlying_) {
            blueprint->color_ = {100, 200, 255, 255}; // Blue for flying enemies
        } else {
            blueprint->color_ = {255, 100, 100, 255}; // Red for ground enemies
        }
    }

    return blueprint;
}

std::unique_ptr<Enemy> EnemyBlueprint::instantiateEnemy(Point position, float offset, std::list<Point>::iterator path_start, std::list<Point>::iterator path_end, float size, int round) const {
    
    char displayChar = ' ';
    if (!possibleChars_.empty()) {
        int randomIndex = rand() % possibleChars_.length();
        displayChar = possibleChars_[randomIndex];
    }


    auto enemy = std::make_unique<Enemy>(
        baseLp_ + powf(round,1.5), baseSpeed_, baseResistance_, isFlying_, std::string(1, displayChar), color_, size
    );

    enemy->setPosition(position);
    enemy->setPath(++path_start, path_end);
    enemy->setOffset(offset);
    
    return enemy;
}
