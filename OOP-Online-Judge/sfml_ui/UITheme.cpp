#include "UITheme.h"

namespace OJUI {
    sf::Color UITheme::Background() { return sf::Color(244, 246, 250); } // #f4f6fa
    sf::Color UITheme::TitleBar() { return sf::Color(17, 24, 39); } // #111827
    sf::Color UITheme::Panel() { return sf::Color(255, 255, 255); } // #ffffff
    sf::Color UITheme::Border() { return sf::Color(197, 204, 214); } // #c5ccd6
    sf::Color UITheme::Button() { return sf::Color(37, 99, 235); } // #2563eb
    sf::Color UITheme::ButtonDark() { return sf::Color(29, 78, 216); } // #1d4ed8
    sf::Color UITheme::ButtonGray() { return sf::Color(229, 231, 235); } // #e5e7eb
    sf::Color UITheme::Danger() { return sf::Color(185, 28, 28); } // #b91c1c
    sf::Color UITheme::Text() { return sf::Color::Black; } // #000000
    sf::Color UITheme::MutedText() { return sf::Color::Black; } // #000000
    sf::Color UITheme::Error() { return sf::Color(220, 38, 38); } // #dc2626
    sf::Color UITheme::Success() { return sf::Color(22, 101, 52); } // #166534
}
