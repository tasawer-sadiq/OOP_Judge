#include "UISidebar.h"
#include "UITheme.h"

UISidebar::UISidebar() {}

void UISidebar::setup(const sf::Font& font, int activeScreen) {
    bg.setPosition(0, 60);
    bg.setSize(sf::Vector2f(200, 720));
    bg.setFillColor(OJUI::UITheme::TitleBar());

    brandText.setFont(font);
    brandText.setString("OOP Judge");
    brandText.setCharacterSize(20);
    brandText.setStyle(sf::Text::Bold);
    brandText.setFillColor(sf::Color::White);
    brandText.setPosition(20, 80);

    const char* labels[] = {"Dashboard", "Problem Bank", "Contest", "Results", "Logout"};
    float y = 130;
    for (int i = 0; i < 5; i++) {
        items[i].screenType = i;
        
        items[i].box.setPosition(20, y);
        items[i].box.setSize(sf::Vector2f(160, 42));
        items[i].box.setOutlineThickness(1);
        
        if (i == activeScreen) {
            items[i].box.setFillColor(OJUI::UITheme::Button());
            items[i].box.setOutlineColor(OJUI::UITheme::Button());
        } else {
            items[i].box.setFillColor(sf::Color::Transparent);
            items[i].box.setOutlineColor(sf::Color(71, 85, 105)); // #475569
        }

        items[i].text.setFont(font);
        items[i].text.setString(labels[i]);
        items[i].text.setCharacterSize(16);
        items[i].text.setFillColor(sf::Color::White);
        items[i].text.setPosition(30, y + 10);

        y += 55;
    }
}

void UISidebar::draw(sf::RenderWindow& window) const {
    window.draw(bg);
    window.draw(brandText);
    for (int i = 0; i < 5; i++) {
        window.draw(items[i].box);
        window.draw(items[i].text);
    }
}

int UISidebar::handleClick(sf::Vector2f mousePos) const {
    for (int i = 0; i < 5; i++) {
        if (items[i].box.getGlobalBounds().contains(mousePos)) {
            return i;
        }
    }
    return -1;
}
