#include "Helper.h"
#include "Interface.h"

void calculateFrameTime(GameState *state) {
    static double start = getTime();
    double end = getTime();
    state->frameTime = (end - start) * 1000.0;
    start = end;
}

void changeKeyState(Key *key, bool pressed) {
    key->previous = key->current;
    key->current = pressed;
}

/**
 * http://www.zedwood.com/article/cpp-utf8-char-to-codepoint
 */
std::string codepointToString(int cp) {
    char c[5] = {0x00, 0x00, 0x00, 0x00, 0x00};
    if (cp <= 0x7F) {
        c[0] = cp;
    } else if (cp <= 0x7FF) {
        c[0] = (cp >> 6) + 192;
        c[1] = (cp & 63) + 128;
    } else if (0xd800 <= cp && cp <= 0xdfff) {
    } // invalid block of utf8
    else if (cp <= 0xFFFF) {
        c[0] = (cp >> 12) + 224;
        c[1] = ((cp >> 6) & 63) + 128;
        c[2] = (cp & 63) + 128;
    } else if (cp <= 0x10FFFF) {
        c[0] = (cp >> 18) + 240;
        c[1] = ((cp >> 12) & 63) + 128;
        c[2] = ((cp >> 6) & 63) + 128;
        c[3] = (cp & 63) + 128;
    }
    return std::string(c);
}
