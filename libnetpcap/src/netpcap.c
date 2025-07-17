#include "netpcap.h"

/*
 * It uses the getifaddrs() system call to get information about network interfaces. This syscall:
 * - Returns a linked list of ifaddrs structures
 * - Contains information about all network interfaces
 * - Includes IP addresses, netmasks, and interface flags
 *
 * It also reads the /proc/net/dev file to get additional interface statistics and information.
 *
 * For more detailed information, it may:
 * - Use ioctl() with SIOCGIFCONF to get interface configurations
 * - Use ioctl() with SIOCGIFFLAGS to get interface flags
 * - Read /sys/class/net/ directory for interface details
*/
int netpcap_findalldevs(netpcap_if **alldevs, char *errbuf) {
    struct ifaddrs *h_ifa;

    if (getifaddrs(&h_ifa) == -1) {
        return -1;
    }

    
    

    return 0;
}

void netpcap_freealldevs(netpcap_if *alldevs) {
    
}


int main(){
    netpcap_if **alldevs;
    char *errbuff;

    netpcap_findalldevs(alldevs, errbuff);
}