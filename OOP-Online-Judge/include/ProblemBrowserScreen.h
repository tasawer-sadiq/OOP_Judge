#ifndef OJ_PROBLEMBROWSERSCREEN_H
#define OJ_PROBLEMBROWSERSCREEN_H

#include "EvaluationEngine.h"
#include "ProblemBank.h"

namespace OJ {

class ProblemBrowserScreen {
public:
    void run(const ProblemBank& bank, EvaluationEngine& engine);
};

}

#endif
