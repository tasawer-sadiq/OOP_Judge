#ifndef OJ_LOGINVIEW_H
#define OJ_LOGINVIEW_H

#include <SFML/Graphics.hpp>
#include "UIButton.h"
#include "UITextInput.h"
#include "../include/UserManager.h"
#include "../include/Session.h"

class LoginView {
private:
    sf::RectangleShape titleBar;
    sf::Text titleText;
    sf::RectangleShape panel;
    sf::Text heading;
    sf::Text subHeading;
    sf::Text userLabel;
    sf::Text passLabel;
    sf::Text errorText;

    UITextInput userIn;
    UITextInput passIn;
    UIButton loginBtn;
    UIButton regBtn;

public:
    LoginView();
    void setup(const sf::Font& font);
    void clear();
    // returns 1 to go to dashboard, 2 to go to register, 0 to stay
    int handleEvent(sf::Event event, sf::RenderWindow& window, OJ::UserManager& userManager, OJ::Session& session);
    void draw(sf::RenderWindow& window) const;
};

#endif
