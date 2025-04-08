#include "Animation.h"
#include <iostream>
#include <cmath>

Animation::Animation()
{
}

Animation::Animation(const std::string& name, const sf::Texture& t)
	:Animation(name, t, 1, 0)
{
}

Animation::Animation(const std::string& name, const sf::Texture& t, size_t frameCount, size_t speed)
	: m_name(name)
	, m_sprite(t)
	, m_frameCount(frameCount)
	, m_currentFrame(0)
	, m_speed(speed)
{
	m_size = Vec2((float)t.getSize().x / frameCount, (float)t.getSize().y);
	m_sprite.setOrigin(m_size.x / 2.0f, m_size.y / 2.0f);
	m_sprite.setTextureRect(sf::IntRect(std::floor(m_currentFrame) * m_size.x, 0, m_size.x, m_size.y));
}


//updates the animation to show next frame, depending on its speed
//animation loops when it reaches the end
void Animation::update()
{
	

	if (m_speed != 0 )
	{
		//add the speed variable to the current frame
		m_gameFrame++;
		/*std::cout << "+++++++++++++++++++++++++++ANIMATION+++++++++++++++++++++ " << m_gameFrame << "\n";
		std::cout << "FRAME COUNT BEFORE " << m_frameCount << "\n";
		std::cout << "FRAME COUNT AFTER " << m_frameCount << "\n";
		std::cout << "SPEED AFTER " << m_speed << "\n";
		std::cout << "TEXTURE SIZE X " << m_size.x << "\n";
		std::cout << "TEXTURE SIZE Y " << m_size.y << "\n";*/
		
		m_currentFrame = (m_gameFrame / m_speed) % m_frameCount;
		//std::cout << "CURRENT FRAME " << m_currentFrame << "\n";
		m_sprite.setTextureRect(sf::IntRect(std::floor(m_currentFrame) * m_size.x, 0, m_size.x, m_size.y));
	}
	//m_currentFrame += m_speed;
	//TODO: 1) calculate the correct frame of animation to play based on currentFrame and speed
	//		2) set the texture rectangle propectly (see constructor for sample)
}

bool Animation::hasEnded() const
{
	/*std::cout << "CURRENT FRAME?\n";
	std::cout << m_currentFrame << "\n";
	std::cout << "FRAME COUNT?\n";
	std::cout << m_frameCount << "\n";*/

	//TODO: Detect when animation has ended (last frame was played) and return true
	if (m_currentFrame >= m_frameCount - 1)
	{
		return true;
	}
	else
		return false;
}

const std::string& Animation::getName() const
{
	return m_name;
}


const Vec2& Animation::getSize() const
{
	return m_size;
}

const size_t& Animation::getFrameCount() const
{
	return m_frameCount;
}
const size_t& Animation::getCurrentFrame() const
{
	return m_currentFrame;
}
const size_t& Animation::getSpeed() const
{
	return m_speed;
}

sf::Sprite& Animation::getSprite()
{
	return m_sprite;
}

// 