#include "ProblemBankView.h"
#include "UITheme.h"
#include <string.h>
#include <cstdio>

ProblemBankView::ProblemBankView() : filteredProblems(nullptr), filteredCount(0), scrollIndex(0), selectedIndex(-1) {}

ProblemBankView::~ProblemBankView() {
    if (filteredProblems) delete[] filteredProblems;
}

void ProblemBankView::setup(const sf::Font& font, OJ::ProblemBank& bank) {
    // Window is 1100x780
    titleBar.setSize(sf::Vector2f(1100, 60));
    titleBar.setFillColor(OJUI::UITheme::TitleBar());
    
    titleText.setFont(font);
    titleText.setString("Problem Bank");
    titleText.setCharacterSize(24);
    titleText.setFillColor(sf::Color::White);
    titleText.setStyle(sf::Text::Bold);
    titleText.setPosition(30, 15);

    sidebar.setup(font, 1);

    // Left Panel: x=210, width=330
    leftPanel.setPosition(210, 75);
    leftPanel.setSize(sf::Vector2f(330, 690));
    leftPanel.setFillColor(OJUI::UITheme::Panel());
    leftPanel.setOutlineThickness(2);
    leftPanel.setOutlineColor(OJUI::UITheme::Border());

    leftTitle.setFont(font);
    leftTitle.setString("Problems");
    leftTitle.setCharacterSize(22);
    leftTitle.setFillColor(OJUI::UITheme::Text());
    leftTitle.setStyle(sf::Text::Bold);
    leftTitle.setPosition(228, 92);

    diffLabel.setFont(font);
    diffLabel.setString("Difficulty (1=Easy, 2=Medium, 3=Hard)");
    diffLabel.setCharacterSize(13);
    diffLabel.setFillColor(OJUI::UITheme::Text());
    diffLabel.setStyle(sf::Text::Bold);
    diffLabel.setPosition(228, 130);

    diffIn.setup(font, 228, 152, 295, 40);

    filterBtn.setup(font, "Filter Difficulty", 228, 202, 295, 40, OJUI::UITheme::Button());

    scrollArea.setPosition(228, 258);
    scrollArea.setSize(sf::Vector2f(290, 460));
    scrollArea.setFillColor(sf::Color::Transparent);

    scrollbarBg.setPosition(522, 258);
    scrollbarBg.setSize(sf::Vector2f(10, 460));
    scrollbarBg.setFillColor(OJUI::UITheme::ButtonGray());
    scrollbarBg.setOutlineThickness(1);
    scrollbarBg.setOutlineColor(OJUI::UITheme::Border());

    scrollThumb.setPosition(523, 258);
    scrollThumb.setSize(sf::Vector2f(8, 80));
    scrollThumb.setFillColor(sf::Color(100, 116, 139));

    scrollHint.setFont(font);
    scrollHint.setString("Scroll with mouse wheel to see more.");
    scrollHint.setCharacterSize(13);
    scrollHint.setFillColor(OJUI::UITheme::MutedText());
    scrollHint.setPosition(228, 728);

    for (int i=0; i<6; i++) {
        rows[i].box.setSize(sf::Vector2f(282, 68));
        rows[i].box.setOutlineThickness(2);
        rows[i].text.setFont(font);
        rows[i].text.setCharacterSize(13);
        rows[i].text.setStyle(sf::Text::Bold);
        rows[i].id = -1;
    }

    // Right Panel: x=550, width=535 (to 1085)
    rightPanel.setPosition(550, 75);
    rightPanel.setSize(sf::Vector2f(530, 690));
    rightPanel.setFillColor(OJUI::UITheme::Panel());
    rightPanel.setOutlineThickness(2);
    rightPanel.setOutlineColor(OJUI::UITheme::Border());

    rightTitle.setFont(font);
    rightTitle.setString("Select a problem");
    rightTitle.setCharacterSize(22);
    rightTitle.setFillColor(OJUI::UITheme::Text());
    rightTitle.setStyle(sf::Text::Bold);
    rightTitle.setPosition(568, 92);

    diffText.setFont(font);
    diffText.setString("");
    diffText.setCharacterSize(15);
    diffText.setFillColor(sf::Color(59, 130, 246));
    diffText.setStyle(sf::Text::Bold);
    diffText.setPosition(568, 120);

    // Statement box: taller (320px now) and wider to fill right panel
    statementBox.setPosition(568, 145);
    statementBox.setSize(sf::Vector2f(500, 320));
    statementBox.setFillColor(sf::Color(248, 250, 252));
    statementBox.setOutlineThickness(2);
    statementBox.setOutlineColor(OJUI::UITheme::Border());

    statementText.setFont(font);
    statementText.setString("");
    statementText.setCharacterSize(15);
    statementText.setFillColor(OJUI::UITheme::Text());
    statementText.setStyle(sf::Text::Bold);
    statementText.setPosition(582, 160);

    ansLabel.setFont(font);
    ansLabel.setString("Your Answer");
    ansLabel.setCharacterSize(16);
    ansLabel.setFillColor(OJUI::UITheme::Text());
    ansLabel.setStyle(sf::Text::Bold);
    ansLabel.setPosition(568, 475);

    ansIn.setup(font, 568, 500, 500, 46);
    ansIn.setBold(true);

    submitBtn.setup(font, "Submit Answer", 568, 560, 500, 50, OJUI::UITheme::Button());

    // Verdict text: supports 2 lines
    verdictText.setFont(font);
    verdictText.setCharacterSize(17);
    verdictText.setStyle(sf::Text::Bold);
    verdictText.setPosition(568, 620);
    verdictText.setString("");

    if (filteredProblems) delete[] filteredProblems;
    filteredCount = bank.count();
    filteredProblems = new OJ::Problem*[filteredCount];
    for (int i=0; i<filteredCount; i++) {
        filteredProblems[i] = bank.getAt(i);
    }
    updateRows(font);
}

void ProblemBankView::updateRows(const sf::Font& font) {
    float y = 258;
    for (int i=0; i<6; i++) {
        int idx = scrollIndex + i;
        if (idx < filteredCount && filteredProblems[idx]) {
            rows[i].id = filteredProblems[idx]->getId();
            int credits = filteredProblems[idx]->getDifficulty() * 100;
            int diff = filteredProblems[idx]->getDifficulty();
            char rowBuf[256];
            sprintf(rowBuf, "%s (Diff: %d)\nCredits: %d", filteredProblems[idx]->getTitle().c_str(), diff, credits);
            rows[i].text.setString(rowBuf);
            rows[i].text.setFillColor(sf::Color::Black);
            rows[i].box.setPosition(228, y);
            rows[i].text.setPosition(238, y + 8);
            
            if (idx == selectedIndex) {
                rows[i].box.setFillColor(sf::Color(219, 234, 254));
                rows[i].box.setOutlineColor(OJUI::UITheme::Button());
            } else {
                rows[i].box.setFillColor(sf::Color::White);
                rows[i].box.setOutlineColor(OJUI::UITheme::Border());
            }
        } else {
            rows[i].id = -1;
        }
        y += 76;
    }
    
    if (filteredCount > 6) {
        float trackHeight = 460 - 80.0f;
        float progress = (float)scrollIndex / (filteredCount - 6);
        scrollThumb.setPosition(523, 258 + progress * trackHeight);
    } else {
        scrollThumb.setPosition(523, 258);
    }
}

static const char* getDiffLabel(int d) {
    if (d <= 1) return "Easy (Difficulty: 1)";
    if (d == 2) return "Medium (Difficulty: 2)";
    if (d == 3) return "Hard (Difficulty: 3)";
    return "Expert (Difficulty: 4+)";
}

void ProblemBankView::updateRightPanel(const sf::Font& font) {
    if (selectedIndex >= 0 && selectedIndex < filteredCount && filteredProblems[selectedIndex]) {
        OJ::Problem* p = filteredProblems[selectedIndex];
        rightTitle.setString(p->getTitle().c_str());
        diffText.setString(getDiffLabel(p->getDifficulty()));
        
        // Wrap at ~55 chars
        CustomString s = p->getStatement();
        char wrapped[2048];
        int pos = 0;
        int lastNL = 0;
        for (int i=0; s.c_str()[i] != '\0' && i < 2000; i++) {
            wrapped[pos++] = s.c_str()[i];
            if (i - lastNL > 52 && s.c_str()[i] == ' ') {
                wrapped[pos-1] = '\n';
                lastNL = i;
            }
        }
        wrapped[pos] = '\0';
        statementText.setString(wrapped);
        ansIn.clear();
        verdictText.setString("");
    } else {
        rightTitle.setString("Select a problem");
        diffText.setString("");
        statementText.setString("");
        ansIn.clear();
        verdictText.setString("");
    }
}

// 4) Manage user interactions within the Problem Bank
int ProblemBankView::handleEvent(sf::Event event, sf::RenderWindow& window, OJ::ProblemBank& bank, OJ::EvaluationEngine& engine, OJ::Session& session, OJ::UserManager& userManager) {
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
        
        int nav = sidebar.handleClick(mousePos);
        if (nav != -1) return nav;

        diffIn.setFocused(diffIn.contains(mousePos));
        ansIn.setFocused(ansIn.contains(mousePos));

        if (filterBtn.contains(mousePos)) {
            int diff = 0;
            const char* dStr = diffIn.getText();
            if (strlen(dStr) > 0) diff = atoi(dStr);
            
            if (filteredProblems) delete[] filteredProblems;
            filteredProblems = new OJ::Problem*[bank.count()];
            filteredCount = 0;
            
            for (int i=0; i<bank.count(); i++) {
                OJ::Problem* p = bank.getAt(i);
                if (p && (diff == 0 || p->getDifficulty() == diff)) {
                    filteredProblems[filteredCount++] = p;
                }
            }
            scrollIndex = 0;
            selectedIndex = -1;
            updateRows(*rightTitle.getFont());
            updateRightPanel(*rightTitle.getFont());
        }

        for (int i=0; i<6; i++) {
            if (rows[i].id != -1 && rows[i].box.getGlobalBounds().contains(mousePos)) {
                selectedIndex = scrollIndex + i;
                updateRows(*rightTitle.getFont());
                updateRightPanel(*rightTitle.getFont());
            }
        }

        if (submitBtn.contains(mousePos) && selectedIndex >= 0 && selectedIndex < filteredCount) {
            OJ::Problem* p = filteredProblems[selectedIndex];
            OJ::Verdict v = engine.evaluateAnswer(ansIn.getText(), *p);
            
            char resultMsg[350];
            sf::Color resultColor;
            if (v.getCode() == OJ::Verdict::Accepted) {
                resultColor = OJUI::UITheme::Success();
                int earned = p->getDifficulty() * 100;
                // Format: Accepted (ProblemName) \n Credits: 100;
                sprintf(resultMsg, "Accepted (%s)\nCredits: %d;", p->getTitle().c_str(), earned);
                
                if(session.getUser()){
                    session.getUser()->setPracticeCredits(session.getUser()->getPracticeCredits()+earned);
                    userManager.rewriteUserFiles(*session.getUser());
                }


            } else {
                resultColor = OJUI::UITheme::Error();
                sprintf(resultMsg, "Wrong Answer (%s)\nTry again!", p->getTitle().c_str());
            }

            selectedIndex++;
            if (selectedIndex >= filteredCount) selectedIndex = 0;
            
            if (selectedIndex < scrollIndex || selectedIndex >= scrollIndex + 6) {
                scrollIndex = selectedIndex;
                if (scrollIndex > filteredCount - 6) scrollIndex = filteredCount > 6 ? filteredCount - 6 : 0;
            }
            updateRows(*rightTitle.getFont());
            updateRightPanel(*rightTitle.getFont());

            verdictText.setFillColor(resultColor);
            verdictText.setString(resultMsg);
        }
    } else if (event.type == sf::Event::MouseWheelScrolled) {
        if (filteredCount > 6) {
            if (event.mouseWheelScroll.delta > 0) scrollIndex--;
            else if (event.mouseWheelScroll.delta < 0) scrollIndex++;
            
            if (scrollIndex < 0) scrollIndex = 0;
            if (scrollIndex > filteredCount - 6) scrollIndex = filteredCount - 6;
            updateRows(*rightTitle.getFont());
        }
    } else if (event.type == sf::Event::TextEntered) {
        diffIn.handleTextEntered(event.text.unicode);
        ansIn.handleTextEntered(event.text.unicode);
    }
    return -1;
}

void ProblemBankView::draw(sf::RenderWindow& window) const {
    window.draw(titleBar);
    window.draw(titleText);
    sidebar.draw(window);
    
    window.draw(leftPanel);
    window.draw(leftTitle);
    window.draw(diffLabel);
    diffIn.draw(window);
    filterBtn.draw(window);
    
    window.draw(scrollbarBg);
    window.draw(scrollThumb);
    for (int i=0; i<6; i++) {
        if (rows[i].id != -1) {
            window.draw(rows[i].box);
            window.draw(rows[i].text);
        }
    }
    window.draw(scrollHint);
    
    window.draw(rightPanel);
    window.draw(rightTitle);
    window.draw(diffText);
    window.draw(statementBox);
    window.draw(statementText);
    window.draw(ansLabel);
    ansIn.draw(window);
    submitBtn.draw(window);
    window.draw(verdictText);
}
