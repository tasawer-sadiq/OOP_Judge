#include "../include/RegisterScreen.h"

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

static int cstrLen(const char* s) {
    if (!s) return 0;
    int i = 0;
    while (s[i] != '\0') i++;
    return i;
}

static bool endsWith(const char* s, const char* suffix) {
    if (!s || !suffix) return false;
    int n = cstrLen(s);
    int m = cstrLen(suffix);
    if (n < m) return false;
    int start = n - m;
    for (int i = 0; i < m; i++) {
        if (s[start + i] != suffix[i]) return false;
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

bool RegisterScreen::run(UserManager& um) {
    cout << "\n--- Register ---\n";

    char uname[64];
    char email[64];
    char pw[64];

    while (true) {
        if (!readRequiredLine("Username: ", uname, (int)sizeof(uname))) return false;
        if (!um.usernameExists(uname)) break;
        cout << "User already exists\n";
    }

    while (true) {
        if (!readRequiredLine("Email: ", email, (int)sizeof(email))) return false;
        if (!endsWith(email, "@email.com")) {
            cout << "Invalid email. It must end with @email.com\n";
            continue;
        }
        if (um.emailExists(email)) {
            cout << "Email already exists\n";
            continue;
        }
        break;
    }

    if (!readRequiredLine("Password: ", pw, (int)sizeof(pw))) return false;

    CustomString token;
    bool ok = um.registerUser(uname, email, pw, token);
    if (!ok) {
        UserManager::AuthError err = um.getLastError();
        if (err == UserManager::AuthInvalidEmail) {
            cout << "Invalid email. It must end with @email.com\n";
        } else if (err == UserManager::AuthUserAlreadyExists) {
            cout << "User already exists\n";
        } else if (err == UserManager::AuthEmailAlreadyExists) {
            cout << "Email already exists\n";
        } else if (err == UserManager::AuthInvalidInput) {
            cout << "Register failed (invalid input)\n";
        } else {
            cout << "Register failed\n";
        }
        return false;
    }

    cout << "Registered. Verification token: " << token << "\n";

    char inTok[96];
    if (!readRequiredLine("Enter token to verify now: ", inTok, (int)sizeof(inTok))) return true;
    bool v = um.verifyUser(uname, inTok);
    cout << (v ? "Verified successfully\n" : "Verification failed\n");

    return true;
}

}
