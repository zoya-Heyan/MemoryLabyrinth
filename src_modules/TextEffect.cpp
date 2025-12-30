#include "TextEffect.hpp"
#include <cmath>
#include <algorithm>

TextEffect::TextEffect()
    : _effectType(EffectType::None)
    , _timer(0.0f)
    , _currentCharIndex(0)
    , _isActive(false)
    , _isComplete(false)
    , _typewriterSpeed(30.0f)  // 30 characters per second
    , _fadeSpeed(2.0f)
    , _glowIntensity(1.0f)
    , _shakeIntensity(0.0f)
    , _shakeOffset(0.0f, 0.0f)
{
}

void TextEffect::setText(const std::string& text, EffectType type) {
    _fullText = text;
    _displayText = "";
    _effectType = type;
    _currentCharIndex = 0;
    _timer = 0.0f;
    _isComplete = false;
    
    if (type == EffectType::None || type == EffectType::FadeIn) {
        _displayText = text;
    }
}

void TextEffect::setFont(const sf::Font& font) {
    _font = font;
    _text.setFont(_font);
}

void TextEffect::setCharacterSize(unsigned int size) {
    _text.setCharacterSize(size);
}

void TextEffect::setFillColor(const sf::Color& color) {
    _text.setFillColor(color);
}

void TextEffect::setPosition(const sf::Vector2f& position) {
    _basePosition = position;
    _text.setPosition(position);
}

void TextEffect::update(float deltaTime) {
    if (!_isActive || _isComplete) return;
    
    _timer += deltaTime;
    
    switch (_effectType) {
        case EffectType::Typewriter:
            updateTypewriter(deltaTime);
            break;
        case EffectType::FadeIn:
            updateFadeIn(deltaTime);
            break;
        case EffectType::Glow:
            updateGlow(deltaTime);
            break;
        case EffectType::Shake:
            updateShake(deltaTime);
            break;
        case EffectType::Wave:
            updateWave(deltaTime);
            break;
        default:
            _isComplete = true;
            break;
    }
}

void TextEffect::draw(sf::RenderWindow& window) {
    if (_displayText.empty()) return;
    
    // Apply shake offset if active
    if (_effectType == EffectType::Shake && _shakeIntensity > 0.0f) {
        _text.setPosition(_basePosition + _shakeOffset);
    } else {
        _text.setPosition(_basePosition);
    }
    
    window.draw(_text);
}

void TextEffect::start() {
    _isActive = true;
    _isComplete = false;
    _timer = 0.0f;
    _currentCharIndex = 0;
    
    if (_effectType == EffectType::Typewriter) {
        _displayText = "";
    } else if (_effectType == EffectType::FadeIn) {
        _displayText = _fullText;
        sf::Color color = _text.getFillColor();
        color.a = 0;
        _text.setFillColor(color);
    }
}

void TextEffect::reset() {
    _isActive = false;
    _isComplete = false;
    _timer = 0.0f;
    _currentCharIndex = 0;
    _displayText = "";
    _shakeOffset = sf::Vector2f(0.0f, 0.0f);
}

bool TextEffect::isComplete() const {
    return _isComplete;
}

void TextEffect::setTypewriterSpeed(float charsPerSecond) {
    _typewriterSpeed = charsPerSecond;
}

void TextEffect::setFadeSpeed(float fadeSpeed) {
    _fadeSpeed = fadeSpeed;
}

void TextEffect::setGlowIntensity(float intensity) {
    _glowIntensity = intensity;
}

void TextEffect::setShakeIntensity(float intensity) {
    _shakeIntensity = intensity;
}

std::string TextEffect::getCurrentText() const {
    return _displayText;
}

sf::FloatRect TextEffect::getGlobalBounds() const {
    return _text.getGlobalBounds();
}

sf::Color TextEffect::getFillColor() const {
    return _text.getFillColor();
}

void TextEffect::updateTypewriter(float deltaTime) {
    float charsToAdd = _typewriterSpeed * deltaTime;
    int totalChars = static_cast<int>(_fullText.length());
    
    while (charsToAdd >= 1.0f && _currentCharIndex < totalChars) {
        _displayText += _fullText[_currentCharIndex];
        _currentCharIndex++;
        charsToAdd -= 1.0f;
    }
    
    // Handle partial characters for smooth animation
    if (_currentCharIndex < totalChars && charsToAdd > 0.0f) {
        float progress = (_currentCharIndex + charsToAdd) / totalChars;
        if (progress >= 1.0f || _currentCharIndex >= totalChars - 1) {
            _displayText = _fullText;
            _currentCharIndex = totalChars;
        }
    }
    
    _text.setString(_displayText);
    
    if (_currentCharIndex >= totalChars) {
        _isComplete = true;
    }
}

void TextEffect::updateFadeIn(float deltaTime) {
    sf::Color color = _text.getFillColor();
    color.a = std::min(255, static_cast<int>(color.a + _fadeSpeed * 255.0f * deltaTime));
    _text.setFillColor(color);
    
    if (color.a >= 255) {
        _isComplete = true;
    }
}

void TextEffect::updateGlow(float deltaTime) {
    float glow = (std::sin(_timer * 3.0f) + 1.0f) * 0.5f; // 0 to 1
    sf::Color color = _text.getFillColor();
    
    // Create glow effect by varying brightness
    float brightness = 0.7f + glow * 0.3f * _glowIntensity;
    color.r = std::min(255, static_cast<int>(color.r * brightness));
    color.g = std::min(255, static_cast<int>(color.g * brightness));
    color.b = std::min(255, static_cast<int>(color.b * brightness));
    
    _text.setFillColor(color);
}

void TextEffect::updateShake(float deltaTime) {
    if (_shakeIntensity > 0.0f) {
        float shakeX = (std::rand() % 200 - 100) / 100.0f * _shakeIntensity;
        float shakeY = (std::rand() % 200 - 100) / 100.0f * _shakeIntensity;
        _shakeOffset = sf::Vector2f(shakeX, shakeY);
    }
}

void TextEffect::updateWave(float deltaTime) {
    // Wave effect would require per-character positioning
    // For now, we'll use a simple vertical wave
    float wave = std::sin(_timer * 2.0f) * 5.0f;
    _text.setPosition(_basePosition + sf::Vector2f(0.0f, wave));
}

