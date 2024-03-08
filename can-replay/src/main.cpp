#include <iostream>
#include <thread>
#include <iomanip>
#include "canreplay.h"
#include "counter.h"
extern "C" {
    #include "canattack.h"
    #include "cargs.h"
}
#define CAN_EXFLAG 0x80000000U

using namespace std;

SafeQueue<CanInfo> safequeue;
bool istest = false;
bool ismock = false;
bool ignoretr = false;
bool isloop = false;
const char* if_name = 0;
int exitcode = 0;

void can_send_mock(int i, CanInfo* can) {
    int len = can->len;
    cout << "[mocksend] " << hex << can->id << "\t" << len << "\t";
    for (int i = 0; i < len; i++) {
        cout << hex << setw(2) << setfill('0');
        cout << (int)can->data[i] << " ";
    }
    for (int i = len; i < 8; i++) {
        cout << "   ";
    }
    cout << "(" << dec << can->time << ")";
    cout << endl;
}

void opttest(int argc, const char** argv) { istest = true; }
void optmock(int argc, const char** argv) { ismock = true; }
void optloop(int argc, const char** argv) { isloop = true; }
void optignoretr(int argc, const char** argv) { ignoretr = true; }
void optif(int argc, const char** argv) { if_name = argv[0]; }

void process(int argc, const char** argv) {
    if (argc == 0 || if_name == 0) {
        cerr << "Using: canreplay -i <interface> <filename>" << endl;
        cerr << "\t-i --interface\t" << endl;
        cerr << "\t-t --test\t" << endl;

        exitcode = 1;
        return;
    }
    
    if (argc == 1) {
        if (istest) {
            CanInfo can;
            can.id = 1024 | CAN_EXFLAG;
            can.len = 4;
            can.data[0] = 0x00;
            can.data[1] = 0x01;
            can.data[2] = 0x02;
            can.data[3] = 0x03;
            can.data[4] = 0x04;
            can.data[5] = 0x05;
            can.data[6] = 0x06;
            can.data[7] = 0x07;

            auto s = can_open(if_name);
            while (1) {
                if (ismock) can_send_mock(s, &can);
                else can_send(s, &can);
            }
            can_close(s);
            return;
        }
        else {
            const char* target = argv[0];
            cout << "target file: " << target << endl;
            cout << "target interface: " << if_name << endl;
            cout << endl;
            cout << "Parsing..." << endl;
            if (parse(target, safequeue) != 0) {
                cerr << "exit" << endl;
                exitcode = 2;
                return;
            }
            else {
                cout << "Attacking..." <<  endl;
                auto s = can_open(if_name);
                if (ismock) {
                    cout << "<<Mock mode>>" << endl;
                    replay(s, safequeue, can_send_mock);
                }
                else {
                    replay(s, safequeue, can_send);
                }
                can_close(s);
            }
        }
    }
    else if (argc > 1) {
        cerr << "only one file allowed" << endl;

        exitcode = 2;
        return;
    }
}

int main(int argc, const char** argv) {
    struct cargs* args = cargs_init();

    cargs_option(args, "i:interface", 1, optif, 0);
    cargs_option(args, "t:test", 0, opttest, 0);
    cargs_option(args, "m:mock", 0, optmock, 0);
    cargs_option(args, "l:loop", 0, optloop, 0);
    cargs_option(args, "ignore-timerelative", 0, optignoretr, 0);
    cargs_args(args, process);
    cargs_run(args, argc, argv);
    cargs_close(args);
    return exitcode;
}
