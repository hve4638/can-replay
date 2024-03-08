#include <iostream>
#include "canreplay.h"
#include "counter.h"
extern "C" {
    #include "canattack.h"
}

using namespace std;

enum ReplayState {
    COUNT,
    REPLAY,
};


void replay(int socketid, SafeQueue<CanInfo> &canqueue, FuncSend sender) {
    if (sender == NULL) {
        sender = can_send;
    }
    ReplayState state = REPLAY;
    unsigned long long currenttime = 0;
    unsigned long long nexttime = 0;

    auto counter = counter_start();
    while(true) {
        if (state == REPLAY) {
            while (true) {
                CanInfo can;
                canqueue.top(can);
                if (can.time > currenttime) {
                    state = COUNT;
                    nexttime = can.time;
                    break;
                }
                else {
                    canqueue.pop(can);
                    sender(socketid, &can);
                }
            }
        }
        else if (state == COUNT) {
            while (currenttime < nexttime) {
                currenttime = counter_end(counter);
            }
            state = REPLAY;
        }
    }
}