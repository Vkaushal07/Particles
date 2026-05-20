#include "Engine.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

Engine::Engine() {
    std::srand(std::time(nullptr));
    unsigned int width = sf::VideoMode::getDesktopMode().width / 2;
    unsigned int height = sf::VideoMode::getDesktopMode().height / 2;
    
    m_Window.create(sf::VideoMode(width, height), "Spawning Matrix Particles", sf::Style::Default);
}

void Engine::run() {
    std::cout << "Starting Particle unit tests...\n";
    Particle p(m_Window, 4, { static_cast<int>(m_Window.getSize().x) / 2, static_cast<int>(m_Window.getSize().y) / 2 });
    p.unitTests();
    std::cout << "Unit tests complete. Starting engine game loop...\n";

    sf::Clock clock;
    while (m_Window.isOpen()) {
        float dt = clock.restart().asSeconds();
        input();
        update(dt);
        draw();
    }
}

void Engine::input() {
    sf::Event event;
    while (m_Window.pollEvent(event)) {
        if (event.type == sf::Event::Closed || 
           (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
            m_Window.close();
        }

        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2i clickPos(event.mouseButton.x, event.mouseButton.y);
            
            // Construct a burst of 5 randomized points
            for (int i = 0; i < 5; ++i) {
                int randomizedPoints = std::rand() % 26 + 25; // range [25:50]
                m_particles.push_back(Particle(m_Window, randomizedPoints, clickPos));
            }
        }
    }
}

void Engine::update(float dtAsSeconds) {
    auto it = m_particles.begin();
    while (it != m_particles.end()) {
        if (it->getTTL() > 0.0f) {
            it->update(dtAsSeconds);
            ++it;
        } else {
            it = m_particles.erase(it);
        }
    }
}

void Engine::draw() {
    m_Window.clear(sf::Color::Black);
    for (const auto& particle : m_particles) {
        m_Window.draw(particle);
    }
    m_Window.display();
}
#endif