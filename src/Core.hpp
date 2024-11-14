//----------------------------

#ifndef CORE_HPP_INCLUDED
#define CORE_HPP_INCLUDED

//----------------------------

#include "include_libs.hpp"

#include "Graph.hpp"

//----------------------------

class Core {
public:

    Core();
    ~Core();

    void init(sf::Vector2u resolution);

    void draw();
    void update();

    bool windowIsOpen() { return m_window.isOpen(); }

private:

    sf::RenderWindow m_window;
    sf::RenderTexture m_canvas;
    sf::Sprite m_sprite;

    sf::Vector2f m_backMousePosition;

    sf::Font m_font;

    Graph m_graph;

    tgui::Gui m_gui;
    tgui::ListBox::Ptr  m_popup;

    tgui::ChildWindow::Ptr m_addEdge;
    tgui::Label::Ptr    m_label0,
                        m_label1;

    tgui::EditBox::Ptr m_editBox[2];
    tgui::CheckBox::Ptr m_check[2];

    tgui::ChildWindow::Ptr m_editWindow;
    tgui::ScrollablePanel::Ptr m_scrollablePanel;

    tgui::FileDialog::Ptr m_fileDialog;

    tgui::ChildWindow::Ptr m_matrixWindow;
    tgui::ScrollablePanel::Ptr m_scrollableMatrix;
    tgui::Button::Ptr m_button;

    sf::Text m_text;

    Vertex *m_first, *m_second, *m_focus, *m_saveFocus;

    bool m_stateLoad;

    void initGUI();
    void updateGUI();
    void updateMatrix();

    void collisionUpdate(tgui::Widget::Ptr widget, float tittleOffset = 20);
};

//----------------------------

#endif // CORE_HPP_INCLUDED

//----------------------------
