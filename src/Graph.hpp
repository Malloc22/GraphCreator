//----------------------------

#ifndef GRAPH_HPP_INCLUDED
#define GRAPH_HPP_INCLUDED

//----------------------------

#include "include_libs.hpp"

#include "Arrow.hpp"

//----------------------------

#define RADIUS 15

//----------------------------

class Vertex;

//----------------------------

class Edge : public sf::Drawable {
public:

    Edge(Vertex* vertex0, Vertex* vertex1) { m_weight[0] = 0; m_weight[1] = 0; m_arrow.link(this); m_vertices[0] = vertex0; m_vertices[1] = vertex1; }
    ~Edge() {  }

    void init(sf::Font* font);
    void setWeight(int weight0, int weight1 = 0) { m_weight[0] = weight0; m_weight[1] = weight1; }
    void setDir(int dir) { m_direction = dir; }

    int* getWeights() { return m_weight; }

    int getDir() { return m_direction; }

    Vertex** getVertices() { return m_vertices; }
    void resetDraw() { m_isDraw = false; }

private:

    mutable Vertex* m_vertices[2];

    mutable sf::RectangleShape m_rect;
    mutable sf::Text m_text;

    mutable Arrow m_arrow;

    int m_direction; //11 undirected /01 or /10 oriented

    int m_weight[2];

    mutable bool m_isDraw;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

//----------------------------

class Vertex : public sf::Drawable {
public:

    typedef std::vector<Edge*> edges_t;

public:

    Vertex();
    ~Vertex();

    void init(sf::Font* font);

    Edge* findEdge(Vertex* vertex);
    Edge* addEdge(Vertex* vertex, int dir, int weight0, int weight1 = 1);
    void removeEdge(Edge* edge, bool isHard = false);

    void setPosition(sf::Vector2f position) { m_circle.setPosition(position); }
    void setName(const std::string& name) { m_text.setString(name); }

    const edges_t& getEdges() { return m_edges; }

    sf::Vector2f getPosition() { return m_circle.getPosition(); }
    std::string getName() { return m_text.getString(); }

    void setId(unsigned int id) { m_id = id; }
    unsigned int getId() { return m_id; }

    void focused() { m_circle.setOutlineColor(sf::Color::Red); }
    void unfocused() { m_circle.setOutlineColor(sf::Color::Blue); }

    bool testPoint(sf::Vector2f pos) { return m_circle.getGlobalBounds().contains(pos.x, pos.y); }

    void setVisited(bool visited) { m_isVisited = visited; }
    bool getVisited() { return m_isVisited; }

    void update();
    void clear();

private:

    edges_t m_edges;

    sf::Font* m_font;

    sf::CircleShape m_circle;
    mutable sf::Text m_text;

    unsigned int m_id = -1;

    bool m_isVisited;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    void addEdge(Edge* edge);
};

//----------------------------

class Graph : public sf::Drawable {
public:

    typedef std::vector<Vertex*> vertices_t;

public:

    Graph();
    ~Graph();

    bool init(sf::Font& font);

    void addVertex(Vertex* vertex) { vertex->init(m_font); vertex->setName(std::to_string(m_counter)); vertex->setId(m_counter++); m_vertices.push_back(vertex); }
    bool removeVertex(Vertex* vertex);

    bool loadFromFile(const std::string& file, bool isSpec = false);
    bool saveToFile(const std::string& file);

    std::map<Vertex*, std::map<Vertex*, int>> getAdjacencyMatrix();

    std::string dfs(Vertex* vertex);
    std::string bfs(Vertex* vertex);

    std::vector<Vertex*> dijkstra(Vertex* start, Vertex* end);

    void handleEvent(const sf::Event& event);

    Vertex* getFocus() { return m_focusVertex; }
    Vertex* getOldFocus() { return m_oldFocusVertex; }
    vertices_t* getVertices() { return &m_vertices; }

    void update(sf::RenderWindow& window);
    void clear();

private:

    sf::Font* m_font;
    vertices_t m_vertices;

    Vertex *m_focusVertex,
           *m_oldFocusVertex;

    unsigned int m_counter;

    bool m_press;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    void resetVisited();
};

//----------------------------

#endif // GRAPH_HPP_INCLUDED

//----------------------------
