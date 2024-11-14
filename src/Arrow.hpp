//----------------------------

#ifndef ARROW_HPP_INCLUDED
#define ARROW_HPP_INCLUDED

//----------------------------

#include "include_libs.hpp"

//----------------------------

#define DEG_TO_RAD(x) x / 57.29577951308232f
#define RAD_TO_DEG(x) x * 57.29577951308232f

#define SIZE_ARROW 15
#define ANGLE 20

//----------------------------

class Edge;

//----------------------------

class Arrow : public sf::Drawable {
public:

    Arrow() {
        m_circle.setFillColor(sf::Color::Transparent);
        m_circle.setOutlineColor(sf::Color::White);
        m_circle.setRadius(25);
        m_circle.setOutlineThickness(1.0f);

        m_circle.setOrigin( m_circle.getGlobalBounds().width-5, m_circle.getGlobalBounds().height-5);

        m_dir = 0b11;
    }

    ~Arrow() {  }

    void link(Edge* edge);
    void setDir(int dir) { m_dir = dir; }

private:

    mutable sf::Vertex m_triangle[3];
    mutable sf::Vertex m_line[2];
    mutable sf::CircleShape m_circle;

    mutable Edge* m_edge;

    int m_dir;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    void findCoordTriangle(sf::Vertex* triangle, int dir) const;
};

//----------------------------

#endif // ARROW_HPP_INCLUDED

//----------------------------
