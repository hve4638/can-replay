#include <iostream>
#include <iomanip>
#include <thread>
#include <unistd.h>
#include "color.h"
#include "canreplay.h"
extern "C" {
    #include "canattack.h"
    #include "cargs.h"
}
#ifndef VERSION
#define VERSION "dev-24.03.09a"
#endif

#define CAN_EXFLAG 0x80000000U

using namespace std;

SafeQueue<CanInfo> safequeue;
bool istest = false;
bool ismock = false;
bool ignoretr = false;
bool isloop = false;
bool isforceexid = false;
bool isverbose = false;
bool showversion = false;
const char* if_name = 0;
const char* position_string = 0;
const char* limit_string = 0;
cantime position = 0;
cantime limit = 0;
bool nolimit = true;
int exitcode = 0;

static void help();
static void can_send_mock(int i, CanInfo* can);

void opttest(int argc, const char** argv) { istest = true; }
void optforceexid(int argc, const char** argv) { isforceexid = true; }
void optmock(int argc, const char** argv) { ismock = true; }
void optverbose(int argc, const char** argv) { isverbose = true; }
void optloop(int argc, const char** argv) { isloop = true; }
void optignoretr(int argc, const char** argv) { ignoretr = true; }
void optif(int argc, const char** argv) { if_name = argv[0]; }
void optposition(int argc, const char** argv) { position_string = argv[0]; }
void optlimit(int argc, const char** argv) { limit_string = argv[0]; nolimit = false; }
void optversion(int argc, const char** argv) { showversion = true; }

void process(int argc, const char** argv) {
    if (showversion) {
        cout << VERSION << endl;
        exitcode = 0;
        return;
    }
    if (!argc) {
        help();
        exitcode = 1;
        return;
    }
    if (!if_name) {
        if (!ismock) {
            help();
            exitcode = 1;
            return;
        }
        else {
            if_name = "mock_interface";
        }
    }

    if (position_string) {
        if (!parse_cantime(position_string, position)) {
            cerr << color::text::red;
            cerr << "args error '--position' : invalid format" << endl;
            cerr << color::text::reset;
            exitcode = 1;
            return;
        }
    }
    if (limit_string) {
        if (!parse_cantime(limit_string, limit)) {
            cerr << color::text::red;
            cerr << "args error '--limit' : invalid format" << endl;
            cerr << color::text::reset;
            exitcode = 1;
            return;
        }
    }
    
    if (isverbose) {
        cout << color::yellow << "Note: Verbose mode may affect performance." << color::white << endl;
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

            if (ismock) {
                while (1) can_send_mock(-1, &can);
            }
            else {
                auto s = can_open(if_name);
                while (1) can_send(s, &can);
                can_close(s);
            }
            return;
        }
        else {
            const char* target = argv[0];
            cout << color::text::green;
            cout << "target file > " << color::text::reset << target << endl;
            cout << color::text::green;
            cout << "interface   > " << color::text::reset << if_name << endl;
            usleep(500000);
            cout << "Parsing..." << endl;

            CanOption option;
            option.force_exid = isforceexid;
            if (parse(target, safequeue, option) != 0) {
                cerr << "exit" << endl;
                exitcode = 2;
                return;
            }
            else {
                CanReplayOption replayOption;
                replayOption.loop = isloop;
                replayOption.verbose = isverbose;
                replayOption.produce_type = BATCH;
                replayOption.ignore_time_relative = ignoretr;
                replayOption.position = position;
                replayOption.limit = limit;
                replayOption.nolimit = nolimit;

                if (isverbose) {
                    cout << color::text::yellow;
                    cout << "Option" << endl;
                    cout << color::text::reset;
                    cout << "- loop         \t: " << replayOption.loop << endl;
                    cout << "- verbose      \t: " << replayOption.verbose << endl;
                    cout << "- producertype \t: " << replayOption.produce_type << endl;
                    cout << "- ignoretr     \t: " << replayOption.ignore_time_relative << endl;
                    cout << "- nolimit      \t: " << replayOption.nolimit << endl;
                    cout << "- position     \t: " << replayOption.position << endl;
                    cout << "- limit        \t: " << replayOption.limit << endl;
                    cout << endl;
                }

                cout << "Attacking..." << endl;
                cout << endl;

                if (ismock) {
                    cout << "Mock mode detected" << endl;
                    if (!replayOption.verbose) {
                        replayOption.verbose = true;
                        cout << color::yellow;
                        cout << "verbose mode automatically enabled" << endl;
                        cout << color::white;
                        replayOption.verbose = true;
                    }
                    replayOption.onsend = can_send_mock;
                    
                    cout << "Wait a second..." << endl;
                    usleep(500000);
                    replay(-1, safequeue, replayOption);
                }
                else {
                    replayOption.onsend = can_send;

                    auto s = can_open(if_name);
                    replay(s, safequeue, replayOption);
                    can_close(s);
                }
            }
        }
    }
    else if (argc > 1) {
        cerr << color::red << "only one file allowed" << color::white << endl;

        exitcode = 2;
        return;
    }
}

int main(int argc, const char** argv) {
    struct cargs* args = cargs_init();

    cargs_option(args, "i:interface", 1, optif, 0);
    cargs_option(args, "t:test", 0, opttest, 0);
    cargs_option(args, "m:mock", 0, optmock, 0);
    cargs_option(args, "v:verbose", 0, optverbose, 0);
    cargs_option(args, "force-exid", 0, optforceexid, 0);
    cargs_option(args, "l:loop", 0, optloop, 0);
    //cargs_option(args, "loop-interval", 1, optloopinterval, 0);
    cargs_option(args, "ignore-timerelative", 0, optignoretr, 0);
    cargs_option(args, "position", 1, optposition, 0);
    cargs_option(args, "limit", 1, optlimit, 0);
    cargs_option(args, "version", 0, optversion, 0);
    cargs_args(args, process);
    cargs_run(args, argc, argv);
    cargs_close(args);
    return exitcode;
}

void help() {
    cerr << "Using: canreplay -i <interface> <filename>" << endl;
    cerr << "\t-i --interface   \t" << endl;
    cerr << "\t-l --loop        \t" << endl;
    cerr << "\t-t --test        \t" << endl;
    cerr << "\t-v --verbose     \t" << endl;
    cerr << "\t-m --mock        \t" << endl;
    cerr << "\t--force-exid     \t" << endl;
    cerr << "\t--position       \t" << endl;
    cerr << "\t--limit          \t" << endl;
    cerr << "\t--loop-interval (WIP) \t" << endl;
    cerr << "\t--ignore-timerelative (WIP)\t" << endl;
    cerr << "\t--version        \t" << endl;
}

void can_send_mock(int i, CanInfo* can) {
    // nothing to do
}