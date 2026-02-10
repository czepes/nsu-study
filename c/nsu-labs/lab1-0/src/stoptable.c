#include "stoptable.h"

int get_stop_value(StopTable *st, unsigned char ch) {
    return st->table[ch];
}

void set_stop_value(StopTable *st, unsigned char ch, int value) {
    st->table[ch] = value;
}

StopTable new_stoptable(char *pattern, int length) {
    StopTable stoptable = {
        .table = {length}
    };

    for (int i = 0; i <= UCHAR_MAX; i++) {
        set_stop_value(&stoptable, i, length);
    }
    
    for (int i = 0; i < length - 1; i++) {
        set_stop_value(&stoptable, pattern[i], length - i - 1);
    }

    return stoptable;
}
