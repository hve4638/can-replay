#include <iostream>
#include "color.h"
#include "canreplay.h"

using namespace std;

bool parse_cantime(const char* timestring, cantime &time) {
    cantime dec = 0;
    cantime frac = 0;
    int frac_count = 0;
    bool addtofrac = false;

    char ch;
    while ((ch = *timestring) != 0) {
        if (ch == '.') {
            if (addtofrac) {
                return false;
            }
            else {
                addtofrac = true;
            }
        }
        else if (ch >= '0' && ch <= '9') {
            if (addtofrac) {
                frac *= 10;
                frac += ch - '0';
                frac_count++;
            }
            else {
                dec *= 10;
                dec += ch - '0';
            }
        }
        else {
            return false;
        }
        timestring++;
    }

    for (int i = frac_count; i < 9; i++) {
        frac *= 10;
    }
    dec *= 1000000000;
    time = dec + frac;

    return true;
} //8.181699470