#ifndef OJ_SFMLAPP_H
#define OJ_SFMLAPP_H

#include <SFML/Graphics.hpp>
#include "../include/VFSManager.h"
#include "../include/UserManager.h"
#include "../include/Session.h"
#include "../include/ProblemBank.h"
#include "../include/ContestManager.h"
#include "../include/EvaluationEngine.h"

#include "LoginView.h"
#include "RegisterView.h"
#include "DashboardView.h"
#include "ProblemBankView.h"
#include "ContestView.h"
#include "ResultView.h"
#include "HomeView.h"


namespace OJUI {

enum ScreenType {
    SCREEN_HOME,
    SCREEN_LOGIN,
    SCREEN_REGISTER,
    SCREEN_DASHBOARD,
    SCREEN_PROBLEM_BANK,
    SCREEN_CONTEST,
    SCREEN_RESULT
};


class SFMLApp {
private:
    sf::RenderWindow window;
    sf::Font font;
    ScreenType currentScreen;

    VFSManager& vfs;
    OJ::UserManager& userManager;
    OJ::Session session;
    OJ::ProblemBank problemBank;
    OJ::ContestManager* contestManager;
    OJ::EvaluationEngine engine;
    OJ::Contest* activeContest;

    LoginView loginView;
    RegisterView registerView;
    DashboardView dashboardView;
    ProblemBankView problemBankView;
    ContestView contestView;
    ResultView resultView;
    HomeView homeView;


    void handleEvent(sf::Event event);
    void update();
    void draw();
    
    void loadLatestResult();

public:
    SFMLApp(VFSManager& vfsRef, OJ::UserManager& userMgrRef);
    ~SFMLApp();
    void run();
};

}

#endif
