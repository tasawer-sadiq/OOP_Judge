#include "UITextInput.h"
#include "UITheme.h"
#include <string>

UITextInput::UITextInput() : focused(false), length(0), isPassword(false) {
    buffer[0] = '\0';
}

void UITextInput::setup(const sf::Font& font, float x, float y, float w, float h, bool isPassword) {
    this->isPassword = isPassword;
    box.setPosition(x, y);
    box.setSize(sf::Vector2f(w, h));
    box.setFillColor(OJUI::UITheme::Panel());
    box.setOutlineThickness(2);
    box.setOutlineColor(sf::Color(148, 163, 184)); // #94a3b8

    text.setFont(font);
    text.setCharacterSize(14);
    text.setFillColor(OJUI::UITheme::Text());
    text.setPosition(x + 10, y + 8);
    clear();
}

void UITextInput::setFocused(bool value) {
    focused = value;
    if (focused) box.setOutlineColor(OJUI::UITheme::Button());
    else box.setOutlineColor(sf::Color(148, 163, 184));
}

void UITextInput::setError(bool error) {
    if (error) box.setOutlineColor(OJUI::UITheme::Error());
    else setFocused(focused); // reset to focused or unfocused
}

void UITextInput::clear() {
    length = 0;
    buffer[0] = '\0';
    text.setString("");
}

void UITextInput::setText(const char* value) {
    strncpy(buffer, value, 255);
    buffer[255] = '\0';
    length = strlen(buffer);
    
    if (isPassword) {
        std::string s(length, '*');
        sf::String stars(s);
        text.setString(stars);
    } else {
        text.setString(buffer);
    }
}

const char* UITextInput::getText() const {
    return buffer;
}

void UITextInput::handleTextEntered(sf::Uint32 unicode) {
    if (!focused) return;
    
    if (unicode == 8) { // Backspace
        if (length > 0) {
            length--;
            buffer[length] = '\0';
        }
    } else if (unicode >= 32 && unicode < 127) { // Printable ASCII
        if (length < 255) {
            buffer[length] = static_cast<char>(unicode);
            length++;
            buffer[length] = '\0';
        }
    }
    
    if (isPassword) {
        std::string s(length, '*');
        sf::String stars(s);
        text.setString(stars);
    } else {
        text.setString(buffer);
    }
}

void UITextInput::draw(sf::RenderWindow& window) const {
    window.draw(box);
    window.draw(text);
}

bool UITextInput::contains(sf::Vector2f mousePos) const {
    return box.getGlobalBounds().contains(mousePos);
}

void UITextInput::setBold(bool bold) {
    if (bold) text.setStyle(sf::Text::Bold);
    else text.setStyle(sf::Text::Regular);
}
