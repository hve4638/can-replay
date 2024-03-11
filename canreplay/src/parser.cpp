#include <iostream>
#include <fstream>
#include "canreplay.h"
#include "color.h"
#define CAN_EXFLAG 0x80000000U

using namespace std;

static void err_no_read(long long position, long long count) {
    cerr << color::red;
    cerr << endl;
    cerr << "Error : Couldn't read bytes" << endl;
    cerr << "- position (" << position << ")" << endl;
    cerr << "- read size (" << count << ")" << endl;
    cerr << endl;
    cerr << color::white;
}

static void err_invalid_format(long long position, long long count) {
    cerr << color::red;
    cerr << endl;
    cerr << "Error : Invalid format" << endl;
    cerr << "- position (" << position << ")" << endl;
    cerr << "- read size (" << count << ")" << endl;
    cerr << endl;
    cerr << color::white;
}

int parse(const char *filename, SafeQueue<CanInfo> &queue, CanOption option) {
    ifstream file(filename, ios::binary);

    if (!file.is_open()) {
        cerr << color::red;
        cerr << endl;
        cerr << "Error : couldn't open file" << endl;
        cerr << "- target (" << filename << ")" << endl;
        cerr << endl;
        cerr << color::white;
        return 1;
    }
    else {
        CanInfo buffer;
        long long position;
        long long gcount;

        while (true) {
            position = file.tellg();
            file.read((char*)&buffer, sizeof(buffer));
            gcount = file.gcount();

            if (gcount == 0) {
                break;
            }
            else if (gcount != sizeof(buffer)) {
                err_invalid_format(position, gcount);
                return 2;
            }
            else {
                if (option.force_exid) {
                    buffer.id = buffer.id | CAN_EXFLAG;
                }
                queue.push(buffer);
            }
        }
    }
    file.close();

    return 0;
}