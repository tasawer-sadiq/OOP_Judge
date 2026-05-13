#include "SFMLApp.h"
#include "UITheme.h"
#include <iostream>

namespace OJUI {

SFMLApp::SFMLApp(VFSManager& vfsRef, OJ::UserManager& userMgrRef)
    : vfs(vfsRef), userManager(userMgrRef), currentScreen(SCREEN_HOME),
      contestManager(nullptr), engine(""), activeContest(nullptr) 
{
    window.create(sf::VideoMode(1100, 780), "OOP Online Judge", sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);

    // Try loading a default font
    if (!font.loadFromFile("C:\\\\Windows\\\\Fonts\\\\arial.ttf")) {
        std::cerr << "Could not load font!" << std::endl;
    }

    problemBank.seedIfMissing(vfs);
    problemBank.loadFromVFS(vfs);
    contestManager = new OJ::ContestManager(vfs, problemBank);

    loginView.setup(font);
    registerView.setup(font);
    dashboardView.setup(font);
    problemBankView.setup(font, problemBank);
    contestView.setup(font);
    resultView.setup(font);
    homeView.setup(font);
}

SFMLApp::~SFMLApp() {
    if (activeContest) {
        delete activeContest;
    }
    if (contestManager) {
        delete contestManager;
    }
}

void SFMLApp::run() {
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            handleEvent(event);
        }

        update();

        window.clear(UITheme::Background());
        draw();
        window.display();
    }
}

// 1) Handle main application events and screen transitions
void SFMLApp::handleEvent(sf::Event event) {
    if (currentScreen == SCREEN_HOME) {
        if (homeView.handleEvent(event, window) == 1) {
            currentScreen = SCREEN_LOGIN;
        }
    } else if (currentScreen == SCREEN_LOGIN) {
        int r = loginView.handleEvent(event, window, userManager, session);
        if (r == 1) {
            currentScreen = SCREEN_DASHBOARD;
            if (session.getUser()) dashboardView.updateSession(session.getUser());
        } else if (r == 2) {
            registerView.clear();
            currentScreen = SCREEN_REGISTER;
        }
    } else if (currentScreen == SCREEN_REGISTER) {
        int r = registerView.handleEvent(event, window, userManager);
        if (r == 1) {
            loginView.clear();
            registerView.clear();
            currentScreen = SCREEN_LOGIN;
        }
    } else if (currentScreen == SCREEN_DASHBOARD) {
        int r = dashboardView.handleEvent(event, window, session);
        if (r == 1) {
            currentScreen = SCREEN_PROBLEM_BANK;
            problemBankView.setup(font, problemBank);
        } else if (r == 2) {
            currentScreen = SCREEN_CONTEST;
            if (activeContest) {
                contestView.setSetupMode(false);
                contestView.updateProblem(font, activeContest, problemBank);
            } else {
                contestView.setSetupMode(true);
            }
        } else if (r == 3) {
            loadLatestResult();
            currentScreen = SCREEN_RESULT;
        } else if (r == 4) {
            if (activeContest) { delete activeContest; activeContest = nullptr; }
            session.end();
            loginView.clear();
            currentScreen = SCREEN_LOGIN;
        } else if (r == 5) { // Start Contest
            if (activeContest) { delete activeContest; activeContest = nullptr; }
            currentScreen = SCREEN_CONTEST;
            contestView.setSetupMode(true);
        } else if (r == 6) { // Resume Contest
            if (activeContest) { delete activeContest; activeContest = nullptr; }
            activeContest = contestManager->resumeContest(session);
            currentScreen = SCREEN_CONTEST;
            if (activeContest) {
                contestView.setSetupMode(false);
                contestView.updateProblem(font, activeContest, problemBank);
            } else {
                contestView.setSetupMode(true);
            }
        } else if (r == 7) { // Delete Account
            if (session.isActive() && session.getUser()) {
                userManager.deleteUser(session.getUser()->getUsername().c_str());
                session.end();
            }
            loginView.clear();
            currentScreen = SCREEN_LOGIN;
        }
    } else if (currentScreen == SCREEN_PROBLEM_BANK) {
        int r = problemBankView.handleEvent(event, window, problemBank, engine, session, userManager);
        if (r == 0) {
            currentScreen = SCREEN_DASHBOARD;
            if (session.getUser()) dashboardView.updateSession(session.getUser());
        }
        else if (r == 2) {
            currentScreen = SCREEN_CONTEST;
            if (activeContest) {
                contestView.setSetupMode(false);
                contestView.updateProblem(font, activeContest, problemBank);
            } else {
                contestView.setSetupMode(true);
            }
        }
        else if (r == 3) { loadLatestResult(); currentScreen = SCREEN_RESULT; }
        else if (r == 4) {
            if (activeContest) { delete activeContest; activeContest = nullptr; }
            session.end(); 
            loginView.clear();
            currentScreen = SCREEN_LOGIN; 
        }
    } else if (currentScreen == SCREEN_CONTEST) {
        int r=contestView.handleEvent(event,window,activeContest,problemBank,engine,*contestManager,session,userManager);
        if (r == 0) { // save back
            if (activeContest) {
                contestManager->saveContestProgress(session, activeContest);
            }
            currentScreen = SCREEN_DASHBOARD;
            if (session.getUser()) dashboardView.updateSession(session.getUser());
        } else if (r == 1) {
            currentScreen = SCREEN_PROBLEM_BANK;
        } else if (r == 3) { // results
            loadLatestResult();
            currentScreen = SCREEN_RESULT;
        } else if (r == 4) { // logout
            if (activeContest) { delete activeContest; activeContest = nullptr; }
            session.end();
            loginView.clear();
            currentScreen = SCREEN_LOGIN;
        } else if (r == 5) { // ended from button
            contestManager->endContest(session, true, userManager, activeContest);
            delete activeContest; activeContest = nullptr;
            loadLatestResult();
            currentScreen = SCREEN_RESULT;
        } else if (r >= 10) {
            int diff = r - 10;
            activeContest = contestManager->startContest(session, diff, 120);
            contestView.setSetupMode(false);
            contestView.updateProblem(font, activeContest, problemBank);
        }
    } else if (currentScreen == SCREEN_RESULT) {
        int r = resultView.handleEvent(event, window);
        if (r == 0) {
            currentScreen = SCREEN_DASHBOARD;
            if (session.getUser()) dashboardView.updateSession(session.getUser());
        }
        else if (r == 1) currentScreen = SCREEN_PROBLEM_BANK;
    }
}

static Directory* findChildDir(Directory* parent, const char* name) {
    if (!parent) return nullptr;
    for (int i=0; i<parent->childCount(); i++) {
        if (parent->childAt(i) && strcmp(parent->childAt(i)->getName(), name)==0)
            return (Directory*)parent->childAt(i);
    }
    return nullptr;
}

static File* findChildFile(Directory* parent, const char* name) {
    if (!parent) return nullptr;
    for (int i=0; i<parent->childCount(); i++) {
        if (parent->childAt(i) && strcmp(parent->childAt(i)->getName(), name)==0)
            return (File*)parent->childAt(i);
    }
    return nullptr;
}

void SFMLApp::loadLatestResult() {
    Directory* root = vfs.getRoot();
    if (!root) return;
    Directory* users = findChildDir(root, "users");
    if (!users || !session.getUser()) return;
    Directory* userDir = findChildDir(users, session.getUser()->getUsername().c_str());
    if (!userDir) return;
    Directory* contestsDir = findChildDir(userDir, "contests");
    if (!contestsDir) return;
    Directory* history = findChildDir(contestsDir, "history");
    if (!history) return;

    Directory* best = nullptr;
    int bestTs = -1;
    for (int i=0; i<history->childCount(); i++) {
        Directory* d = (Directory*)history->childAt(i);
        if (!d) continue;
        const char* n = d->getName();
        if (strncmp(n, "run_", 4) == 0) {
            int ts = atoi(n+4);
            if (ts > bestTs) { bestTs = ts; best = d; }
        }
    }

    if (best) {
        File* rf = findChildFile(best, "result.txt");
        if (rf && rf->getData()) {
            OJ::ContestResult r;
            OJ::ContestResult::deserialize(rf->getData(), r);
            resultView.updateResult(font, r);
        }
    }
}

void SFMLApp::update() {
    if (currentScreen == SCREEN_CONTEST && activeContest) {
        contestView.updateScore(font, activeContest);
        if (activeContest->remainingSeconds() <= 0) {
            contestManager->endContest(session, true, userManager, activeContest);
            delete activeContest; activeContest = nullptr;
            loadLatestResult();
            currentScreen = SCREEN_RESULT;
        }
    }
}

void SFMLApp::draw() {
    if (currentScreen == SCREEN_HOME) homeView.draw(window);
    else if (currentScreen == SCREEN_LOGIN) loginView.draw(window);
    else if (currentScreen == SCREEN_REGISTER) registerView.draw(window);
    else if (currentScreen == SCREEN_DASHBOARD) dashboardView.draw(window);
    else if (currentScreen == SCREEN_PROBLEM_BANK) problemBankView.draw(window);
    else if (currentScreen == SCREEN_CONTEST) contestView.draw(window);
    else if (currentScreen == SCREEN_RESULT) resultView.draw(window);
}

}
