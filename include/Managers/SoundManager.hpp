#pragma once
#include <SFML/Audio.hpp>
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <filesystem> // C++17 feature for automated directory scanning

class SoundManager {
private:
    std::string m_bgmDirPath;
    std::string m_sfxDirPath;

    std::unordered_map<std::string, sf::SoundBuffer> m_sfxBuffers;
    std::vector<std::unique_ptr<sf::Sound>> m_sfxPool;
    size_t m_maxSfxChannels;

    sf::Music m_bgm;
    float m_sfxVolume;
    float m_bgmVolume;

public:
    explicit SoundManager(size_t maxChannels = 16);
    ~SoundManager() = default;

    void setAudioDirectories(const std::string& bgmPath = "assets/audio/bgm/", 
                             const std::string& sfxPath = "assets/audio/sfx/");

    // Automatically scans and loads all .wav files in the SFX directory
    size_t loadAllSFX();

    // Manual load for a single SFX file if needed
    bool loadSFX(const std::string& name, const std::string& filename);

    // Play SFX using its filename (without extension) as key
    void playSFX(const std::string& name);

    // BGM handling (BGM is streamed from disk when played, so no preloading needed)
    bool playBGM(const std::string& filename, bool loop = true);
    void stopBGM();
    void pauseBGM();
    void resumeBGM();

    void setSFXVolume(float volume);
    void setBGMVolume(float volume);
    void setMasterVolume(float volume);

    void update();
};