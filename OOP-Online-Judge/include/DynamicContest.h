#ifndef OJ_DYNAMICCONTEST_H
#define OJ_DYNAMICCONTEST_H

#include "Contest.h"

namespace OJ {

class DynamicContest : public Contest {
private:
    int targetDiff;
protected:
    void buildDifficultyProfile(int* outDiffs, int& outCount) const override {
        outCount = 5;
        for (int i = 0; i < 5; i++) outDiffs[i] = targetDiff;
    }

public:
    DynamicContest(int durationSec, int diff)
        : Contest("custom", "CustomContest", durationSec), targetDiff(diff) {}
};

}

#endif
