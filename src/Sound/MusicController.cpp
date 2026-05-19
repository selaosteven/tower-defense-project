#include "MusicController.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>

MusicController& MusicController::getInstance() {
    static MusicController instance;
    return instance;
}

MusicController::~MusicController() {
    close();
}

void MusicController::init() {
    // Already called Mix_OpenAudio in init_sdl, but we can do extra setup here if needed
    Mix_AllocateChannels(32);
}

void MusicController::close() {
    for (auto& pair : musics_) {
        if(pair.second) Mix_FreeMusic(pair.second);
    }
    musics_.clear();
    for (auto& pair : sounds_) {
        for (auto* chunk : pair.second) {
            if(chunk) Mix_FreeChunk(chunk);
        }
    }
    sounds_.clear();
}

void MusicController::loadMusic(const std::string& name, const std::string& path) {
    if (musics_.find(name) == musics_.end()) {
        Mix_Music* music = Mix_LoadMUS(path.c_str());
        if (music) {
            musics_[name] = music;
        } else {
            std::cerr << "Failed to load music: " << path << " Error: " << Mix_GetError() << std::endl;
        }
    }
}

void MusicController::loadSound(const std::string& name, const std::string& path) {
    Mix_Chunk* chunk = Mix_LoadWAV(path.c_str());
    if (chunk) {
        std::cout << "Add sound variant to : " << name << " (" << path << ")" << std::endl;
        sounds_[name].push_back(chunk);
    } else {
        std::cerr << "Failed to load sound: " << path << " Error: " << Mix_GetError() << std::endl;
    }
}

void MusicController::playMusic(const std::string& name, int loops, int fade_ms) {
    auto it = musics_.find(name);
    if (it != musics_.end()) {
        if (fade_ms > 0) {
            Mix_FadeInMusic(it->second, loops, fade_ms);
        } else {
            Mix_PlayMusic(it->second, loops);
        }
    }
}

void MusicController::pauseMusic() {
    if (Mix_PlayingMusic() != 0) {
        Mix_PauseMusic();
    }
}

void MusicController::resumeMusic() {
    if (Mix_PausedMusic() != 0) {
        Mix_ResumeMusic();
    }
}

void MusicController::stopMusic(int fade_ms) {
    if (fade_ms > 0) {
        Mix_FadeOutMusic(fade_ms);
    } else {
        Mix_HaltMusic();
    }
}

int MusicController::playSound(const std::string& name, int loops) {
    auto it = sounds_.find(name);
    if (it != sounds_.end() && !it->second.empty()) {
        int idx = rand() % it->second.size();
        int channel = Mix_PlayChannel(-1, it->second[idx], loops);
        if (channel != -1) {
            float speedDampening = (time_scale_ > 1.0f) ? (1.0f / time_scale_) : 1.0f;
            Mix_Volume(channel, static_cast<int>(MIX_MAX_VOLUME * speedDampening));
        }
        return channel;
    }
    return -1;
}

int MusicController::playSoundWithVariation(const std::string& name, float volumeVariation, int loops) {
    auto it = sounds_.find(name);
    if (it != sounds_.end() && !it->second.empty()) {
        int idx = rand() % it->second.size();
        // Find a free channel
        int channel = Mix_PlayChannel(-1, it->second[idx], loops);
        if (channel != -1) {
            // Apply volume variation: random between (1 - volumeVariation) and 1.0
            float minVolume = std::max(0.1f, 1.0f - volumeVariation);
            float randomFactor = minVolume + (static_cast<float>(rand()) / RAND_MAX) * (1.0f - minVolume);
            float speedDampening = (time_scale_ > 1.0f) ? (1.0f / time_scale_) : 1.0f;
            int volume = static_cast<int>(MIX_MAX_VOLUME * randomFactor * speedDampening);
            Mix_Volume(channel, volume);
        }
        return channel;
    }
    return -1;
}

void MusicController::setListenerPosition(float camX, float camY, float zoomScale, float winW, float winH) {
    listener_camX_ = camX;
    listener_camY_ = camY;
    listener_scale_ = zoomScale;
    listener_winW_ = winW;
    listener_winH_ = winH;
}

int MusicController::playSoundSpatial(const std::string& name, float sourceX, float sourceY, float baseVolume, float volumeVariation, int loops) {
    auto it = sounds_.find(name);
    if (it != sounds_.end() && !it->second.empty()) {
        int idx = rand() % it->second.size();
        int channel = Mix_PlayChannel(-1, it->second[idx], loops);
        if (channel != -1) {
            float minVolume = std::max(0.1f, 1.0f - volumeVariation);
            float randomFactor = minVolume + (static_cast<float>(rand()) / RAND_MAX) * (1.0f - minVolume);
            float varVolume = baseVolume * randomFactor;
            
            float screenX = sourceX * listener_scale_ + listener_camX_;
            float screenY = sourceY * listener_scale_ + listener_camY_;
            float centerX = listener_winW_ / 2.0f;
            float centerY = listener_winH_ / 2.0f;
            float dx = screenX - centerX;
            float dy = screenY - centerY;
            float dist = std::sqrt(dx * dx + dy * dy);
            float maxDist = std::sqrt(centerX * centerX + centerY * centerY) * 1.5f; 
            int dist255 = static_cast<int>(std::min(255.0f, (dist / maxDist) * 255.0f));
            float angle = std::atan2(dx, -dy) * 180.0f / 3.14159265f;
            if (angle < 0) angle += 360.0f;
            Mix_SetPosition(channel, static_cast<Sint16>(angle), static_cast<Uint8>(dist255));
            float speedDampening = (time_scale_ > 1.0f) ? (1.0f / time_scale_) : 1.0f;
            int finalVolume = static_cast<int>(MIX_MAX_VOLUME * varVolume * speedDampening);
            Mix_Volume(channel, finalVolume);
        }
        return channel;
    }
    return -1;
}

void MusicController::pauseSound(int channel) {
    Mix_Pause(channel);
}

void MusicController::resumeSound(int channel) {
    Mix_Resume(channel);
}

void MusicController::stopSound(int channel) {
    Mix_HaltChannel(channel);
}

void MusicController::pauseAllSounds() {
    Mix_Pause(-1);
    pauseMusic();
}

void MusicController::resumeAllSounds() {
    Mix_Resume(-1);
    resumeMusic();
}

void MusicController::update(bool isGamePaused, float timeScale) {
    if (isGamePaused && !is_paused_) {
        pauseAllSounds();
        is_paused_ = true;
    } else if (!isGamePaused && is_paused_) {
        resumeAllSounds();
        is_paused_ = false;
    }
    time_scale_ = timeScale;
}
