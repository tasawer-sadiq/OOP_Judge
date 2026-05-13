#ifndef OJ_DASHBOARDVIEW_H
#define OJ_DASHBOARDVIEW_H

#include <SFML/Graphics.hpp>
#include "UISidebar.h"
#include "UIButton.h"
#include "../include/Session.h"

class DashboardView {
private:
    sf::RectangleShape titleBar;
    sf::Text titleText;
    UISidebar sidebar;
    UIButton deleteBtn;
    sf::RectangleShape panel;
    sf::Text welcomeText;
    
    sf::RectangleShape browseBox;
    sf::Text browseTitle;
    sf::Text browseSub;
    
    sf::RectangleShape startBox;
    sf::Text startTitle;
    sf::Text startSub;
    
    sf::RectangleShape resumeBox;
    sf::Text resumeTitle;
    sf::Text resumeSub;
    
    sf::RectangleShape resultBox;
    sf::Text resultTitle;
    sf::Text resultSub;

    sf::RectangleShape scoreBar;
    sf::Text scoreText;

public:
    DashboardView();
    void setup(const sf::Font& font);
    void updateSession(OJ::User* user);
    // returns screen index or -1
    int handleEvent(sf::Event event, sf::RenderWindow& window, OJ::Session& session);
    void draw(sf::RenderWindow& window) const;
};

#endif
