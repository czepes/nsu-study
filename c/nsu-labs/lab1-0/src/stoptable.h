#ifndef STOPTABLE_HEADER
#define STOPTABLE_HEADER

#include <limits.h>

typedef struct {
    char table[UCHAR_MAX + 1];
} StopTable;

int get_stop_value(StopTable *st, unsigned char ch);
void set_stop_value(StopTable *st, unsigned char ch, int value);
StopTable new_stoptable(char *pattern, int length);

#endif
