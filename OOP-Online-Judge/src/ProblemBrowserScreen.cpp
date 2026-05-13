#include "../include/ProblemBrowserScreen.h"

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

void ProblemBrowserScreen::run(const ProblemBank& bank, EvaluationEngine& engine) {
    cout << "\n--- Problem Browser ---\n";
    cout << "Total problems: " << bank.count() << "\n";

    char buf[32];
    if (!readRequiredLine("Filter difficulty (1-5) or 0 for all: ", buf, (int)sizeof(buf))) return;
    int diff = toInt(buf, 0);

    Problem* list[500];
    int listSize = 0;
    if (diff >= 1 && diff <= 5) {
        bank.filterByDifficulty(diff, list, listSize, 500);
    } else {
        for (int i = 0; i < bank.count() && listSize < 500; i++) {
            Problem* p = bank.getAt(i);
            if (p) list[listSize++] = p;
        }
    }

    cout<<"Showing "<<listSize<<" problems\n";
    for(int i=0;i<listSize;i++){
        cout<<"  id="<<list[i]->getId()<<" diff="<<list[i]->getDifficulty()<<" title="<<list[i]->getTitle()<<"\n";
    }


    if (!readRequiredLine("Enter a problem id to view statement (or 0 to back): ", buf, (int)sizeof(buf))) return;
    int pid = toInt(buf, 0);
    if (pid == 0) return;

    Problem* p = bank.getById(pid);
    if (!p) {
        cout << "Problem not found\n";
        return;
    }

    cout << "\n[Problem " << p->getId() << "] " << p->getTitle() << "\n";
    cout << "Difficulty: " << p->getDifficulty() << "\n";
    cout << "Statement: " << p->getStatement() << "\n";

    if (p->isShortAnswer()) {
        char ans[512];
        if (!readRequiredLine("\nYour answer: ", ans, (int)sizeof(ans))) return;
        Verdict v = engine.evaluateAnswer(ans, *p);
        cout << "Verdict: " << Verdict::codeToString(v.getCode()) << " | " << v.getMessage() << "\n";
        if (v.getCode() == Verdict::Accepted) {
            int credit = p->getDifficulty() * 100;
            cout << "Practice credit earned: " << credit << "\n";
        } else {
            cout << "Practice credit earned: 0\n";
        }
    } else {
        cout << "\nOnly short-answer problems are supported in practice mode.\n";
    }

    readRequiredLine("\nType anything to continue: ", buf, (int)sizeof(buf));
}

}
