#include "DashboardView.h"
#include "UITheme.h"
#include <string.h>

DashboardView::DashboardView() {}

void DashboardView::setup(const sf::Font& font) {
    titleBar.setSize(sf::Vector2f(1100, 60));
    titleBar.setFillColor(OJUI::UITheme::TitleBar());
    
    titleText.setFont(font);
    titleText.setString("Dashboard");
    titleText.setCharacterSize(24);
    titleText.setFillColor(sf::Color::White);
    titleText.setPosition(30, 15);
    
    deleteBtn.setup(font, "Delete Account", 880, 10, 180, 40, OJUI::UITheme::Danger());

    sidebar.setup(font, 0);

    panel.setPosition(210, 75);
    panel.setSize(sf::Vector2f(870, 690));
    panel.setFillColor(OJUI::UITheme::Panel());
    panel.setOutlineThickness(2);
    panel.setOutlineColor(OJUI::UITheme::Border());

    welcomeText.setFont(font);
    welcomeText.setCharacterSize(32);
    welcomeText.setFillColor(OJUI::UITheme::Text());
    welcomeText.setStyle(sf::Text::Bold);
    welcomeText.setPosition(240, 100);

    // Box setup helper
    auto setupBox = [&](sf::RectangleShape& b, sf::Text& t, sf::Text& s, const char* title, const char* sub, float x, float y) {
        b.setPosition(x, y);
        b.setSize(sf::Vector2f(380, 140));
        b.setFillColor(sf::Color::White);
        b.setOutlineThickness(2);
        b.setOutlineColor(OJUI::UITheme::Border());

        t.setFont(font);
        t.setString(title);
        t.setCharacterSize(20);
        t.setFillColor(OJUI::UITheme::Text());
        t.setStyle(sf::Text::Bold);
        t.setPosition(x + 20, y + 20);

        s.setFont(font);
        s.setString(sub);
        s.setCharacterSize(16);
        s.setFillColor(OJUI::UITheme::MutedText());
        s.setPosition(x + 20, y + 60);
    };

    setupBox(browseBox,browseTitle,browseSub,"Browse Problems","50 OOP questions",240,170);
    setupBox(startBox,startTitle,startSub,"Start Contest","Start Contest",660,170);
    setupBox(resumeBox,resumeTitle,resumeSub,"Resume Contest","Load active contest",240,340);
    setupBox(resultBox,resultTitle,resultSub,"View Result","Last saved score",660,340);

    scoreBar.setPosition(230, 520);
    scoreBar.setSize(sf::Vector2f(830, 100));
    scoreBar.setFillColor(sf::Color::White);
    scoreBar.setOutlineThickness(2);
    scoreBar.setOutlineColor(OJUI::UITheme::Border());

    scoreText.setFont(font);
    scoreText.setCharacterSize(18);
    scoreText.setFillColor(OJUI::UITheme::Text());
    scoreText.setStyle(sf::Text::Bold);
    scoreText.setPosition(270, 555);
    scoreText.setString("Active: None | Time: 00:00 | Score: 0");
}

void DashboardView::updateSession(OJ::User* user) {
    if (!user) return;
    char buf[256];
    sprintf(buf, "Welcome, %s", user->getUsername().c_str());
    welcomeText.setString(buf);

    sprintf(buf, "Practice Credits: %d | Contest Credits: %d | Total Credits: %d", 
            user->getPracticeCredits(), user->getContestCredits(),
            user->getPracticeCredits() + user->getContestCredits());
    scoreText.setString(buf);
}

int DashboardView::handleEvent(sf::Event event, sf::RenderWindow& window, OJ::Session& session) {
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
        
        int nav = sidebar.handleClick(mousePos);
        if (nav != -1) return nav; // 0=Dash, 1=ProbBank, 2=Contest, 3=Results, 4=Logout
        
        if (browseBox.getGlobalBounds().contains(mousePos)) return 1;
        if (startBox.getGlobalBounds().contains(mousePos)) return 5; // special for start
        if (resumeBox.getGlobalBounds().contains(mousePos)) return 6; // special for resume
        if (resultBox.getGlobalBounds().contains(mousePos)) return 3;
        if (deleteBtn.contains(mousePos)) return 7; // special for delete account
    }
    return -1;
}

void DashboardView::draw(sf::RenderWindow& window) const {
    window.draw(titleBar);
    window.draw(titleText);
    deleteBtn.draw(window);
    sidebar.draw(window);
    window.draw(panel);
    window.draw(welcomeText);
    
    window.draw(browseBox);
    window.draw(browseTitle);
    window.draw(browseSub);
    
    window.draw(startBox);
    window.draw(startTitle);
    window.draw(startSub);
    
    window.draw(resumeBox);
    window.draw(resumeTitle);
    window.draw(resumeSub);
    
    window.draw(resultBox);
    window.draw(resultTitle);
    window.draw(resultSub);

    window.draw(scoreBar);
    window.draw(scoreText);
}
