#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <linux/can.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include "canattack.h"

int sockets = 0;

void can_test(int id) {
    printf("can_test: %d\n", id);
}

int can_open(const char* if_name) {
    int s;
    struct sockaddr_can addr;
    struct ifreq ifr;

    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);

    strcpy(ifr.ifr_name, if_name);
    ioctl(s, SIOCGIFINDEX, &ifr);

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    bind(s, (struct sockaddr*)&addr, sizeof(addr));
    
    return s;
}

void can_send(int socketid, CanInfo* caninfo) {
    struct can_frame frame;
    frame.can_id = caninfo->id;
    frame.can_dlc = caninfo->len;
    
    memcpy(frame.data, caninfo->data, sizeof(frame.data));
    write(socketid, &frame, sizeof(struct can_frame));
}

void can_close(int socketid) {
    close(socketid);
}