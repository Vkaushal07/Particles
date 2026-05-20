#include "Particle.h"
#include <cmath>
#include <cstdlib>

Particle::Particle(RenderTarget& target, int numPoints, Vector2i mouseClickPosition)
    : m_A(2, numPoints), m_numPoints(numPoints), m_ttl(TTL)
{
    m_radiansPerSec = ((float)rand() / RAND_MAX) * M_PI;

    m_cartesianPlane.setCenter(0, 0);
    m_cartesianPlane.setSize(target.getSize().x, -static_cast<float>(target.getSize().y));

    m_centerCoordinate = target.mapPixelToCoords(mouseClickPosition, m_cartesianPlane);

    m_vx = static_cast<float>(rand() % 400 + 100);
    if (rand() % 2 != 0) m_vx *= -1.0f;
    m_vy = static_cast<float>(rand() % 500 + 500); // Launch upwards velocity

    m_color1 = Color::White;
    m_color2 = HSLtoRGB(static_cast<float>(rand() % 360), 1.0f, 0.5f);

    m_maxTrailSize = 15; // Tail length for motion-blur sparkles

    float theta = ((float)rand() / RAND_MAX) * (M_PI / 2.0);
    float dTheta = 2.0 * M_PI / (m_numPoints - 1);

    for (int j = 0; j < m_numPoints; ++j) {
        float r = static_cast<float>(rand() % 60 + 20);
        float dx = r * cos(theta);
        float dy = r * sin(theta);

        m_A(0, j) = m_centerCoordinate.x + dx;
        m_A(1, j) = m_centerCoordinate.y + dy;
        theta += dTheta;
    }
}

Color Particle::HSLtoRGB(float h, float s, float l) {
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

    return Color(
        static_cast<Uint8>((r + m) * 255),
        static_cast<Uint8>((g + m) * 255),
        static_cast<Uint8>((b + m) * 255)
    );
}

void Particle::draw(RenderTarget& target, RenderStates states) const {
    // 1. Draw extra credit trail spots
    for (size_t i = 0; i < m_trailPositions.size(); ++i) {
        CircleShape trailDot(2.0f);
        trailDot.setOrigin(2.0f, 2.0f);
        trailDot.setPosition(static_cast<Vector2f>(target.mapCoordsToPixel(m_trailPositions[i], m_cartesianPlane)));
        
        Color trailColor = m_color2;
        trailColor.a = static_cast<Uint8>(255 * (1.0f - (static_cast<float>(i) / m_maxTrailSize)));
        trailDot.setFillColor(trailColor);
        target.draw(trailDot);
    }

    // 2. Draw core active particle layout
    VertexArray lines(TriangleFan, m_numPoints + 1);
    Vector2f pixelCenter = static_cast<Vector2f>(target.mapCoordsToPixel(m_centerCoordinate, m_cartesianPlane));
    lines[0].position = pixelCenter;
    lines[0].color = m_color1;

    for (int j = 1; j <= m_numPoints; ++j) {
        Vector2f cartesianPos(m_A(0, j - 1), m_A(1, j - 1));
        lines[j].position = static_cast<Vector2f>(target.mapCoordsToPixel(cartesianPos, m_cartesianPlane));
        lines[j].color = m_color2;
    }

    target.draw(lines);
}

void Particle::update(float dt) {
    m_ttl -= dt;

    // Track position history for trails
    m_trailPositions.insert(m_trailPositions.begin(), m_centerCoordinate);
    if (m_trailPositions.size() > m_maxTrailSize) {
        m_trailPositions.pop_back();
    }

    rotate(dt * m_radiansPerSec);
    scale(SCALE);

    float dx = m_vx * dt;
    m_vy -= G * dt; // Apply gravity pull sequence
    float dy = m_vy * dt;
    translate(dx, dy);

    // Color shifting spectrum cycle updates
    float hue = std::fmod((m_ttl / TTL) * 360.0f * 2.0f, 360.0f);
    m_color2 = HSLtoRGB(hue, 1.0f, 0.5f);
}

void Particle::translate(double xShift, double yShift) {
    TranslationMatrix T(xShift, yShift, m_numPoints);
    m_A = T + m_A;
    m_centerCoordinate.x += xShift;
    m_centerCoordinate.y += yShift;
}

void Particle::rotate(double theta) {
    Vector2f temp = m_centerCoordinate;
    translate(-temp.x, -temp.y);
    RotationMatrix R(theta);
    m_A = R * m_A;
    translate(temp.x, temp.y);
}

void Particle::scale(double c) {
    Vector2f temp = m_centerCoordinate;
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
    cout << "Testing Matrix Operations...\n";
    RotationMatrix R(M_PI / 2.0);
    Matrix testVec(2, 1);
    testVec(0, 0) = 1.0; testVec(1, 0) = 0.0;
    Matrix rotated = R * testVec;
    if (almostEqual(rotated(0, 0), 0.0) && almostEqual(rotated(1, 0), 1.0)) {
        cout << " -> RotationMatrix: PASSED\n"; score++;
    }
    ScalingMatrix S(0.5);
    Matrix scaled = S * testVec;
    if (almostEqual(scaled(0, 0), 0.5) && almostEqual(scaled(1, 0), 0.0)) {
        cout << " -> ScalingMatrix: PASSED\n"; score++;
    }
    TranslationMatrix T(10.0, 5.0, 1);
    Matrix translated = T + testVec;
    if (almostEqual(translated(0, 0), 11.0) && almostEqual(translated(1, 0), 5.0)) {
        cout << " -> TranslationMatrix: PASSED\n"; score++;
    }
    cout << "Unit Testing Score: " << score << " / 3 benchmarks passed.\n";
}