#ifndef OJ_CONTEST_H
#define OJ_CONTEST_H

#include <ctime>
#include <cstdlib>

#include "CustomString.h"
#include "ProblemBank.h"
#include "VFSManager.h"
#include "TextUtil.h"

namespace OJ {

class Contest {
protected:
    CustomString contestType;  // "easy" | "balanced" | "hard"
    CustomString contestName;

    long long startEpoch;
    int durationSeconds;

    int problemIds[100];
    int attempts[100];
    int solved[100];
    CustomString drafts[100];
    int problemCountVal;
    int totalSolved;

    int solvedHistory[200];
    int solvedHistoryCount;

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

    static long long nowEpoch() {
        return (long long)std::time(NULL);
    }

    static bool parseIntAfterKey(const char* s, const char* key, int& out) {
        if (!s || !key) return false;
        for (int i = 0; s[i] != '\0'; i++) {
            int k = 0;
            while (key[k] != '\0' && s[i + k] != '\0' && s[i + k] == key[k]) k++;
            if (key[k] == '\0') {
                // parse integer at s[i+k]
                const char* p = s + i + k;
                int sign = 1;
                if (*p == '-') { sign = -1; p++; }
                int val = 0;
                while (*p >= '0' && *p <= '9') {
                    val = val * 10 + (*p - '0');
                    p++;
                }
                out = val * sign;
                return true;
            }
        }
        return false;
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

    static void ensureSeededRand() {
        static bool seeded = false;
        if (!seeded) {
            std::srand((unsigned int)std::time(NULL));
            seeded = true;
        }
    }

    static int randIndex(int n) {
        if (n <= 0) return 0;
        return std::rand() % n;
    }

    static void writeIntFile(VFSManager& vfs, Directory* dir, const char* fileName, int value) {
        char buf[32];
        TextUtil::clearBuf(buf, (int)sizeof(buf));
        int pos = 0;
        TextUtil::appendInt(buf, (int)sizeof(buf), pos, value);
        // delete if exists then recreate (A3 File has no setter)
        dir->remove(fileName);
        vfs.createFile(dir, fileName, buf);
    }

    static int readIntFile(Directory* dir, const char* fileName, int def = 0) {
        File* f = findChildFile(dir, fileName);
        if (!f) return def;
        // parse int
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

    static void writeTextChunked(VFSManager& vfs, Directory* dir, const char* baseName, const char* text) {
        // Part storage logic
        if (!dir || !baseName) return;
        if (!text) text = "";

        // remove old chunks
        for (int i = 0; i < dir->childCount(); ) {
            File* f = (File*)(dir->childAt(i));
            if (!f) { i++; continue; }
            const char* n = f->getName();
            if (!n) { i++; continue; }

            // match prefix baseName
            int k = 0;
            while (baseName[k] != '\0' && n[k] != '\0' && baseName[k] == n[k]) k++;
            if (baseName[k] == '\0') {
                // remove will shift array
                dir->remove(n);
                continue;
            }
            i++;
        }

        int len = 0;
        while (text[len] != '\0') len++;

        int part = 0;
        int pos = 0;
        while (pos < len) {
            char chunk[96];
            int j = 0;
            while (pos < len && j < 90) {
                chunk[j++] = text[pos++];
            }
            chunk[j] = '\0';

            char fname[32];
            TextUtil::clearBuf(fname, (int)sizeof(fname));
            int fp = 0;
            TextUtil::appendCStr(fname, (int)sizeof(fname), fp, baseName);
            TextUtil::appendChar(fname, (int)sizeof(fname), fp, '_');
            TextUtil::appendInt(fname, (int)sizeof(fname), fp, part);
            TextUtil::appendCStr(fname, (int)sizeof(fname), fp, ".txt");
            vfs.createFile(dir, fname, chunk);
            part++;
        }

        if (len == 0) {
            char fname[32];
            TextUtil::clearBuf(fname, (int)sizeof(fname));
            int fp = 0;
            TextUtil::appendCStr(fname, (int)sizeof(fname), fp, baseName);
            TextUtil::appendCStr(fname, (int)sizeof(fname), fp, "_0.txt");
            vfs.createFile(dir, fname, "");
        }
    }

    static CustomString readTextChunked(Directory* dir, const char* baseName) {
        CustomString out("");
        if (!dir || !baseName) return out;

        // read parts in order baseName_0.txt, baseName_1.txt, ... until missing
        for (int part = 0; part < 200; part++) {
            char fname[32];
            TextUtil::clearBuf(fname, (int)sizeof(fname));
            int fp = 0;
            TextUtil::appendCStr(fname, (int)sizeof(fname), fp, baseName);
            TextUtil::appendChar(fname, (int)sizeof(fname), fp, '_');
            TextUtil::appendInt(fname, (int)sizeof(fname), fp, part);
            TextUtil::appendCStr(fname, (int)sizeof(fname), fp, ".txt");
            File* f = findChildFile(dir, fname);
            if (!f) break;
            out.append(f->getData());
        }
        return out;
    }

    virtual void buildDifficultyProfile(int* outDiffs, int& outCount) const = 0;

    void selectProblems(ProblemBank& bank, const int* diffs, int diffCount) {
        problemCountVal = 0;

        ensureSeededRand();

        for (int i = 0; i < diffCount; i++) {
            int diff = diffs[i];
            Problem* pool[100];
            int poolCount = 0;
            bank.filterByDifficulty(diff, pool, poolCount, 100);

            if (poolCount == 0) continue;

            // pick unique by id
            int chosenId = -1;
            for (int tries = 0; tries < 50; tries++) {
                int idx = randIndex(poolCount);
                int candidate = pool[idx]->getId();

                bool already = false;
                for (int k = 0; k < problemCountVal; k++) {
                    if (problemIds[k] == candidate) { already = true; break; }
                }

                if (!already) { chosenId = candidate; break; }
            }

            if (chosenId == -1) {
                chosenId = pool[0]->getId();
            }

            if (problemCountVal < 100) {
                problemIds[problemCountVal] = chosenId;
                attempts[problemCountVal] = 0;
                solved[problemCountVal] = 0;
                drafts[problemCountVal] = CustomString("");
                problemCountVal++;
            }
        }
    }

public:
    Contest(const char* typeName, const char* name, int durationSec)
        : contestType(typeName), contestName(name), startEpoch(0), durationSeconds(durationSec),
          problemCountVal(0), totalSolved(0), solvedHistoryCount(0) {}

    virtual ~Contest() {}

    const CustomString& getType() const { return contestType; }
    const CustomString& getName() const { return contestName; }

    void start(ProblemBank& bank) {
        totalSolved = 0;
        solvedHistoryCount = 0;
        int diffs[100];
        int diffCount = 0;
        buildDifficultyProfile(diffs,diffCount);
        selectProblems(bank,diffs,diffCount);
        startEpoch=nowEpoch();
    }



    int problemCount() const { return problemCountVal; }
    int problemIdAt(int index) const { return problemIds[index]; }

    int getAttemptsAt(int index) const { return attempts[index]; }
    bool isSolvedAt(int index) const { return solved[index] != 0; }

    bool isProblemSolved(int pid) const {
        for (int i = 0; i < solvedHistoryCount; i++) {
            if (solvedHistory[i] == pid) return true;
        }
        return false;
    }

    int remainingSeconds() const {
        if (startEpoch == 0) return durationSeconds;
        long long elapsed = nowEpoch() - startEpoch;
        int rem = durationSeconds - (int)elapsed;
        return (rem < 0) ? 0 : rem;
    }

    bool hasEnded() const {
        return remainingSeconds() <= 0;
    }

    void recordAttempt(int index, bool accepted, const char* draftCode) {
        if (index < 0 || index >= problemCountVal) return;
        if (hasEnded()) return;

        attempts[index] = attempts[index] + 1;
        int pid = problemIds[index];
        bool already = isProblemSolved(pid);

        if (accepted && !already) {
            if (solvedHistoryCount < 200) {
                solvedHistory[solvedHistoryCount++] = pid;
            }
            totalSolved++;
            solved[index] = 1;
        }
        drafts[index] = draftCode;
    }

    int getTotalSolved() const { return totalSolved; }

    void replaceProblem(int index, int newId) {
        if (index < 0 || index >= problemCountVal) return;
        problemIds[index] = newId;
        attempts[index] = 0;
        solved[index] = 0;
        drafts[index] = "";
    }

    // Persistence under: userDir/contests/active/
    bool saveProgress(VFSManager& vfs, Directory* userDir) const {
        if (!userDir) return false;

        Directory* contests = findChildDir(userDir, "contests");
        if (!contests) contests = vfs.createDirectory(userDir, "contests");
        if (!contests) return false;

        // recreate active
        contests->remove("active");
        Directory* active = vfs.createDirectory(contests, "active");
        if (!active) return false;

        // meta
        char meta[96];
        TextUtil::clearBuf(meta, (int)sizeof(meta));
        int mp = 0;
        TextUtil::appendCStr(meta, (int)sizeof(meta), mp, "type=");
        TextUtil::appendCStr(meta, (int)sizeof(meta), mp, contestType.c_str());
        TextUtil::appendCStr(meta, (int)sizeof(meta), mp, ";dur=");
        TextUtil::appendInt(meta, (int)sizeof(meta), mp, durationSeconds);
        TextUtil::appendCStr(meta, (int)sizeof(meta), mp, ";start=");
        TextUtil::appendLongLong(meta, (int)sizeof(meta), mp, startEpoch);
        TextUtil::appendChar(meta, (int)sizeof(meta), mp, ';');
        vfs.createFile(active, "meta.txt", meta);

        writeIntFile(vfs, active, "count.txt", problemCountVal);

        // problems / attempts / solved
        for (int i = 0; i < problemCountVal; i++) {
            char f1[16];
            char f2[16];
            char f3[16];
            TextUtil::clearBuf(f1, (int)sizeof(f1));
            TextUtil::clearBuf(f2, (int)sizeof(f2));
            TextUtil::clearBuf(f3, (int)sizeof(f3));
            int p1 = 0;
            int p2 = 0;
            int p3 = 0;
            TextUtil::appendChar(f1, (int)sizeof(f1), p1, 'p');
            TextUtil::appendInt(f1, (int)sizeof(f1), p1, i);
            TextUtil::appendCStr(f1, (int)sizeof(f1), p1, ".txt");
            TextUtil::appendChar(f2, (int)sizeof(f2), p2, 'a');
            TextUtil::appendInt(f2, (int)sizeof(f2), p2, i);
            TextUtil::appendCStr(f2, (int)sizeof(f2), p2, ".txt");
            TextUtil::appendChar(f3, (int)sizeof(f3), p3, 's');
            TextUtil::appendInt(f3, (int)sizeof(f3), p3, i);
            TextUtil::appendCStr(f3, (int)sizeof(f3), p3, ".txt");
            vfs.createFile(active, f1, "");
            writeIntFile(vfs, active, f1, problemIds[i]);
            vfs.createFile(active, f2, "");
            writeIntFile(vfs, active, f2, attempts[i]);
            vfs.createFile(active, f3, "");
            writeIntFile(vfs, active, f3, solved[i]);
        }
        
        writeIntFile(vfs, active, "h_count.txt", solvedHistoryCount);
        for (int i = 0; i < solvedHistoryCount; i++) {
            char hf[16];
            TextUtil::clearBuf(hf, (int)sizeof(hf));
            int hp = 0;
            TextUtil::appendChar(hf, (int)sizeof(hf), hp, 'h');
            TextUtil::appendInt(hf, (int)sizeof(hf), hp, i);
            TextUtil::appendCStr(hf, (int)sizeof(hf), hp, ".txt");
            writeIntFile(vfs, active, hf, solvedHistory[i]);
        }

        Directory* draftsDir = vfs.createDirectory(active, "drafts");
        if (draftsDir) {
            for (int i = 0; i < problemCountVal; i++) {
                char base[24];
                TextUtil::clearBuf(base, (int)sizeof(base));
                int bp = 0;
                TextUtil::appendChar(base, (int)sizeof(base), bp, 'p');
                TextUtil::appendInt(base, (int)sizeof(base), bp, problemIds[i]);
                writeTextChunked(vfs, draftsDir, base, drafts[i].c_str());
            }
        }

        return true;
    }

    bool loadProgress(VFSManager& vfs, Directory* userDir) {
        (void)vfs;
        if (!userDir) return false;

        Directory* contests = findChildDir(userDir, "contests");
        if (!contests) return false;
        Directory* active = findChildDir(contests, "active");
        if (!active) return false;

        // count
        int count = readIntFile(active, "count.txt", 0);
        if (count <= 0) return false;

        // meta start/dur are optional for demo; keep defaults if missing
        File* meta = findChildFile(active, "meta.txt");
        if (meta) {
            const char* m = meta->getData();
            if (m) {
                int dur = durationSeconds;
                int start = (int)startEpoch;
                parseIntAfterKey(m, "dur=", dur);
                parseIntAfterKey(m, "start=", start);
                durationSeconds = dur;
                startEpoch = (long long)start;

                char typeBuf[24];
                typeBuf[0] = '\0';
                if (parseStrAfterKey(m, "type=", typeBuf, (int)sizeof(typeBuf))) {
                    contestType = typeBuf;
                }
            }
        }

        problemCountVal = 0;
        totalSolved = 0;
        solvedHistoryCount = 0;

        int hCount = readIntFile(active, "h_count.txt", 0);
        for (int i = 0; i < hCount && solvedHistoryCount < 200; i++) {
            char hf[16];
            TextUtil::clearBuf(hf, (int)sizeof(hf));
            int hp = 0;
            TextUtil::appendChar(hf, (int)sizeof(hf), hp, 'h');
            TextUtil::appendInt(hf, (int)sizeof(hf), hp, i);
            TextUtil::appendCStr(hf, (int)sizeof(hf), hp, ".txt");
            int pid = readIntFile(active, hf, -1);
            if (pid != -1) solvedHistory[solvedHistoryCount++] = pid;
        }
        totalSolved = solvedHistoryCount;

        for (int i = 0; i < count; i++) {
            char f1[16];
            char f2[16];
            char f3[16];
            TextUtil::clearBuf(f1, (int)sizeof(f1));
            TextUtil::clearBuf(f2, (int)sizeof(f2));
            TextUtil::clearBuf(f3, (int)sizeof(f3));
            int p1 = 0;
            int p2 = 0;
            int p3 = 0;
            TextUtil::appendChar(f1, (int)sizeof(f1), p1, 'p');
            TextUtil::appendInt(f1, (int)sizeof(f1), p1, i);
            TextUtil::appendCStr(f1, (int)sizeof(f1), p1, ".txt");
            TextUtil::appendChar(f2, (int)sizeof(f2), p2, 'a');
            TextUtil::appendInt(f2, (int)sizeof(f2), p2, i);
            TextUtil::appendCStr(f2, (int)sizeof(f2), p2, ".txt");
            TextUtil::appendChar(f3, (int)sizeof(f3), p3, 's');
            TextUtil::appendInt(f3, (int)sizeof(f3), p3, i);
            TextUtil::appendCStr(f3, (int)sizeof(f3), p3, ".txt");

            int pid = readIntFile(active, f1, -1);
            int att = readIntFile(active, f2, 0);
            int sol = readIntFile(active, f3, 0);

            if (pid == -1) continue;
            if (problemCountVal < 100) {
                problemIds[problemCountVal] = pid;
                attempts[problemCountVal] = att;
                solved[problemCountVal] = sol;
                drafts[problemCountVal] = CustomString("");
                problemCountVal++;
            }
        }

        Directory* draftsDir = findChildDir(active, "drafts");
        if (draftsDir) {
            for (int i = 0; i < problemCountVal; i++) {
                char base[24];
                TextUtil::clearBuf(base, (int)sizeof(base));
                int bp = 0;
                TextUtil::appendChar(base, (int)sizeof(base), bp, 'p');
                TextUtil::appendInt(base, (int)sizeof(base), bp, problemIds[i]);
                drafts[i] = readTextChunked(draftsDir, base);
            }
        }

        return true;
    }

private:
    static int readIntFrom(const char* s) {
        if (!s) return 0;
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
};

}

#endif
