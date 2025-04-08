#include "Assets.h"
#include <iostream>
#include <fstream>
#include <iostream>
#include <fstream>


Assets::Assets()
{
}

Assets::~Assets()
{
}


void Assets::addTexture(std::string& name, std::string& path, bool wantRepeated)
{
    sf::Texture texture;

    if (texture.loadFromFile(path))
    {
        texture.setRepeated(wantRepeated);
        this->m_texturesMap[name] = texture;
        this->m_texturesMap[name].setSmooth(true);
    }
}

void Assets::addAnimation(std::string& name, const std::string& texture, const size_t& frames, const size_t& speed)
{
    this->m_animationsMap[name] = Animation(name, m_texturesMap.at(texture),frames, speed);
}

void Assets::addSound(std::string& soundName, std::string& path)
{
    m_soundBufferMap[soundName] = sf::SoundBuffer();

    if (!m_soundBufferMap[soundName].loadFromFile(path))
    {
        std::cerr << "Could not load sound file: " << path << std::endl;
        m_soundBufferMap.erase(soundName);
    }
    else
    {
        std::cout << "Loaded Sound: " << path << std::endl;
        m_soundMap[soundName] = sf::Sound(m_soundBufferMap[soundName]);
        m_soundMap[soundName].setVolume(25);
    }
}

void Assets::addFont(std::string& name, std::string& path)
{
    sf::Font font;

    if (font.loadFromFile(path))
    {
        this->m_fontsMap[name] = font;
    }
}

const sf::Texture& Assets::getTexture(std::string name) const
{
    return m_texturesMap.at(name);
}

const Animation& Assets::getAnimation(std::string name) const
{
    return m_animationsMap.at(name);
}

sf::Sound& Assets::getSound(std::string soundName)
{
    return m_soundMap.at(soundName);
}

const sf::Font& Assets::getFont(std::string name) const
{
    return m_fontsMap.at(name);
}

void Assets::loadFromFile(const std::string& path)
{
    // Reads in config file here
    
    std::cout << "Loading From File" << "\n";

    std::ifstream fin(path);
    if (!fin) {
        std::cerr << "Error opening file!" << std::endl;
    }
    std::string assetType, asset, assetPath, animationTex;
    size_t animationFrameCount, animationSpeed;
    while (!fin.eof())
    {
        fin >> assetType;
        if (assetType == "Texture") {
            fin >> asset >> assetPath;
            
            sf::Texture texture;
            if (!texture.loadFromFile(assetPath))
            {
                std::cout << "ERROR LOADING TEXTURE" << std::endl;
            }
            m_texturesMap.insert({ asset,texture });
            std::cout << "Loaded Textures:" << assetPath << "\n";
            
        }
        else if (assetType == "Animation") {
            fin >> asset >> animationTex >> animationFrameCount >> animationSpeed;
            addAnimation(asset, animationTex, animationFrameCount, animationSpeed);
        }
        else if (assetType == "Sound") {
            fin >> asset >> assetPath;
            addSound(asset, assetPath);
        }
        else if (assetType == "Font") {
            fin >> asset >> assetPath;

            sf::Font font;
            if (!font.loadFromFile(assetPath))
            {
                std::cout << "ERROR LOADING TEXTURE" << std::endl;
            }
            m_fontsMap.insert({ asset,font });
            std::cout << "Loaded Font:" << assetPath << "\n";

 
        }
    }
    fin.close();
    std::cout << "Finished Loading From File" << "\n";
}

