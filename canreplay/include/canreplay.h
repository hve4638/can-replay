#include "safequeue.h"
#include "caninfo.h"

typedef void (*FuncSend)(int, CanInfo*);
typedef unsigned long long cantime;

enum ProduceType {
    BATCH,
    REALTIME
};

typedef struct {
    unsigned char force_exid;
} CanOption;

typedef struct {
    bool loop;
    bool verbose;
    bool ignore_time_relative;
    ProduceType produce_type;
    FuncSend onsend;
    cantime position;
    cantime limit;
    bool nolimit;
} CanReplayOption;

int parse(const char *filename, SafeQueue<CanInfo> &queue, CanOption option);
void replay(int socketid, SafeQueue<CanInfo> &canqueue, CanReplayOption canreplayoption);
bool parse_cantime(const char*, cantime&);