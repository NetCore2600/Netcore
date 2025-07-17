#ifndef _NETPCAP_H_
#define _NETPCAP_H_

#include <sys/socket.h>
#include <stdint.h>
#include <sys/time.h>

/***********************************|
|              STRUCTS              |
|__________________________________*/

// Packet capture address
typedef struct _netpcap_addr {
    struct _netpcap_addr *next;     // Pointer to the next address in the list
    struct sockaddr *addr;          // IP address assigned to the interface
    struct sockaddr *netmask;       // Subnet mask for this address
    struct sockaddr *broadaddr;     // Broadcast address (if applicable)
} netpcap_addr;

// Packet capture interface
typedef struct _netpcap_if {
    char *name;                     // Interface name (e.g., "eth0")
    char *description;              // Human-readable description (optional)
    netpcap_addr *addresses;        // Linked list of addresses assigned to the interface
    int flags;                      // Interface flags (e.g., IFF_UP, IFF_LOOPBACK)
    struct _netpcap_if *next;       // Pointer to the next interface in the list
} netpcap_if;

// Interface address (like getifaddrs)
typedef struct _ifaddrs {
    struct _ifaddrs  *ifa_next;     // Pointer to the next item in the list
    char            *ifa_name;      // Interface name (e.g., "eth0")
    unsigned int     ifa_flags;     // Flags (e.g., IFF_UP, IFF_BROADCAST)
    struct sockaddr *ifa_addr;      // IP address of the interface
    struct sockaddr *ifa_netmask;   // Subnet mask of the interface
    union {
        struct sockaddr *ifu_broadaddr; // Broadcast address (if applicable)
        struct sockaddr *ifu_dstaddr;   // Point-to-point destination address (if applicable)
    };
} ifaddrs;

// Forward declarations
struct netpcap_handle;              // Opaque handle for capturing (similar to pcap_t)
typedef struct netpcap_handle netpcap_t;

// BPF-related types (reimplement if needed)
typedef struct bpf_program {
    unsigned int bf_len;            // Number of instructions in the BPF program
    struct bpf_insn *bf_insns;      // Pointer to BPF instruction array
} bpf_program;

typedef struct bpf_insn {
    uint16_t code;                  // Opcode for this instruction
    uint8_t jt;                     // Jump offset if condition is true
    uint8_t jf;                     // Jump offset if condition is false
    uint32_t k;                     // Generic operand (e.g., constant, offset)
} bpf_insn;

typedef void (*pcap_handler)(
    unsigned char *user,            // User-defined pointer passed to callback
    const struct pcap_pkthdr *h,    // Pointer to packet metadata (timestamp, length)
    const unsigned char *bytes      // Pointer to raw packet data
);

// Packet header (metadata for a captured packet)
struct pcap_pkthdr {
    struct timeval ts;              // Timestamp when packet was captured
    uint32_t caplen;                // Number of bytes actually captured
    uint32_t len;                   // Original length of the packet on the wire
};



/***********************************|
|            PROTOTYPES             |
|__________________________________*/

// /src/netpcap.c
int netpcap_findalldevs(netpcap_if **alldevs, char *errbuf);                       // List all network interfaces
void netpcap_freealldevs(netpcap_if *alldevs);                                     // Free memory from netpcap_findalldevs

netpcap_t *netpcap_open_live(const char *device, int snaplen, int promisc,
                             int to_ms, char *errbuf);                             // Open a live capture on a device

int netpcap_datalink(netpcap_t *p);                                                // Get data link type (e.g., Ethernet)
void netpcap_close(netpcap_t *p);                                                  // Close the capture handle

int netpcap_compile(netpcap_t *p, struct bpf_program *fp, const char *str,
                    int optimize, uint32_t netmask);                               // Compile a BPF filter string

int netpcap_setfilter(netpcap_t *p, struct bpf_program *fp);                       // Apply compiled filter to capture
void netpcap_freecode(struct bpf_program *fp);                                     // Free memory used by compiled BPF code

int netpcap_lookupnet(const char *device, uint32_t *net,
                      uint32_t *mask, char *errbuf);                               // Get network address and netmask for device

int netpcap_loop(netpcap_t *p, int cnt, pcap_handler callback,
                 unsigned char *user);                                             // Capture packets in a loop and call callback
                                      // Free list allocated by getifaddrs
int getifaddrs(ifaddrs **ifap);
void freeifaddrs(ifaddrs *ifa);
void print_interfaces();

#endif // _NETPCAP_H_
