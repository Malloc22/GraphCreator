//----------------------------

#include "Core.hpp"

//----------------------------

Core::Core() {

    m_first = nullptr;
    m_second = nullptr;
    m_focus = nullptr;

    m_stateLoad = false;
}

//----------------------------

Core::~Core() {

}

//----------------------------

void Core::initGUI() {

    tgui::MenuBar::Ptr m_bar = tgui::MenuBar::create();
    m_gui.add(m_bar);

    m_bar->addMenu("File");
    m_bar->addMenuItem("Load");
    m_bar->addMenuItem("Save");
    m_bar->addMenuItem("Matrix");
    m_bar->addMenu("?");
    m_bar->addMenuItem("About");

    m_fileDialog = tgui::FileDialog::create();
    m_fileDialog->setFileTypeFilters({ {"Graph", {"*.gcg", "*.gcg"}} }, 0);

    m_fileDialog->onFileSelect([&](const std::vector<tgui::Filesystem::Path>& paths){
        if(m_stateLoad) {
            m_graph.loadFromFile(paths[0].asString().toStdString());
            m_focus = nullptr;
            m_first = nullptr;
            m_second = nullptr;
        } else
            m_graph.saveToFile((paths[0].asString()).toStdString());

    });

    m_bar->onMenuItemClick([&](const std::vector<tgui::String>& menuItem){
        if(menuItem.size() == 2 && menuItem[0] == "File" && menuItem[1] == "Load") {
            m_gui.add(m_fileDialog);
            m_fileDialog->setTitle("Load");
            m_fileDialog->setPath(tgui::Filesystem::getCurrentWorkingDirectory());
            m_fileDialog->setFileMustExist(true);
            m_fileDialog->setConfirmButtonText();
            m_stateLoad = true;
        }
        else if(menuItem.size() == 2 && menuItem[0] == "File" && menuItem[1] == "Save") {
            m_gui.add(m_fileDialog);
            m_fileDialog->setTitle("Save");
            m_fileDialog->setConfirmButtonText("Save");
            m_fileDialog->setPath(tgui::Filesystem::getCurrentWorkingDirectory());
            m_fileDialog->setFileMustExist(false);
            m_fileDialog->setFilename("graph.gcg");
            m_stateLoad = false;
        }
        else if(menuItem.size() == 2 && menuItem[0] == "File" && menuItem[1] == "Matrix") {
            this->updateMatrix();
            m_gui.add(m_matrixWindow);
        }
    });

    m_popup = tgui::ListBox::create();

    m_popup->addItem("create vertex");
    m_popup->addItem("add edge");
    m_popup->addItem("edit vertex");
    m_popup->setSize({ m_popup->getSize().x, 60 });

    m_popup->onItemSelect([&](tgui::String item){
        if(item == "create vertex") {

            Vertex* vertex = new Vertex();
            vertex->setPosition(m_backMousePosition);

            m_graph.addVertex(vertex);
        }

        else if(item == "add edge") {

            m_addEdge->setPosition({ m_backMousePosition.x, m_backMousePosition.y });

            m_gui.add(m_addEdge);
        }

        else if(item == "edit vertex") {

            m_scrollablePanel->removeAllWidgets();

            float shift = 10;

            if(m_focus == nullptr) {
                m_popup->setVisible(false);
                m_popup->setSelectedItem("");
                return;
            }

            m_saveFocus = m_focus;

            m_editWindow->setTitle("edit vertex (" + m_focus->getName() + ")");

            const Vertex::edges_t* edges = &m_focus->getEdges();

            for(const auto& it : *edges) {

                tgui::Label::Ptr label;
                tgui::Button::Ptr bt;

                if(it->getDir() == 4)
                    label = tgui::Label::create(m_focus->getName() + " - " + m_focus->getName());

                else if(it->getDir() == 0)
                    label = tgui::Label::create(it->getVertices()[0]->getName() + "--" + it->getVertices()[1]->getName());

                else if(it->getDir() == 3)
                    label = tgui::Label::create(it->getVertices()[0]->getName() + "<-->" + it->getVertices()[1]->getName());

                else if(it->getDir() & 1 << 1) {
                    if(it->getVertices()[0] == m_focus)
                        label = tgui::Label::create(it->getVertices()[0]->getName() + "-->" + it->getVertices()[1]->getName());
                    else
                        label = tgui::Label::create(it->getVertices()[1]->getName() + "<--" + it->getVertices()[0]->getName());
                }
                else if(it->getDir() & 1) {
                    if(it->getVertices()[0] == m_focus)
                        label = tgui::Label::create(it->getVertices()[0]->getName() + "<--" + it->getVertices()[1]->getName());
                    else
                        label = tgui::Label::create(it->getVertices()[1]->getName() + "-->" + it->getVertices()[0]->getName());
                }

                bt = tgui::Button::create("x");
                bt->setSize(40, 20);

                label->setPosition({ 10, shift });
                bt->setPosition({ 70, shift });

                bt->onClick([=]{ m_saveFocus->removeEdge(it, it->getVertices()[0] != it->getVertices()[1]); m_scrollablePanel->remove(bt); m_scrollablePanel->remove(label); });

                shift += label->getSize().y + 10;

                m_scrollablePanel->add(label);
                m_scrollablePanel->add(bt);
            }

            m_gui.add(m_editWindow);
        }

        m_popup->setVisible(false);
        m_popup->setSelectedItem("");
    });

    m_gui.add(m_popup);
    m_popup->setVisible(false);

    m_addEdge = tgui::ChildWindow::create("add edge");
    m_addEdge->setSize({100, 200});

    m_label0 = tgui::Label::create("first: ");
    m_label1 = tgui::Label::create("second: ");

    m_label0->setPosition({ 10, 5 });
    m_label1->setPosition({ 10, 5 + m_label1->getSize().y });

    m_addEdge->add(m_label0);
    m_addEdge->add(m_label1);

    float shift = 4 + m_label1->getPosition().y + m_label1->getSize().y;
    for(int i = 0; i < 2; ++i) {
        m_editBox[i] = tgui::EditBox::create();
        m_editBox[i]->setSize({ 60, 20 });
        m_editBox[i]->setPosition({ m_label1->getPosition().x, shift });
        m_editBox[i]->setDefaultText("weight");
        m_editBox[i]->setInputValidator("[0-9]*");
        m_editBox[i]->setMaximumCharacters(2);
        m_editBox[i]->setEnabled(false);

        shift += 24;

        m_addEdge->add(m_editBox[i]);
    }

    m_editBox[0]->setEnabled(true);

    tgui::Button::Ptr bt = tgui::Button::create("link");
    bt->setPosition({ m_addEdge->getSize().x - bt->getSize().x - 10, m_addEdge->getSize().y - 50 });
    m_addEdge->add(bt);

    bt->onClick([&]{

        if(m_first == nullptr || m_second == nullptr)
            return;

        int dir = 0;

        if(m_first == m_second)
            dir = 4;
        else {
            dir |= m_check[0]->isChecked() << 1;
            dir |= m_check[1]->isChecked();
        }

        int weight0 = 0,
            weight1 = 0;
        try {
            weight0 = std::stoi(m_editBox[0]->getText().toStdString());

            if(m_check[1]->isChecked())
                weight1 = std::stoi(m_editBox[1]->getText().toStdString());
        } catch(...) {  }

        m_first->addEdge(m_second, dir, weight0, weight1);
    });

    shift = 4;
    for(int i = 0; i < 2; ++i) {
        m_check[i] = tgui::CheckBox::create();
        m_check[i]->setPosition({ m_editBox[1]->getPosition().x, m_editBox[1]->getPosition().y + m_editBox[1]->getSize().y + shift });

        shift += 4 + m_check[i]->getSize().y;

        m_addEdge->add(m_check[i]);
    }

    /*m_check[0]->onClick([&]{
        m_editBox[0]->setEnabled(!m_check[0]->isChecked());
    });*/

    m_check[1]->onClick([&]{
        m_editBox[1]->setEnabled(!m_check[1]->isChecked());
    });

    m_check[0]->setText("-->");
    m_check[1]->setText("<--");

    m_editWindow = tgui::ChildWindow::create("edit vertex");
    m_editWindow->setSize({ 150, 200 });

    m_scrollablePanel = tgui::ScrollablePanel::create();
    m_scrollablePanel->setSize({ 150, 178 });

    m_editWindow->add(m_scrollablePanel);

    m_matrixWindow = tgui::ChildWindow::create();
    m_matrixWindow->setSize({ 400, 400 });
    m_matrixWindow->setTitle("matrix");

    m_scrollableMatrix = tgui::ScrollablePanel::create();
    m_scrollableMatrix->setSize({ m_matrixWindow->getSize().x, m_matrixWindow->getSize().y - 22 });

    m_matrixWindow->add(m_scrollableMatrix);

    m_button = tgui::Button::create("apply");

    m_button->onPress([&]{

        std::vector<Vertex*>* vertices = m_graph.getVertices();

        if(vertices->empty()) {
            m_gui.remove(m_matrixWindow);
            return;
        }

        std::string data;

        for(Vertex* it : *vertices)
            data += std::to_string((int)it->getPosition().x) + ":" + std::to_string((int)it->getPosition().y) + ";";

        data += "\n";

        size_t size = vertices->size() * vertices->size();

        for(size_t i = 0; i < size; ++i) {
            tgui::EditBox::Ptr edit = m_scrollableMatrix->get<tgui::EditBox>(std::to_string(i));

            if(i != 0 && i % vertices->size() == 0)
                data += "\n";

            std::string buff = edit->getText().toStdString();

            data += (!buff.empty() ? buff : "0");
            data += " ";
        }

        m_graph.loadFromFile(data, true);
        m_gui.remove(m_matrixWindow);

        m_focus = nullptr;
        m_first = nullptr;
        m_second = nullptr;
    });
}

//----------------------------

void Core::updateMatrix() {

    m_scrollableMatrix->removeAllWidgets();

    std::vector<Vertex*>* vertices = m_graph.getVertices();
    auto map = m_graph.getAdjacencyMatrix();

    tgui::Label::Ptr label;
    tgui::EditBox::Ptr edit;

    sf::Vector2f position { 40, 20 };
    sf::Vector2f size = { 30, 20 };
    sf::Vector2f shift = { 5, 8 };

    for(Vertex* it : *vertices) {
        label = tgui::Label::create(it->getName());
        label->setSize({ size.x, size.y });
        label->setPosition({ position.x, position.y });
        position.x += size.x + shift.x;

        label->setHorizontalAlignment(tgui::HorizontalAlignment::Center);
        label->setVerticalAlignment(tgui::VerticalAlignment::Center);

        m_scrollableMatrix->add(label);
    }

    int current = 0;

    position.x = 40;
    position.x -= (size.x + shift.x);
    position.y += size.y + shift.y;

    float buffPosX = position.x;

    for(auto it : *vertices) {

        position.x = buffPosX;

        label = tgui::Label::create(it->getName());
        label->setSize({ size.x, size.y });
        label->setPosition({ position.x, position.y });

        label->setHorizontalAlignment(tgui::HorizontalAlignment::Center);
        label->setVerticalAlignment(tgui::VerticalAlignment::Center);

        m_scrollableMatrix->add(label);

        position.x += size.x + shift.x;

        for(auto it0 : *vertices) {

            edit = tgui::EditBox::create();
            edit->setWidgetName(std::to_string(current));
            edit->setSize({ size.x, size.y });
            edit->setPosition({ position.x, position.y });
            edit->setText(std::to_string(map[it][it0]));
            edit->setInputValidator("[0-9]*");
            edit->setMaximumCharacters(2);

            position.x += size.x + shift.x;

            ++current;

            m_scrollableMatrix->add(edit);
        }

        position.y += size.y + shift.y;
    }

    position.x = m_matrixWindow->getSize().x;
    position.x -= (m_button->getSize().x + shift.x);
    position.y += shift.y + size.y;

    m_button->setPosition({ position.x, position.y });

    m_scrollableMatrix->add(m_button);
}

//----------------------------

void Core::updateGUI() {

    if(m_addEdge->isVisible()) {

        Vertex  *current = m_graph.getFocus(),
                *old = m_graph.getOldFocus();

        if(current != nullptr && old != nullptr) {

            m_first = old;
            m_second = current;

            m_label0->setText("first: " + old->getName());
            m_label1->setText("second: " + current->getName());
        }
    }

    this->collisionUpdate(m_addEdge, 0);
    this->collisionUpdate(m_editWindow, 0);
    this->collisionUpdate(m_matrixWindow, 0);
}

//----------------------------

void Core::collisionUpdate(tgui::Widget::Ptr widget, float tittleOffset) {
    if(!widget->isVisible())
        return;

    if(widget->getPosition().x + widget->getSize().x > m_window.getSize().x)
        widget->setPosition({ m_window.getSize().x - widget->getSize().x, widget->getPosition().y });

    if(widget->getPosition().y + widget->getSize().y + tittleOffset > m_window.getSize().y)
        widget->setPosition( { widget->getPosition().x, m_window.getSize().y - widget->getSize().y - tittleOffset } );

    if(widget->getPosition().x < 0)
        widget->setPosition({ 0, widget->getPosition().y });

    if(widget->getPosition().y < 0)
        widget->setPosition({ widget->getPosition().x, 0 });
}

//----------------------------

void Core::init(sf::Vector2u resolution) {

    std::string tittle = TITTLE;

#if debug_v
    tittle += " [debug]";
#else
    tittle += " [release]";
#endif // debug_v

    m_window.create(sf::VideoMode(resolution.x, resolution.y), tittle, sf::Style::Close);
    m_window.setVerticalSyncEnabled(true);
    m_window.setFramerateLimit(60);

    m_canvas.create(resolution.x, resolution.y);
    m_sprite.setTexture(m_canvas.getTexture());

    m_font.loadFromFile("resources/Arialuni.ttf");
    m_graph.init(m_font);

    m_gui.setTarget(m_canvas);
    m_text.setFont(m_font);

    this->initGUI();

    m_text.setPosition(5, m_window.getSize().y - m_text.getCharacterSize() - 5);
}

//----------------------------

void Core::draw() {

    m_canvas.clear({ 80, 80, 80 });
    m_canvas.draw(m_graph);
    m_gui.draw();
    m_canvas.draw(m_text);
    m_canvas.display();

    m_window.clear();
    m_window.draw(m_sprite);
    m_window.display();
}

//----------------------------

void Core::update() {

    sf::Event event;

    while(m_window.pollEvent(event)) {

        m_graph.handleEvent(event);

        if(event.type == sf::Event::Closed)
            m_window.close();

        else if(event.type == sf::Event::KeyPressed) {
            if(event.key.code == sf::Keyboard::Escape)
                m_window.close();

            else if(event.key.code == sf::Keyboard::Delete) {
                if(m_graph.getFocus() != nullptr) {
                    m_graph.removeVertex(m_graph.getFocus());
                    m_first = nullptr;
                    m_second = nullptr;

                    m_gui.remove(m_editWindow);
                }
            }

            else if(event.key.code == sf::Keyboard::F1) {
                if(m_focus)
                    m_text.setString("dfs: " + m_graph.dfs(m_focus));
            }

            else if(event.key.code == sf::Keyboard::F2) {
                if(m_focus)
                    m_text.setString("bfs: " + m_graph.bfs(m_focus));
            }

            else if(event.key.code == sf::Keyboard::F3) {
                if(m_first && m_second) {
                    auto data = m_graph.dijkstra(m_first, m_second);
                    std::string str = "path not found!";
                    if(data.size() > 1) {
                        str.clear();
                        for(auto it : data) {
                            str += it->getName();
                            if(it != data.back())
                                str += "->";
                        }
                    }

                    m_text.setString("dijkstra: " + str);
                }
            }

            else if(event.key.code == sf::Keyboard::F6) {
                m_graph.clear();
            }
        }

        else if(event.type == sf::Event::MouseButtonPressed) {
            if(event.mouseButton.button == sf::Mouse::Right) {
                m_popup->setVisible(true);
                m_popup->setPosition({ (float)event.mouseButton.x, (float)event.mouseButton.y });
                m_backMousePosition = { (float)event.mouseButton.x, (float)event.mouseButton.y };
            }

            else if(event.mouseButton.button == sf::Mouse::Left && m_popup->isVisible()) {
                if (!m_popup->isMouseOnWidget({(float)event.mouseButton.x, (float)event.mouseButton.y})) {
                    m_popup->setVisible(false);
                }
            }

            else if(event.mouseButton.button == sf::Mouse::Left) {
                m_focus = m_graph.getFocus();
            }
        }

        m_gui.handleEvent(event);

        this->collisionUpdate(m_popup, 0);
    }

    m_graph.update(m_window);

    this->updateGUI();
}

//----------------------------
