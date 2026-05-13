#ifndef OJ_RESULTVIEW_H
#define OJ_RESULTVIEW_H

#include <SFML/Graphics.hpp>
#include "UIButton.h"
#include "../include/ContestResult.h"

class ResultView {
private:
    sf::RectangleShape titleBar;
    sf::Text titleText;
    sf::RectangleShape panel;
    sf::Text heading;
    sf::Text scoreText;
    
    sf::Text typeText;
    sf::Text solvedText;
    sf::Text wrongText;
    sf::Text minutesText;

    UIButton dashBtn;
    UIButton probBtn;

public:
    ResultView();
    void setup(const sf::Font& font);
    void updateResult(const sf::Font& font, const OJ::ContestResult& res);
    int handleEvent(sf::Event event, sf::RenderWindow& window);
    void draw(sf::RenderWindow& window) const;
};

#endif
