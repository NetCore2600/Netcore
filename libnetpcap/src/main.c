#include "netpcap.h"
#include <stdio.h>

int main() {
    ifaddrs *ifap;

    print_interfaces();

    // getifaddrs(&ifap);

    // while (ifap) {
    //     printf("name: %s\n", ifap->ifa_name);
    //     ifap = ifap->ifa_next;
    // }

    // freeifaddrs(ifap);
} 