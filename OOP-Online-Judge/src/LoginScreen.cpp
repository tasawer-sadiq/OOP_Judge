#include "../include/LoginScreen.h"

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

bool LoginScreen::run(UserManager& um, Session& session) {
    cout << "\n--- Login ---\n";

    char uname[64];
    char pw[64];
    if (!readRequiredLine("Username: ", uname, (int)sizeof(uname))) return false;
    if (!readRequiredLine("Password: ", pw, (int)sizeof(pw))) return false;

    bool ok = um.login(uname, pw, session);
    if (ok) {
        cout << "Login successful\n";
        return true;
    }

    UserManager::AuthError err = um.getLastError();
    if (err == UserManager::AuthWrongPassword) {
        cout << "Wrong password\n";
    } else if (err == UserManager::AuthUserNotVerified) {
        cout << "Account not verified\n";
    } else if (err == UserManager::AuthUserNotFound) {
        cout << "User not found\n";
    } else {
        cout << "Login failed\n";
    }
    return ok;
}

}
