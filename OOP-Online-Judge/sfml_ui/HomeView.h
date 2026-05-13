#ifndef OJ_HOMEVIEW_H
#define OJ_HOMEVIEW_H

#include <SFML/Graphics.hpp>
#include "UIButton.h"

namespace OJUI {

class HomeView {
private:
    sf::Text titleText;
    sf::RectangleShape background;

    // Start Button (Circular)
    sf::CircleShape startCircle;
    sf::Text startText;

public:
    HomeView();
    void setup(const sf::Font& font);
    // returns 1 to go to login screen, 0 to stay
    int handleEvent(sf::Event event, sf::RenderWindow& window);
    void draw(sf::RenderWindow& window) const;
};

}

#endif
