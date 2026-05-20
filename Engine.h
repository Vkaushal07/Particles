#ifndef ENGINE_H
#define ENGINE_H

#include <SFML/Graphics.hpp>
#include <vector>
#include "Particle.h"

class Engine {
public:
    Engine();
    void run();

private:
    void input();
    void update(float dtAsSeconds);
    void draw();

    sf::RenderWindow m_Window;
    std::vector<Particle> m_particles;
};

#endif