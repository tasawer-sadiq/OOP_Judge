#ifndef OJ_BALANCEDCONTEST_H
#define OJ_BALANCEDCONTEST_H

#include "Contest.h"

namespace OJ {

class BalancedContest : public Contest {
protected:
    void buildDifficultyProfile(int* outDiffs, int& outCount) const {
        // 5 problems: 1, 2, 3, 4, 5
        outCount = 0;
        outDiffs[outCount++] = 1;
        outDiffs[outCount++] = 2;
        outDiffs[outCount++] = 3;
        outDiffs[outCount++] = 4;
        outDiffs[outCount++] = 5;
    }

public:
    BalancedContest(int durationSec)
        : Contest("balanced", "BalancedContest", durationSec) {}
};

}

#endif
