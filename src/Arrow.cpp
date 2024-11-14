//----------------------------

#include "Arrow.hpp"
#include "Graph.hpp"

//----------------------------

void Arrow::draw(sf::RenderTarget& target, sf::RenderStates states) const {

    if(m_dir == 4) {
        Vertex* vertex = m_edge->getVertices()[0];
        m_circle.setPosition(vertex->getPosition());
        target.draw(m_circle);

        return;
    }

    m_line[0].position = m_edge->getVertices()[0]->getPosition();
    m_line[1].position = m_edge->getVertices()[1]->getPosition();

    float len = std::sqrt(std::pow((m_line[1].position.x - m_line[0].position.x), 2) + std::pow((m_line[1].position.y - m_line[0].position.y), 2));
    sf::Vector2 pos0 = m_line[0].position + (m_line[1].position - m_line[0].position) * (RADIUS / len);
    sf::Vector2 pos1 = m_line[1].position + (m_line[0].position - m_line[1].position) * (RADIUS / len);

    m_line[0].position = pos0;
    m_line[1].position = pos1;

    if(m_dir == 0b11) {

        findCoordTriangle(&m_triangle[0], 1);
        target.draw(m_triangle, 3, sf::Triangles);

        findCoordTriangle(&m_triangle[0], 2);
        target.draw(m_triangle, 3, sf::Triangles);
    }

    else if(m_dir != 0) {

        findCoordTriangle(&m_triangle[0], m_dir);
        target.draw(m_triangle, 3, sf::Triangles);

    }

    target.draw(m_line, 2, sf::Lines);
};

//----------------------------

void Arrow::findCoordTriangle(sf::Vertex* triangle, int dir) const {

    if(dir > 2)
        return;

    --dir;

    float delta_x = m_line[0].position.x - m_line[1].position.x;
    float delta_y = m_line[0].position.y - m_line[1].position.y;

    float angle_rad = std::atan2(delta_y, delta_x);
    float angle_deg = RAD_TO_DEG(angle_rad);

    angle_deg -= 180.0f;
    angle_deg = std::abs(angle_deg);

    triangle[0].position = m_line[dir].position;
    triangle[1].position = {    m_line[dir].position.x + (dir ? 1 : -1) * std::cos(DEG_TO_RAD((180-(angle_deg - ANGLE)))) * SIZE_ARROW,
                                m_line[dir].position.y + (dir ? 1 : -1) * std::sin(DEG_TO_RAD((180-(angle_deg - ANGLE)))) * SIZE_ARROW };

    triangle[2].position = {    m_line[dir].position.x + (dir ? 1 : -1) * std::cos(DEG_TO_RAD((180-(angle_deg + ANGLE)))) * SIZE_ARROW,
                                m_line[dir].position.y + (dir ? 1 : -1) * std::sin(DEG_TO_RAD((180-(angle_deg + ANGLE)))) * SIZE_ARROW };
}

//----------------------------

void Arrow::link(Edge* edge) {
    m_edge = edge;
}

//----------------------------
