#ifndef MUSICCONTROLLER_H
#define MUSICCONTROLLER_H

#include <string>
#include <map>
#include <vector>
#include <SDL2/SDL_mixer.h>

class MusicController {
public:
    static MusicController& getInstance();

    // Prevent copy
    MusicController(const MusicController&) = delete;
    MusicController& operator=(const MusicController&) = delete;

    // Initialize and Close
    void init();
    void close();

    // Load resources
    void loadMusic(const std::string& name, const std::string& path);
    void loadSound(const std::string& name, const std::string& path);

    // Playback
    void playMusic(const std::string& name, int loops = -1, int fade_ms = 0);
    void pauseMusic();
    void resumeMusic();
    void stopMusic(int fade_ms = 0);

    // Sounds
    // Returns channel id
    int playSound(const std::string& name, int loops = 0);
    // Play sound with volume variation (0.0 = no variation, 1.0 = full variation range)
    int playSoundWithVariation(const std::string& name, float volumeVariation = 0.2f, int loops = 0);
    // Spatial sound setup
    void setListenerPosition(float camX, float camY, float zoomScale, float winW, float winH);
    int playSoundSpatial(const std::string& name, float sourceX, float sourceY, float baseVolume = 1.0f, float volumeVariation = 0.2f, int loops = 0);
    void pauseSound(int channel);
    void resumeSound(int channel);
    void stopSound(int channel);

    // Global pause
    void pauseAllSounds();
    void resumeAllSounds();

    // Update in game loop (e.g. handle paused game state)
    void update(bool isGamePaused, float timeScale = 1.0f);

private:
    MusicController() = default;
    ~MusicController();

    std::map<std::string, Mix_Music*> musics_;
    std::map<std::string, std::vector<Mix_Chunk*>> sounds_;

    bool is_paused_ = false;

    float listener_camX_ = 0.0f;
    float listener_camY_ = 0.0f;
    float listener_scale_ = 1.0f;
    float listener_winW_ = 800.0f;
    float listener_winH_ = 600.0f;
    float time_scale_ = 1.0f;
};

#endif
