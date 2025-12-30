#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include <cmath>

struct Particle {
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Color color;
    float lifetime;
    float maxLifetime;
    float size;
    float rotation;
    float rotationSpeed;
};

class ParticleSystem {
public:
    ParticleSystem();
    
    // Create particle effects
    void createMemoryLossEffect(const sf::Vector2f& position, int count = 30);
    void createChoiceEffect(const sf::Vector2f& position, const sf::Color& color, int count = 20);
    void createFloatingParticles(int count = 15);
    void createSparkle(const sf::Vector2f& position, const sf::Color& color);
    
    // Update and render
    void update(float deltaTime);
    void draw(sf::RenderWindow& window);
    
    // Clear all particles
    void clear();
    
    // Check if system is active
    bool isEmpty() const { return _particles.empty(); }

private:
    std::vector<Particle> _particles;
    std::mt19937 _rng;
    
    // Helper functions
    sf::Color randomColor(float alpha = 255.0f);
    sf::Vector2f randomVelocity(float speed);
};

