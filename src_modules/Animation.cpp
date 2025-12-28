#include " Animation.hpp"
#include <iostream>

Animation::Animation(const std::vector<std::string>& frameFiles, float frameTime)
    : m_currentFrame(0),
      m_frameTime(frameTime)
{
    for (const auto& file : frameFiles) {
        sf::Texture tex;
        if (!tex.loadFromFile(file)) {
            std::cerr << "Failed to load frame: " << file << "\n";
            continue;
        }
        m_textures.push_back(std::move(tex));
    }

    if (!m_textures.empty()) {
        m_sprite.setTexture(m_textures[0]);
    }
}

void Animation::update() {
    if (m_textures.empty()) return;

    if (m_clock.getElapsedTime().asSeconds() > m_frameTime) {
        m_currentFrame = (m_currentFrame + 1) % m_textures.size();
        m_sprite.setTexture(m_textures[m_currentFrame]);
        m_clock.restart();
    }
}

void Animation::setPosition(float x, float y) {
    m_sprite.setPosition(x, y);
}

void Animation::draw(sf::RenderWindow& window) {
    if (!m_textures.empty()) {
        window.draw(m_sprite);
    }
}