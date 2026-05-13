#ifndef OJ_UISIDEBAR_H
#define OJ_UISIDEBAR_H

#include <SFML/Graphics.hpp>

class UISidebar {
private:
    sf::RectangleShape bg;
    sf::Text brandText;
    
    struct NavItem {
        sf::RectangleShape box;
        sf::Text text;
        int screenType; // 0=Dash, 1=ProbBank, 2=Contest, 3=Results, 4=Logout
    };
    NavItem items[5];

public:
    UISidebar();
    void setup(const sf::Font& font, int activeScreen); // ActiveScreen: 0,1,2,3
    void draw(sf::RenderWindow& window) const;
    int handleClick(sf::Vector2f mousePos) const; // returns clicked screen type, or -1
};

#endif
