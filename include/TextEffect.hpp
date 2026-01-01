#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

class TextEffect {
public:
    enum class EffectType {
        None,
        Typewriter,      // 打字机效果
        FadeIn,          // 淡入效果
        Glow,            // 发光效果
        Shake,           // 震动效果
        Wave             // 波浪效果
    };
    
    TextEffect();
    
    // Set text and effect
    void setText(const std::string& text, EffectType type = EffectType::Typewriter);
    void setFont(const sf::Font& font);
    void setCharacterSize(unsigned int size);
    void setFillColor(const sf::Color& color);
    void setPosition(const sf::Vector2f& position);
    
    // Update and render
    void update(float deltaTime);
    void draw(sf::RenderWindow& window);
    
    // Control
    void start();
    void reset();
    bool isComplete() const;
    
    // Settings
    void setTypewriterSpeed(float charsPerSecond);
    void setFadeSpeed(float fadeSpeed);
    void setGlowIntensity(float intensity);
    void setShakeIntensity(float intensity);
    
    // Get current displayed text
    std::string getCurrentText() const;
    sf::FloatRect getGlobalBounds() const;
    sf::Color getFillColor() const;

private:
    std::string _fullText;
    std::string _displayText;
    EffectType _effectType;
    
    // Text properties
    sf::Text _text;
    sf::Font _font;
    sf::Color _baseColor;  // Base color to avoid accumulation
    
    // Animation state
    float _timer;
    int _currentCharIndex;
    bool _isActive;
    bool _isComplete;
    
    // Effect parameters
    float _typewriterSpeed;  // characters per second
    float _fadeSpeed;
    float _glowIntensity;
    float _shakeIntensity;
    
    // Shake effect
    sf::Vector2f _basePosition;
    sf::Vector2f _shakeOffset;
    
    // Helper functions
    void updateTypewriter(float deltaTime);
    void updateFadeIn(float deltaTime);
    void updateGlow(float deltaTime);
    void updateShake(float deltaTime);
    void updateWave(float deltaTime);
};

