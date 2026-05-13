#ifndef OJ_EVALUATIONENGINE_H
#define OJ_EVALUATIONENGINE_H

#include "CustomString.h"
#include "Submission.h"
#include "Verdict.h"
#include "OutputComparator.h"
#include "Problem.h"
#include "TextUtil.h"

namespace OJ {

class EvaluationEngine {
private:
    CustomString workDir;

    static bool isWs(char c) {
        return c == ' ' || c == '\t' || c == '\r' || c == '\n';
    }

    static char lowerAscii(char c) {
        if (c >= 'A' && c <= 'Z') return (char)(c - 'A' + 'a');
        return c;
    }

    static void normalizeAnswer(const char* in, char* out, int cap) {
        if (!out || cap <= 0) return;
        out[0] = '\0';
        if (!in) in = "";
        int n = TextUtil::strLen(in);
        int i = 0;
        while (i < n && isWs(in[i])) i++;
        int j = n - 1;
        while (j >= i && isWs(in[j])) j--;
        int pos = 0;
        bool prevWs = false;
        for (int k = i; k <= j; k++) {
            char c = in[k];
            bool ws = isWs(c);
            if (ws) {
                if (!prevWs) {
                    TextUtil::appendChar(out, cap, pos, ' ');
                }
                prevWs = true;
            } else {
                TextUtil::appendChar(out, cap, pos, c);
                prevWs = false;
            }
        }
    }

    static bool sameNormalized(const char* a, const char* b) {
        char na[256];
        char nb[256];
        normalizeAnswer(a, na, (int)sizeof(na));
        normalizeAnswer(b, nb, (int)sizeof(nb));
        int i = 0;
        while (na[i] != '\0' && nb[i] != '\0') {
            if (lowerAscii(na[i]) != lowerAscii(nb[i])) return false;
            i++;
        }
        return na[i] == '\0' && nb[i] == '\0';
    }

public:
    EvaluationEngine() : workDir(".tmp_eval") {}
    explicit EvaluationEngine(const char* dir) : workDir(dir) {}

    Verdict evaluateAnswer(const char* answer, const Problem& problem) const {
        if (problem.testCaseCount() <= 0) {
            return Verdict(Verdict::InternalError, "No accepted answers configured");
        }
        for (int i = 0; i < problem.testCaseCount(); i++) {
            const TestCase& tc = problem.testCaseAt(i);
            if (sameNormalized(answer, tc.getExpectedOutput().c_str())) {
                return Verdict(Verdict::Accepted, "Correct");
            }
        }
        return Verdict(Verdict::WrongAnswer, "Incorrect answer");
    }
};

}

#endif
