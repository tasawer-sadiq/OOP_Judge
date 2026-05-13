#ifndef OJ_UIBUTTON_H
#define OJ_UIBUTTON_H

#include <SFML/Graphics.hpp>

class UIButton {
private:
    sf::RectangleShape box;
    sf::Text label;

public:
    UIButton();
    void setup(const sf::Font& font, const char* text, float x, float y, float w, float h, sf::Color bgColor, sf::Color textColor = sf::Color::White);
    void draw(sf::RenderWindow& window) const;
    bool contains(sf::Vector2f mousePos) const;
    void setText(const char* text);
};

#endif
