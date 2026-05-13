# OOP-Online-Judge — Workflow & Phase Plan (Generic)

## Purpose

Build the project phase-by-phase, keeping each phase small, testable, and easy to integrate.

This document is tool-agnostic (you can use any method to share code). The key idea is coordination: **one file has one owner at a time**.

## Coordination Rules (Tool-Agnostic)

Most important rule:

```text
One contributor should work on one file at a time.
Do not let two contributors edit the same file concurrently.
```

Recommended team habits:

- Before starting, sync to the latest shared project copy (however your team shares code).
- Announce which files you are taking ownership of for a short time.
- Integrate changes in small batches; do not wait until the very end of a phase.
- Share only code that compiles and runs the basic flow.
- Keep filenames aligned with class names and keep interfaces stable.

## Recommended Project Structure

```text
ProjectRoot/
  OOP-Online-Judge/
    include/            # headers
    src/                # implementations
    main.cpp            # entry point / integration test

  Raw/                  # notes + helper files shared with the team
    Steps.md
    runcode.txt
    project docs (optional)
```

## Global Constraints

- Do not use `std::string`.
- Do not use STL containers (e.g., `vector`, `list`, `map`, `queue`, `stack`).
- Prefer one class per `.h/.cpp` pair where applicable.
- Keep code compiling frequently; treat `main.cpp` as an integration checkpoint.

## How To Execute Any Phase (Checklist)

```text
1) Define the phase scope (which classes/files are in this phase).
2) Assign file ownership (one owner per file).
3) Implement the required classes and behaviors.
4) Integrate everyone’s work into the shared codebase.
5) Build and run a quick integration flow.
6) Record what’s done + what’s next, then start the next phase.
```

## Phase-Wise Development Plan

Each phase lists its goal, target files/components, and a clear “done” definition.

## Phase 0: Setup

Goal:

Create the initial folder structure and a working build/run path.

Files to create:

```text
OOP-Online-Judge/include/
OOP-Online-Judge/src/
OOP-Online-Judge/main.cpp
Raw/
build/run instructions (compiler command or IDE task)
```

Completion rule:

```text
Project folder exists and is shared with the team.
Basic project compiles.
```

## Phase 1: Foundation Classes

Goal:

Build the basic classes that replace STL and support the whole project.

Files to create:

```text
include/CustomString.h
src/CustomString.cpp

include/DynamicArray.h
src/DynamicArray.cpp

include/LinkedList.h
src/LinkedList.cpp

include/KeyValueStore.h
src/KeyValueStore.cpp

include/VFSManager.h
src/VFSManager.cpp
```

Suggested parallelization:

```text
Assign each `.h/.cpp` pair to one contributor.
Use `main.cpp` (or a small test harness) to validate integration.
```

Completion rule:

```text
CustomString works.
DynamicArray works.
LinkedList works.
KeyValueStore works.
VFSManager basic functions work.
No std::string used.
No STL containers used.
Code compiles.
```

## Phase 2: User System

Goal:

Build registration, verification, login, session, and account deletion.

Files to create:

```text
include/User.h
src/User.cpp

include/UserManager.h
src/UserManager.cpp

include/Session.h
src/Session.cpp

include/VerificationService.h
src/VerificationService.cpp
```

Completion rule:

```text
User can register.
Verification token works.
Verified user can log in.
Unverified user cannot log in.
Session starts and ends.
User folder is created in VFS.
Account deletion removes user data.
Code compiles.
```

## Phase 3: Problem Bank

Goal:

Store and load coding problems from VFS.

Files to create:

```text
include/Problem.h
src/Problem.cpp

include/TestCase.h
src/TestCase.cpp

include/ProblemBank.h
src/ProblemBank.cpp
```

Completion rule:

```text
Problem stores title, difficulty, statement, constraints, and examples.
TestCase stores input and expected output.
ProblemBank can load problems.
ProblemBank can filter by difficulty.
ProblemBank can return a problem by ID.
Minimum 10 problems per difficulty are planned.
Code compiles.
```

## Phase 4: Contest System

Goal:

Build contest variants, random problem selection, timer data, save progress, and resume progress.

Files to create:

```text
include/Contest.h
src/Contest.cpp

include/EasyContest.h
src/EasyContest.cpp

include/BalancedContest.h
src/BalancedContest.cpp

include/HardContest.h
src/HardContest.cpp

include/ContestManager.h
src/ContestManager.cpp
```

Completion rule:

```text
Base Contest class exists.
Derived contest classes exist.
Polymorphism is used.
Contest can select problems.
Contest can save progress.
Contest can resume progress.
Timer uses start time and duration.
Code compiles.
```

## Phase 5: Evaluation Engine

Goal:

Compile submitted code, run it against test cases, compare output, and return verdict.

Files to create:

```text
include/Submission.h
src/Submission.cpp

include/EvaluationEngine.h
src/EvaluationEngine.cpp

include/OutputComparator.h
src/OutputComparator.cpp

include/Verdict.h
src/Verdict.cpp
```

Completion rule:

```text
Code can be submitted.
Compilation error is detected.
Accepted verdict works.
Wrong answer verdict works.
Runtime error is handled.
Temporary files are cleaned.
Code compiles.
```

## Phase 6: Scoring System

Goal:

Calculate score using correctness, attempts, and time.

Files to create:

```text
include/ScoreCalculator.h
src/ScoreCalculator.cpp

include/ContestResult.h
src/ContestResult.cpp
```

Example formula:

```text
score = difficulty * 100 - wrongAttempts * 10 - minutesUsed
```

Completion rule:

```text
Correct solution gives score.
Wrong attempts reduce score.
Late solving reduces score.
Score never becomes negative.
Contest result stores total score.
Code compiles.
```

## Phase 7: UI System

Goal:

Build screens and connect user interaction with backend classes.

Files to create:

```text
include/UIManager.h
src/UIManager.cpp

include/LoginScreen.h
src/LoginScreen.cpp

include/RegisterScreen.h
src/RegisterScreen.cpp

include/DashboardScreen.h
src/DashboardScreen.cpp

include/ProblemBrowserScreen.h
src/ProblemBrowserScreen.cpp

include/ContestScreen.h
src/ContestScreen.cpp

include/ResultScreen.h
src/ResultScreen.cpp
```

Completion rule:

```text
Login screen works.
Register screen works.
Dashboard works.
Problem browser works.
Contest screen shows timer.
Result screen shows score.
UI connects to backend classes.
Code compiles.
```

## Phase 8: Full Integration

Goal:

Connect all modules into one working application.

Test full flow:

```text
Register user.
Verify user.
Log in.
Browse problems.
Start contest.
Write code.
Submit code.
Get verdict.
Update score.
Save progress.
Close program.
Resume contest.
End contest.
Keep or delete code.
Delete account.
```

Completion rule:

```text
Full user journey works.
No STL containers.
No std::string.
All persistent data goes through VFS.
Temporary evaluation files are cleaned.
No memory leaks.
Code compiles without warnings.
UML matches code.
```

## Phase Integration Order (Generic)

Use this order inside every phase:

```text
1) Assign file ownership for the phase.
2) Each contributor implements their owned files.
3) Integrate changes into the shared project copy.
4) Build and run a quick end-to-end check.
5) Fix integration issues immediately.
6) Mark the phase complete and move to the next one.
```

## Final Advice

```text
One file, one owner, one integration at a time.
```

Integrate after each phase (and ideally multiple times within a phase). Keeping the integration surface small makes bugs faster to find and fix.
