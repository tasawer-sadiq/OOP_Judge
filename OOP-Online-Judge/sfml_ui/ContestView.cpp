#include "ContestView.h"
#include "UITheme.h"
#include <string.h>
#include <cstdio>

ContestView::ContestView() : currentQIndex(0), inSetupMode(false) {}

void ContestView::setSetupMode(bool mode) {
    inSetupMode = mode;
    if (mode) currentQIndex = 0;
}

void ContestView::setup(const sf::Font& font) {
    // Window is now 1100x780
    titleBar.setSize(sf::Vector2f(1100, 60));
    titleBar.setFillColor(OJUI::UITheme::TitleBar());
    
    titleText.setFont(font);
    titleText.setString("Contest");
    titleText.setCharacterSize(24);
    titleText.setFillColor(sf::Color::White);
    titleText.setStyle(sf::Text::Bold);
    titleText.setPosition(30, 15);

    sidebar.setup(font, 2);

    // Main panel: starts at x=210 (after sidebar), spans to 1080
    panel.setPosition(210, 75);
    panel.setSize(sf::Vector2f(870, 690));
    panel.setFillColor(OJUI::UITheme::Panel());
    panel.setOutlineThickness(2);
    panel.setOutlineColor(OJUI::UITheme::Border());

    // Score bar at top of panel
    scoreBar.setPosition(225, 90);
    scoreBar.setSize(sf::Vector2f(840, 65));
    scoreBar.setFillColor(sf::Color::White);
    scoreBar.setOutlineThickness(2);
    scoreBar.setOutlineColor(OJUI::UITheme::Border());

    scoreText.setFont(font);
    scoreText.setCharacterSize(20);
    scoreText.setFillColor(OJUI::UITheme::Text());
    scoreText.setStyle(sf::Text::Bold);
    scoreText.setPosition(245, 113);

    noteText.setFont(font);
    noteText.setString("Contest: Time runs down. Wrong answers move to next problem. Correct answers replace with a fresh question.");
    noteText.setCharacterSize(13);
    noteText.setFillColor(OJUI::UITheme::MutedText());
    noteText.setPosition(225, 168);

    // Question title + difficulty on same row
    qTitle.setFont(font);
    qTitle.setString("Question");
    qTitle.setCharacterSize(22);
    qTitle.setFillColor(OJUI::UITheme::Text());
    qTitle.setStyle(sf::Text::Bold);
    qTitle.setPosition(225, 195);

    diffText.setFont(font);
    diffText.setString("");
    diffText.setCharacterSize(16);
    diffText.setFillColor(sf::Color(59, 130, 246));  // blue badge
    diffText.setStyle(sf::Text::Bold);
    diffText.setPosition(225, 222);

    // Statement box: much taller now (280 height)
    statementBox.setPosition(225, 265);
    statementBox.setSize(sf::Vector2f(845, 280));
    statementBox.setFillColor(sf::Color(248, 250, 252));
    statementBox.setOutlineThickness(2);
    statementBox.setOutlineColor(OJUI::UITheme::Border());

    statementText.setFont(font);
    statementText.setString("");
    statementText.setCharacterSize(16);
    statementText.setFillColor(OJUI::UITheme::Text());
    statementText.setStyle(sf::Text::Bold);
    statementText.setPosition(240, 279);

    // Answer section below statement box
    ansLabel.setFont(font);
    ansLabel.setString("Your Answer");
    ansLabel.setCharacterSize(18);
    ansLabel.setFillColor(OJUI::UITheme::Text());
    ansLabel.setStyle(sf::Text::Bold);
    ansLabel.setPosition(225, 565);

    ansIn.setup(font, 225, 592, 845, 48);

    // Buttons row at bottom
    submitBtn.setup(font, "Submit", 225, 665, 260, 50, OJUI::UITheme::Button());
    saveBtn.setup(font, "Save Back", 505, 665, 260, 50, OJUI::UITheme::ButtonGray(), OJUI::UITheme::Text());
    endBtn.setup(font, "End Contest", 785, 665, 260, 50, OJUI::UITheme::Danger());

    // Setup mode elements
    setupTitle.setFont(font);
    setupTitle.setString("Select Contest Difficulty");
    setupTitle.setCharacterSize(28);
    setupTitle.setFillColor(OJUI::UITheme::Text());
    setupTitle.setStyle(sf::Text::Bold);
    setupTitle.setPosition(375, 150);

    easyBtn.setup(font, "Easy", 375, 220, 140, 50, sf::Color(34, 197, 94));
    stdBtn.setup(font, "Standard", 530, 220, 140, 50, sf::Color(59, 130, 246));
    hardBtn.setup(font, "Hard", 685, 220, 140, 50, sf::Color(239, 68, 68));

    diffLabel.setFont(font);
    diffLabel.setString("Select a category to begin");
    diffLabel.setCharacterSize(18);
    diffLabel.setFillColor(OJUI::UITheme::Text());
    diffLabel.setPosition(375, 310);

    // Active mode elements (on the right)
    activeDiffLabel.setFont(font);
    activeDiffLabel.setString("Difficulty:");
    activeDiffLabel.setCharacterSize(18);
    activeDiffLabel.setFillColor(OJUI::UITheme::Text());
    activeDiffLabel.setPosition(830, 217);

    activeDiffIn.setup(font, 930, 212, 100, 35);
    activeDiffIn.setText("1");

    verdictText.setFont(font);
    verdictText.setCharacterSize(16);
    verdictText.setStyle(sf::Text::Bold);
    verdictText.setPosition(225, 715);
    verdictText.setString("");
}

void ContestView::updateScore(const sf::Font& font, OJ::Contest* contest) {
    if (!contest) return;
    int sec = contest->remainingSeconds();
    int min = sec / 60;
    sec = sec % 60;
    
    int solvedC = contest->getTotalSolved();
    int wrongAttempts = 0;
    for (int i=0; i<contest->problemCount(); i++) {
        bool solved = contest->isSolvedAt(i);
        wrongAttempts += OJ::ScoreCalculator::computeWrongAttempts(contest->getAttemptsAt(i), solved);
    }
    
    char buf[256];
    sprintf(buf, "Time: %02d:%02d | Solved: %d | Wrong: %d", 
        min, sec, solvedC, wrongAttempts);
    scoreText.setString(buf);
}

// Difficulty level to label
static const char* contestDiffLabel(int d) {
    if (d <= 1) return "Easy (Difficulty: 1)";
    if (d == 2) return "Medium (Difficulty: 2)";
    if (d == 3) return "Hard (Difficulty: 3)";
    return "Expert (Difficulty: 4+)";
}

void ContestView::updateProblem(const sf::Font& font, OJ::Contest* contest, OJ::ProblemBank& bank) {
    verdictText.setString("");
    if (!contest || currentQIndex >= contest->problemCount()) {
        qTitle.setString("No more questions.");
        diffText.setString("");
        statementText.setString("");
        return;
    }
    
    int pId = contest->problemIdAt(currentQIndex);
    OJ::Problem* p = bank.getById(pId);
    if (p) {
        char buf[200];
        sprintf(buf, "Q%d: %s", currentQIndex + 1, p->getTitle().c_str());
        qTitle.setString(buf);

        // Show difficulty label
        diffText.setString(contestDiffLabel(p->getDifficulty()));
        
        // Wrap statement at ~80 chars (fits the wider 845px box)
        CustomString s = p->getStatement();
        char wrapped[2048];
        int pos = 0;
        int lastNL = 0;
        for (int i=0; s.c_str()[i] != '\0' && i < 2000; i++) {
            wrapped[pos++] = s.c_str()[i];
            if (i - lastNL > 78 && s.c_str()[i] == ' ') {
                wrapped[pos-1] = '\n';
                lastNL = i;
            }
        }
        wrapped[pos] = '\0';
        statementText.setString(wrapped);
    }
}

int ContestView::handleEvent(sf::Event event, sf::RenderWindow& window, OJ::Contest* contest, OJ::ProblemBank& bank, OJ::EvaluationEngine& engine, OJ::ContestManager& cm, OJ::Session& session, OJ::UserManager& userManager) {

    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos(sf::Vector2f(event.mouseButton.x, event.mouseButton.y));
        
        int nav = sidebar.handleClick(mousePos);
        if (nav != -1) return nav;

        if (inSetupMode) {
            if (easyBtn.contains(mousePos)) return 11; // Variant 1
            if (stdBtn.contains(mousePos)) return 13; // Variant 3
            if (hardBtn.contains(mousePos)) return 15; // Variant 5
            return -1;
        }

        OJ::Problem* pool[50];
        int poolCount = 0;

        if (submitBtn.contains(mousePos)) {
            // 6) Evaluate answer and update contest progress
            if (contest && currentQIndex < contest->problemCount()) {
                OJ::Problem* p = bank.getById(contest->problemIdAt(currentQIndex));
                if (p) {
                    if (contest->isSolvedAt(currentQIndex)) {
                        verdictText.setString("Problem already solved");
                        verdictText.setFillColor(sf::Color(239, 68, 68)); // Error color
                    } else {
                        OJ::Verdict v=engine.evaluateAnswer(ansIn.getText(),*p);
                        bool acc=(v.getCode()==OJ::Verdict::Accepted);
                        contest->recordAttempt(currentQIndex,acc,ansIn.getText());
                        
                        if (acc) {
                            verdictText.setString("Accepted! Fresh question loaded.");
                            verdictText.setFillColor(OJUI::UITheme::Success());
                            // Replace current problem with a new one from bank
                            int nextId = -1;
                            OJ::Problem* pool[50];
                            int poolCount = 0;
                            bank.filterByDifficulty(p->getDifficulty(), pool, poolCount, 50);
                            if (poolCount > 0) nextId = pool[rand() % poolCount]->getId();
                            
                            if (nextId != -1) {
                                contest->replaceProblem(currentQIndex, nextId);
                                ansIn.clear();
                                updateProblem(*qTitle.getFont(), contest, bank);
                            }
                        } else {
                            verdictText.setString("Wrong Answer. Try again or move to next question.");
                            verdictText.setFillColor(OJUI::UITheme::Error());
                            // Move to next problem on wrong answer
                            currentQIndex++;
                            if (currentQIndex >= contest->problemCount()) currentQIndex = 0;
                            ansIn.clear();
                            updateProblem(*qTitle.getFont(), contest, bank);
                        }
                    }
                }
                return -1;
            }
        } else if (saveBtn.contains(mousePos)) {
            return 0;
        } else if (endBtn.contains(mousePos)) {
            cm.endContest(session, true, userManager, contest);
            return 5;
        }

        ansIn.setFocused(ansIn.contains(mousePos));
        activeDiffIn.setFocused(activeDiffIn.contains(mousePos));
    } else if (event.type == sf::Event::TextEntered) {
        if (!inSetupMode) {
            ansIn.handleTextEntered(event.text.unicode);
            
            // If Enter is pressed in difficulty box, trigger change
            if (event.text.unicode == 13 && activeDiffIn.isFocused()) {
                int d = 0;
                sscanf(activeDiffIn.getText(), "%d", &d);
                if (d >= 1 && d <= 5 && contest) {
                    OJ::Problem* pool[50];
                    int poolCount = 0;
                    bank.filterByDifficulty(d, pool, poolCount, 50);
                    if (poolCount > 0) {
                        contest->replaceProblem(currentQIndex, pool[rand() % poolCount]->getId());
                        ansIn.clear();
                        updateProblem(*qTitle.getFont(), contest, bank);
                    }
                }
            } else {
                activeDiffIn.handleTextEntered(event.text.unicode);
            }
        }
    }
    return -1;
}

void ContestView::draw(sf::RenderWindow& window) const {
    window.draw(titleBar);
    window.draw(titleText);
    sidebar.draw(window);
    
    window.draw(panel);
    if (inSetupMode) {
        window.draw(setupTitle);
        easyBtn.draw(window);
        stdBtn.draw(window);
        hardBtn.draw(window);
        
        window.draw(diffLabel);
    } else {
        window.draw(scoreBar);
        window.draw(scoreText);
        window.draw(noteText);
        
        window.draw(qTitle);
        window.draw(diffText);
        window.draw(statementBox);
        window.draw(statementText);
        
        window.draw(ansLabel);
        ansIn.draw(window);
        
        submitBtn.draw(window);
        saveBtn.draw(window);
        endBtn.draw(window);

        window.draw(activeDiffLabel);
        activeDiffIn.draw(window);
        window.draw(verdictText);
    }
}
