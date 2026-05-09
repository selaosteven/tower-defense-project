#include "Entities/EnemyBlueprint.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <cstdlib> 
#include <ctime>   
#include <cmath>

using json = nlohmann::json;

EnemyBlueprint::EnemyBlueprint() : baseLp_(0), baseSpeed_(0), baseResistance_(0), isFlying_(false) {
    // Seed random number generator 
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

    // Store enemy attributes 
    auto blueprint = std::make_unique<EnemyBlueprint>();
    blueprint->enemyType_ = j.value("type", "DefaultEnemy");
    blueprint->baseLp_ = j.value("lp", 10.0f);
    blueprint->baseSpeed_ = j.value("speed", 1.0f);
    blueprint->baseResistance_ = j.value("resistance", 0.0f);
    blueprint->isFlying_ = j.value("isFlying", false);
    blueprint->possibleChars_ = j.value("chars", "X");
    
    // Load color if provided, else default color based on flying and ground type
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

    float offsetSpeed = (rand() / (float)RAND_MAX - 0.5f) * 0.2 - 0.1;
    offsetSpeed+=1.0f;
    auto enemy = std::make_unique<Enemy>(
        baseLp_ + powf(round,1.5), baseSpeed_*offsetSpeed, baseResistance_, isFlying_, std::string(1, displayChar), color_, size
    );

    enemy->setPosition(position);
    enemy->setPath(++path_start, path_end);
    enemy->setOffset(offset);
    enemy->setResistance(baseResistance_*enemy->getLp());
    return enemy;
}


std::vector<WaveEnemyConfig> EnemyBlueprint::loadWaveCreator(const std::string& filepath) {
    std::vector<WaveEnemyConfig> waveConfig; 
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Warning: Could not open " << filepath << ". Using default wave generation." << std::endl;
        return waveConfig;
    }

    json j;
    try {
        file >> j;
    } catch (const json::parse_error& e) {
        std::cerr << "JSON parsing error in " << filepath << ": " << e.what() << std::endl;
        return waveConfig;
    }

    // For each enemy, we gather the wave scaler
    if (j.contains("enemies") && j["enemies"].is_array()) {
        waveConfig.clear();
        for (const auto& enemy_json : j["enemies"]) {
            WaveEnemyConfig config;
            config.enemyType = enemy_json.value("type", "Regular");
            config.baseQuantity = enemy_json.value("baseQuantity", 1);
            config.linearScaler = enemy_json.value("linearScaler", 1.0f);
            config.exponentialScaler = enemy_json.value("exponentialScaler", 1.0f);
            waveConfig.push_back(config);
        }
    }
    return waveConfig;
}