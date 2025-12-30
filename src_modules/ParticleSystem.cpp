#include "ParticleSystem.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>

ParticleSystem::ParticleSystem() 
    : _rng(std::chrono::steady_clock::now().time_since_epoch().count())
{
}

void ParticleSystem::createMemoryLossEffect(const sf::Vector2f& position, int count) {
    for (int i = 0; i < count; ++i) {
        Particle p;
        p.position = position;
        p.velocity = randomVelocity(50.0f + (i % 3) * 20.0f);
        p.color = sf::Color(150, 100, 200, 200); // Purple-ish memory color
        p.maxLifetime = 1.5f + (i % 5) * 0.2f;
        p.lifetime = p.maxLifetime;
        p.size = 3.0f + (i % 4);
        p.rotation = static_cast<float>(i * 360 / count);
        p.rotationSpeed = (i % 2 == 0 ? 1.0f : -1.0f) * (100.0f + i * 10.0f);
        _particles.push_back(p);
    }
}

void ParticleSystem::createChoiceEffect(const sf::Vector2f& position, const sf::Color& color, int count) {
    for (int i = 0; i < count; ++i) {
        Particle p;
        p.position = position;
        p.velocity = randomVelocity(80.0f);
        p.color = sf::Color(color.r, color.g, color.b, 180);
        p.maxLifetime = 1.0f;
        p.lifetime = p.maxLifetime;
        p.size = 2.0f + (i % 3);
        p.rotation = static_cast<float>(i * 45);
        p.rotationSpeed = (i % 2 == 0 ? 1.0f : -1.0f) * 200.0f;
        _particles.push_back(p);
    }
}

void ParticleSystem::createFloatingParticles(int count) {
    std::uniform_real_distribution<float> xDist(0.0f, 1200.0f);
    std::uniform_real_distribution<float> yDist(0.0f, 800.0f);
    std::uniform_real_distribution<float> speedDist(10.0f, 30.0f);
    std::uniform_real_distribution<float> lifetimeDist(3.0f, 8.0f);
    
    for (int i = 0; i < count; ++i) {
        Particle p;
        p.position = sf::Vector2f(xDist(_rng), yDist(_rng));
        p.velocity = sf::Vector2f(0.0f, -speedDist(_rng));
        p.color = randomColor(50.0f + (i % 3) * 20.0f); // Subtle ambient particles
        p.maxLifetime = lifetimeDist(_rng);
        p.lifetime = p.maxLifetime;
        p.size = 1.0f + (i % 2);
        p.rotation = 0.0f;
        p.rotationSpeed = 0.0f;
        _particles.push_back(p);
    }
}

void ParticleSystem::createSparkle(const sf::Vector2f& position, const sf::Color& color) {
    for (int i = 0; i < 8; ++i) {
        Particle p;
        p.position = position;
        float angle = static_cast<float>(i * 45) * 3.14159f / 180.0f;
        float speed = 100.0f;
        p.velocity = sf::Vector2f(std::cos(angle) * speed, std::sin(angle) * speed);
        p.color = color;
        p.maxLifetime = 0.5f;
        p.lifetime = p.maxLifetime;
        p.size = 4.0f;
        p.rotation = 0.0f;
        p.rotationSpeed = 500.0f;
        _particles.push_back(p);
    }
}

void ParticleSystem::update(float deltaTime) {
    for (auto& particle : _particles) {
        // Update position
        particle.position += particle.velocity * deltaTime;
        
        // Update rotation
        particle.rotation += particle.rotationSpeed * deltaTime;
        
        // Update lifetime
        particle.lifetime -= deltaTime;
        
        // Apply gravity/fade
        particle.velocity.y += 50.0f * deltaTime; // Slight downward drift
    }
    
    // Remove dead particles
    _particles.erase(
        std::remove_if(_particles.begin(), _particles.end(),
            [](const Particle& p) { return p.lifetime <= 0.0f; }),
        _particles.end()
    );
}

void ParticleSystem::draw(sf::RenderWindow& window) {
    for (const auto& particle : _particles) {
        float alpha = (particle.lifetime / particle.maxLifetime) * 255.0f;
        sf::Color drawColor = particle.color;
        drawColor.a = static_cast<sf::Uint8>(alpha);
        
        // Draw as a small circle
        sf::CircleShape shape(particle.size);
        shape.setFillColor(drawColor);
        shape.setPosition(particle.position);
        shape.setOrigin(particle.size, particle.size);
        shape.setRotation(particle.rotation);
        
        window.draw(shape);
    }
}

void ParticleSystem::clear() {
    _particles.clear();
}

sf::Color ParticleSystem::randomColor(float alpha) {
    std::uniform_int_distribution<int> colorDist(100, 255);
    return sf::Color(
        colorDist(_rng),
        colorDist(_rng),
        colorDist(_rng),
        static_cast<sf::Uint8>(alpha)
    );
}

sf::Vector2f ParticleSystem::randomVelocity(float speed) {
    std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * 3.14159f);
    std::uniform_real_distribution<float> speedDist(speed * 0.5f, speed);
    float angle = angleDist(_rng);
    float vel = speedDist(_rng);
    return sf::Vector2f(std::cos(angle) * vel, std::sin(angle) * vel);
}

