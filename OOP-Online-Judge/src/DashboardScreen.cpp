#include "../include/DashboardScreen.h"

#include <iostream>

using namespace std;

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

int DashboardScreen::show(const Session& session) {
    cout << "\n=== Dashboard ===\n";
    if (session.getUser()) {
        cout << "User: " << session.getUser()->getUsername() << "\n";
    }
    cout << "1) Browse Problems\n";
    cout << "2) Start Contest\n";
    cout << "3) Resume Contest\n";
    cout << "4) View Last Result\n";
    cout << "5) Logout\n";
    cout << "6) Delete Account\n";
    cout << "0) Exit\n";

    char buf[32];
    if (!readRequiredLine("Choose: ", buf, (int)sizeof(buf))) return ActionExit;
    int ch = toInt(buf, -1);

    if (ch == 1) return ActionBrowseProblems;
    if (ch == 2) return ActionStartContest;
    if (ch == 3) return ActionResumeContest;
    if (ch == 4) return ActionViewLastResult;
    if (ch == 5) return ActionLogout;
    if (ch == 6) return ActionDeleteAccount;
    if (ch == 0) return ActionExit;
    return ActionExit;
}

}
