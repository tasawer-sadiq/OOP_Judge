#include <cstdlib>
#include <ctime>
#include <iostream>

using namespace std;

#include "include/UIManager.h"
#include "include/UserManager.h"
#include "include/VFSManager.h"

//  demo dependencies
#include "include/ContestManager.h"
#include "include/ContestResult.h"
#include "include/EvaluationEngine.h"
#include "include/ProblemBank.h"
#include "include/Submission.h"

static bool sameName(const char *a, const char *b) {
  if (a == b)
    return true;
  if (!a || !b)
    return false;
  int i = 0;
  while (a[i] != '\0' && b[i] != '\0') {
    if (a[i] != b[i])
      return false;
    i++;
  }
  return a[i] == '\0' && b[i] == '\0';
}

static Directory *findChildDir(Directory *parent, const char *name) {
  if (!parent)
    return NULL;
  for (int i = 0; i < parent->childCount(); i++) {
    FSE *child = parent->childAt(i);
    if (child && sameName(child->getName(), name))
      return (Directory *)child;
  }
  return NULL;
}

static File *findChildFile(Directory *parent, const char *name) {
  if (!parent)
    return NULL;
  for (int i = 0; i < parent->childCount(); i++) {
    FSE *child = parent->childAt(i);
    if (child && sameName(child->getName(), name))
      return (File *)child;
  }
  return NULL;
}

static Directory *getUserDir(VFSManager &vfs, const char *username) {
  Directory *root = vfs.getRoot();
  if (!root)
    return NULL;

  Directory *users = findChildDir(root, "users");
  if (!users)
    return NULL;

  return findChildDir(users, username);
}

static Directory *findLatestRunDir(Directory *history) {
  if (!history)
    return NULL;

  Directory *best = NULL;
  int bestTs = -1;

  for (int i = 0; i < history->childCount(); i++) {
    FSE *child = history->childAt(i);
    if (!child || !child->getName())
      continue;
    Directory *d = (Directory *)child;

    const char *n = d->getName();
    if (!(n[0] == 'r' && n[1] == 'u' && n[2] == 'n' && n[3] == '_'))
      continue;

    int ts = 0;
    int j = 4;
    bool ok = false;
    while (n[j] >= '0' && n[j] <= '9') {
      ok = true;
      ts = ts * 10 + (n[j] - '0');
      j++;
    }
    if (!ok)
      continue;

    if (ts > bestTs) {
      bestTs = ts;
      best = d;
    }
  }

  return best;
}

// Removed writeTempSource since we just use variables for evaluation now

static int runUI() {
  VFSManager vfs;
  vfs.init("OJ_VOL");

  OJ::UserManager um(vfs);
  OJ::UIManager ui(vfs, um);
  ui.run();
  return 0;
}

static int runIntegrationDemo() {
  cout << "Running Full Integration Demo\n\n";

  // 1) Initialize VFS
  VFSManager vfs;
  vfs.init("OJ_VOL");

  // 2) Problems
  OJ::ProblemBank bank;
  bank.seedIfMissing(vfs);
  bank.loadFromVFS(vfs);
  cout << "Problems loaded: " << bank.count() << "\n";

  // 3) Register/verify/login
  OJ::UserManager um(vfs);
  OJ::Session session;

  CustomString token;
  bool reg=um.registerUser("musa","musa@email.com","pw",token);
  if (reg) {
    cout << "Registered user musa (token=" << token << ")\n";
    um.verifyUser("musa", token.c_str());
    cout << "Verified user musa\n";
  }

  bool loginOk=um.login("musa", "pw", session);
  cout<<"Login: "<<(loginOk ? "OK" : "FAIL")<<"\n\n";
  if (!loginOk)
    return 1;

  // 4) Start contest
  OJ::ContestManager cm(vfs, bank);
  OJ::Contest *contest = cm.startContest(session, 4, 120); // balanced
  if (!contest) {
    cout << "startContest failed\n";
    return 1;
  }
  cout << "Contest started (type=" << contest->getType() << ")\n";

  // 5) Simulate close/resume
  delete contest;
  contest = NULL;

  contest = cm.resumeContest(session);
  if (!contest) {
    cout << "resumeContest failed\n";
    return 1;
  }
  cout << "Contest resumed. Problems=" << contest->problemCount() << "\n";

  if (contest->problemCount() <= 0) {
    cout << "No problems in contest\n";
    delete contest;
    return 1;
  }

  // Pick first problem and submit a correct answer/solution.
  int pid = contest->problemIdAt(0);
  OJ::Problem *p = bank.getById(pid);
  if (!p) {
    cout << "Problem not found: id=" << pid << "\n";
    delete contest;
    return 1;
  }

  // 7) Initialize engine and evaluate a submission
  OJ::EvaluationEngine engine("");
  OJ::Verdict v;
  CustomString submitted("");

  // We just take input in a variable
  submitted = p->testCaseAt(0).getExpectedOutput();
  v = engine.evaluateAnswer(submitted.c_str(), *p);

  cout << "Submission verdict: " << OJ::Verdict::codeToString(v.getCode())
       << " | " << v.getMessage() << "\n";

  bool accepted = (v.getCode() == OJ::Verdict::Accepted);
  contest->recordAttempt(0, accepted, submitted.c_str());

  // Save progress after attempt
  Directory *userDir = getUserDir(vfs, "musa");
  if (userDir) {
    contest->saveProgress(vfs, userDir);
  }

  delete contest;
  contest = NULL;

  // 7) End contest (keep) -> scoring + result.txt persisted and moved to
  // history
  cout<<"\nEnding contest (keep=true) ...\n";
  cm.endContest(session,true,um);

  // 8) Load latest history result
  userDir = getUserDir(vfs, "musa");
  Directory *contestsDir = userDir ? findChildDir(userDir, "contests") : NULL;
  Directory *history =
      contestsDir ? findChildDir(contestsDir, "history") : NULL;

  Directory *runDir = findLatestRunDir(history);
  if (!runDir) {
    cout << "No history run directory found\n";
    return 0;
  }

  File *rf = findChildFile(runDir, "result.txt");
  if (!rf || !rf->getData()) {
    cout << "result.txt not found\n";
    return 0;
  }

  OJ::ContestResult r;
  OJ::ContestResult::deserialize(rf->getData(), r);

  cout << "\nContest Result (from VFS):\n";
  cout << "  type=" << r.getContestType() << "\n";
  cout << "  score=" << r.getTotalScore() << "\n";
  cout << "  solved=" << r.getSolvedCount() << "\n";
  cout << "  wrongAttempts=" << r.getWrongAttempts() << "\n";
  cout << "  minutesUsed=" << r.getMinutesUsed() << "\n";

  cout << "\nIntegration demo complete.\n";
  return 0;
}

#include "sfml_ui/SFMLApp.h"

int main(int argc, char **argv) {
  if (argc >= 2 && sameName(argv[1], "--sfml")) {
    VFSManager vfs;
    vfs.init("OJ_VOL");

    OJ::UserManager userManager(vfs);

    OJUI::SFMLApp app(vfs, userManager);
    app.run();
    return 0;
  }

  if (argc >= 2 && (sameName(argv[1], "--demo") || sameName(argv[1], "demo"))) {
    return runIntegrationDemo();
  }


  return runUI();
}
