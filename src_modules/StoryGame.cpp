#include "StoryGame.hpp"
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
    , _memoryPoints(10)  // Initial memory points
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
    
    // Load font - try to load font from assets directory
    bool fontLoaded = false;
    
    // Try to load font from assets directory
    const char* fontPaths[] = {
        "../assets/Roboto-SemiBold.ttf"           // Roboto font
    };
    
    for (const char* path : fontPaths) {
        if (_font.loadFromFile(path)) {
            fontLoaded = true;
            std::cout << "Font loaded: " << path << std::endl;
            break;
        }
    }
    
    if (!fontLoaded) {
        std::cerr << "Warning: Failed to load font.\n";
    }
    
    // Initialize text objects
    _titleText.setFont(_font);
    _titleText.setCharacterSize(48);
    _titleText.setFillColor(sf::Color(200, 50, 50));
    
    _mainText.setFont(_font);
    _mainText.setCharacterSize(24);
    _mainText.setFillColor(sf::Color(220, 220, 220));
    _mainText.setLineSpacing(1.5f);
    
    _statsText.setFont(_font);
    _statsText.setCharacterSize(20);
    _statsText.setFillColor(sf::Color(100, 200, 255));
    
    for (int i = 0; i < 9; ++i) {
        _choiceTexts[i].setFont(_font);
        _choiceTexts[i].setCharacterSize(22);
        _choiceTexts[i].setFillColor(sf::Color(200, 180, 150));
    }
    
    _inputText.setFont(_font);
    _inputText.setCharacterSize(24);
    _inputText.setFillColor(sf::Color(255, 255, 255));
    
    _consequenceText.setFont(_font);
    _consequenceText.setCharacterSize(22);
    _consequenceText.setFillColor(sf::Color(255, 200, 100));
    
    // Initialize background
    _background.setSize(sf::Vector2f(1200, 800));
    _background.setFillColor(_bgColor);
    
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
                if (event.key.code == sf::Keyboard::Enter && !_currentInput.empty()) {
                    _playerName = _currentInput;
                    _currentInput.clear();
                } else if (event.key.code == sf::Keyboard::C || 
                          (event.key.code == sf::Keyboard::Enter && _playerName.empty() == false)) {
                    if (!_playerName.empty()) {
                        _state = GameState::Exploring;
                        _scenes.push_back(generateRandomScene());
                    }
                }
            } else if (_state == GameState::Exploring) {
                if (event.key.code >= sf::Keyboard::Num1 && 
                    event.key.code <= sf::Keyboard::Num9) {
                    int choice = event.key.code - sf::Keyboard::Num1 + 1;
                    if (!_scenes.empty() && choice > 0 && 
                        choice <= static_cast<int>(_scenes.back().choices.size())) {
                        Choice& selectedChoice = _scenes.back().choices[choice - 1];
                        
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
    }
    
    // If familiarity reaches a certain level, trigger special story
    if (_familiarity >= 50 && _state == GameState::Exploring) {
        // Can add special story here
    }
}

void StoryGame::render() {
    _window.clear(_bgColor);
    
    // Draw background
    _window.draw(_background);
    
    float yPos = 50.0f;
    
    // Display title
    _titleText.setString(
                        "                  Memory Labyrinth\n");
    _titleText.setPosition(50.0f, yPos);
    _window.draw(_titleText);
    yPos += 100.0f;
    
    // Display statistics
    displayStats();
    _statsText.setPosition(50.0f, yPos);
    _window.draw(_statsText);
    yPos += 60.0f;
    
    if (_state == GameState::WakingUp) {
        std::string wakeText = "You slowly open your eyes...\n\n";
        wakeText += "The cold ground presses against your cheek. You slowly sit up and look around.\n\n";
        wakeText += "This is an unfamiliar street, but the air is filled with an indescribable sense of familiarity.\n\n";
        wakeText += "You look at the wall beside you, written in red paint:\n";
        wakeText += "\"Please enter your name:\"\n\n";
        
        if (!_playerName.empty()) {
            wakeText += "Your name appears on the wall: " + _playerName + "\n\n";
            wakeText += "You feel uneasy. Every time you try to think, something disappears from your mind.\n\n";
            wakeText += "You must move forward, but each step will make you lose more memories...\n";
            wakeText += "Yet this street becomes increasingly familiar.\n\n";
            wakeText += "Press C or Enter to begin your journey...\n";
        } else {
            wakeText += "> " + _currentInput + "_";
        }
        
        _mainText.setString(wakeText);
        _mainText.setPosition(50.0f, yPos);
        _window.draw(_mainText);
    } else if (_state == GameState::Exploring) {
        displayScene();
        float sceneY = yPos;
        
        if (!_scenes.empty()) {
            Scene& scene = _scenes.back();
            
            std::string sceneText = scene.description + "\n\n";
            
            if (scene.hasMemory && !scene.memory.description.empty()) {
                sceneText += "[Memory Flash] " + scene.memory.description + "\n\n";
                if (!scene.memoryGained) {
                    gainMemory(scene.memory);
                    scene.memoryGained = true;
                }
            }
            
            _mainText.setString(sceneText);
            _mainText.setPosition(50.0f, sceneY);
            _window.draw(_mainText);
            sceneY += _mainText.getGlobalBounds().height + 30.0f;
            
            // Display choices
            std::string choicesText = "Your choices:\n\n";
            _mainText.setString(choicesText);
            _mainText.setPosition(50.0f, sceneY);
            _window.draw(_mainText);
            sceneY += 40.0f;
            
            for (size_t i = 0; i < scene.choices.size(); ++i) {
                std::string choiceStr = "  [" + std::to_string(i + 1) + "] " + scene.choices[i].text;
                if (scene.choices[i].memoryCost > 0) {
                    choiceStr += " (-" + std::to_string(scene.choices[i].memoryCost) + " memory)";
                }
                _choiceTexts[i].setString(choiceStr);
                _choiceTexts[i].setPosition(70.0f, sceneY);
                _window.draw(_choiceTexts[i]);
                sceneY += 35.0f;
            }
        }
        
        // Display consequence
        if (_showConsequence) {
            _consequenceText.setPosition(50.0f, sceneY + 20.0f);
            _window.draw(_consequenceText);
        }
    } else if (_state == GameState::GameOver) {
        std::string gameOverText = "═══════════════════════════════════════════════════════\n";
        gameOverText += "                      Game Over\n";
        gameOverText += "═══════════════════════════════════════════════════════\n\n";
        gameOverText += "All your memories have faded away.\n\n";
        gameOverText += "You stand in the center of the street, not knowing who you are, not knowing where to go.\n\n";
        gameOverText += "But this street... you remember it.\n";
        gameOverText += "You've been here before...\n\n";
        gameOverText += "Steps: " + std::to_string(_steps) + "\n";
        gameOverText += "Street Familiarity: " + std::to_string(_familiarity) + "%\n\n";
        gameOverText += "Press Enter or ESC to exit...\n";
        
        _mainText.setString(gameOverText);
        _mainText.setPosition(50.0f, yPos);
        _window.draw(_mainText);
    }
    
    _window.display();
}

void StoryGame::displayScene() {
    // This method is now handled directly in render()
}

void StoryGame::displayStats() {
    std::string stats = "Steps: " + std::to_string(_steps) + "  |  ";
    stats += "Memory: " + std::to_string(_memoryPoints) + "/10  |  ";
    stats += "Familiarity: " + std::to_string(_familiarity) + "%\n\n";
    
    if (!_memories.empty()) {
        stats += "Remaining Memories: ";
        for (size_t i = 0; i < _memories.size() && i < 5; ++i) {
            stats += _memories[i].description;
            if (i < _memories.size() - 1 && i < 4) {
                stats += ", ";
            }
        }
        if (_memories.size() > 5) {
            stats += "...";
        }
    }
    
    _statsText.setString(stats);
}

void StoryGame::loseMemory(int amount) {
    _memoryPoints = std::max(0, _memoryPoints - amount);
    
    if (_memoryPoints <= 0 && !_memories.empty()) {
        // Lose the last memory
        _lostMemories.push_back(_memories.back());
        _memories.pop_back();
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
