#include "StoryGame.hpp"
#include "ParticleSystem.hpp"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <sstream>

StoryGame::StoryGame() 
    : _window(sf::VideoMode(1200, 800), "Memory Labyrinth", sf::Style::Close)
    , _state(GameState::WakingUp)
    , _steps(0)
    , _memoryPoints(10)
    , _familiarity(0)
    , _currentSceneIndex(0)
    , _rng(std::chrono::steady_clock::now().time_since_epoch().count())
    , _gameRunning(true)
    , _selectedChoice(-1)
    , _waitingForInput(false)
    , _showConsequence(false)
    , _consequenceTimer(0.0f)
    , _bgColor(sf::Color(20, 20, 30))
{
    _window.setFramerateLimit(60);

    // Load font
    _font.loadFromFile("../assets/Roboto-SemiBold.ttf");

    /* =========================
       🎨 TEXT COLOR THEME
       ========================= */

    // Title – vivid memory red
    _titleText.setFont(_font);
    _titleText.setCharacterSize(48);
    _titleText.setFillColor(sf::Color(255, 80, 80));

    // Main story text – cool bright white
    _mainText.setFont(_font);
    _mainText.setCharacterSize(22);
    _mainText.setFillColor(sf::Color(235, 240, 255));
    _mainText.setLineSpacing(1.4f);

        // ===== Base colors (DO NOT CHANGE IN RENDER) =====
    _titleBaseColor       = sf::Color(255, 80, 80);     // Title red
    _mainBaseColor        = sf::Color(235, 240, 255);   // Main text
    _choiceBaseColor      = sf::Color(255, 225, 190);   // Choices
    _statsBaseColor       = sf::Color(120, 220, 255);   // Stats
    _consequenceBaseColor = sf::Color(255, 215, 120);   // Consequence

    _titleText.setFillColor(_titleBaseColor);
    _mainText.setFillColor(_mainBaseColor);
    _statsText.setFillColor(_statsBaseColor);
    _consequenceText.setFillColor(_consequenceBaseColor);

    for (int i = 0; i < 9; ++i) {
        _choiceTexts[i].setFillColor(_choiceBaseColor);
    }

    // Stats – cyber cyan
    _statsText.setFont(_font);
    _statsText.setCharacterSize(20);
    _statsText.setFillColor(sf::Color(120, 220, 255));

    // Choices – warm readable highlight
    for (int i = 0; i < 9; ++i) {
        _choiceTexts[i].setFont(_font);
        _choiceTexts[i].setCharacterSize(22);
        _choiceTexts[i].setFillColor(sf::Color(255, 225, 190));
    }

    // Input – soft white
    _inputText.setFont(_font);
    _inputText.setCharacterSize(24);
    _inputText.setFillColor(sf::Color(245, 245, 255));

    // Consequence / events – memory flash yellow
    _consequenceText.setFont(_font);
    _consequenceText.setCharacterSize(22);
    _consequenceText.setFillColor(sf::Color(255, 215, 120));

    /* ========================= */

    _background.setSize(sf::Vector2f(1200, 800));
    _background.setFillColor(_bgColor);

    _titleBox.setSize(sf::Vector2f(1100, 80));
    _titleBox.setFillColor(sf::Color(30, 30, 45, 200));
    _titleBox.setOutlineColor(sf::Color(120, 180, 255, 150));
    _titleBox.setOutlineThickness(2.0f);
    _titleBox.setPosition(50.0f, 40.0f);

    _statsBox.setSize(sf::Vector2f(1100, 60));
    _statsBox.setFillColor(sf::Color(25, 25, 40, 180));
    _statsBox.setOutlineColor(sf::Color(100, 160, 220, 120));
    _statsBox.setOutlineThickness(1.5f);
    _statsBox.setPosition(50.0f, 140.0f);

    _textBox.setSize(sf::Vector2f(1100, 400));
    _textBox.setFillColor(sf::Color(15, 15, 25, 200));
    _textBox.setOutlineColor(sf::Color(80, 140, 200, 100));
    _textBox.setOutlineThickness(1.0f);

    for (int i = 0; i < 9; ++i) {
        _choiceBoxes[i].setSize(sf::Vector2f(1050, 35));
        _choiceBoxes[i].setFillColor(sf::Color(25, 25, 40, 150));
        _choiceBoxes[i].setOutlineColor(sf::Color(160, 200, 255, 100));
        _choiceBoxes[i].setOutlineThickness(1.0f);
    }

    _ambientParticleInterval = 2.0f;
    _ambientParticleTimer.restart();
    _gameOverParticlesCreated = false;

    _useTextEffects = false;

    initializeGame();
}

void StoryGame::initializeGame() {
    // Initialize story texts
    _streetDescriptions = {
        "A dimly lit street, with flickering streetlights",
        "A narrow alley, walls covered in graffiti",
        "An empty intersection, traffic lights flashing eerily",
        "A familiar path, but you can't remember when you came here",
        "A rundown shopping street, shop windows reflecting your figure",
        "An uphill road, its end disappearing into darkness",
        "A downhill road, you feel you've walked here before",
        "A circular street, you seem to have returned to the starting point",
        "A straight avenue, but the buildings on both sides begin to look familiar",
        "A winding path, every turn makes your heart race"
    };
    
    _memoryLossTexts = {
        "You feel dizzy... something is slipping from your mind",
        "Memories slip through your fingers like grains of sand",
        "You try to recall, but nothing comes to mind",
        "A blurry image flashes in your mind, then disappears",
        "You feel you've lost something important, but don't know what",
        "Fragments of memory scatter before your eyes",
        "You try to grasp a thought, but it's already gone"
    };
    
    _familiarityTexts = {
        "This street... you seem to have seen it somewhere",
        "A strange sense of familiarity washes over you",
        "You begin to realize you've been to this place before",
        "Something stirs in the depths of your memory",
        "You feel you're getting closer to the truth",
        "Every detail of this place makes you uneasy",
        "You realize you're heading toward a place you once fled from"
    };
    
    // Initialize memories
    _memories = {
        {"Your name", 1},
        {"The wall you saw when you woke up", 2},
        {"Why you are here", 3},
        {"Your past", 4},
        {"Your family", 5},
        {"Your friends", 6},
        {"Your work", 7},
        {"Your dreams", 8},
        {"Your fears", 9},
        {"Who you are", 10}
    };
}

void StoryGame::run() {
    sf::Clock clock;
    
    while (_window.isOpen() && _gameRunning) {
        sf::Time dt = clock.restart();
        float deltaTime = dt.asSeconds();
        
        processInput();
        update();
        
        // Update particle system
        _particleSystem.update(deltaTime);
        
        // Update text effects
        if (_useTextEffects) {
            _mainTextEffect.update(deltaTime);
            _titleTextEffect.update(deltaTime);
            _consequenceTextEffect.update(deltaTime);
        }
        
        // Update title glow effect
        _titleGlowIntensity = (std::sin(_glowTimer.getElapsedTime().asSeconds() * 2.0f) + 1.0f) * 0.5f;
        
        // Create ambient floating particles periodically
        if (_ambientParticleTimer.getElapsedTime().asSeconds() >= _ambientParticleInterval) {
            _particleSystem.createFloatingParticles(3);
            _ambientParticleTimer.restart();
            _ambientParticleInterval = 1.5f + (std::rand() % 100) / 100.0f;
        }
        
        if (_showConsequence) {
            _consequenceTimer -= deltaTime;
            if (_consequenceTimer <= 0.0f) {
                _showConsequence = false;
            }
        }
        
        render();
    }
}

void StoryGame::processInput() {
    sf::Event event;
    while (_window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            _window.close();
            _gameRunning = false;
        }
        
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Escape) {
                _window.close();
                _gameRunning = false;
            }
            
            if (_state == GameState::WakingUp) {
                if (event.key.code == sf::Keyboard::Enter) {
                    if (!_currentInput.empty() && _playerName.empty()) {
                        // First Enter: confirm name
                        _playerName = _currentInput;
                        _currentInput.clear();
                    } else if (!_playerName.empty()) {
                        // Second Enter: start game
                        _state = GameState::Exploring;
                        _scenes.push_back(generateRandomScene());
                    }
                } else if (event.key.code == sf::Keyboard::C && !_playerName.empty()) {
                    // C key: start game if name is set
                    _state = GameState::Exploring;
                    _scenes.push_back(generateRandomScene());
                }
            } else if (_state == GameState::Exploring) {
                if (event.key.code >= sf::Keyboard::Num1 && 
                    event.key.code <= sf::Keyboard::Num9) {
                    int choice = event.key.code - sf::Keyboard::Num1 + 1;
                    if (!_scenes.empty() && choice > 0 && 
                        choice <= static_cast<int>(_scenes.back().choices.size())) {
                        Choice& selectedChoice = _scenes.back().choices[choice - 1];
                        
                        // Create particle effect for choice selection
                        float choiceY = 280.0f + (choice - 1) * 35.0f;
                        _particleSystem.createChoiceEffect(
                            sf::Vector2f(100.0f, choiceY), 
                            sf::Color(100, 200, 255), 
                            15
                        );
                        
                        // Display consequence
                        _consequenceText.setString(selectedChoice.consequence);
                        _showConsequence = true;
                        _consequenceTimer = 3.0f;
                        
                        // Consume memory
                        if (selectedChoice.memoryCost > 0) {
                            loseMemory(selectedChoice.memoryCost);
                        }
                        
                        // Increase steps
                        _steps++;
                        
                        // Lose memory each step
                        loseMemory(1);
                        
                        // Increase familiarity
                        increaseFamiliarity();
                        
                        // Generate new scene
                        _scenes.push_back(generateRandomScene());
                        
                        // Random event
                        std::uniform_int_distribution<int> eventDist(0, 2);
                        if (eventDist(_rng) == 0) {
                            triggerRandomEvent();
                        }
                    }
                } else if (event.key.code == sf::Keyboard::Q) {
                    _gameRunning = false;
                }
            } else if (_state == GameState::GameOver) {
                if (event.key.code == sf::Keyboard::Enter || 
                    event.key.code == sf::Keyboard::Escape) {
                    _gameRunning = false;
                }
            }
        }
        
        if (event.type == sf::Event::TextEntered && _state == GameState::WakingUp && _playerName.empty()) {
            if (event.text.unicode < 128) {
                if (event.text.unicode == '\b' && !_currentInput.empty()) {
                    _currentInput.pop_back();
                } else if (event.text.unicode != '\r' && event.text.unicode != '\n') {
                    _currentInput += static_cast<char>(event.text.unicode);
                }
            }
        }
    }
}

void StoryGame::update() {
    // Check game over condition
    if (_memoryPoints <= 0 && _state == GameState::Exploring) {
        _state = GameState::GameOver;
        // Create dramatic particle effect when game ends (one time)
        if (!_gameOverParticlesCreated) {
            for (int i = 0; i < 10; ++i) {
                _particleSystem.createMemoryLossEffect(
                    sf::Vector2f(200.0f + i * 80.0f, 300.0f), 
                    25
                );
            }
            _gameOverParticlesCreated = true;
        }
    }
    
    // If familiarity reaches a certain level, trigger special story
    if (_familiarity >= 50 && _state == GameState::Exploring) {
        // Can add special story here
    }
}

void StoryGame::render() {
    // ===== Background =====
    float bgPulse = (std::sin(_glowTimer.getElapsedTime().asSeconds() * 0.5f) + 1.0f) * 0.5f;
    sf::Color bgColor = _bgColor;
    bgColor.r = static_cast<sf::Uint8>(20 + bgPulse * 5);
    bgColor.g = static_cast<sf::Uint8>(20 + bgPulse * 5);
    bgColor.b = static_cast<sf::Uint8>(30 + bgPulse * 5);

    _window.clear(bgColor);
    _background.setFillColor(bgColor);
    _window.draw(_background);
    _particleSystem.draw(_window);

    float yPos = 50.0f;

    // ===== Title Box =====
    // Use base colors, not current colors
    sf::Color titleBoxBaseColor(30, 30, 45, 200);
    titleBoxBaseColor.a = static_cast<sf::Uint8>(200 + _titleGlowIntensity * 55);
    _titleBox.setFillColor(titleBoxBaseColor);

    sf::Color titleOutlineBaseColor(120, 180, 255, 150);
    titleOutlineBaseColor.a = static_cast<sf::Uint8>(150 + _titleGlowIntensity * 105);
    _titleBox.setOutlineColor(titleOutlineBaseColor);

    _window.draw(_titleBox);

    // ===== Title Text (STABLE COLOR) =====
    _titleText.setString("                  Memory Labyrinth");
    _titleText.setPosition(70.0f, yPos + 15.0f);

    {
        sf::Color base = _titleBaseColor;
        float glow = 0.7f + _titleGlowIntensity * 0.3f;
        sf::Color drawColor(
            static_cast<sf::Uint8>(base.r * glow),
            static_cast<sf::Uint8>(base.g * glow),
            static_cast<sf::Uint8>(base.b * glow),
            base.a
        );
        _titleText.setFillColor(drawColor);
        _window.draw(_titleText);
    }

    yPos += 90.0f;

    // ===== Stats =====
    displayStats();
    float statsHeight = std::max(50.0f, std::min(100.0f, _statsText.getGlobalBounds().height + 20.0f));
    _statsBox.setSize({1100, statsHeight});
    _window.draw(_statsBox);

    _statsText.setFillColor(_statsBaseColor);
    _statsText.setPosition(70.0f, yPos + 10.0f);
    _window.draw(_statsText);
    yPos += statsHeight + 5.0f;

    // ===== Waking Up =====
    if (_state == GameState::WakingUp) {
        std::string wakeText =
            "You slowly open your eyes...\n\n"
            "The cold ground presses against your cheek.\n\n"
            "Please enter your name:\n\n";

        wakeText += _playerName.empty()
            ? "> " + _currentInput + "_"
            : "Your name appears on the wall: " + _playerName + "\n\nPress ENTER to begin...\n";

        _mainText.setString(wakeText);
        float textHeight = _mainText.getLocalBounds().height + 50.0f;
        float textBoxHeight = std::max(350.0f, textHeight);

        // Use base color, not current color (avoid color accumulation)
        sf::Color boxBaseColor(15, 15, 25, 220);
        _textBox.setFillColor(boxBaseColor);
        _textBox.setSize({1100, textBoxHeight});
        _textBox.setPosition(50.0f, yPos);
        _window.draw(_textBox);

        _mainText.setFillColor(_mainBaseColor);
        _mainText.setPosition(70.0f, yPos + 20.0f);
        _window.draw(_mainText);
    }

    // ===== Exploring =====
    else if (_state == GameState::Exploring && !_scenes.empty()) {
        Scene& scene = _scenes.back();
        float sceneY = yPos;

        std::string sceneText = scene.description + "\n\n";
        _mainText.setString(sceneText);

        float sceneTextHeight = _mainText.getGlobalBounds().height;
        _textBox.setSize({1100, sceneTextHeight + 40.0f});
        _textBox.setPosition(50.0f, sceneY);
        _window.draw(_textBox);

        _mainText.setFillColor(_mainBaseColor);
        _mainText.setPosition(70.0f, sceneY + 20.0f);
        _window.draw(_mainText);

        sceneY += sceneTextHeight + 60.0f;

        // ===== Choices =====
        for (size_t i = 0; i < scene.choices.size(); ++i) {
            _choiceTexts[i].setString(
                "[" + std::to_string(i + 1) + "] " + scene.choices[i].text
            );

            float pulse = (std::sin(_glowTimer.getElapsedTime().asSeconds() * 2.0f + i) + 1.0f) * 0.5f;
            sf::Color base = _choiceBaseColor;
            float glow = 0.9f + pulse * 0.1f;

            sf::Color drawColor(
                static_cast<sf::Uint8>(base.r * glow),
                static_cast<sf::Uint8>(base.g * glow),
                static_cast<sf::Uint8>(base.b * glow),
                base.a
            );

            _choiceTexts[i].setFillColor(drawColor);
            _choiceTexts[i].setPosition(85.0f, sceneY);
            _window.draw(_choiceTexts[i]);

            sceneY += 38.0f;
        }

        // ===== Consequence =====
        if (_showConsequence) {
            sf::Color base = _consequenceBaseColor;
            float pulse = (std::sin(_glowTimer.getElapsedTime().asSeconds() * 4.0f) + 1.0f) * 0.5f;
            float glow = 0.8f + pulse * 0.2f;

            sf::Color drawColor(
                static_cast<sf::Uint8>(base.r * glow),
                static_cast<sf::Uint8>(base.g * glow),
                static_cast<sf::Uint8>(base.b * glow),
                static_cast<sf::Uint8>(std::min(255.0f, (_consequenceTimer / 3.0f) * 255.0f))
            );

            _consequenceText.setFillColor(drawColor);
            _consequenceText.setPosition(70.0f, sceneY + 10.0f);
            _window.draw(_consequenceText);
        }
    }

    else if (_state == GameState::GameOver) {
        // Create dramatic background effect
        float gameOverPulse = (std::sin(_glowTimer.getElapsedTime().asSeconds() * 1.5f) + 1.0f) * 0.5f;
        sf::Color gameOverBg(30, 10, 10);
        gameOverBg.r = static_cast<sf::Uint8>(30 + gameOverPulse * 20);
        gameOverBg.g = static_cast<sf::Uint8>(10 + gameOverPulse * 10);
        gameOverBg.b = static_cast<sf::Uint8>(10 + gameOverPulse * 10);
        _window.clear(gameOverBg);
        _background.setFillColor(gameOverBg);
        _window.draw(_background);
        
        // Draw particles (memory loss effect)
        _particleSystem.draw(_window);
        
        // Main game over box with animated glow
        float boxGlow = (std::sin(_glowTimer.getElapsedTime().asSeconds() * 2.0f) + 1.0f) * 0.5f;
        sf::Color boxColor(50, 15, 15);
        boxColor.a = static_cast<sf::Uint8>(220 + boxGlow * 35);
        _textBox.setFillColor(boxColor);
        
        sf::Color outlineColor(220, 60, 60);
        outlineColor.a = static_cast<sf::Uint8>(180 + boxGlow * 75);
        _textBox.setOutlineColor(outlineColor);
        _textBox.setOutlineThickness(4.0f + boxGlow * 2.0f);
        _textBox.setSize(sf::Vector2f(1100, 600));
        _textBox.setPosition(50.0f, 100.0f);
        _window.draw(_textBox);
        
        float gameOverY = 120.0f;
        
        // Game Over Title with dramatic effect
        float titlePulse = (std::sin(_glowTimer.getElapsedTime().asSeconds() * 3.0f) + 1.0f) * 0.5f;
        sf::Text gameOverTitle;
        gameOverTitle.setFont(_font);
        gameOverTitle.setString(
                               "                         GAME OVER\n"
                               );
        gameOverTitle.setCharacterSize(42);
        gameOverTitle.setStyle(sf::Text::Bold);
        sf::Color titleColor(255, 100, 100);
        titleColor.r = static_cast<sf::Uint8>(200 + titlePulse * 55);
        titleColor.g = static_cast<sf::Uint8>(80 + titlePulse * 20);
        titleColor.b = static_cast<sf::Uint8>(80 + titlePulse * 20);
        gameOverTitle.setFillColor(titleColor);
        gameOverTitle.setPosition(70.0f, gameOverY);
        _window.draw(gameOverTitle);
        gameOverY += 120.0f;
        
        // Main narrative text with fade effect
        std::string narrativeText = 
            "All your memories have faded away...\n\n"
            "You stand in the center of the street,\n"
            "not knowing who you are,\n"
            "not knowing where to go.\n\n"
            "But this street...\n"
            "You remember it.\n"
            "You've been here before...\n";
        
        _mainText.setString(narrativeText);
        _mainText.setCharacterSize(24);
        sf::Color narrativeColor(240, 200, 200);
        float narrativeGlow = 0.85f + (std::sin(_glowTimer.getElapsedTime().asSeconds() * 1.2f) + 1.0f) * 0.15f;
        narrativeColor.r = static_cast<sf::Uint8>(narrativeColor.r * narrativeGlow);
        narrativeColor.g = static_cast<sf::Uint8>(narrativeColor.g * narrativeGlow);
        narrativeColor.b = static_cast<sf::Uint8>(narrativeColor.b * narrativeGlow);
        _mainText.setFillColor(narrativeColor);
        _mainText.setPosition(70.0f, gameOverY);
        _window.draw(_mainText);
        gameOverY += 220.0f;
        
        // Statistics box with glow
        sf::RectangleShape statsBox;
        statsBox.setSize(sf::Vector2f(1050, 120));
        float statsGlow = (std::sin(_glowTimer.getElapsedTime().asSeconds() * 1.8f) + 1.0f) * 0.5f;
        sf::Color statsBoxColor(40, 25, 25);
        statsBoxColor.a = static_cast<sf::Uint8>(180 + statsGlow * 40);
        statsBox.setFillColor(statsBoxColor);
        sf::Color statsOutlineColor(150, 100, 100);
        statsOutlineColor.a = static_cast<sf::Uint8>(120 + statsGlow * 60);
        statsBox.setOutlineColor(statsOutlineColor);
        statsBox.setOutlineThickness(2.0f);
        statsBox.setPosition(70.0f, gameOverY);
        _window.draw(statsBox);
        
        // Statistics text
        std::string statsText = 
            "                                        Final Statistics:\n"
            "                                            Steps Taken: " + std::to_string(_steps) + "\n"
            "                                            Street Familiarity: " + std::to_string(_familiarity) + "%\n"
            "                                            Memories Lost: " + std::to_string(_lostMemories.size());
        
        sf::Text statsDisplay;
        statsDisplay.setFont(_font);
        statsDisplay.setString(statsText);
        statsDisplay.setCharacterSize(22);
        statsDisplay.setFillColor(sf::Color(255, 220, 180));
        statsDisplay.setPosition(90.0f, gameOverY + 15.0f);
        _window.draw(statsDisplay);
        gameOverY += 140.0f;
        
        // Exit prompt with blinking effect
        float blinkSpeed = 2.5f;
        float blink = (std::sin(_glowTimer.getElapsedTime().asSeconds() * blinkSpeed) + 1.0f) * 0.5f;
        sf::Text exitPrompt;
        exitPrompt.setFont(_font);
        exitPrompt.setString(">>> Press ENTER or ESC to exit <<<");
        exitPrompt.setCharacterSize(20);
        exitPrompt.setStyle(sf::Text::Bold);
        sf::Color promptColor(255, 180, 120);
        promptColor.a = static_cast<sf::Uint8>(150 + blink * 105);
        exitPrompt.setFillColor(promptColor);
        exitPrompt.setPosition(70.0f, gameOverY);
        _window.draw(exitPrompt);
        
        // Add corner decorations (simple lines)
        float cornerGlow = (std::sin(_glowTimer.getElapsedTime().asSeconds() * 1.0f) + 1.0f) * 0.5f;
        sf::Color cornerColor(200, 80, 80);
        cornerColor.a = static_cast<sf::Uint8>(100 + cornerGlow * 80);
        
        // Top-left corner
        sf::RectangleShape corner1(sf::Vector2f(80.0f, 2.0f));
        corner1.setFillColor(cornerColor);
        corner1.setPosition(50.0f, 100.0f);
        corner1.setRotation(0.0f);
        _window.draw(corner1);
        
        sf::RectangleShape corner2(sf::Vector2f(80.0f, 2.0f));
        corner2.setFillColor(cornerColor);
        corner2.setPosition(50.0f, 100.0f);
        corner2.setRotation(90.0f);
        _window.draw(corner2);
        
        // Top-right corner
        sf::RectangleShape corner3(sf::Vector2f(80.0f, 2.0f));
        corner3.setFillColor(cornerColor);
        corner3.setPosition(1150.0f, 100.0f);
        corner3.setRotation(0.0f);
        _window.draw(corner3);
        
        sf::RectangleShape corner4(sf::Vector2f(80.0f, 2.0f));
        corner4.setFillColor(cornerColor);
        corner4.setPosition(1150.0f, 100.0f);
        corner4.setRotation(90.0f);
        _window.draw(corner4);
        
        // Bottom-left corner
        sf::RectangleShape corner5(sf::Vector2f(80.0f, 2.0f));
        corner5.setFillColor(cornerColor);
        corner5.setPosition(50.0f, 700.0f);
        corner5.setRotation(0.0f);
        _window.draw(corner5);
        
        sf::RectangleShape corner6(sf::Vector2f(80.0f, 2.0f));
        corner6.setFillColor(cornerColor);
        corner6.setPosition(50.0f, 700.0f);
        corner6.setRotation(90.0f);
        _window.draw(corner6);
        
        // Bottom-right corner
        sf::RectangleShape corner7(sf::Vector2f(80.0f, 2.0f));
        corner7.setFillColor(cornerColor);
        corner7.setPosition(1150.0f, 700.0f);
        corner7.setRotation(0.0f);
        _window.draw(corner7);
        
        sf::RectangleShape corner8(sf::Vector2f(80.0f, 2.0f));
        corner8.setFillColor(cornerColor);
        corner8.setPosition(1150.0f, 700.0f);
        corner8.setRotation(90.0f);
        _window.draw(corner8);
    }

    _window.display();
}

void StoryGame::displayScene() {
    // This method is now handled directly in render()
}

void StoryGame::displayStats() {
    std::string stats = "Steps: " + std::to_string(_steps) + "  |  ";
    stats += "Memory: " + std::to_string(_memoryPoints) + "/10  |  ";
    stats += "Familiarity: " + std::to_string(_familiarity) + "%";
    
    if (!_memories.empty()) {
        stats += "\nRemaining Memories: ";
        for (size_t i = 0; i < _memories.size() && i < 4; ++i) {
            stats += _memories[i].description;
            if (i < _memories.size() - 1 && i < 3) {
                stats += ", ";
            }
        }
        if (_memories.size() > 4) {
            stats += " ... (" + std::to_string(_memories.size() - 4) + " more)";
        }
    }
    
    _statsText.setString(stats);
    _statsText.setLineSpacing(1.2f);
}

void StoryGame::loseMemory(int amount) {
    _memoryPoints = std::max(0, _memoryPoints - amount);
    
    // Create particle effect for memory loss
    _particleSystem.createMemoryLossEffect(sf::Vector2f(600.0f, 200.0f), 25);
    
    if (_memoryPoints <= 0 && !_memories.empty()) {
        // Lose the last memory
        _lostMemories.push_back(_memories.back());
        _memories.pop_back();
        _particleSystem.createMemoryLossEffect(sf::Vector2f(600.0f, 300.0f), 40);
    } else if (!_memories.empty()) {
        std::uniform_int_distribution<int> loseDist(0, 2);
        if (loseDist(_rng) == 0) {
            // Randomly lose a memory
            std::uniform_int_distribution<size_t> dist(0, _memories.size() - 1);
            size_t index = dist(_rng);
            _lostMemories.push_back(_memories[index]);
            _memories.erase(_memories.begin() + index);
        }
    }
}

void StoryGame::gainMemory(const Memory& memory) {
    // Check if it already exists
    bool exists = false;
    for (const auto& m : _memories) {
        if (m.description == memory.description) {
            exists = true;
            break;
        }
    }
    
    if (!exists) {
        _memories.push_back(memory);
        _memoryPoints = std::min(10, _memoryPoints + 1);
        // Create sparkle effect for gaining memory
        _particleSystem.createSparkle(sf::Vector2f(600.0f, 250.0f), sf::Color(255, 200, 100));
    }
}

void StoryGame::increaseFamiliarity() {
    std::uniform_int_distribution<int> incDist(1, 3);
    _familiarity = std::min(100, _familiarity + incDist(_rng));
}

void StoryGame::triggerRandomEvent() {
    std::vector<std::string> events = {
        "You hear footsteps in the distance, but when you turn around, there's nothing.",
        "A cold wind blows past, you feel someone watching you.",
        "You see new writing appear on the wall, but when you approach, it disappears.",
        "You feel someone calling your name, but the voice comes from all directions.",
        "You see your shadow moving on the wall, but you haven't moved.",
        "You hear someone crying, but can't find the source of the sound.",
        "You feel this street changing, but can't say what's different."
    };
    
    std::uniform_int_distribution<size_t> dist(0, events.size() - 1);
    std::string eventText = "[Event] " + events[dist(_rng)];
    _consequenceText.setString(eventText);
    _showConsequence = true;
    _consequenceTimer = 3.0f;
}

Scene StoryGame::generateRandomScene() {
    Scene scene;
    
    // Generate street description
    std::uniform_int_distribution<size_t> descDist(0, _streetDescriptions.size() - 1);
    scene.description = _streetDescriptions[descDist(_rng)];
    
    // Adjust description based on familiarity
    if (_familiarity > 30) {
        scene.description += " This street feels unsettlingly familiar.";
    }
    if (_familiarity > 60) {
        scene.description += " You begin to remember some details...";
    }
    
    // Generate choices
    std::vector<std::pair<std::string, std::string>> choiceTemplates = {
        {"Continue forward", "You take a step, the path beneath your feet seems more familiar"},
        {"Turn left", "You turn left, a strange feeling washes over you"},
        {"Turn right", "You turn right, you feel you've walked this path before"},
        {"Stop and observe", "You stop and carefully observe your surroundings"},
        {"Check the wall", "You approach the wall and find some blurry writing on it"},
        {"Look back", "You look back, but the path you came from has become unfamiliar"},
        {"Quickly walk", "You quicken your pace, wanting to escape this place"},
        {"Walk slowly", "You slow down, trying to remember every detail"}
    };
    
    std::uniform_int_distribution<int> choiceNumDist(2, 4);
    int numChoices = choiceNumDist(_rng);  // 2-4 choices
    std::shuffle(choiceTemplates.begin(), choiceTemplates.end(), _rng);
    
    for (int i = 0; i < numChoices && i < static_cast<int>(choiceTemplates.size()); ++i) {
        Choice choice;
        choice.text = choiceTemplates[i].first;
        choice.consequence = choiceTemplates[i].second;
        std::uniform_int_distribution<int> costDist(0, 2);
        choice.memoryCost = (costDist(_rng) == 0) ? 1 : 0;  // 30% chance to consume memory
        scene.choices.push_back(choice);
    }
    
    // Randomly trigger memory
    std::uniform_int_distribution<int> memoryDist(0, 3);
    scene.hasMemory = (memoryDist(_rng) == 0);  // 25% chance
    scene.memoryGained = false;
    if (scene.hasMemory && !_lostMemories.empty()) {
        std::uniform_int_distribution<size_t> memDist(0, _lostMemories.size() - 1);
        scene.memory = _lostMemories[memDist(_rng)];
    }
    
    return scene;
}
