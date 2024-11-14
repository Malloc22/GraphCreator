//----------------------------

#include "Graph.hpp"

//----------------------------

//----------------------------
//  class Edge
//----------------------------

//----------------------------

void Edge::draw(sf::RenderTarget& target, sf::RenderStates states) const {

    if(m_isDraw)
        return;

    m_arrow.setDir(m_direction);
    target.draw(m_arrow);

    sf::Vector2f pos;

    pos.x = m_vertices[0]->getPosition().x + m_vertices[1]->getPosition().x;
    pos.y = m_vertices[0]->getPosition().y + m_vertices[1]->getPosition().y;

    pos /= 2.0f;

    if(m_direction == 4) {
        if(m_weight[0] != 0) {

            pos = m_vertices[0]->getPosition();

            pos.x -= RADIUS+10;
            pos.y -= RADIUS+10;

            m_rect.setPosition(pos);
            target.draw(m_rect);

            m_text.setString(std::to_string(m_weight[0]));

            sf::FloatRect textRect = m_text.getLocalBounds();
            m_text.setOrigin(   textRect.left + textRect.width/2.0f,
                                textRect.top  + textRect.height/2.0f);

            m_text.setPosition((int)pos.x, (int)pos.y);

            target.draw(m_text);
        }
    }
    else if(m_direction == 3) {

        sf::Vector2f newpos = pos;

        newpos.y -= (m_rect.getSize().y + 5);

        m_rect.setPosition(newpos);
        target.draw(m_rect);

        m_text.setString(std::to_string(m_weight[0]));

        sf::FloatRect textRect = m_text.getLocalBounds();
        m_text.setOrigin(   textRect.left + textRect.width/2.0f,
                            textRect.top  + textRect.height/2.0f);

        m_text.setPosition((int)newpos.x, (int)newpos.y);
        target.draw(m_text);

        pos.y += m_rect.getSize().y + 5;

        m_rect.setPosition(pos);
        target.draw(m_rect);

        m_text.setString(std::to_string(m_weight[1]));

        textRect = m_text.getLocalBounds();
        m_text.setOrigin(   textRect.left + textRect.width/2.0f,
                            textRect.top  + textRect.height/2.0f);

        m_text.setPosition((int)pos.x, (int)pos.y);

        target.draw(m_text);

    }
    else if(m_direction & 1 || m_direction & 1 << 1 || m_direction == 0) {
        m_rect.setPosition(pos);
        target.draw(m_rect);

        m_text.setString(std::to_string(m_weight[0]));

        sf::FloatRect textRect = m_text.getLocalBounds();
        m_text.setOrigin(   textRect.left + textRect.width/2.0f,
                            textRect.top  + textRect.height/2.0f);

        m_text.setPosition((int)pos.x, (int)pos.y);

        target.draw(m_text);
    }

    m_isDraw = true;
};

//----------------------------

void Edge::init(sf::Font* font) {

    m_text.setFont(*font);
    m_text.setCharacterSize(12);
    m_text.setFillColor(sf::Color::Black);

    m_rect.setFillColor(sf::Color::White);
    m_rect.setOutlineColor(sf::Color::Black);

    m_rect.setOutlineThickness(1);
    m_rect.setSize({ 14, 12 });

    m_rect.setOrigin({ m_rect.getSize().x / 2.0f, m_rect.getSize().y / 2.0f });
}

//----------------------------

//----------------------------
//  class Vertex
//----------------------------

//----------------------------

Vertex::Vertex() {

    m_circle.setRadius(RADIUS);
    m_circle.setOrigin(m_circle.getGlobalBounds().width / 2.0f, m_circle.getGlobalBounds().height / 2.0f);
    m_circle.setFillColor(sf::Color::White);

    m_circle.setOutlineThickness(-2);
    m_circle.setOutlineColor(sf::Color::Blue);

    m_text.setFillColor(sf::Color::Black);
    m_text.setCharacterSize(12);
}

//----------------------------

Vertex::~Vertex() {
    this->clear();
}

//----------------------------

void Vertex::draw(sf::RenderTarget& target, sf::RenderStates states) const {

    sf::FloatRect textRect = m_text.getLocalBounds();
    m_text.setOrigin(   textRect.left + textRect.width/2.0f,
                        textRect.top  + textRect.height/2.0f);

    m_text.setPosition((int)m_circle.getPosition().x, (int)m_circle.getPosition().y);

    for(auto it : m_edges)
        target.draw(*it);

    target.draw(m_circle);
    target.draw(m_text);
};

//----------------------------

void Vertex::init(sf::Font* font) {
    m_text.setFont(*font);
    m_font = font;
}

//----------------------------

Edge* Vertex::findEdge(Vertex* vertex) {
    for(auto it : m_edges) {
        if((it->getVertices()[0] == this && it->getVertices()[1] == vertex) || (it->getVertices()[0] == vertex && it->getVertices()[1] == this)) {
            return it;
        }
    }

    return nullptr;
}

//----------------------------

Edge* Vertex::addEdge(Vertex* vertex, int dir, int weight0, int weight1) {

    for(auto it : m_edges) {
        if((it->getVertices()[0] == this && it->getVertices()[1] == vertex) || (it->getVertices()[0] == vertex && it->getVertices()[1] == this)) {

            if(it->getVertices()[0] == vertex && it->getVertices()[1] == this && it->getDir() != 0b01 && it->getDir() != 3) {
                it->setDir(3);
                it->setWeight(it->getWeights()[0], weight0);
            } else {
                it->setDir(dir);
                it->setWeight(weight0, weight1);
            }

            return it;
        }
    }

    Edge* edge = new Edge(this, vertex);
    edge->setDir(dir);
    edge->setWeight(weight0, weight1);
    edge->init(m_font);

    m_edges.push_back(edge);

    if(this != vertex)
        vertex->addEdge(edge);

    return nullptr;
}

//----------------------------

void Vertex::addEdge(Edge* edge) {
    m_edges.push_back(edge);
}

//----------------------------

void Vertex::removeEdge(Edge* edge, bool isHard) {

    if(!isHard) {
        m_edges.erase(std::remove(m_edges.begin(), m_edges.end(), edge), m_edges.end());
        return;
    }

    Vertex* vertex = ( edge->getVertices()[0] != this ? edge->getVertices()[0] : (edge->getVertices()[1] != this ? edge->getVertices()[1] : nullptr) );
    vertex->removeEdge(edge);

    m_edges.erase(std::remove(m_edges.begin(), m_edges.end(), edge), m_edges.end());

    delete edge;
}

//----------------------------

void Vertex::update() {
    for(auto it : m_edges)
        it->resetDraw();
}

//----------------------------

void Vertex::clear() {
    for(auto it : m_edges) {
        if(it) {
            if(this != it->getVertices()[0])
                it->getVertices()[0]->removeEdge(it);

            else if(this != it->getVertices()[1])
                it->getVertices()[1]->removeEdge(it);

            delete it;
            it = nullptr;
        }
    }

    m_edges.clear();
    m_edges.shrink_to_fit();
}

//----------------------------

//----------------------------
//  class Graph
//----------------------------

//----------------------------

Graph::Graph() {
    m_focusVertex = nullptr;
    m_oldFocusVertex = nullptr;
    m_counter = 1;
    m_press = false;
}

//----------------------------

Graph::~Graph() {

    this->clear();
}

//----------------------------

void Graph::draw(sf::RenderTarget& target, sf::RenderStates states) const {

    for(auto it : m_vertices)
        target.draw(*it);

};

//----------------------------

void Graph::resetVisited() {

    for(Vertex* vertex : m_vertices)
        vertex->setVisited(false);

}

//----------------------------

bool Graph::init(sf::Font& font) {

    m_font = &font;

    return true;
}

//----------------------------

bool Graph::removeVertex(Vertex* vertex) {

    m_vertices.erase(std::remove(m_vertices.begin(), m_vertices.end(), vertex), m_vertices.end());
    delete vertex;

    m_focusVertex = nullptr;
    m_oldFocusVertex = nullptr;

    return true;
}

//----------------------------

bool Graph::loadFromFile(const std::string& file, bool isSpec) {

    std::iostream* stream;

    if(isSpec)
        stream = new std::stringstream(file);
    else
        stream = new std::fstream();

    if(!isSpec) {
        ((std::fstream*)stream)->open(file, std::ios::in);

        if(!(*(std::fstream*)stream).is_open())
            return false;
    }

    m_counter = 1;
    this->clear();

    std::vector<sf::Vector2f> posm;

    std::string str;
    *stream >> str;

    std::stringstream ss(str);
    std::string t;
    char del = ';';

    sf::Vector2f pos;
    while(getline(ss, t, del)) {

        try {

            size_t p = t.find(':');

            pos.x = std::stof(t.substr(0, p));
            pos.y = std::stof(t.substr(p+1, t.size()));

        } catch(...) {  }

        posm.push_back(pos);
    }

    for(auto it : posm) {
        Vertex* vertex = new Vertex();
        vertex->setPosition(it);

        this->addVertex(vertex);
    }

    std::vector<int> weights;

    while(*stream >> str) {
        try {
            weights.push_back(std::stol(str));
        } catch(...) {  }
    }

    Vertex* main;
    int count = 0;
    for(size_t i = 0; i < weights.size(); ++i) {

        if((i % m_vertices.size()) == 0) {
            main = m_vertices[count];
            count++;
        }

        if(weights[i] == 0)
            continue;

        Vertex* vertex = m_vertices[i%m_vertices.size()];

        if(vertex == main) {
            main->addEdge(vertex, 4, weights[i]);
            continue;
        }

        Edge* edge = main->findEdge(vertex);
        if(edge) {
            if(weights[i] == edge->getWeights()[0])
                main->addEdge(vertex, 0, weights[i]);
            else
                main->addEdge(vertex, 3, edge->getWeights()[0], weights[i]);
        }
        else {
            main->addEdge(vertex, 1 << 1, weights[i]);
        }
    }

    delete stream;

    return true;
}

//----------------------------

bool Graph::saveToFile(const std::string& file) {

    auto data = this->getAdjacencyMatrix();

    std::fstream stream;

    stream.open(file, std::ios::out);

    for(Vertex* it : m_vertices)
        stream << it->getPosition().x << ":" << it->getPosition().y << ";";

    stream << "\n";

    for(auto it : m_vertices) {
        for(auto it0 : m_vertices)
            stream << data[it][it0] << " ";

        stream << "\n";
    }

    stream.close();

    return true;
}

//----------------------------

std::map<Vertex*, std::map<Vertex*, int>> Graph::getAdjacencyMatrix() {

    std::map<Vertex*, std::map<Vertex*, int>> data;
    std::map<Vertex*, int> buff;

    for(Vertex* it : m_vertices) {
        for(Vertex* it0 : m_vertices)
            buff[it0] = 0;

        data[it] = buff;
    }

    for(Vertex* it : m_vertices) {
        for(Edge* edge : it->getEdges()) {
            int dir = edge->getDir();

            if(edge->getVertices()[0] == it) {
                if(dir == 4)
                    data[it][edge->getVertices()[1]] = edge->getWeights()[0];

                else if(dir == 3)
                    data[it][edge->getVertices()[1]] = edge->getWeights()[0];

                else if(dir & 1 << 1)
                    data[it][edge->getVertices()[1]] = edge->getWeights()[0];

                else if(dir & 1)
                    data[it][edge->getVertices()[1]] = edge->getWeights()[1];

                else if(dir == 0)
                    data[it][edge->getVertices()[1]] = edge->getWeights()[0];
            }
            else {

                if(dir == 3)
                    data[it][edge->getVertices()[0]] = edge->getWeights()[1];

                else if(dir & 1)
                    data[it][edge->getVertices()[0]] = edge->getWeights()[1];

                else if(dir == 0)
                    data[it][edge->getVertices()[0]] = edge->getWeights()[0];

            }
        }
    }

    return data;
}

//----------------------------

std::string Graph::dfs(Vertex* vertex) {

    std::string arr;

    this->resetVisited();

    std::stack<Vertex*> stack;
    stack.push(vertex);

    while(!stack.empty()) {

        Vertex* vertex = stack.top();
        stack.pop();

        if(!vertex->getVisited()) {
            vertex->setVisited(true);

            arr += vertex->getName() + " ";

            for(Edge* edge : vertex->getEdges()) {
                Vertex* buff;

                int dir = edge->getDir();
                if(edge->getVertices()[0] != vertex) {
                    if(!(dir & 1) && dir != 0)
                        continue;

                    buff = edge->getVertices()[0];
                }
                else {
                    buff = edge->getVertices()[1];

                    if(!(dir & 1 << 1) && dir != 0)
                        continue;
                }

                stack.push(buff);
            }
        }
    }

    return arr;
}

//----------------------------

std::string Graph::bfs(Vertex* vertex) {

    std::string arr;

    this->resetVisited();

    std::queue<Vertex*> q;
    vertex->setVisited(true);
    q.push(vertex);

    while(!q.empty()) {

        Vertex* vertex = q.front();
        arr += vertex->getName() + " ";
        q.pop();

        for(Edge* edge : vertex->getEdges()) {
            Vertex* buff;

            int dir = edge->getDir();
            if(edge->getVertices()[0] != vertex) {
                if(!(dir & 1) && dir != 0)
                    continue;

                buff = edge->getVertices()[0];
            }
            else {
                buff = edge->getVertices()[1];

                if(!(dir & 1 << 1) && dir != 0)
                    continue;
            }

            if(!buff->getVisited()) {
                buff->setVisited(true);
                q.push(buff);
            }
        }
    }

    return arr;
}

//----------------------------

std::vector<Vertex*> Graph::dijkstra(Vertex* start, Vertex* end) {

    std::unordered_map<Vertex*, int> distances;
    std::unordered_map<Vertex*, Vertex*> previous;
    std::priority_queue<std::pair<int, Vertex*>, std::vector<std::pair<int, Vertex*>>, std::greater<>> minHeap;

    for(Vertex* vertex : m_vertices) {
        distances[vertex] = std::numeric_limits<int>::max(); // Изначально все расстояния бесконечны
        previous[vertex] = nullptr;
    }

    distances[start] = 0;
    minHeap.emplace(0, start);

    while(!minHeap.empty()) {
        auto [currentDist, currentVertex] = minHeap.top();
        minHeap.pop();

        if (currentVertex == end) {
            break; // Если достигли конечной вершины, выходим из цикла
        }

        for(Edge* edge : currentVertex->getEdges()) {

            Vertex* buff;

            int dir = edge->getDir();
            int res = 0;
            if(edge->getVertices()[0] != currentVertex) {
                if(!(dir & 1) && dir != 0)
                    continue;

                buff = edge->getVertices()[0];

                res = 1;
            }
            else {
                buff = edge->getVertices()[1];

                if(!(dir & 1 << 1) && dir != 0)
                    continue;

                res = 0;
            }

            if(dir == 0) res = 0;

            int newDist = currentDist + edge->getWeights()[res];
            if(newDist < distances[buff]) {
                distances[buff] = newDist;
                previous[buff] = currentVertex;
                minHeap.emplace(newDist, buff);
            }
        }
    }

    std::vector<Vertex*> path;
    for (Vertex* at = end; at != nullptr; at = previous.at(at))
        path.push_back(at);

    std::reverse(path.begin(), path.end()); // Разворот пути

    return path;
}

//----------------------------

void Graph::handleEvent(const sf::Event& event) {

    if(event.type == sf::Event::MouseButtonPressed) {
        if(event.mouseButton.button == sf::Mouse::Left) {

            if(m_focusVertex != nullptr) {
                m_focusVertex->unfocused();
                m_oldFocusVertex = m_focusVertex;
            }

            m_focusVertex = nullptr;

            for(auto it : m_vertices) {
                if(it->testPoint({ (float)event.mouseButton.x, (float)event.mouseButton.y })) {
                    it->focused();
                    m_focusVertex = it;
                    break;
                }
            }

            m_press = true;
        }
    }

    if(event.type == sf::Event::MouseButtonReleased) {
        if(event.mouseButton.button == sf::Mouse::Left) {
            m_press = false;
        }
    }

}

//----------------------------

void Graph::update(sf::RenderWindow& window) {
    for(auto it : m_vertices)
        it->update();

    if(m_press && m_focusVertex != nullptr) {
        sf::Vector2f pos = { (float)sf::Mouse::getPosition(window).x, (float)sf::Mouse::getPosition(window).y };
        m_focusVertex->setPosition(pos);
    }
}

//----------------------------

void Graph::clear() {
    for(auto it : m_vertices)
        delete it;

    m_focusVertex = nullptr;
    m_oldFocusVertex = nullptr;

    m_vertices.clear();
    m_vertices.shrink_to_fit();
}

//----------------------------
