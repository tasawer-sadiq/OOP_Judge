#ifndef OJ_UITHEME_H
#define OJ_UITHEME_H

#include <SFML/Graphics.hpp>

namespace OJUI {
    struct UITheme {
        static sf::Color Background();
        static sf::Color TitleBar();
        static sf::Color Panel();
        static sf::Color Border();
        static sf::Color Button();
        static sf::Color ButtonDark();
        static sf::Color ButtonGray();
        static sf::Color Danger();
        static sf::Color Text();
        static sf::Color MutedText();
        static sf::Color Error();
        static sf::Color Success();
    };
}

#endif
