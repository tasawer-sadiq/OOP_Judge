#ifndef OJ_PROBLEMBANKVIEW_H
#define OJ_PROBLEMBANKVIEW_H

#include <SFML/Graphics.hpp>
#include "UISidebar.h"
#include "UIButton.h"
#include "UITextInput.h"
#include "../include/ProblemBank.h"
#include "../include/EvaluationEngine.h"
#include "../include/Session.h"
#include "../include/UserManager.h"

class ProblemBankView {
private:
    sf::RectangleShape titleBar;
    sf::Text titleText;
    UISidebar sidebar;
    
    // Left panel
    sf::RectangleShape leftPanel;
    sf::Text leftTitle;
    sf::Text diffLabel;
    UITextInput diffIn;
    UIButton filterBtn;
    
    sf::RectangleShape scrollArea;
    sf::RectangleShape scrollbarBg;
    sf::RectangleShape scrollThumb;
    sf::Text scrollHint;

    struct ProblemRow {
        sf::RectangleShape box;
        sf::Text text;
        int id;
    };
    ProblemRow rows[6];
    
    // Right panel
    sf::RectangleShape rightPanel;
    sf::Text rightTitle;
    sf::Text diffText;
    sf::RectangleShape statementBox;
    sf::Text statementText;
    sf::Text ansLabel;
    UITextInput ansIn;
    UIButton submitBtn;
    sf::Text verdictText;

    OJ::Problem** filteredProblems;
    int filteredCount;
    int scrollIndex;
    int selectedIndex;

    void updateRows(const sf::Font& font);
    void updateRightPanel(const sf::Font& font);

public:
    ProblemBankView();
    ~ProblemBankView();
    void setup(const sf::Font& font, OJ::ProblemBank& bank);
    int handleEvent(sf::Event event, sf::RenderWindow& window, OJ::ProblemBank& bank, OJ::EvaluationEngine& engine, OJ::Session& session, OJ::UserManager& userManager);
    void draw(sf::RenderWindow& window) const;
};

#endif
