#ifndef OJ_HARDCONTEST_H
#define OJ_HARDCONTEST_H

#include "Contest.h"

namespace OJ {

class HardContest : public Contest {
protected:
    void buildDifficultyProfile(int* outDiffs, int& outCount) const {
        // Set difficulty sequence
        outCount = 0;
        outDiffs[outCount++] = 1;
        outDiffs[outCount++] = 2;
        outDiffs[outCount++] = 3;
        outDiffs[outCount++] = 4;
        outDiffs[outCount++] = 5;
    }

public:
    HardContest(int durationSec)
        : Contest("hard", "HardContest", durationSec) {}
};

}

#endif
