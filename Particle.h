#pragma once

#include "Matrices.h"
#include <SFML/Graphics.hpp>
#include <vector>

#define M_PI 3.1415926535897932384626433

const float G = 1000; // Gravity
const float TTL = 5.0; // Time To Live
const float SCALE = 0.999;

using namespace Matrices;
using namespace sf;

class Particle : public Drawable
{
public:
    Particle(RenderTarget& target, int numPoints, Vector2i mouseClickPosition);
    virtual void draw(RenderTarget& target, RenderStates states) const override;
    void update(float dt);
    float getTTL() { return m_ttl; }

    // Functions for unit testing
    bool almostEqual(double a, double b, double eps = 0.0001);
    void unitTests();

private:
    float m_ttl;
    int m_numPoints;
    Vector2f m_centerCoordinate;
    float m_radiansPerSec;
    float m_vx;
    float m_vy;
    View m_cartesianPlane;
    Color m_color1;
    Color m_color2;
    Matrix m_A;

    void rotate(double theta);
    void scale(double c);
    void translate(double xShift, double yShift);

    // Extra Credit Properties
    Color HSLtoRGB(float hue, float saturation, float lightness);
    std::vector<Vector2f> m_trailPositions;
    size_t m_maxTrailSize;
};