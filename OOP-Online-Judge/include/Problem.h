#ifndef OJ_PROBLEM_H
#define OJ_PROBLEM_H

#include "CustomString.h"
#include "TestCase.h"
#include "VFSManager.h"
#include "TextUtil.h"

namespace OJ {

class Problem {
private:
    int kind; // 0=code (compile/run), 1=qa (short answer)
    int id;
    int difficulty; // 1-5
    CustomString title;
    CustomString statement;
    CustomString constraints;
    CustomString examples;
    TestCase testCases[50];
    int tcCount;

    static int toInt(const char* s) {
        if (!s) return 0;
        int i = 0;
        int sign = 1;
        if (s[i] == '-') { sign = -1; i++; }
        int val = 0;
        while (s[i] >= '0' && s[i] <= '9') {
            val = val * 10 + (s[i] - '0');
            i++;
        }
        return val * sign;
    }

    static bool startsWith(const char* s, const char* prefix) {
        if (!s || !prefix) return false;
        int i = 0;
        while (prefix[i] != '\0') {
            if (s[i] != prefix[i]) return false;
            i++;
        }
        return true;
    }

    static bool parseKeyInt(const char* meta, const char* key, int& outVal) {
        if (!meta || !key) return false;
        const char* p = meta;
        while (*p != '\0') {
            if (startsWith(p, key)) {
                int k = 0;
                while (key[k] != '\0') k++;
                p += k;
                outVal = toInt(p);
                return true;
            }
            p++;
        }
        return false;
    }

    static bool parseKeyStr(const char* meta, const char* key, char* outBuf, int outCap) {
        if (!meta || !key || !outBuf || outCap <= 0) return false;
        const char* p = meta;
        while (*p != '\0') {
            if (startsWith(p, key)) {
                int k = 0;
                while (key[k] != '\0') k++;
                p += k;
                int i = 0;
                while (*p != '\0' && *p != ';' && i < outCap - 1) {
                    outBuf[i++] = *p++;
                }
                outBuf[i] = '\0';
                return true;
            }
            p++;
        }
        return false;
    }

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

    static File* findChildFile(Directory* parent, const char* name) {
        if (!parent) return NULL;
        for (int i = 0; i < parent->childCount(); i++) {
            File* f = (File*)(parent->childAt(i));
            if (f && sameName(f->getName(), name)) return f;
        }
        return NULL;
    }

    static Directory* findChildDir(Directory* parent, const char* name) {
        if (!parent) return NULL;
        for (int i = 0; i < parent->childCount(); i++) {
            Directory* d = (Directory*)(parent->childAt(i));
            if (d && sameName(d->getName(), name)) return d;
        }
        return NULL;
    }

    static void writeTextChunked(VFSManager& vfs, Directory* dir, const char* baseName, const char* text) {
        if (!dir || !baseName) return;
        if (!text) text = "";

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

public:
    enum Kind {
        Code = 0,
        ShortAnswer = 1
    };

    Problem()
        : kind(Code), id(0), difficulty(1), title(""), statement(""), constraints(""), examples(""), tcCount(0) {}

    Problem(int pid, int diff, const char* t, const char* st, const char* con, const char* ex)
        : kind(Code), id(pid), difficulty(diff), title(t), statement(st), constraints(con), examples(ex), tcCount(0) {}

    Problem(int pid, int diff, Kind k, const char* t, const char* st, const char* con, const char* ex)
        : kind((int)k), id(pid), difficulty(diff), title(t), statement(st), constraints(con), examples(ex), tcCount(0) {}

    int getId() const { return id; }
    int getDifficulty() const { return difficulty; }
    Kind getKind() const { return (Kind)kind; }
    bool isShortAnswer() const { return kind == (int)ShortAnswer; }
    const CustomString& getTitle() const { return title; }
    const CustomString& getStatement() const { return statement; }

    int testCaseCount() const { return tcCount; }
    const TestCase& testCaseAt(int i) const { return testCases[i]; }

    void addTestCase(const TestCase& tc) {
        if (tcCount < 50) {
            testCases[tcCount++] = tc;
        }
    }

    bool saveToVFS(VFSManager& vfs, Directory* problemsRoot) const {
        if (problemsRoot == NULL) return false;

        char dName[8];
        TextUtil::clearBuf(dName, (int)sizeof(dName));
        int dp = 0;
        TextUtil::appendChar(dName, (int)sizeof(dName), dp, 'd');
        TextUtil::appendInt(dName, (int)sizeof(dName), dp, difficulty);

        Directory* dDir = NULL;
        for (int i = 0; i < problemsRoot->childCount(); i++) {
            Directory* cand = (Directory*)(problemsRoot->childAt(i));
            if (cand != NULL && cand->getName() != NULL) {
                const char* cn = cand->getName();
                int j = 0;
                while (cn[j] != '\0' && dName[j] != '\0' && cn[j] == dName[j]) j++;
                if (cn[j] == '\0' && dName[j] == '\0') { dDir = cand; break; }
            }
        }
        if (dDir == NULL) dDir = vfs.createDirectory(problemsRoot, dName);
        if (dDir == NULL) return false;

        char pName[16];
        TextUtil::clearBuf(pName, (int)sizeof(pName));
        int pp = 0;
        TextUtil::appendChar(pName, (int)sizeof(pName), pp, 'p');
        TextUtil::appendInt(pName, (int)sizeof(pName), pp, id);
        Directory* pDir = vfs.createDirectory(dDir, pName);
        if (pDir == NULL) return false;

        char meta[96];
        TextUtil::clearBuf(meta, (int)sizeof(meta));
        int mp = 0;
        TextUtil::appendCStr(meta, (int)sizeof(meta), mp, "id=");
        TextUtil::appendInt(meta, (int)sizeof(meta), mp, id);
        TextUtil::appendCStr(meta, (int)sizeof(meta), mp, ";diff=");
        TextUtil::appendInt(meta, (int)sizeof(meta), mp, difficulty);
        TextUtil::appendCStr(meta, (int)sizeof(meta), mp, ";title=");
        TextUtil::appendCStr(meta, (int)sizeof(meta), mp, title.c_str());
        TextUtil::appendCStr(meta, (int)sizeof(meta), mp, ";kind=");
        TextUtil::appendCStr(meta, (int)sizeof(meta), mp, (kind == (int)ShortAnswer) ? "qa" : "code");
        TextUtil::appendChar(meta, (int)sizeof(meta), mp, ';');
        vfs.createFile(pDir, "meta.txt", meta);
        writeTextChunked(vfs, pDir, "statement", statement.c_str());
        vfs.createFile(pDir, "constraints.txt", constraints.c_str());
        vfs.createFile(pDir, "examples.txt", examples.c_str());

        Directory* tcs = vfs.createDirectory(pDir, "tcs");
        if (tcs == NULL) return true;

        for (int i = 0; i < tcCount; i++) {
            char tcDirName[16];
            TextUtil::clearBuf(tcDirName, (int)sizeof(tcDirName));
            int tcp = 0;
            TextUtil::appendCStr(tcDirName, (int)sizeof(tcDirName), tcp, "tc");
            TextUtil::appendInt(tcDirName, (int)sizeof(tcDirName), tcp, i + 1);
            Directory* tcDir = vfs.createDirectory(tcs, tcDirName);
            if (tcDir == NULL) continue;
            vfs.createFile(tcDir, "in.txt", testCases[i].getInput().c_str());
            vfs.createFile(tcDir, "out.txt", testCases[i].getExpectedOutput().c_str());
        }

        return true;
    }

    static bool loadFromVFS(Directory* pDir, Problem& outProblem) {
        if (pDir == NULL) return false;

        File* meta = NULL;
        File* con = NULL;
        File* ex = NULL;
        Directory* tcs = NULL;

        for (int i = 0; i < pDir->childCount(); i++) {
            FSE* child = pDir->childAt(i);
            if (!child) continue;
            const char* n = child->getName();
            if (!n) continue;

            File* asFile = (File*)(child);
            Directory* asDir = (Directory*)(child);

            if (startsWith(n, "meta.txt") && n[8] == '\0') meta = asFile;
            else if (startsWith(n, "constraints.txt") && n[15] == '\0') con = asFile;
            else if (startsWith(n, "examples.txt") && n[12] == '\0') ex = asFile;
            else if (startsWith(n, "tcs") && n[3] == '\0') tcs = asDir;
        }

        if (meta == NULL) return false;

        int pid = 0;
        int diff = 1;
        parseKeyInt(meta->getData(), "id=", pid);
        parseKeyInt(meta->getData(), "diff=", diff);

        char titleBuf[64];
        titleBuf[0] = '\0';
        parseKeyStr(meta->getData(), "title=", titleBuf, (int)sizeof(titleBuf));

        char kindBuf[16];
        kindBuf[0] = '\0';
        parseKeyStr(meta->getData(), "kind=", kindBuf, (int)sizeof(kindBuf));

        Kind k = Code;
        if (kindBuf[0] == 'q' && kindBuf[1] == 'a' && kindBuf[2] == '\0') {
            k = ShortAnswer;
        }

        CustomString statementText = readTextChunked(pDir, "statement");
        if (statementText.empty()) {
            File* oldStatement = findChildFile(pDir, "statement.txt");
            if (oldStatement) statementText = oldStatement->getData();
        }

        outProblem = Problem(pid, diff, k,
            titleBuf,
            statementText.c_str(),
            con ? con->getData() : "",
            ex ? ex->getData() : "");

        if (tcs != NULL) {
            for (int i = 0; i < tcs->childCount(); i++) {
                Directory* tcDir = (Directory*)(tcs->childAt(i));
                if (tcDir == NULL) continue;

                File* inF = NULL;
                File* outF = NULL;
                for (int j = 0; j < tcDir->childCount(); j++) {
                    File* f = (File*)(tcDir->childAt(j));
                    if (!f) continue;
                    const char* fn = f->getName();
                    if (!fn) continue;
                    if (startsWith(fn, "in.txt") && fn[6] == '\0') inF = f;
                    else if (startsWith(fn, "out.txt") && fn[7] == '\0') outF = f;
                }

                outProblem.addTestCase(TestCase(inF ? inF->getData() : "", outF ? outF->getData() : ""));
            }
        }

        return true;
    }
};

}

#endif
