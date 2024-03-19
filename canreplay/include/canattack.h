#pragma once
#include "caninfo.h"

int can_open(const char*);
void can_close(int);
void can_send(int, CanInfo*);