#ifndef OJ_USERMANAGER_H
#define OJ_USERMANAGER_H

#include "User.h"
#include "Session.h"
#include "VerificationService.h"
#include "VFSManager.h"
#include "TextUtil.h"

namespace OJ {

class UserManager {
public:
    enum AuthError {
        AuthNone = 0,
        AuthInvalidInput,
        AuthInvalidEmail,
        AuthUserAlreadyExists,
        AuthEmailAlreadyExists,
        AuthUserNotFound,
        AuthUserNotVerified,
        AuthWrongPassword
    };

private:
    VFSManager* vfs;
    Directory* usersDir;
    User* users[100];
    int userCountVal;
    int nextId;

    AuthError lastError;

    static bool sameUsername(const User* u, const char* name) {
        if (!u) return false;
        return u->getUsername().equals(name);
    }

    int findIndexByUsername(const char* username) const {
        for (int i = 0; i < userCountVal; i++) {
            if (sameUsername(users[i], username)) return i;
        }
        return -1;
    }

    static int cstrLen(const char* s) {
        if (!s) return 0;
        int i = 0;
        while (s[i] != '\0') i++;
        return i;
    }

    static bool cstrEndsWith(const char* s, const char* suffix) {
        if (!s || !suffix) return false;
        int n = cstrLen(s);
        int m = cstrLen(suffix);
        if (m == 0) return true;
        if (n < m) return false;
        int start = n - m;
        for (int i = 0; i < m; i++) {
            if (s[start + i] != suffix[i]) return false;
        }
        return true;
    }

    static bool sameName(const char* a, const char* b) {
        if (a == b) return true;
        if (!a || !b) return false;
        int i = 0;
        while (a[i] != '\0' && b[i] != '\0') {
            if (a[i] != b[i]) return false;
            i++;
        }
        return a[i] == '\0' && b[i] == '\0';
    }

    bool userDirExistsInVFS(const char* username) const {
        if (!usersDir || !username || username[0] == '\0') return false;
        for (int i = 0; i < usersDir->childCount(); i++) {
            Directory* d = (Directory*)(usersDir->childAt(i));
            if (d && sameName(d->getName(), username)) return true;
        }
        return false;
    }

    bool emailExistsInVFS(const char* email) const {
        if (!usersDir || !email || email[0] == '\0') return false;
        for (int i = 0; i < usersDir->childCount(); i++) {
            Directory* d = (Directory*)(usersDir->childAt(i));
            if (!d) continue;

            for (int j = 0; j < d->childCount(); j++) {
                File* f = (File*)(d->childAt(j));
                if (f && sameName(f->getName(), "email.txt") && sameName(f->getData(), email)) {
                    return true;
                }
            }
        }
        return false;
    }

    Directory* ensureUsersDir() {
        if (usersDir != NULL) return usersDir;
        Directory* root = vfs->getRoot();
        if (root == NULL) return NULL;

        FSE* found = root->search("users");
        Directory* asDir = (Directory*)(found);
        if (asDir != NULL) {
            usersDir = asDir;
            return usersDir;
        }

        usersDir = vfs->createDirectory(root, "users");
        return usersDir;
    }

    static bool parseIntAfterKey(const char* s, const char* key, int& outVal) {
        if (!s || !key) return false;
        for (int i = 0; s[i] != '\0'; i++) {
            int k = 0;
            while (key[k] != '\0' && s[i + k] != '\0' && s[i + k] == key[k]) k++;
            if (key[k] == '\0') {
                const char* p = s + i + k;
                int sign = 1;
                if (*p == '-') { sign = -1; p++; }
                int v = 0;
                while (*p >= '0' && *p <= '9') {
                    v = v * 10 + (*p - '0');
                    p++;
                }
                outVal = v * sign;
                return true;
            }
        }
        return false;
    }

    void writeUserFiles(const User& u) {
        Directory* uDir = vfs->createDirectory(usersDir, u.getUsername().c_str());
        if (uDir == NULL) return;

        char profile[96];
        TextUtil::clearBuf(profile, (int)sizeof(profile));
        int p = 0;
        TextUtil::appendCStr(profile, (int)sizeof(profile), p, "id=");
        TextUtil::appendInt(profile, (int)sizeof(profile), p, u.getId());
        TextUtil::appendCStr(profile, (int)sizeof(profile), p, ";u=");
        TextUtil::appendCStr(profile, (int)sizeof(profile), p, u.getUsername().c_str());
        TextUtil::appendCStr(profile, (int)sizeof(profile), p, ";v=");
        TextUtil::appendInt(profile, (int)sizeof(profile), p, u.isVerified() ? 1 : 0);
        TextUtil::appendChar(profile, (int)sizeof(profile), p, ';');

        char token[96];
        TextUtil::clearBuf(token, (int)sizeof(token));
        int t = 0;
        TextUtil::appendCStr(token, (int)sizeof(token), t, "token=");
        TextUtil::appendCStr(token, (int)sizeof(token), t, u.getVerificationToken().c_str());
        TextUtil::appendChar(token, (int)sizeof(token), t, ';');

        vfs->createFile(uDir, "profile.txt", profile);
        vfs->createFile(uDir, "email.txt", u.getEmail().c_str());
        vfs->createFile(uDir, "verify.txt", token);
        vfs->createFile(uDir, "progress.txt", "");
    }

public:
    void rewriteUserFiles(const User& u) {
        if (!usersDir) return;
        FSE* found = usersDir->search(u.getUsername().c_str());
        Directory* uDir = (Directory*)(found);
        if (uDir == NULL) return;

        uDir->remove("profile.txt");
        uDir->remove("email.txt");
        uDir->remove("verify.txt");

        char profile[128];
        TextUtil::clearBuf(profile, (int)sizeof(profile));
        int p = 0;
        TextUtil::appendCStr(profile, (int)sizeof(profile), p, "id=");
        TextUtil::appendInt(profile, (int)sizeof(profile), p, u.getId());
        TextUtil::appendCStr(profile, (int)sizeof(profile), p, ";u=");
        TextUtil::appendCStr(profile, (int)sizeof(profile), p, u.getUsername().c_str());
        TextUtil::appendCStr(profile, (int)sizeof(profile), p, ";v=");
        TextUtil::appendInt(profile, (int)sizeof(profile), p, u.isVerified() ? 1 : 0);
        TextUtil::appendCStr(profile, (int)sizeof(profile), p, ";pc=");
        TextUtil::appendInt(profile, (int)sizeof(profile), p, u.getPracticeCredits());
        TextUtil::appendCStr(profile, (int)sizeof(profile), p, ";cc=");
        TextUtil::appendInt(profile, (int)sizeof(profile), p, u.getContestCredits());
        TextUtil::appendChar(profile, (int)sizeof(profile), p, ';');

        char token[96];
        TextUtil::clearBuf(token, (int)sizeof(token));
        int t = 0;
        TextUtil::appendCStr(token, (int)sizeof(token), t, "token=");
        TextUtil::appendCStr(token, (int)sizeof(token), t, u.getVerificationToken().c_str());
        TextUtil::appendChar(token, (int)sizeof(token), t, ';');

        vfs->createFile(uDir, "profile.txt", profile);
        vfs->createFile(uDir, "email.txt", u.getEmail().c_str());
        vfs->createFile(uDir, "verify.txt", token);
    }

    UserManager(VFSManager& manager)
        : vfs(&manager), usersDir(NULL), userCountVal(0), nextId(1), lastError(AuthNone) {
        usersDir = ensureUsersDir();
    }

    ~UserManager() {
        for (int i = 0; i < userCountVal; i++) {
            delete users[i];
        }
    }

    AuthError getLastError() const { return lastError; }

    bool usernameExists(const char* username) const {
        if (!username || username[0] == '\0') return false;
        return findIndexByUsername(username) != -1 || userDirExistsInVFS(username);
    }

    bool emailExists(const char* email) const {
        if (!email || email[0] == '\0') return false;
        for (int i = 0; i < userCountVal; i++) {
            if (users[i] && users[i]->getEmail().equals(email)) return true;
        }
        return emailExistsInVFS(email);
    }

    bool registerUser(const char* username, const char* email, const char* password, CustomString& outToken) {
        lastError = AuthNone;
        if (!vfs || !username || !email || !password) {
            lastError = AuthInvalidInput;
            return false;
        }
        if (username[0] == '\0' || email[0] == '\0' || password[0] == '\0') {
            lastError = AuthInvalidInput;
            return false;
        }

        if (!cstrEndsWith(email, "@email.com")) {
            lastError = AuthInvalidEmail;
            return false;
        }
        if (usersDir == NULL) usersDir = ensureUsersDir();
        if (usersDir == NULL) {
            lastError = AuthInvalidInput;
            return false;
        }

        if (usernameExists(username)) {
            lastError = AuthUserAlreadyExists;
            return false;
        }
        if (emailExists(email)) {
            lastError = AuthEmailAlreadyExists;
            return false;
        }

        User* u = new User(nextId, username, email, password);
        nextId++;

        CustomString token = VerificationService::generateToken(*u);
        u->setVerificationToken(token);
        u->setVerified(false);

        if (userCountVal < 100) {
            users[userCountVal++] = u;
        }
        outToken = token;

        writeUserFiles(*u);
        lastError = AuthNone;
        return true;
    }

    bool verifyUser(const char* username, const char* token) {
        int idx = findIndexByUsername(username);
        if (idx == -1) return false;

        User* u = users[idx];
        if (u->isVerified()) return true;
        if (!u->getVerificationToken().equals(token)) return false;

        u->setVerified(true);
        rewriteUserFiles(*u);
        return true;
    }

    bool login(const char* username, const char* password, Session& session) {
        lastError = AuthNone;
        int idx = findIndexByUsername(username);
        if (idx == -1) {
            lastError = AuthUserNotFound;
            return false;
        }

        User* u = users[idx];

        if (usersDir) {
            // Load credits from profile
            FSE* found = usersDir->search(u->getUsername().c_str());
            Directory* uDir = (Directory*)(found);
            if (uDir) {
                File* prof = (File*)(uDir->search("profile.txt"));
                if (prof && prof->getData()) {
                    int pc = 0, cc = 0;
                    if (parseIntAfterKey(prof->getData(), "pc=", pc)) u->setPracticeCredits(pc);
                    if (parseIntAfterKey(prof->getData(), "cc=", cc)) u->setContestCredits(cc);
                }
            }
        }

        if (!u->isVerified()) {
            lastError = AuthUserNotVerified;
            return false;
        }
        if (!u->checkPassword(password)) {
            lastError = AuthWrongPassword;
            return false;
        }

        session.start(u);
        lastError = AuthNone;
        return true;
    }

    void logout(Session& session) {
        session.end();
    }

    bool deleteUser(const char* username) {
        int idx = findIndexByUsername(username);
        if (idx == -1) return false;

        if (usersDir == NULL) usersDir = ensureUsersDir();
        if (usersDir != NULL) {
            usersDir->remove(username);
        }

        delete users[idx];
        for (int i = idx; i < userCountVal - 1; i++) {
            users[i] = users[i + 1];
        }
        userCountVal--;
        return true;
    }

    int userCount() const { return userCountVal; }
};

}

#endif
