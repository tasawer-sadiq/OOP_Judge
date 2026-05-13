#include "LoginView.h"
#include "UITheme.h"

LoginView::LoginView() {}

void LoginView::setup(const sf::Font& font) {
    titleBar.setSize(sf::Vector2f(1100, 60));
    titleBar.setFillColor(OJUI::UITheme::TitleBar());
    
    titleText.setFont(font);
    titleText.setString("Login");
    titleText.setCharacterSize(24);
    titleText.setFillColor(sf::Color::White);
    titleText.setStyle(sf::Text::Bold);
    titleText.setPosition(30, 15);

    panel.setPosition(350, 140);
    panel.setSize(sf::Vector2f(400, 420));
    panel.setFillColor(OJUI::UITheme::Panel());
    panel.setOutlineThickness(2);
    panel.setOutlineColor(OJUI::UITheme::Border());

    heading.setFont(font);
    heading.setString("OOP Online Judge");
    heading.setCharacterSize(28);
    heading.setFillColor(OJUI::UITheme::Text());
    heading.setStyle(sf::Text::Bold);
    heading.setPosition(380, 160);

    subHeading.setFont(font);
    subHeading.setString("Login with username and password.");
    subHeading.setCharacterSize(14);
    subHeading.setFillColor(OJUI::UITheme::MutedText());
    subHeading.setPosition(380, 205);

    userLabel.setFont(font);
    userLabel.setString("Username");
    userLabel.setCharacterSize(16);
    userLabel.setFillColor(OJUI::UITheme::Text());
    userLabel.setStyle(sf::Text::Bold);
    userLabel.setPosition(380, 250);

    userIn.setup(font, 380, 275, 340, 40);

    passLabel.setFont(font);
    passLabel.setString("Password");
    passLabel.setCharacterSize(16);
    passLabel.setFillColor(OJUI::UITheme::Text());
    passLabel.setStyle(sf::Text::Bold);
    passLabel.setPosition(380, 330);

    passIn.setup(font, 380, 355, 340, 40, true);

    loginBtn.setup(font, "Login", 380, 425, 160, 42, OJUI::UITheme::Button());
    regBtn.setup(font, "Register", 560, 425, 160, 42, OJUI::UITheme::ButtonGray(), OJUI::UITheme::Text());

    errorText.setFont(font);
    errorText.setCharacterSize(14);
    errorText.setFillColor(OJUI::UITheme::Error());
    errorText.setStyle(sf::Text::Bold);
    errorText.setPosition(380, 490);
    errorText.setString("");
}

void LoginView::clear() {
    userIn.clear();
    passIn.clear();
    errorText.setString("");
}

int LoginView::handleEvent(sf::Event event, sf::RenderWindow& window, OJ::UserManager& userManager, OJ::Session& session) {
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
        
        userIn.setFocused(userIn.contains(mousePos));
        passIn.setFocused(passIn.contains(mousePos));

        if (loginBtn.contains(mousePos)) {
            if (userManager.login(userIn.getText(), passIn.getText(), session)) {
                errorText.setString("");
                return 1; // Dashboard
            } else {
                errorText.setString("Invalid credentials or unverified user.");
            }
        } else if (regBtn.contains(mousePos)) {
            errorText.setString("");
            return 2; // Register
        }
    } else if (event.type == sf::Event::TextEntered) {
        userIn.handleTextEntered(event.text.unicode);
        passIn.handleTextEntered(event.text.unicode);
    }
    return 0;
}

void LoginView::draw(sf::RenderWindow& window) const {
    window.draw(titleBar);
    window.draw(titleText);
    window.draw(panel);
    window.draw(heading);
    window.draw(subHeading);
    window.draw(userLabel);
    userIn.draw(window);
    window.draw(passLabel);
    passIn.draw(window);
    loginBtn.draw(window);
    regBtn.draw(window);
    window.draw(errorText);
}
