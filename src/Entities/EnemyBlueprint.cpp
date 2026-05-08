#include "Entities/EnemyBlueprint.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <cstdlib> // For rand()
#include <ctime>   // For time()

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

    return blueprint;
}

std::unique_ptr<Enemy> EnemyBlueprint::instantiateEnemy(Point position, float offset, std::list<Point>::iterator path_start, std::list<Point>::iterator path_end) const {
    
    char displayChar = ' ';
    if (!possibleChars_.empty()) {
        int randomIndex = rand() % possibleChars_.length();
        displayChar = possibleChars_[randomIndex];
    }

    auto enemy = std::make_unique<Enemy>(
        baseLp_, baseSpeed_, baseResistance_, isFlying_, std::string(1, displayChar)
    );

    enemy->setPosition(position);
    enemy->setPath(++path_start, path_end);
    enemy->setOffset(offset);
    
    return enemy;
}
