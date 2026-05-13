#ifndef OJ_CONTESTVIEW_H
#define OJ_CONTESTVIEW_H

#include <SFML/Graphics.hpp>
#include "UISidebar.h"
#include "UIButton.h"
#include "UITextInput.h"
#include "../include/Contest.h"
#include "../include/ProblemBank.h"
#include "../include/EvaluationEngine.h"
#include "../include/Session.h"
#include "../include/ContestManager.h"

class ContestView {
private:
    sf::RectangleShape titleBar;
    sf::Text titleText;
    UISidebar sidebar;
    
    sf::RectangleShape panel;
    sf::RectangleShape scoreBar;
    sf::Text scoreText;
    
    sf::Text noteText;
    sf::Text qTitle;
    sf::Text diffText;
    sf::RectangleShape statementBox;
    sf::Text statementText;
    
    sf::Text ansLabel;
    UITextInput ansIn;
    
    UIButton submitBtn;
    UIButton saveBtn;
    UIButton endBtn;

    // Setup mode elements
    bool inSetupMode;
    sf::Text setupTitle;
    UIButton easyBtn;
    UIButton stdBtn;
    UIButton hardBtn;
    sf::Text diffLabel;

    // Active mode elements
    sf::Text activeDiffLabel;
    UITextInput activeDiffIn;
    sf::Text verdictText; // For "Problem already solved" etc.

    int currentQIndex;

public:
    ContestView();
    void setup(const sf::Font& font);
    void setSetupMode(bool mode);
    void updateScore(const sf::Font& font, OJ::Contest* contest);
    void updateProblem(const sf::Font& font, OJ::Contest* contest, OJ::ProblemBank& bank);
    // returns nav index, or 0 if dashboard, -1 to stay, 10+variant for start contest
    int handleEvent(sf::Event event, sf::RenderWindow& window, OJ::Contest* contest, OJ::ProblemBank& bank, OJ::EvaluationEngine& engine, OJ::ContestManager& cm, OJ::Session& session, OJ::UserManager& userManager);
    void draw(sf::RenderWindow& window) const;
};

#endif
