### *netnetpcap_findalldevs*

The `netpcap_findalldevs()` function is part of the libpcap (or WinPcap/Npcap on Windows) library, which provides facilities for low-level network packet capture. This function is used to retrieve a list of all available network interfaces on the system that can be used for packet capturing.

Here's how it works in detail:


🧩 Function Prototype
`int netpcap_findalldevs(netpcap_if **alldevs, char *errbuf);`
    - `netpcap_if **alldevs`: A pointer to a pointer where the function will store the head of a linked list of network devices (interfaces).
    - `char *errbuf`: A user-allocated buffer (at least PCAP_ERRBUF_SIZE bytes) where the function stores any error message.²


📋 What It Does
- It scans the system for available network interfaces.
- For each interface that supports packet capture, it creates a `netpcap_if` structure (defined by libpcap).
- It returns these as a linked list via the `alldevs` parameter.
- Each `netpcap_if` structure contains:
    - The name of the interface (e.g., "eth0")
    - A human-readable description (if available)
    - A list of addresses assigned to the interface (e.g., IP, netmask)
    - Flags (e.g., whether the interface is up, loopback, etc.)


✅ Return Value
- Returns 0 on success.
- Returns -1 on failure and stores the error message in errbuf.


🧽 Cleanup
After you're done using the list of devices, you must free the memory using:

`pcap_freealldevs(alldevs);`

📌 Example in C

```c
#include <pcap.h>
#include <stdio.h>

int main() {
    netpcap_if *alldevs, *dev;
    char errbuf[PCAP_ERRBUF_SIZE];

    if (netpcap_findalldevs(&alldevs, errbuf) == -1) {
        fprintf(stderr, "Error: %s\n", errbuf);
        return 1;
    }

    for (dev = alldevs; dev != NULL; dev = dev->next) {
        printf("Interface: %s\n", dev->name);
        if (dev->description)
            printf("  Description: %s\n", dev->description);
    }

    pcap_freealldevs(alldevs);
    return 0;
}
```

Let me explain how pcap_findalldevs works internally on Linux:

1. It uses the getifaddrs() system call to get information about network interfaces. This syscall:
    - Returns a linked list of ifaddrs structures
    - Contains information about all network interfaces
    - Includes IP addresses, netmasks, and interface flags
2. It also reads the /proc/net/dev file to get additional interface statistics and information.
3. For more detailed information, it may:
    - Use ioctl() with SIOCGIFCONF to get interface configurations
    - Use ioctl() with SIOCGIFFLAGS to get interface flags
    - Read /sys/class/net/ directory for interface details


Here's a simplified version of what happens internally:
```c
// Pseudo-code of what happens internally
struct ifaddrs *ifap;
if (getifaddrs(&ifap) == -1) {
    // Error handling
}

// Read /proc/net/dev
FILE *fp = fopen("/proc/net/dev", "r");
if (fp) {
    // Parse interface statistics
}

// For each interface, get more details
struct ifreq ifr;
int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
if (sockfd >= 0) {
    // Use ioctl to get more interface info
    ioctl(sockfd, SIOCGIFCONF, &ifr);
    ioctl(sockfd, SIOCGIFFLAGS, &ifr);
}
```

The actual implementation in libpcap is more complex and handles various edge cases, but these are the main system calls and file operations it uses to gather interface information.

The reason it needs these system calls and file operations is because:

1. Network interfaces are kernel-managed resources
2. The kernel maintains the current state of all network interfaces
3. These interfaces can change dynamically (e.g., when USB network adapters are plugged in/out)
4. The kernel needs to maintain security boundaries around network interface access

This is why `pcap_findalldevs` might require root privileges on some systems - it's accessing low-level network information that could potentially be used for network configuration or monitoring.

## Network stack
[ User Space ]
     |
 [ Socket API ]
     |
 [ Kernel Networking Stack ]
     |
 [ Network Interface (eth0, wlan0) ]
     |
 [ Driver ]
     |
 [ Hardware NIC ]

## Netlink

MAN: https://www.man7.org/linux/man-pages/man7/netlink.7.html
https://github.com/torvalds/linux/blob/master/net/netlink/af_netlink.c
DOC: https://docs.kernel.org/userspace-api/netlink/intro.html

Ressources:
https://www.youtube.com/watch?v=vqBC_WtDc6Q