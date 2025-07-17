#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "netlink.h"

#define BUFFER_SIZE 8192

// Structure to hold interface information
typedef struct ifaddr_info {
    char name[IF_NAMESIZE];
    int index;
    unsigned int flags;
    unsigned int mtu;
    unsigned char mac[6];
    int family;
    char addr[INET6_ADDRSTRLEN];
    char netmask[INET6_ADDRSTRLEN];
    char broadcast[INET6_ADDRSTRLEN];
    struct ifaddr_info *next;
} ifaddr_info_t;

// Parse netlink attributes
static void parse_rtattr(struct rtattr *tb[], int max, struct rtattr *rta, int len) {
    memset(tb, 0, sizeof(struct rtattr *) * (max + 1));
    while (RTA_OK(rta, len)) {
        if (rta->rta_type <= max) {
            tb[rta->rta_type] = rta;
        }
        rta = RTA_NEXT(rta, len);
    }
}

// Get interface information
static ifaddr_info_t* get_interface_info(int fd) {
    struct {
        struct nlmsghdr nlh;
        struct ifinfomsg ifm;
    } req;
    
    struct sockaddr_nl addr;
    struct msghdr msg;
    struct iovec iov;
    char buffer[BUFFER_SIZE];
    ifaddr_info_t *head = NULL, *current = NULL;
    
    // Prepare request for interface dump
    memset(&req, 0, sizeof(req));
    req.nlh.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifinfomsg));
    req.nlh.nlmsg_type = RTM_GETLINK;
    req.nlh.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
    req.nlh.nlmsg_seq = 1;
    req.nlh.nlmsg_pid = getpid();
    
    req.ifm.ifi_family = AF_UNSPEC;
    
    // Send request
    memset(&addr, 0, sizeof(addr));
    addr.nl_family = AF_NETLINK;
    
    if (sendto(fd, &req, req.nlh.nlmsg_len, 0, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("sendto");
        return NULL;
    }
    
    // Receive response
    while (1) {
        memset(&msg, 0, sizeof(msg));
        iov.iov_base = buffer;
        iov.iov_len = sizeof(buffer);
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;
        msg.msg_name = &addr;
        msg.msg_namelen = sizeof(addr);
        
        int len = recvmsg(fd, &msg, 0);
        if (len < 0) {
            perror("recvmsg");
            break;
        }
        
        struct nlmsghdr *nlh = (struct nlmsghdr*)buffer;
        
        while (NLMSG_OK(nlh, len)) {
            if (nlh->nlmsg_type == NLMSG_DONE) {
                return head;
            }
            
            if (nlh->nlmsg_type == NLMSG_ERROR) {
                struct nlmsgerr *err = (struct nlmsgerr*)NLMSG_DATA(nlh);
                if (err->error != 0) {
                    fprintf(stderr, "Netlink error: %s\n", strerror(-err->error));
                }
                return head;
            }
            
            if (nlh->nlmsg_type == RTM_NEWLINK) {
                struct ifinfomsg *ifm = (struct ifinfomsg*)NLMSG_DATA(nlh);
                struct rtattr *tb[IFLA_MAX + 1];
                
                parse_rtattr(tb, IFLA_MAX, IFLA_RTA(ifm), 
                           nlh->nlmsg_len - NLMSG_LENGTH(sizeof(*ifm)));
                
                // Create new interface info structure
                ifaddr_info_t *info = calloc(1, sizeof(ifaddr_info_t));
                if (!info) {
                    perror("calloc");
                    return head;
                }
                
                info->index = ifm->ifi_index;
                info->flags = ifm->ifi_flags;
                info->family = ifm->ifi_family;
                
                // Extract interface name
                if (tb[IFLA_IFNAME]) {
                    strncpy(info->name, (char*)RTA_DATA(tb[IFLA_IFNAME]), IF_NAMESIZE - 1);
                }
                
                // Extract MTU
                if (tb[IFLA_MTU]) {
                    info->mtu = *(unsigned int*)RTA_DATA(tb[IFLA_MTU]);
                }
                
                // Extract MAC address
                if (tb[IFLA_ADDRESS]) {
                    memcpy(info->mac, RTA_DATA(tb[IFLA_ADDRESS]), 6);
                }
                
                // Add to linked list
                if (!head) {
                    head = current = info;
                } else {
                    current->next = info;
                    current = info;
                }
            }
            
            nlh = NLMSG_NEXT(nlh, len);
        }
    }
}

// Get IP addresses for interfaces
static void get_ip_addresses(int fd, ifaddr_info_t *interfaces) {
    struct {
        struct nlmsghdr nlh;
        struct ifaddrmsg ifa;
    } req;
    
    struct sockaddr_nl addr;
    struct msghdr msg;
    struct iovec iov;
    char buffer[BUFFER_SIZE];
    
    // Prepare request for address dump
    memset(&req, 0, sizeof(req));
    req.nlh.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifaddrmsg));
    req.nlh.nlmsg_type = RTM_GETADDR;
    req.nlh.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
    req.nlh.nlmsg_seq = 2;
    req.nlh.nlmsg_pid = getpid();
    
    req.ifa.ifa_family = AF_UNSPEC;
    
    // Send request
    memset(&addr, 0, sizeof(addr));
    addr.nl_family = AF_NETLINK;
    
    if (sendto(fd, &req, req.nlh.nlmsg_len, 0, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("sendto");
        return;
    }
    
    // Receive response
    while (1) {
        memset(&msg, 0, sizeof(msg));
        iov.iov_base = buffer;
        iov.iov_len = sizeof(buffer);
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;
        msg.msg_name = &addr;
        msg.msg_namelen = sizeof(addr);
        
        int len = recvmsg(fd, &msg, 0);
        if (len < 0) {
            perror("recvmsg");
            break;
        }
        
        struct nlmsghdr *nlh = (struct nlmsghdr*)buffer;
        
        while (NLMSG_OK(nlh, len)) {
            if (nlh->nlmsg_type == NLMSG_DONE) {
                return;
            }
            
            if (nlh->nlmsg_type == NLMSG_ERROR) {
                struct nlmsgerr *err = (struct nlmsgerr*)NLMSG_DATA(nlh);
                if (err->error != 0) {
                    fprintf(stderr, "Netlink error: %s\n", strerror(-err->error));
                }
                return;
            }
            
            if (nlh->nlmsg_type == RTM_NEWADDR) {
                struct ifaddrmsg *ifa = (struct ifaddrmsg*)NLMSG_DATA(nlh);
                struct rtattr *tb[IFA_MAX + 1];
                
                parse_rtattr(tb, IFA_MAX, IFA_RTA(ifa), 
                           nlh->nlmsg_len - NLMSG_LENGTH(sizeof(*ifa)));
                
                // Find corresponding interface
                ifaddr_info_t *info = interfaces;
                while (info && info->index != ifa->ifa_index) {
                    info = info->next;
                }
                
                if (info) {
                    info->family = ifa->ifa_family;
                    
                    // Extract IP address
                    if (tb[IFA_ADDRESS]) {
                        if (ifa->ifa_family == AF_INET) {
                            inet_ntop(AF_INET, RTA_DATA(tb[IFA_ADDRESS]), 
                                    info->addr, INET_ADDRSTRLEN);
                        } else if (ifa->ifa_family == AF_INET6) {
                            inet_ntop(AF_INET6, RTA_DATA(tb[IFA_ADDRESS]), 
                                    info->addr, INET6_ADDRSTRLEN);
                        }
                    }
                    
                    // Extract broadcast address
                    if (tb[IFA_BROADCAST]) {
                        if (ifa->ifa_family == AF_INET) {
                            inet_ntop(AF_INET, RTA_DATA(tb[IFA_BROADCAST]), 
                                    info->broadcast, INET_ADDRSTRLEN);
                        }
                    }
                    
                    // Calculate netmask from prefix length
                    if (ifa->ifa_family == AF_INET) {
                        uint32_t mask = htonl(~((1 << (32 - ifa->ifa_prefixlen)) - 1));
                        inet_ntop(AF_INET, &mask, info->netmask, INET_ADDRSTRLEN);
                    } else if (ifa->ifa_family == AF_INET6) {
                        snprintf(info->netmask, sizeof(info->netmask), "/%d", ifa->ifa_prefixlen);
                    }
                }
            }
            
            nlh = NLMSG_NEXT(nlh, len);
        }
    }
}

// Main function to get all interface addresses
ifaddr_info_t* netlink_getifaddrs(void) {
    int fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (fd < 0) {
        perror("socket");
        return NULL;
    }
    
    // Get interface information
    ifaddr_info_t *interfaces = get_interface_info(fd);
    if (!interfaces) {
        close(fd);
        return NULL;
    }
    
    // Get IP addresses
    get_ip_addresses(fd, interfaces);
    
    close(fd);
    return interfaces;
}

// Free the interface list
void free_ifaddrs(ifaddr_info_t *interfaces) {
    while (interfaces) {
        ifaddr_info_t *next = interfaces->next;
        free(interfaces);
        interfaces = next;
    }
}

// Helper function to print interface flags
void print_flags(unsigned int flags) {
    printf("Flags: ");
    if (flags & IFF_UP) printf("UP ");
    if (flags & IFF_BROADCAST) printf("BROADCAST ");
    if (flags & IFF_DEBUG) printf("DEBUG ");
    if (flags & IFF_LOOPBACK) printf("LOOPBACK ");
    if (flags & IFF_POINTOPOINT) printf("POINTOPOINT ");
    if (flags & IFF_RUNNING) printf("RUNNING ");
    if (flags & IFF_NOARP) printf("NOARP ");
    if (flags & IFF_PROMISC) printf("PROMISC ");
    if (flags & IFF_NOTRAILERS) printf("NOTRAILERS ");
    if (flags & IFF_ALLMULTI) printf("ALLMULTI ");
    if (flags & IFF_MASTER) printf("MASTER ");
    if (flags & IFF_SLAVE) printf("SLAVE ");
    if (flags & IFF_MULTICAST) printf("MULTICAST ");
    printf("\n");
}

// Example usage
int main() {
    printf("Fetching network interfaces using Netlink...\n\n");
    
    ifaddr_info_t *interfaces = netlink_getifaddrs();
    if (!interfaces) {
        fprintf(stderr, "Failed to get interface information\n");
        return 1;
    }
    
    ifaddr_info_t *current = interfaces;
    while (current) {
        printf("Interface: %s (index: %d)\n", current->name, current->index);
        
        // Print flags
        print_flags(current->flags);
        
        // Print MTU
        if (current->mtu > 0) {
            printf("MTU: %u\n", current->mtu);
        }
        
        // Print MAC address
        printf("MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
               current->mac[0], current->mac[1], current->mac[2],
               current->mac[3], current->mac[4], current->mac[5]);
        
        // Print IP address
        if (strlen(current->addr) > 0) {
            printf("IP Address: %s\n", current->addr);
        }
        
        // Print netmask
        if (strlen(current->netmask) > 0) {
            printf("Netmask: %s\n", current->netmask);
        }
        
        // Print broadcast
        if (strlen(current->broadcast) > 0) {
            printf("Broadcast: %s\n", current->broadcast);
        }
        
        printf("Family: %s\n", 
               current->family == AF_INET ? "IPv4" : 
               current->family == AF_INET6 ? "IPv6" : "Unknown");
        
        printf("---\n");
        current = current->next;
    }
    
    free_ifaddrs(interfaces);
    return 0;
}