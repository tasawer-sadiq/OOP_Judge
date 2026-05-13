#ifndef OJ_REGISTERVIEW_H
#define OJ_REGISTERVIEW_H

#include <SFML/Graphics.hpp>
#include "UIButton.h"
#include "UITextInput.h"
#include "../include/UserManager.h"

class RegisterView {
private:
    sf::RectangleShape titleBar;
    sf::Text titleText;
    sf::RectangleShape panel;
    sf::Text heading;
    sf::Text subHeading;
    
    sf::Text userLabel;
    sf::Text emailLabel;
    sf::Text passLabel;
    sf::Text errorText;

    UITextInput userIn;
    UITextInput emailIn;
    UITextInput passIn;
    UIButton regBtn;
    UIButton backBtn;

public:
    RegisterView();
    void setup(const sf::Font& font);
    void clear();
    // returns 1 to go to login, 0 to stay
    int handleEvent(sf::Event event, sf::RenderWindow& window, OJ::UserManager& userManager);
    void draw(sf::RenderWindow& window) const;
};

#endif
