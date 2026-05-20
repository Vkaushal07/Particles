#ifndef PARTICLE_H
#define PARTICLE_H

#include <SFML/Graphics.hpp>
#include "Matrices.h"
#include <vector>

const float PI = 3.14159265359f;
const float G = 600.0f;       
const float TTL = 5.0f;       
const float SCALE = 0.993f;   

class Particle : public sf::Drawable {
public:
    Particle(sf::RenderTarget& target, int numPoints, sf::Vector2i mouseClickPosition);
    
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    void update(float dt);
    float getTTL() const { return m_ttl; }

    void unitTests();
    bool almostEqual(double a, double b, double eps = 0.001);

private:
    void rotate(double theta);
    void scale(double c);
    void translate(double xShift, double yShift);
    
    // Extra Credit: Helper to create smooth rainbow cycles
    sf::Color HSLtoRGB(float hue, float saturation, float lightness);

    Matrix m_A;
    int m_numPoints;
    float m_ttl;
    float m_radiansPerSec;
    float m_vx;
    float m_vy;
    sf::Vector2f m_centerCoordinate;
    sf::Color m_color1;
    sf::Color m_color2;
    sf::View m_cartesianPlane;

    // Extra Credit: Structure to store motion blur trails
    std::vector<sf::Vector2f> m_trailPositions;
    size_t m_maxTrailSize;
};

#endif