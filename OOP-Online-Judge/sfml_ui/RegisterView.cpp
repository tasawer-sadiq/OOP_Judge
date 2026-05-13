#include "RegisterView.h"
#include "UITheme.h"
#include <string.h>

RegisterView::RegisterView() {}

void RegisterView::setup(const sf::Font& font) {
    titleBar.setSize(sf::Vector2f(1100, 60));
    titleBar.setFillColor(OJUI::UITheme::TitleBar());
    
    titleText.setFont(font);
    titleText.setString("Register");
    titleText.setCharacterSize(24);
    titleText.setFillColor(sf::Color::White);
    titleText.setStyle(sf::Text::Bold);
    titleText.setPosition(30, 15);

    panel.setPosition(330, 100);
    panel.setSize(sf::Vector2f(440, 520));
    panel.setFillColor(OJUI::UITheme::Panel());
    panel.setOutlineThickness(2);
    panel.setOutlineColor(OJUI::UITheme::Border());

    heading.setFont(font);
    heading.setString("Create Account");
    heading.setCharacterSize(28);
    heading.setFillColor(OJUI::UITheme::Text());
    heading.setStyle(sf::Text::Bold);
    heading.setPosition(360, 120);

    subHeading.setFont(font);
    subHeading.setString("Enter details to register.");
    subHeading.setCharacterSize(14);
    subHeading.setFillColor(OJUI::UITheme::MutedText());
    subHeading.setPosition(360, 165);

    userLabel.setFont(font);
    userLabel.setString("Username");
    userLabel.setCharacterSize(16);
    userLabel.setFillColor(OJUI::UITheme::Text());
    userLabel.setStyle(sf::Text::Bold);
    userLabel.setPosition(360, 205);

    userIn.setup(font, 360, 230, 380, 40);

    emailLabel.setFont(font);
    emailLabel.setString("Email");
    emailLabel.setCharacterSize(16);
    emailLabel.setFillColor(OJUI::UITheme::Text());
    emailLabel.setStyle(sf::Text::Bold);
    emailLabel.setPosition(360, 290);

    emailIn.setup(font, 360, 315, 380, 40);

    passLabel.setFont(font);
    passLabel.setString("Password");
    passLabel.setCharacterSize(16);
    passLabel.setFillColor(OJUI::UITheme::Text());
    passLabel.setStyle(sf::Text::Bold);
    passLabel.setPosition(360, 375);

    passIn.setup(font, 360, 400, 380, 40, true);

    regBtn.setup(font, "Register Account", 360, 475, 380, 44, OJUI::UITheme::Button());

    errorText.setFont(font);
    errorText.setCharacterSize(14);
    errorText.setFillColor(OJUI::UITheme::Error());
    errorText.setStyle(sf::Text::Bold);
    errorText.setPosition(360, 540);
    errorText.setString("");

    backBtn.setup(font, "<- Back", 330, 65, 80, 30, OJUI::UITheme::Background(), OJUI::UITheme::Text());
}

void RegisterView::clear() {
    userIn.clear();
    emailIn.clear();
    passIn.clear();
    errorText.setString("");
}

int RegisterView::handleEvent(sf::Event event, sf::RenderWindow& window, OJ::UserManager& userManager) {
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
        
        userIn.setFocused(userIn.contains(mousePos));
        emailIn.setFocused(emailIn.contains(mousePos));
        passIn.setFocused(passIn.contains(mousePos));
        emailIn.setError(false);

        if (backBtn.contains(mousePos)) return 1;

        if (regBtn.contains(mousePos)) {
            const char* u = userIn.getText();
            const char* e = emailIn.getText();
            const char* p = passIn.getText();

            if (userManager.usernameExists(u)) {
                errorText.setString("User already exists.");
                return 0;
            }
            if (userManager.emailExists(e)) {
                errorText.setString("Email already exists.");
                return 0;
            }
            const char* ext = strstr(e, "@email.com");
            if (!ext || strcmp(ext, "@email.com") != 0) {
                errorText.setString("Email must end with @email.com");
                emailIn.setError(true);
                return 0;
            }
            
            CustomString token;
            if (userManager.registerUser(u, e, p, token)) {
                userManager.verifyUser(u, token.c_str());
                errorText.setString("Success! You can now login.");
                errorText.setFillColor(OJUI::UITheme::Success());
                return 1; // Back to login
            } else {
                errorText.setString("Registration failed.");
                errorText.setFillColor(OJUI::UITheme::Error());
            }
        }
    } else if (event.type == sf::Event::TextEntered) {
        userIn.handleTextEntered(event.text.unicode);
        emailIn.handleTextEntered(event.text.unicode);
        passIn.handleTextEntered(event.text.unicode);
    }
    return 0;
}

void RegisterView::draw(sf::RenderWindow& window) const {
    window.draw(titleBar);
    window.draw(titleText);
    window.draw(panel);
    window.draw(heading);
    window.draw(subHeading);
    window.draw(userLabel);
    userIn.draw(window);
    window.draw(emailLabel);
    emailIn.draw(window);
    window.draw(passLabel);
    passIn.draw(window);
    regBtn.draw(window);
    backBtn.draw(window);
    window.draw(errorText);
}
