#pragma once
#include <SFML/Graphics.hpp>

class Player {
public:
    float x, y;
    Player();
    void handleInput();
    void update(float dt);
    void draw(sf::RenderWindow& window);

private:
    float speed;
    sf::Texture texture;
    sf::Sprite sprite;
};