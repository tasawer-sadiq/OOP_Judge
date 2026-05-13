#include "ResultView.h"
#include "UITheme.h"
#include <string.h>

ResultView::ResultView() {}

void ResultView::setup(const sf::Font& font) {
    titleBar.setSize(sf::Vector2f(1100, 60));
    titleBar.setFillColor(OJUI::UITheme::TitleBar());
    
    titleText.setFont(font);
    titleText.setString("Result");
    titleText.setCharacterSize(24);
    titleText.setFillColor(sf::Color::White);
    titleText.setStyle(sf::Text::Bold);
    titleText.setPosition(30, 15);

    panel.setPosition(300, 80);
    panel.setSize(sf::Vector2f(500, 550));
    panel.setFillColor(OJUI::UITheme::Panel());
    panel.setOutlineThickness(2);
    panel.setOutlineColor(OJUI::UITheme::Border());

    heading.setFont(font);
    heading.setString("Contest Result");
    heading.setCharacterSize(32);
    heading.setFillColor(OJUI::UITheme::Text());
    heading.setStyle(sf::Text::Bold);
    heading.setPosition(330, 100);

    scoreText.setFont(font);
    scoreText.setString("Score: 0");
    scoreText.setCharacterSize(40);
    scoreText.setFillColor(OJUI::UITheme::Button());
    scoreText.setStyle(sf::Text::Bold);
    scoreText.setPosition(330, 150);

    auto setupRow = [&](sf::Text& t, float y) {
        t.setFont(font);
        t.setCharacterSize(22);
        t.setFillColor(OJUI::UITheme::Text());
        t.setStyle(sf::Text::Bold);
        t.setPosition(330, y);
    };

    setupRow(typeText, 220);
    setupRow(solvedText, 280);
    setupRow(wrongText, 340);
    setupRow(minutesText, 400);

    dashBtn.setup(font, "Dashboard", 330, 480, 180, 50, OJUI::UITheme::Button());
    probBtn.setup(font, "Problems", 530, 480, 180, 50, OJUI::UITheme::ButtonGray(), OJUI::UITheme::Text());
}

void ResultView::updateResult(const sf::Font& font, const OJ::ContestResult& res) {
    char buf[256];
    sprintf(buf, "Score: %d", res.getTotalScore());
    scoreText.setString(buf);

    sprintf(buf, "Contest Type:\n%s", res.getContestType().c_str());
    typeText.setString(buf);

    sprintf(buf, "Solved: %d", res.getSolvedCount());
    solvedText.setString(buf);

    sprintf(buf, "Wrong Attempts: %d", res.getWrongAttempts());
    wrongText.setString(buf);

    sprintf(buf, "Minutes Used: %d", res.getMinutesUsed());
    minutesText.setString(buf);
}

int ResultView::handleEvent(sf::Event event, sf::RenderWindow& window) {
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
        if (dashBtn.contains(mousePos)) return 0;
        if (probBtn.contains(mousePos)) return 1;
    }
    return -1;
}

void ResultView::draw(sf::RenderWindow& window) const {
    window.draw(titleBar);
    window.draw(titleText);
    window.draw(panel);
    window.draw(heading);
    window.draw(scoreText);
    window.draw(typeText);
    window.draw(solvedText);
    window.draw(wrongText);
    window.draw(minutesText);
    dashBtn.draw(window);
    probBtn.draw(window);
}
