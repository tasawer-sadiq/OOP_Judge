#ifndef OJ_UITEXTINPUT_H
#define OJ_UITEXTINPUT_H

#include <SFML/Graphics.hpp>
#include <string.h>

class UITextInput {
private:
    sf::RectangleShape box;
    sf::Text text;
    bool focused;
    char buffer[256];
    int length;
    bool isPassword;

public:
    UITextInput();
    void setup(const sf::Font& font, float x, float y, float w, float h, bool isPassword = false);
    void setFocused(bool value);
    bool isFocused() const { return focused; }
    void clear();
    void setText(const char* value);
    const char* getText() const;
    void handleTextEntered(sf::Uint32 unicode);
    void draw(sf::RenderWindow& window) const;
    bool contains(sf::Vector2f mousePos) const;
    void setError(bool error);
    void setBold(bool bold);
};

#endif
