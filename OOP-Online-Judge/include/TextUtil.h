#ifndef OJ_TEXTUTIL_H
#define OJ_TEXTUTIL_H

// Lightweight text utilities (no <cstdio>, no std::string)

namespace OJ {
namespace TextUtil {

inline int strLen(const char* s) {
    if (!s) return 0;
    int i = 0;
    while (s[i] != '\0') i++;
    return i;
}

inline void clearBuf(char* out, int cap) {
    if (!out || cap <= 0) return;
    out[0] = '\0';
}

inline bool appendChar(char* out, int cap, int& pos, char c) {
    if (!out || cap <= 0) return false;
    if (pos < 0) pos = 0;
    if (pos >= cap - 1) {
        out[cap - 1] = '\0';
        return false;
    }
    out[pos++] = c;
    out[pos] = '\0';
    return true;
}

inline bool appendCStr(char* out, int cap, int& pos, const char* s) {
    if (!out || cap <= 0) return false;
    if (!s) s = "";
    for (int i = 0; s[i] != '\0'; i++) {
        if (!appendChar(out, cap, pos, s[i])) return false;
    }
    return true;
}

inline bool appendInt(char* out, int cap, int& pos, int value) {
    if (!out || cap <= 0) return false;

    // handle sign
    if (value == 0) {
        return appendChar(out, cap, pos, '0');
    }

    long long v = value;
    if (v < 0) {
        if (!appendChar(out, cap, pos, '-')) return false;
        v = -v;
    }

    // write digits into temp reversed
    char tmp[24];
    int n = 0;
    while (v > 0 && n < (int)sizeof(tmp)) {
        tmp[n++] = (char)('0' + (v % 10));
        v /= 10;
    }

    for (int i = n - 1; i >= 0; i--) {
        if (!appendChar(out, cap, pos, tmp[i])) return false;
    }

    return true;
}

inline bool appendLongLong(char* out, int cap, int& pos, long long value) {
    if (!out || cap <= 0) return false;
    if (value == 0) return appendChar(out, cap, pos, '0');

    long long v = value;
    if (v < 0) {
        if (!appendChar(out, cap, pos, '-')) return false;
        v = -v;
    }

    char tmp[32];
    int n = 0;
    while (v > 0 && n < (int)sizeof(tmp)) {
        tmp[n++] = (char)('0' + (v % 10));
        v /= 10;
    }
    for (int i = n - 1; i >= 0; i--) {
        if (!appendChar(out, cap, pos, tmp[i])) return false;
    }
    return true;
}

inline bool appendUInt(char* out, int cap, int& pos, unsigned int value) {
    if (!out || cap <= 0) return false;

    if (value == 0u) {
        return appendChar(out, cap, pos, '0');
    }

    char tmp[24];
    int n = 0;
    while (value > 0u && n < (int)sizeof(tmp)) {
        tmp[n++] = (char)('0' + (value % 10u));
        value /= 10u;
    }

    for (int i = n - 1; i >= 0; i--) {
        if (!appendChar(out, cap, pos, tmp[i])) return false;
    }

    return true;
}

inline bool endsWith(const char* s, const char* suffix) {
    if (!s || !suffix) return false;
    int n = strLen(s);
    int m = strLen(suffix);
    if (m == 0) return true;
    if (n < m) return false;
    int start = n - m;
    for (int i = 0; i < m; i++) {
        if (s[start + i] != suffix[i]) return false;
    }
    return true;
}

} // namespace TextUtil
} // namespace OJ

#endif
