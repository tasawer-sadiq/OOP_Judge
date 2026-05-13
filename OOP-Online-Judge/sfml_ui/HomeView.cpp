#include "HomeView.h"
#include "UITheme.h"

namespace OJUI {

HomeView::HomeView() {}

void HomeView::setup(const sf::Font& font) {
    background.setSize(sf::Vector2f(1100, 780));
    background.setFillColor(UITheme::TitleBar());

    titleText.setFont(font);
    titleText.setString("OOP JUDGE");
    titleText.setCharacterSize(100); // Increased size
    titleText.setFillColor(sf::Color::White);
    titleText.setStyle(sf::Text::Bold);
    titleText.setOutlineThickness(3); // Makes it look even bolder
    titleText.setOutlineColor(sf::Color::White);

    // Center title
    sf::FloatRect textBounds = titleText.getLocalBounds();
    titleText.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
    titleText.setPosition(550, 300);

    // Setup circular ">" button
    float radius = 50.0f;
    startCircle.setRadius(radius);
    startCircle.setFillColor(UITheme::Button());
    startCircle.setOutlineThickness(3);
    startCircle.setOutlineColor(UITheme::ButtonDark());
    startCircle.setOrigin(radius, radius);
    startCircle.setPosition(550, 500);

    startText.setFont(font);
    startText.setString(">");
    startText.setCharacterSize(50);
    startText.setFillColor(sf::Color::White);
    startText.setStyle(sf::Text::Bold);
    
    sf::FloatRect btnTextBounds = startText.getLocalBounds();
    startText.setOrigin(btnTextBounds.left + btnTextBounds.width / 2.0f, btnTextBounds.top + btnTextBounds.height / 2.0f);
    startText.setPosition(550, 495); // Slightly offset for visual centering
}

int HomeView::handleEvent(sf::Event event, sf::RenderWindow& window) {
    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        // Check distance for circular button
        sf::Vector2f btnPos = startCircle.getPosition();
        float distSq = (mousePos.x - btnPos.x)*(mousePos.x - btnPos.x) + (mousePos.y - btnPos.y)*(mousePos.y - btnPos.y);
        if (distSq <= startCircle.getRadius() * startCircle.getRadius()) {
            return 1;
        }
    }
    return 0;
}

void HomeView::draw(sf::RenderWindow& window) const {
    window.draw(background);
    window.draw(titleText);
    window.draw(startCircle);
    window.draw(startText);
}

}
