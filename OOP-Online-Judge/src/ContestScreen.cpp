#include "../include/ContestScreen.h"

#include <iostream>

using namespace std;

#include "../include/Submission.h"
#include "../include/Verdict.h"
#include "../include/ScoreCalculator.h"

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

static Directory* findChildDir(Directory* parent, const char* name) {
    if (!parent) return NULL;
    for (int i = 0; i < parent->childCount(); i++) {
        Directory* d = (Directory*)(parent->childAt(i));
        if (d && sameName(d->getName(), name)) return d;
    }
    return NULL;
}

static Directory* getUserDir(VFSManager& vfs, const Session& session) {
    if (!session.isActive() || !session.getUser()) return NULL;
    Directory* root = vfs.getRoot();
    if (!root) return NULL;
    Directory* users = findChildDir(root, "users");
    if (!users) return NULL;
    return findChildDir(users, session.getUser()->getUsername().c_str());
}

static void endTimedContestAndReturn(ContestManager& cm, VFSManager& vfs, const Session& session, Contest* contest, UserManager& um) {
    Directory* userDir = getUserDir(vfs, session);
    if (userDir && contest) contest->saveProgress(vfs, userDir);
    delete contest;
    cm.endContest(session, true, um);
    cout << "Contest time ended. Returning to dashboard.\n";
}

static void computeLiveScore(const Contest* contest, ProblemBank& bank, int& solvedCount,
                             int& wrongAttempts, int& credit, int& score) {
    solvedCount = 0;
    wrongAttempts = 0;
    credit = 0;
    score = 0;
    if (!contest) return;
    for (int i = 0; i < contest->problemCount(); i++) {
        bool solved = contest->isSolvedAt(i);
        int attempts = contest->getAttemptsAt(i);
        wrongAttempts += ScoreCalculator::computeWrongAttempts(attempts, solved);
        if (solved) {
            solvedCount++;
            Problem* p = bank.getById(contest->problemIdAt(i));
            int diff = p ? p->getDifficulty() : 1;
            credit += diff * 100;
        }
    }
    score = ScoreCalculator::computeScore(credit, wrongAttempts, 0);
}

void ContestScreen::startNew(ContestManager& cm, ProblemBank& bank, VFSManager& vfs, const Session& session, EvaluationEngine& engine, UserManager& um) {
    cout << "\n--- Start Contest ---\n";
    char buf[32];
    if (!readRequiredLine("Variant (1-5): ", buf, (int)sizeof(buf))) return;
    int variant = toInt(buf, 4);
    if (!readRequiredLine("Duration seconds (e.g., 120): ", buf, (int)sizeof(buf))) return;
    int dur = toInt(buf, 120);
    Contest* contest = cm.startContest(session, variant, dur);
    if (!contest) {
        cout << "Failed to start contest\n";
        return;
    }
    runLoop(cm, bank, vfs, session, engine, contest, um);
}

void ContestScreen::resumeExisting(ContestManager& cm, ProblemBank& bank, VFSManager& vfs, const Session& session, EvaluationEngine& engine, UserManager& um) {
    cout << "\n--- Resume Contest ---\n";
    Contest* contest = cm.resumeContest(session);
    if (!contest) {
        cout << "No active contest found\n";
        return;
    }
    runLoop(cm, bank, vfs, session, engine, contest, um);
}

void ContestScreen::runLoop(ContestManager& cm, ProblemBank& bank, VFSManager& vfs, const Session& session, EvaluationEngine& engine, Contest* contest, UserManager& um) {
    if (!contest) return;
    while (true) {
        cout << "\n=== Contest ===\n";
        cout << "Type: " << contest->getType() << "\n";
        cout << "Remaining seconds: " << contest->remainingSeconds() << "\n";
        int solvedCount = 0;
        int wrongAttempts = 0;
        int credit = 0;
        int liveScore = 0;
        computeLiveScore(contest, bank, solvedCount, wrongAttempts, credit, liveScore);
        cout << "Solved: " << solvedCount << " | Wrong attempts: " << wrongAttempts << " | Credit: " << credit << " | Current score: " << liveScore << "\n";
        for (int i = 0; i < contest->problemCount(); i++) {
            cout << "  [" << i << "] id=" << contest->problemIdAt(i) << " attempts=" << contest->getAttemptsAt(i) << " solved=" << (contest->isSolvedAt(i) ? "yes" : "no") << "\n";
        }
        if (contest->hasEnded()) {
            endTimedContestAndReturn(cm, vfs, session, contest, um);
            return;
        }
        cout << "\n1) Submit Solution\n2) Save and Back\n3) End Contest (Keep + Score)\n4) End Contest (Discard)\n";
        char buf[64];
        if (!readRequiredLine("Choose: ", buf, (int)sizeof(buf))) {
            Directory* userDir = getUserDir(vfs, session);
            if (userDir) contest->saveProgress(vfs, userDir);
            delete contest;
            return;
        }
        if (contest->hasEnded()) {
            endTimedContestAndReturn(cm, vfs, session, contest, um);
            return;
        }
        int ch = toInt(buf, 0);
        if (ch == 2) {
            Directory* userDir = getUserDir(vfs, session);
            if (userDir) contest->saveProgress(vfs, userDir);
            delete contest;
            return;
        }
        if (ch == 3) {
            Directory* userDir = getUserDir(vfs, session);
            if (userDir) contest->saveProgress(vfs, userDir);
            delete contest;
            cm.endContest(session, true, um);
            return;
        }
        if (ch == 4) {
            delete contest;
            cm.endContest(session, false, um);
            return;
        }
        if (ch != 1) continue;
        if (!readRequiredLine("Problem index: ", buf, (int)sizeof(buf))) continue;
        if (contest->hasEnded()) {
            endTimedContestAndReturn(cm, vfs, session, contest, um);
            return;
        }
        int idx = toInt(buf, -1);
        if (idx < 0 || idx >= contest->problemCount()) {
            cout << "Invalid index\n";
            continue;
        }
        int pid = contest->problemIdAt(idx);
        Problem* p = bank.getById(pid);
        if (!p) {
            cout << "Problem not found in bank\n";
            continue;
        }
        cout << "\n[Problem " << p->getId() << "] " << p->getTitle() << "\n";
        cout << "Statement: " << p->getStatement() << "\n\n";
        Verdict v;
        CustomString draft("");
        if (p->isShortAnswer()) {
            char ans[512];
            if (!readRequiredLine("Your answer: ", ans, (int)sizeof(ans))) continue;
            if (contest->hasEnded()) {
                endTimedContestAndReturn(cm, vfs, session, contest, um);
                return;
            }
            v = engine.evaluateAnswer(ans, *p);
            draft = ans;
        } else {
            cout << "Only short-answer problems are supported in this mode.\n";
            continue;
        }
        cout << "Verdict: " << Verdict::codeToString(v.getCode()) << " | " << v.getMessage() << "\n";
        bool accepted = (v.getCode() == Verdict::Accepted);
        contest->recordAttempt(idx, accepted, draft.c_str());
        computeLiveScore(contest, bank, solvedCount, wrongAttempts, credit, liveScore);
        cout << "Updated credit: " << credit << " | Updated score: " << liveScore << "\n";
        Directory* userDir = getUserDir(vfs, session);
        if (userDir) contest->saveProgress(vfs, userDir);
    }
}

}
