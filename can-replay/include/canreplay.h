#include "safequeue.h"
#include "caninfo.h"

typedef void (*FuncSend)(int, CanInfo*);
int parse(const char *filename, SafeQueue<CanInfo> &queue);
void replay(int socketid, SafeQueue<CanInfo> &canqueue, FuncSend send = NULL);