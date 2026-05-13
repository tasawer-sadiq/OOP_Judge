#include "../include/UIManager.h"

#include <iostream>

using namespace std;

#include "../include/LoginScreen.h"
#include "../include/RegisterScreen.h"
#include "../include/DashboardScreen.h"
#include "../include/ProblemBrowserScreen.h"
#include "../include/ContestScreen.h"
#include "../include/ResultScreen.h"

namespace OJ {

static bool isBlank(const char* s) {
    if (!s) return true;
    int i = 0;
    while (s[i] != '\0') {
        char c = s[i];
        if (c != ' ' && c != '\t' && c != '\r' && c != '\n') return false;
        i++;
    }
    return true;
}

static bool readLine(const char* prompt, char* out, int cap) {
    if (!out || cap <= 0) return false;
    out[0] = '\0';
    if (prompt) cout << prompt;

    cin.getline(out, cap);
    if (!cin) {
        if (cin.eof()) return false;
        cin.clear();
        out[0] = '\0';
        return true;
    }
    return true;
}

static bool readRequiredLine(const char* prompt, char* out, int cap) {
    while (true) {
        if (!readLine(prompt, out, cap)) return false;
        if (!isBlank(out)) return true;
        cout << "Input required.\n";
    }
}

static int toInt(const char* s, int defVal = 0) {
    if (!s) return defVal;
    int i = 0;
    while (s[i] == ' ' || s[i] == '\t') i++;
    int sign = 1;
    if (s[i] == '-') { sign = -1; i++; }
    int v = 0;
    bool any = false;
    while (s[i] >= '0' && s[i] <= '9') {
        any = true;
        v = v * 10 + (s[i] - '0');
        i++;
    }
    return any ? v * sign : defVal;
}

UIManager::UIManager(VFSManager& vfsMgr, UserManager& um)
    : vfs(&vfsMgr), userManager(&um), session(), bank(), contestManager(NULL), engine(".tmp_eval") {}

UIManager::~UIManager() {
    delete contestManager;
    contestManager = NULL;
}

void UIManager::init() {
    if (!vfs) return;

    bank.seedIfMissing(*vfs);
    bank.loadFromVFS(*vfs);

    delete contestManager;
    contestManager = new ContestManager(*vfs, bank);
}

void UIManager::run() {
    init();

    while (true) {
        if (!session.isActive()) {
            cout << "\n=== Online Judge ===\n";
            cout << "1) Login\n";
            cout << "2) Register\n";
            cout << "0) Exit\n";

            char buf[32];
            if (!readRequiredLine("Choose: ", buf, (int)sizeof(buf))) break;
            int ch = toInt(buf, -1);

            if (ch == 0) break;
            if (ch == 1) {
                LoginScreen ls;
                ls.run(*userManager, session);
            } else if (ch == 2) {
                RegisterScreen rs;
                rs.run(*userManager);
            }
            continue;
        }

        DashboardScreen ds;
        int action = ds.show(session);

        if (action == DashboardScreen::ActionLogout) {
            userManager->logout(session);
            continue;
        }

        if (action == DashboardScreen::ActionDeleteAccount) {
            const char* uname = session.getUser() ? session.getUser()->getUsername().c_str() : "";
            userManager->logout(session);
            userManager->deleteUser(uname);
            cout << "Account deleted (if existed).\n";
            continue;
        }

        if (action == DashboardScreen::ActionBrowseProblems) {
            ProblemBrowserScreen ps;
            ps.run(bank, engine);
            continue;
        }

        if (!contestManager) {
            cout << "InternalError: ContestManager not ready\n";
            continue;
        }

        if (action == DashboardScreen::ActionStartContest) {
            ContestScreen cs;
            cs.startNew(*contestManager, bank, *vfs, session, engine, *userManager);
            continue;
        }

        if (action == DashboardScreen::ActionResumeContest) {
            ContestScreen cs;
            cs.resumeExisting(*contestManager, bank, *vfs, session, engine, *userManager);
            continue;
        }

        if (action == DashboardScreen::ActionViewLastResult) {
            ResultScreen rs;
            rs.showLast(*vfs, session);
            continue;
        }

        if (action == DashboardScreen::ActionExit) {
            break;
        }
    }

    cout << "Goodbye.\n";
}

}
