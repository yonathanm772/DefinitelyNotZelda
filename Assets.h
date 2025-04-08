#pragma once
#pragma once
#include "Animation.h"
#include <SFML/Audio.hpp>
#include <map>
typedef std::map<std::string, sf::Texture> TexturesMap;
typedef std::map<std::string, Animation> AnimationsMap;
typedef std::map<std::string, sf::Sound> SoundMap;
typedef std::map<std::string, sf::SoundBuffer> SoundBufferMap;
typedef std::map<std::string, sf::Font> FontsMap;

class GameEngine;

class Assets
{
	TexturesMap		m_texturesMap;
	AnimationsMap	m_animationsMap;
	FontsMap		m_fontsMap;
	SoundMap		m_soundMap;
	SoundBufferMap	m_soundBufferMap;

public:
	Assets();
	~Assets();
	void addTexture(std::string& name, std::string& path, bool wantRepeated = false);
	void addAnimation(std::string& name, const std::string& texture, const size_t& frames, const size_t& speed);
	void addSound(std::string& soundName, std::string &path);
	void addFont(std::string& name, std::string& path);

	const sf::Texture& getTexture(std::string name) const;
	const Animation& getAnimation(std::string name) const;
	sf::Sound& getSound(std::string soundName);
	const sf::Font& getFont(std::string name) const;
	void loadFromFile(const std::string& path);
};

