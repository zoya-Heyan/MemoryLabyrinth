#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class Animation {
public:
    Animation(const std::vector<std::string>& frameFiles, float frameTime);

    void update();                       
    void setPosition(float x, float y);  
    void draw(sf::RenderWindow& window); 

private:
    std::vector<sf::Texture> m_textures; 
    sf::Sprite m_sprite;
    int m_currentFrame;
    float m_frameTime;
    sf::Clock m_clock;
};