#include "Game.hpp"
#include " Animation.hpp"
#include <iostream>


Game::Game()
    : _window(sf::VideoMode(1191, 670), "Go Far")
    , _PlayerAnimation({
        "../assets/walk1.png",
        "../assets/walk2.png",
        "../assets/walk3.png",
        "../assets/walk4.png",
        "../assets/walk5.png",
        "../assets/walk6.png",
        "../assets/walk7.png",
        "../assets/walk8.png",
        "../assets/walk9.png",
        "../assets/walk10.png",
        "../assets/walk11.png",
        "../assets/walk12.png",
        "../assets/walk13.png",
        "../assets/walk14.png",
        "../assets/walk15.png",
        "../assets/walk16.png"
    }, 0.2f)//...means pics
{

    _window.setFramerateLimit(60);

    // 加载字体
    if (!_font.loadFromFile("../assets/Roboto-SemiBold.ttf")) {
        std::cerr << "Failed to load font\n";
    }

    // 加载背景
    if (!_backgroundTexture.loadFromFile("../assets/background.jpg")) {
        std::cerr << "Failed to load background image\n";
    }

    if (!_backgroundTexture2.loadFromFile("../assets/chara.png")) {
        std::cerr << "Failed to load the image\n";
    }

    _backgroundSprite.setTexture(_backgroundTexture);
    _backgroundSprite2.setTexture(_backgroundTexture2);

    // 文本设置
    _text1.setFont(_font);
    _text1.setString("EXIT");
    _text1.setCharacterSize(50);
    _text1.setFillColor(sf::Color(128, 118, 105));
    _text1.setPosition(120.f, 480.f);

    _text2.setFont(_font);
    _text2.setString("START!");
    _text2.setCharacterSize(55);
    _text2.setFillColor(sf::Color(128, 118, 105));
    _text2.setPosition(115.f, 550.f);

    // 构造函数体结束
}

void Game::run() {
    sf::Clock clock;
    while (_window.isOpen()) {
        sf::Time dt = clock.restart();//帧率控制
        processEvents();
        update(dt.asSeconds());
        render();
    }
}

void Game::processEvents() {

    sf::Event event;
    while (_window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) 
            _window.close();

        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
            _window.close();

        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mousePos(
                static_cast<float>(event.mouseButton.x),
                static_cast<float>(event.mouseButton.y)
            );

            if (_text1.getGlobalBounds().contains(mousePos)) {
                _window.close();
            }

            if (_text2.getGlobalBounds().contains(mousePos)) {
                _isPlaying = true;
                if (!_backgroundTexture.loadFromFile("../assets/back2.jpg")) {
                    std::cerr << "Failed to load game background image\n";
                        if (_player.x == 935.0) {
                            _player.x = 935.5;
                            sf::CircleShape circle(50.0f);
                            circle.setFillColor(sf::Color::Red);        
                            circle.setOutlineThickness(5.f);             
                            circle.setOutlineColor(sf::Color::Blue);     
                            circle.setPosition(600.f, 108.f);   
                            _window.draw(circle); 
                                   
                        }
                }
            _backgroundSprite.setTexture(_backgroundTexture);
            }
        }
    }
}

void Game::update(float dt) {
    if (_isPlaying) {
        _player.handleInput();   // 键盘输入
        _player.update(dt);      // 动画更新
    }
}

void Game::render() {
    _window.clear(sf::Color::Black);

    if (!_isPlaying) {
        // 主菜单
        _window.draw(_backgroundSprite);
        _window.draw(_text1); // EXIT出现
        _window.draw(_text2); // START
    } else {
        // 游戏界面
        _PlayerAnimation.draw(_window);
        _window.draw(_backgroundSprite);  // 背景
        _player.draw(_window);           
        
    }

    _window.display();
}