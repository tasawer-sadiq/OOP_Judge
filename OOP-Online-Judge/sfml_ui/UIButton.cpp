#include "UIButton.h"
#include "UITheme.h"

UIButton::UIButton() {}

void UIButton::setup(const sf::Font& font, const char* text, float x, float y, float w, float h, sf::Color bgColor, sf::Color textColor) {
    box.setPosition(x, y);
    box.setSize(sf::Vector2f(w, h));
    box.setFillColor(bgColor);
    box.setOutlineThickness(2);
    
    // adjust border color based on background
    if (bgColor == OJUI::UITheme::Button()) box.setOutlineColor(OJUI::UITheme::ButtonDark());
    else if (bgColor == OJUI::UITheme::ButtonGray()) box.setOutlineColor(sf::Color(156, 163, 175));
    else if (bgColor == OJUI::UITheme::Danger()) box.setOutlineColor(sf::Color(153, 27, 27));
    else box.setOutlineColor(OJUI::UITheme::Border());

    label.setFont(font);
    label.setString(text);
    label.setCharacterSize(14);
    label.setFillColor(textColor);
    
    // Center text
    sf::FloatRect textBounds = label.getLocalBounds();
    label.setPosition(x + (w - textBounds.width) / 2.0f, y + (h - textBounds.height) / 2.0f - textBounds.top);
}

void UIButton::setText(const char* text) {
    label.setString(text);
    sf::FloatRect textBounds = label.getLocalBounds();
    label.setPosition(box.getPosition().x + (box.getSize().x - textBounds.width) / 2.0f, box.getPosition().y + (box.getSize().y - textBounds.height) / 2.0f - textBounds.top);
}

void UIButton::draw(sf::RenderWindow& window) const {
    window.draw(box);
    window.draw(label);
}

bool UIButton::contains(sf::Vector2f mousePos) const {
    return box.getGlobalBounds().contains(mousePos);
}
