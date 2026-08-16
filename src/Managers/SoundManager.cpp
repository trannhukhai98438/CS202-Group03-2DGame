#include "Managers/SoundManager.hpp"
#include <iostream>

namespace fs = std::filesystem;

SoundManager::SoundManager(size_t maxChannels)
    : m_bgmDirPath("assets/audio/bgm/"),
      m_sfxDirPath("assets/audio/sfx/"),
      m_maxSfxChannels(maxChannels),
      m_sfxVolume(100.0f),
      m_bgmVolume(80.0f) {
}

void SoundManager::setAudioDirectories(const std::string& bgmPath, const std::string& sfxPath) {
    m_bgmDirPath = bgmPath;
    m_sfxDirPath = sfxPath;
}

size_t SoundManager::loadAllSFX() {
    size_t loadedCount = 0;

    if (!fs::exists(m_sfxDirPath) || !fs::is_directory(m_sfxDirPath)) {
        std::cerr << "[SoundManager] ERROR: SFX directory does not exist: " << m_sfxDirPath << std::endl;
        return 0;
    }

    // Iterate through all files inside the audio/sfx/ directory
    for (const auto& entry : fs::directory_iterator(m_sfxDirPath)) {
        if (entry.is_regular_file() && entry.path().extension() == ".wav") {
            // Extract key name from filename without extension (e.g. "sfx_jump.wav" -> "sfx_jump")
            std::string key = entry.path().stem().string();
            std::string filename = entry.path().filename().string();

            if (loadSFX(key, filename)) {
                loadedCount++;
            }
        }
    }

    std::cout << "[SoundManager] Automatically loaded " << loadedCount << " SFX files." << std::endl;
    return loadedCount;
}

bool SoundManager::loadSFX(const std::string& name, const std::string& filename) {
    std::string fullPath = m_sfxDirPath + filename;
    
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile(fullPath)) {
        std::cerr << "[SoundManager] ERROR: Failed to load SFX file: " << fullPath << std::endl;
        return false;
    }
    
    m_sfxBuffers[name] = std::move(buffer);
    return true;
}

void SoundManager::playSFX(const std::string& name) {
    auto it = m_sfxBuffers.find(name);
    if (it == m_sfxBuffers.end()) {
        std::cerr << "[SoundManager] WARNING: SFX not found in cache: " << name << std::endl;
        return;
    }

    for (auto& sound : m_sfxPool) {
        if (sound->getStatus() == sf::Sound::Stopped) {
            sound->setBuffer(it->second);
            sound->setVolume(m_sfxVolume);
            sound->play();
            return;
        }
    }

    if (m_sfxPool.size() < m_maxSfxChannels) {
        auto sound = std::make_unique<sf::Sound>();
        sound->setBuffer(it->second);
        sound->setVolume(m_sfxVolume);
        sound->play();
        m_sfxPool.push_back(std::move(sound));
    }
}

bool SoundManager::playBGM(const std::string& filename, bool loop) {
    std::string fullPath = m_bgmDirPath + filename;

    // Automatically append .mp3 if no audio extension is present
    if (fullPath.find('.') == std::string::npos) {
        fullPath += ".mp3";
    }

    if (!m_bgm.openFromFile(fullPath)) {
        std::cerr << "[SoundManager] ERROR: Failed to stream BGM file: " << fullPath << std::endl;
        return false;
    }

    m_bgm.setLoop(loop);
    m_bgm.setVolume(m_bgmVolume);
    m_bgm.play();
    return true;
}

void SoundManager::stopBGM() { m_bgm.stop(); }
void SoundManager::pauseBGM() { m_bgm.pause(); }
void SoundManager::resumeBGM() {
    if (m_bgm.getStatus() == sf::Sound::Paused) m_bgm.play();
}

void SoundManager::setSFXVolume(float volume) {
    m_sfxVolume = volume;
    for (auto& sound : m_sfxPool) sound->setVolume(m_sfxVolume);
}

void SoundManager::setBGMVolume(float volume) {
    m_bgmVolume = volume;
    m_bgm.setVolume(m_bgmVolume);
}

void SoundManager::setMasterVolume(float volume) {
    setSFXVolume(volume);
    setBGMVolume(volume);
}

void SoundManager::update() {}