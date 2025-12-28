#include "Player.hpp"
#include <iostream>

Player::Player() {
    // 加载贴图
    if (!texture.loadFromFile("../assets/character.png")) {
        std::cerr << "Failed to load character.png" << std::endl;
    }

    sprite.setTexture(texture);

    // 假设 sprite sheet 每帧 100x100，一行 4 帧
    sprite.setTextureRect(sf::IntRect(0, 0, 100, 100));
    sprite.setPosition(0, 500); // 初始位置

    speed = 3.0f;

}
float x, y;
void Player::handleInput() {
    
    // 移动逻辑
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        sprite.move(-speed, 0);
        x-=3.0;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        sprite.move(speed, 0);
        x+=3.0;
    }
}

void Player::update(float dt) {
    // 动画逻辑（切换帧）
    static sf::Clock clock;
    static int frame = 0;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::A)){
        // 动画帧切换，每 120ms 换一帧
        if (clock.getElapsedTime().asMilliseconds() > 120) {
            frame = (frame + 1) % 4;  // 4 帧循环
            sprite.setTextureRect(sf::IntRect(frame * 100, 0, 100, 100));
            clock.restart();
        }
    }
}

void Player::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}