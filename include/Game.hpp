#pragma once
#include <SFML/Graphics.hpp>
#include "Player.hpp"
#include " Animation.hpp"

class Game {
public:
    Game();
    void run();

private:
    void processEvents();
    void update(float dt);
    void render();

private:
    sf::RenderWindow _window;
    sf::Font _font;
    sf::Text _text1;
    sf::Text _text2;

private:
    sf::Texture _backgroundTexture;
    sf::Texture _backgroundTexture2;

    sf::Sprite _backgroundSprite;
    sf::Sprite _backgroundSprite2;

    Animation _PlayerAnimation;
private:
    bool _isPlaying = false;

    Player _player;

};