#include <iostream>
#include <iomanip>
#include <unistd.h>
#include "canreplay.h"
#include "counter.h"
#include "color.h"
extern "C" {
    #include "canattack.h"
}
#define CANLEN_SPECIAL 2146483648
#define CANID_NOUSE 0
#define CANID_ENDOFSTREAM 1

using namespace std;

enum ReplayState {
    COUNT,
    SEND,
    EXIT,
};

static void show_info(CanInfo*);

void replay(int socketid, SafeQueue<CanInfo> &canqueue, CanReplayOption option) {
    cantime currenttime = 0;
    cantime nexttime = 0;
    FuncSend onsend = option.onsend;
    bool verbose = option.verbose;
    bool loop = option.loop;
    cantime position = option.position;
    cantime limit = option.limit;
    bool nolimit = option.nolimit;

    if (option.produce_type == REALTIME) {
        if (loop) {
            loop = false;
            cerr << color::yellow;
            cerr << "Realtime producer could not support loop mode" << endl;
            cerr << "Loop mode automatically disabled" << endl;
            cerr << color::white;
        }
        cerr << color::yellow;
        cerr << "--position and --limit option are ignored" << endl;
        cerr << color::white;
        position = 0;
        limit = 0;
        nolimit = true;
    }
    if (onsend == NULL) {
        cerr << color::red;
        cerr << "Invalid option : onsend is null" << endl;
        cerr << color::white;
        return;
    }
    if (position > 0) {
        unsigned long long count = 0;
        while (true) {
            CanInfo can;
            canqueue.top(can);
            if (can.len != CANLEN_SPECIAL && can.time < position) {
                canqueue.pop();
                count++;
            }
            else {
                if (verbose) {
                    cout << color::text::yellow;
                    cout << "skip " << count << " packets" << endl;
                    cout << color::text::reset;
                }
                break;
            }
        }
    }
    
    if (option.produce_type == BATCH) {
        CanInfo can;
        can.len = CANLEN_SPECIAL;
        can.id = CANID_ENDOFSTREAM;
        canqueue.push(can);
    }
    
    ReplayState state = SEND;
    auto counter = counter_start();
    while(state != EXIT) {
        if (state == SEND) {
            while (true) {
                CanInfo can;
                canqueue.top(can);

                if (can.len == CANLEN_SPECIAL) {
                    canqueue.pop();
                    if (can.id == CANID_ENDOFSTREAM) {
                        if (loop) {
                            // usleep(5000);
                            canqueue.push(can);
                            counter = counter_start();
                            currenttime = 0;
                            nexttime = 0;
                            cout << "endofloop" << endl;
                        }
                        else {
                            state = EXIT;
                            break;
                        }
                    }
                    else {
                        cerr << color::text::red;
                        cerr << "error : unknown special info (" << can.id << ")" << endl;
                        cerr << color::text::reset;
                    }
                }
                else if (!nolimit && can.time > limit) {
                    canqueue.pop();
                }
                else if (can.time > currenttime) {
                    state = COUNT;
                    nexttime = can.time;
                    break;
                }
                else {
                    canqueue.pop();
                    if (loop) canqueue.push(can);

                    if (verbose) show_info(&can);
                    onsend(socketid, &can);
                }
            }
        }
        else if (state == COUNT) {
            while (currenttime < nexttime) {
                currenttime = counter_end(counter) + position;
            }
            state = SEND;
        }
    }
}

void show_info(CanInfo* can) {
    int len = can->len;
    cout << hex << can->id << "\t" << len << "\t";
    for (int i = 0; i < len; i++) {
        cout << hex << setw(2) << setfill('0');
        cout << (int)can->data[i] << " ";
    }
    for (int i = len; i < 8; i++) {
        cout << "   ";
    }
    cantime deci = can->time / 1000000000;
    cantime frac = can->time % 1000000000;
    cout << dec;
    cout << "(" << deci << ".";
    cout << setw(9) << setfill('0') << left;
    cout << frac << ")";
    cout << right;
    cout << endl;
}