#ifndef OJ_CONTESTMANAGER_H
#define OJ_CONTESTMANAGER_H

#include <ctime>

#include "Contest.h"
#include "EasyContest.h"
#include "BalancedContest.h"
#include "HardContest.h"
#include "DynamicContest.h"
#include "Session.h"
#include "ScoreCalculator.h"
#include "ContestResult.h"
#include "TextUtil.h"
#include "UserManager.h"

namespace OJ {

class ContestManager {
private:
    VFSManager* vfs;
    ProblemBank* bank;

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

    static File* findChildFile(Directory* parent, const char* name) {
        if (!parent) return NULL;
        for (int i = 0; i < parent->childCount(); i++) {
            File* f = (File*)(parent->childAt(i));
            if (f && sameName(f->getName(), name)) return f;
        }
        return NULL;
    }

    static int readIntFile(Directory* dir, const char* fileName, int def = 0) {
        File* f = findChildFile(dir, fileName);
        if (!f) return def;
        const char* s = f->getData();
        if (!s) return def;
        int sign = 1;
        int i = 0;
        if (s[i] == '-') { sign = -1; i++; }
        int val = 0;
        while (s[i] >= '0' && s[i] <= '9') {
            val = val * 10 + (s[i] - '0');
            i++;
        }
        return val * sign;
    }

    static bool parseStrAfterKey(const char* s, const char* key, char* outBuf, int outCap) {
        if (!s || !key || !outBuf || outCap <= 0) return false;
        for (int i = 0; s[i] != '\0'; i++) {
            int k = 0;
            while (key[k] != '\0' && s[i + k] != '\0' && s[i + k] == key[k]) k++;
            if (key[k] == '\0') {
                const char* p = s + i + k;
                int j = 0;
                while (*p != '\0' && *p != ';' && j < outCap - 1) {
                    outBuf[j++] = *p++;
                }
                outBuf[j] = '\0';
                return true;
            }
        }
        return false;
    }

    static bool parseIntAfterKey(const char* s, const char* key, int& outVal) {
        if (!s || !key) return false;
        for (int i = 0; s[i] != '\0'; i++) {
            int k = 0;
            while (key[k] != '\0' && s[i + k] != '\0' && s[i + k] == key[k]) k++;
            if (key[k] == '\0') {
                const char* p = s + i + k;
                int sign = 1;
                if (*p == '-') { sign = -1; p++; }
                int v = 0;
                while (*p >= '0' && *p <= '9') {
                    v = v * 10 + (*p - '0');
                    p++;
                }
                outVal = v * sign;
                return true;
            }
        }
        return false;
    }

    Directory* getUserDir(const Session& session) const {
        if (!vfs) return NULL;
        if (!session.isActive() || !session.getUser()) return NULL;

        Directory* root = vfs->getRoot();
        if (!root) return NULL;

        Directory* users = findChildDir(root, "users");
        if (!users) return NULL;

        // find username directory directly under /users
        const char* uname = session.getUser()->getUsername().c_str();
        return findChildDir(users, uname);
    }

public:
    ContestManager(VFSManager& vfsMgr, ProblemBank& pb)
        :vfs(&vfsMgr),bank(&pb){}

    // 2) Initialize and start a new contest
    Contest* startContest(const Session& session, int variant /*1-5*/, int durationSec) {
        if (!vfs || !bank) return NULL;
        Directory* userDir = getUserDir(session);
        if (!userDir) return NULL;

        Contest* c = NULL;
        if (variant == 11) c = new EasyContest(durationSec);
        else if (variant == 13) c = new BalancedContest(durationSec);
        else if (variant == 15) c = new HardContest(durationSec);
        else {
            int d = variant - 10;
            if (d < 1) d = 1;
            if (d > 5) d = 5;
            c = new DynamicContest(durationSec, d);
        }

        c->start(*bank);
        c->saveProgress(*vfs, userDir);
        return c;
    }

    // 3) Restore an active contest from persistence
    Contest* resumeContest(const Session& session) {
        if (!vfs || !bank) return NULL;
        Directory* userDir = getUserDir(session);
        if (!userDir) return NULL;

        Directory* contests = findChildDir(userDir, "contests");
        if (!contests) return NULL;
        Directory* active = findChildDir(contests, "active");
        if (!active) return NULL;

        File* meta = NULL;
        for (int i = 0; i < active->childCount(); i++) {
            File* f = (File*)(active->childAt(i));
            if (f && sameName(f->getName(), "meta.txt")) { meta = f; break; }
        }
        if (!meta) return NULL;

        const char* m = meta->getData();
        if (!m) return NULL;

        // parse type=...
        char type[24];
        type[0] = '\0';
        for (int i = 0; m[i] != '\0'; i++) {
            if (m[i]=='t'&&m[i+1]=='y'&&m[i+2]=='p'&&m[i+3]=='e'&&m[i+4]=='=') {
                int j = 0;
                i += 5;
                while (m[i] != '\0' && m[i] != ';' && j < 23) {
                    type[j++] = m[i++];
                }
                type[j] = '\0';
                break;
            }
        }

        int duration = 600;
        for (int i = 0; m[i] != '\0'; i++) {
            if (m[i]=='d'&&m[i+1]=='u'&&m[i+2]=='r'&&m[i+3]=='=') {
                duration = 0;
                i += 4;
                while (m[i] >= '0' && m[i] <= '9') {
                    duration = duration * 10 + (m[i]-'0');
                    i++;
                }
                break;
            }
        }

        Contest* c = NULL;
        if (sameName(type, "easy")) c = new EasyContest(duration);
        else if (sameName(type, "balanced")) c = new BalancedContest(duration);
        else c = new HardContest(duration);

        if (!c->loadProgress(*vfs, userDir)) {
            delete c;
            return NULL;
        }

        return c;
    }

    bool saveContestProgress(const Session& session, Contest* activeContest) {
        if (!vfs || !activeContest) return false;
        Directory* userDir = getUserDir(session);
        if (!userDir) return false;
        return activeContest->saveProgress(*vfs, userDir);
    }

    bool endContest(const Session& session, bool keep, UserManager& userManager, Contest* activeContest = NULL) {
        if (!vfs) return false;
        Directory* userDir = getUserDir(session);
        if (!userDir) return false;

        if (activeContest) {
            activeContest->saveProgress(*vfs, userDir);
        }

        Directory* contests = findChildDir(userDir, "contests");
        if (!contests) return false;
        Directory* active = findChildDir(contests, "active");
        if (!active) return false;

        if (!keep) {
            // discard completely
            contests->remove("active");
            vfs->createDirectory(contests, "active");
            return true;
        }

        // Phase 6: compute and persist contest result into active/ so it moves into history.
        // Uses only VFS stored progress (p/a/s + meta), so it works even if Contest object is gone.
        {
            int count = readIntFile(active, "count.txt", 0);
            File* meta = findChildFile(active, "meta.txt");

            char typeBuf[24];
            typeBuf[0] = '\0';
            int startEpoch = 0;
            int dur = 0;

            if (meta && meta->getData()) {
                const char* m = meta->getData();
                parseStrAfterKey(m, "type=", typeBuf, (int)sizeof(typeBuf));
                parseIntAfterKey(m, "start=", startEpoch);
                parseIntAfterKey(m, "dur=", dur);
            }

            long long endEpoch = (long long)std::time(NULL);
            int minutes = ScoreCalculator::minutesUsed((long long)startEpoch, endEpoch);

            int solvedCount = 0;
            int wrongAttempts = 0;
            int basePoints = 0;

            for (int i = 0; i < count; i++) {
                char fa[16];
                char fs[16];
                TextUtil::clearBuf(fa, (int)sizeof(fa));
                TextUtil::clearBuf(fs, (int)sizeof(fs));
                int p2 = 0;
                int p3 = 0;
                TextUtil::appendChar(fa, (int)sizeof(fa), p2, 'a');
                TextUtil::appendInt(fa, (int)sizeof(fa), p2, i);
                TextUtil::appendCStr(fa, (int)sizeof(fa), p2, ".txt");
                TextUtil::appendChar(fs, (int)sizeof(fs), p3, 's');
                TextUtil::appendInt(fs, (int)sizeof(fs), p3, i);
                TextUtil::appendCStr(fs, (int)sizeof(fs), p3, ".txt");

                int att = readIntFile(active, fa, 0);
                int sol = readIntFile(active, fs, 0);
                wrongAttempts += ScoreCalculator::computeWrongAttempts(att, sol != 0);
            }

            int hCount = readIntFile(active, "h_count.txt", 0);
            solvedCount = hCount;
            for (int i = 0; i < hCount; i++) {
                char hf[16];
                TextUtil::clearBuf(hf, (int)sizeof(hf));
                int hp = 0;
                TextUtil::appendChar(hf, (int)sizeof(hf), hp, 'h');
                TextUtil::appendInt(hf, (int)sizeof(hf), hp, i);
                TextUtil::appendCStr(hf, (int)sizeof(hf), hp, ".txt");
                int pid = readIntFile(active, hf, -1);
                if (pid != -1) {
                    int diff = 1;
                    if (bank) {
                        Problem* p = bank->getById(pid);
                        if (p) diff = p->getDifficulty();
                    }
                    basePoints += diff * 100;
                }
            }

            int score = ScoreCalculator::computeScore(basePoints, wrongAttempts, minutes);
            ContestResult r(typeBuf[0] ? typeBuf : "unknown", score, solvedCount, wrongAttempts, minutes, endEpoch);

            char buf[96];
            r.serialize(buf, (int)sizeof(buf));
            vfs->createFile(active, "result.txt", buf);

            if (session.getUser()) {
                session.getUser()->setContestCredits(session.getUser()->getContestCredits() + score);
                userManager.rewriteUserFiles(*session.getUser());
            }
        }

        // keep: move children of active into history/run_<epoch>
        Directory* history = findChildDir(contests, "history");
        if (!history) history = vfs->createDirectory(contests, "history");
        if (!history) return false;

        char runName[24];
        TextUtil::clearBuf(runName, (int)sizeof(runName));
        int rp = 0;
        TextUtil::appendCStr(runName, (int)sizeof(runName), rp, "run_");
        TextUtil::appendInt(runName, (int)sizeof(runName), rp, (int)std::time(NULL));
        Directory* runDir = vfs->createDirectory(history, runName);
        if (!runDir) return false;

        // move each child by name (always take index 0 because move shrinks array)
        while (active->childCount() > 0) {
            FSE* child = active->childAt(0);
            if (!child) break;
            const char* nm = child->getName();
            if (!nm) break;
            active->move(nm, runDir);
        }

        contests->remove("active");
        vfs->createDirectory(contests, "active");
        return true;
    }
};

}

#endif
