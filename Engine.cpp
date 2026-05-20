#include "Engine.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

Engine::Engine() {
    std::srand(std::time(nullptr));
    unsigned int width = VideoMode::getDesktopMode().width / 2;
    unsigned int height = VideoMode::getDesktopMode().height / 2;
    
    m_Window.create(VideoMode(width, height), "Spawning Matrix Particles", Style::Default);
}

void Engine::run() {
    cout << "Starting Particle unit tests..." << endl;
    Particle p(m_Window, 4, { (int)m_Window.getSize().x / 2, (int)m_Window.getSize().y / 2 });
    p.unitTests();
    cout << "Unit tests complete. Starting engine..." << endl;

    Clock clock;
    while (m_Window.isOpen()) {
        float dt = clock.restart().asSeconds();
        input();
        update(dt);
        draw();
    }
}

void Engine::input() {
    Event event;
    while (m_Window.pollEvent(event)) {
        if (event.type == Event::Closed || 
           (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)) {
            m_Window.close();
        }

        if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
            Vector2i clickPos(event.mouseButton.x, event.mouseButton.y);
            
            // Generate standard burst packet of 5 unique matrix-calculated points
            for (int i = 0; i < 5; ++i) {
                int randomizedPoints = std::rand() % 26 + 25; // Range [25:50]
                m_particles.push_back(Particle(m_Window, randomizedPoints, clickPos));
            }
        }
    }
}

// LINUX-STRICT: Using clean while loop iterator to ensure safe memory erasure
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
    m_Window.clear(Color::Black);
    for (const auto& particle : m_particles) {
        m_Window.draw(particle);
    }
    m_Window.display();
}