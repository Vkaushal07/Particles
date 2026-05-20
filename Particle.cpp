#include "Particle.h"
#include <cmath>
#include <cstdlib>

Particle::Particle(sf::RenderTarget& target, int numPoints, sf::Vector2i mouseClickPosition) 
    : m_A(2, numPoints), m_numPoints(numPoints), m_ttl(TTL) 
{
    m_radiansPerSec = ((float)std::rand() / RAND_MAX) * PI * 2.0f; // Faster spin

    m_cartesianPlane.setCenter(0, 0);
    m_cartesianPlane.setSize(target.getSize().x, -static_cast<float>(target.getSize().y));

    m_centerCoordinate = target.mapPixelToCoords(mouseClickPosition, m_cartesianPlane);

    m_vx = static_cast<float>(std::rand() % 500 + 150); 
    if (std::rand() % 2 != 0) m_vx *= -1.0f;
    m_vy = static_cast<float>(std::rand() % 600 + 400); 

    m_color1 = sf::Color::White;
    // Initial color selection
    m_color2 = HSLtoRGB(static_cast<float>(std::rand() % 360), 1.0f, 0.5f);

    m_maxTrailSize = 12; // Length of the sparkle trail

    float theta = ((float)std::rand() / RAND_MAX) * (PI / 2.0f);
    float dTheta = 2.0f * PI / (m_numPoints - 1);

    for (int j = 0; j < m_numPoints; ++j) {
        float r = static_cast<float>(std::rand() % 40 + 15);
        float dx = r * std::cos(theta);
        float dy = r * std::sin(theta);

        m_A(0, j) = m_centerCoordinate.x + dx;
        m_A(1, j) = m_centerCoordinate.y + dy;
        theta += dTheta;
    }
}

// Extra Credit: Smooth mathematical HSL spectrum generation
sf::Color Particle::HSLtoRGB(float h, float s, float l) {
    float c = (1.0f - std::abs(2.0f * l - 1.0f)) * s;
    float x = c * (1.0f - std::abs(std::fmod(h / 60.0f, 2.0f) - 1.0f));
    float m = l - c / 2.0f;
    float r = 0, g = 0, b = 0;

    if (h >= 0 && h < 60) { r = c; g = x; b = 0; }
    else if (h >= 60 && h < 120) { r = x; g = c; b = 0; }
    else if (h >= 120 && h < 180) { r = 0; g = c; b = x; }
    else if (h >= 180 && h < 240) { r = 0; g = x; b = c; }
    else if (h >= 240 && h < 300) { r = x; g = 0; b = c; }
    else if (h >= 300 && h < 360) { r = c; g = 0; b = x; }

    return sf::Color(
        static_cast<sf::Uint8>((r + m) * 255),
        static_cast<sf::Uint8>((g + m) * 255),
        static_cast<sf::Uint8>((b + m) * 255)
    );
}

void Particle::update(float dt) {
    m_ttl -= dt;

    // Save current pixel center position to history array before transforming
    m_trailPositions.insert(m_trailPositions.begin(), m_centerCoordinate);
    if (m_trailPositions.size() > m_maxTrailSize) {
        m_trailPositions.pop_back();
    }

    rotate(dt * m_radiansPerSec);
    scale(SCALE);

    float dx = m_vx * dt;
    m_vy -= G * dt; 
    float dy = m_vy * dt;
    translate(dx, dy);

    // Extra Credit: Dynamically shift colors through a rainbow spectrum based on TTL life
    float hue = std::fmod((m_ttl / TTL) * 360.0f * 2.0f, 360.0f);
    m_color2 = HSLtoRGB(hue, 1.0f, 0.5f);
}

void Particle::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    // 1. Render fading trail history dots first
    for (size_t i = 0; i < m_trailPositions.size(); ++i) {
        sf::CircleShape trailDot(2.0f);
        trailDot.setOrigin(2.0f, 2.0f);
        trailDot.setPosition(static_cast<sf::Vector2f>(target.mapCoordsToPixel(m_trailPositions[i], m_cartesianPlane)));
        
        // Make older trail positions progressively more transparent
        sf::Color trailColor = m_color2;
        trailColor.a = static_cast<sf::Uint8>(255 * (1.0f - (static_cast<float>(i) / m_maxTrailSize)));
        trailDot.setFillColor(trailColor);
        
        target.draw(trailDot);
    }

    // 2. Render core active structural particle
    sf::VertexArray lines(sf::TriangleFan, m_numPoints + 1);
    sf::Vector2f pixelCenter = static_cast<sf::Vector2f>(target.mapCoordsToPixel(m_centerCoordinate, m_cartesianPlane));
    lines[0].position = pixelCenter;
    lines[0].color = m_color1;

    for (int j = 1; j <= m_numPoints; ++j) {
        sf::Vector2f cartesianPos(m_A(0, j - 1), m_A(1, j - 1));
        lines[j].position = static_cast<sf::Vector2f>(target.mapCoordsToPixel(cartesianPos, m_cartesianPlane));
        lines[j].color = m_color2;
    }

    target.draw(lines);
}

// Keep translate, rotate, scale, almostEqual, and unitTests exactly as they were...
void Particle::translate(double xShift, double yShift) {
    TranslationMatrix T(xShift, yShift, m_numPoints);
    m_A = T + m_A;
    m_centerCoordinate.x += xShift;
    m_centerCoordinate.y += yShift;
}

void Particle::rotate(double theta) {
    sf::Vector2f temp = m_centerCoordinate;
    translate(-temp.x, -temp.y);
    RotationMatrix R(theta);
    m_A = R * m_A;
    translate(temp.x, temp.y);
}

void Particle::scale(double c) {
    sf::Vector2f temp = m_centerCoordinate;
    translate(-temp.x, -temp.y);
    ScalingMatrix S(c);
    m_A = S * m_A;
    translate(temp.x, temp.y);
}

bool Particle::almostEqual(double a, double b, double eps) {
    return std::abs(a - b) < eps;
}

void Particle::unitTests() {
    int score = 0;
    std::cout << "Testing Matrix Operations...\n";
    RotationMatrix R(PI / 2.0); 
    Matrix testVec(2, 1);
    testVec(0, 0) = 1.0; testVec(1, 0) = 0.0;
    Matrix rotated = R * testVec;
    if (almostEqual(rotated(0, 0), 0.0) && almostEqual(rotated(1, 0), 1.0)) {
        std::cout << " -> RotationMatrix: PASSED\n"; score++;
    }
    ScalingMatrix S(0.5);
    Matrix scaled = S * testVec;
    if (almostEqual(scaled(0, 0), 0.5) && almostEqual(scaled(1, 0), 0.0)) {
        std::cout << " -> ScalingMatrix: PASSED\n"; score++;
    }
    TranslationMatrix T(10.0, 5.0, 1);
    Matrix translated = T + testVec;
    if (almostEqual(translated(0, 0), 11.0) && almostEqual(translated(1, 0), 5.0)) {
        std::cout << " -> TranslationMatrix: PASSED\n"; score++;
    }
    std::cout << "Unit Testing Score: " << score << " / 3 Match Benchmarks Met.\n";
}