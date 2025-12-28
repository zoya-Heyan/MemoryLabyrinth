#pragma once
#include <string>
#include <vector>
#include <map>
#include <random>
#include <memory>
#include <SFML/Graphics.hpp>

// 记忆片段
struct Memory {
    std::string description;
    int importance;  // 重要性，决定消失顺序
};

// 游戏状态
enum class GameState {
    WakingUp,      // 醒来
    Exploring,     // 探索中
    MemoryLoss,    // 记忆消失
    GameOver,      // 游戏结束
    Victory        // 胜利（如果实现）
};

// 选择项
struct Choice {
    std::string text;
    std::string consequence;
    int memoryCost;  // 消耗的记忆点数
};

// 场景
struct Scene {
    std::string description;
    std::vector<Choice> choices;
    bool hasMemory;  // 是否触发记忆
    bool memoryGained;  // 记忆是否已被获取
    Memory memory;
};

class StoryGame {
public:
    StoryGame();
    void run();
    
private:
    void initializeGame();
    void processInput();
    void update();
    void render();
    void displayScene();
    void displayMemoryLoss();
    void displayStats();
    
    // 游戏机制
    void loseMemory(int amount = 1);
    void gainMemory(const Memory& memory);
    void increaseFamiliarity();
    void triggerRandomEvent();
    
    // 场景生成
    Scene generateRandomScene();
    std::string generateStreetDescription();
    
    // 游戏状态
    GameState _state;
    std::string _playerName;
    int _steps;              // 步数
    int _memoryPoints;       // 记忆点数
    int _familiarity;        // 街道熟悉度
    std::vector<Memory> _memories;  // 拥有的记忆
    std::vector<Memory> _lostMemories;  // 已失去的记忆
    
    // 肉鸽元素
    std::vector<Scene> _scenes;
    int _currentSceneIndex;
    std::mt19937 _rng;
    
    // 剧情文本
    std::vector<std::string> _streetDescriptions;
    std::vector<std::string> _memoryLossTexts;
    std::vector<std::string> _familiarityTexts;
    
    bool _gameRunning;
    
    // SFML 窗口和渲染
    sf::RenderWindow _window;
    sf::Font _font;
    sf::Text _titleText;
    sf::Text _mainText;
    sf::Text _statsText;
    sf::Text _choiceTexts[9];  // 最多9个选择
    sf::Text _inputText;
    sf::Text _consequenceText;
    std::string _currentInput;
    int _selectedChoice;
    bool _waitingForInput;
    bool _showConsequence;
    float _consequenceTimer;
    
    // 背景
    sf::RectangleShape _background;
    sf::Color _bgColor;
};

