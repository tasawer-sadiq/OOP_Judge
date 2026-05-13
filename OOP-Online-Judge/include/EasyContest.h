#ifndef OJ_EASYCONTEST_H
#define OJ_EASYCONTEST_H

#include "Contest.h"

namespace OJ {

class EasyContest : public Contest {
protected:
    void buildDifficultyProfile(int* outDiffs, int& outCount) const {
        // 5 problems: 1, 1, 2, 2, 3
        outCount = 0;
        outDiffs[outCount++] = 1;
        outDiffs[outCount++] = 2;
        outDiffs[outCount++] = 3;
        outDiffs[outCount++] = 4;
        outDiffs[outCount++] = 5;
    }

public:
    EasyContest(int durationSec)
        : Contest("easy", "EasyContest", durationSec) {}
};

}

#endif
