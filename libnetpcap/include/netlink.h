/*
 * Complete Netlink Headers Implementation
 * This file contains all necessary definitions for netlink_getifaddrs.c
 */

#ifndef _NETLINK_H_
#define _NETLINK_H_

#include <stdint.h>
#include <sys/types.h>

/* ===== Basic Type Definitions ===== */
typedef uint8_t  __u8;
typedef uint16_t __u16;
typedef uint32_t __u32;
typedef uint64_t __u64;
typedef int8_t   __s8;
typedef int16_t  __s16;
typedef int32_t  __s32;
typedef int64_t  __s64;

/* ===== Socket Address Family ===== */
#define AF_NETLINK      16
#define PF_NETLINK      AF_NETLINK

/* ===== Netlink Protocol Numbers ===== */
#define NETLINK_ROUTE           0   /* Routing/device hook */
#define NETLINK_UNUSED          1   /* Unused number */
#define NETLINK_USERSOCK        2   /* Reserved for user mode socket protocols */
#define NETLINK_FIREWALL        3   /* Unused number, formerly ip_queue */
#define NETLINK_SOCK_DIAG       4   /* socket monitoring */
#define NETLINK_NFLOG           5   /* netfilter/iptables ULOG */
#define NETLINK_XFRM            6   /* ipsec */
#define NETLINK_SELINUX         7   /* SELinux event notifications */
#define NETLINK_ISCSI           8   /* Open-iSCSI */
#define NETLINK_AUDIT           9   /* auditing */
#define NETLINK_FIB_LOOKUP      10  
#define NETLINK_CONNECTOR       11
#define NETLINK_NETFILTER       12  /* netfilter subsystem */
#define NETLINK_IP6_FW          13
#define NETLINK_DNRTMSG         14  /* DECnet routing messages */
#define NETLINK_KOBJECT_UEVENT  15  /* Kernel messages to userspace */
#define NETLINK_GENERIC         16
#define NETLINK_SCSITRANSPORT   18  /* SCSI Transports */
#define NETLINK_ECRYPTFS        19
#define NETLINK_RDMA            20
#define NETLINK_CRYPTO          21  /* Crypto layer */
#define NETLINK_SMC             22  /* SMC monitoring */

/* ===== Netlink Socket Address ===== */
struct sockaddr_nl {
    __u16       nl_family;      /* AF_NETLINK */
    __u16       nl_pad;         /* zero */
    __u32       nl_pid;         /* port ID */
    __u32       nl_groups;      /* multicast groups mask */
};

/* ===== Netlink Message Header ===== */
struct nlmsghdr {
    __u32       nlmsg_len;      /* Length of message including header */
    __u16       nlmsg_type;     /* Message content */
    __u16       nlmsg_flags;    /* Additional flags */
    __u32       nlmsg_seq;      /* Sequence number */
    __u32       nlmsg_pid;      /* Sending process port ID */
};

/* ===== Netlink Message Types ===== */
#define NLMSG_NOOP      0x1     /* Nothing */
#define NLMSG_ERROR     0x2     /* Error */
#define NLMSG_DONE      0x3     /* End of a dump */
#define NLMSG_OVERRUN   0x4     /* Data lost */

#define NLMSG_MIN_TYPE  0x10    /* < 0x10: reserved control messages */

/* ===== Netlink Message Flags ===== */
#define NLM_F_REQUEST   0x01    /* It is request message */
#define NLM_F_MULTI     0x02    /* Multipart message, terminated by NLMSG_DONE */
#define NLM_F_ACK       0x04    /* Reply with ack, with zero or error code */
#define NLM_F_ECHO      0x08    /* Echo this request */
#define NLM_F_DUMP_INTR 0x10    /* Dump was inconsistent due to sequence change */
#define NLM_F_DUMP_FILTERED 0x20 /* Dump was filtered as requested */

/* GET request flags */
#define NLM_F_ROOT      0x100   /* specify tree root */
#define NLM_F_MATCH     0x200   /* return all matching */
#define NLM_F_ATOMIC    0x400   /* atomic GET */
#define NLM_F_DUMP      (NLM_F_ROOT|NLM_F_MATCH)

/* NEW request flags */
#define NLM_F_REPLACE   0x100   /* Override existing */
#define NLM_F_EXCL      0x200   /* Do not touch, if it exists */
#define NLM_F_CREATE    0x400   /* Create, if it does not exist */
#define NLM_F_APPEND    0x800   /* Add to end of list */

/* Modifiers to DELETE request */
#define NLM_F_NONREC    0x100   /* Do not delete recursively */
#define NLM_F_BULK      0x200   /* Delete multiple objects */

/* Flags for ACK message */
#define NLM_F_CAPPED    0x100   /* request was capped */
#define NLM_F_ACK_TLVS  0x200   /* extended ACK TVLs were included */

/* ===== Netlink Message Macros ===== */
#define NLMSG_ALIGNTO   4U
#define NLMSG_ALIGN(len) (((len)+NLMSG_ALIGNTO-1) & ~(NLMSG_ALIGNTO-1))
#define NLMSG_HDRLEN    ((int) NLMSG_ALIGN(sizeof(struct nlmsghdr)))
#define NLMSG_LENGTH(len) ((len) + NLMSG_HDRLEN)
#define NLMSG_SPACE(len) NLMSG_ALIGN(NLMSG_LENGTH(len))
#define NLMSG_DATA(nlh) ((void*)(((char*)nlh) + NLMSG_LENGTH(0)))
#define NLMSG_NEXT(nlh,len) ((len) -= NLMSG_ALIGN((nlh)->nlmsg_len), \
                            (struct nlmsghdr*)(((char*)(nlh)) + NLMSG_ALIGN((nlh)->nlmsg_len)))
#define NLMSG_OK(nlh,len) ((len) >= (int)sizeof(struct nlmsghdr) && \
                          (nlh)->nlmsg_len >= sizeof(struct nlmsghdr) && \
                          (nlh)->nlmsg_len <= (len))
#define NLMSG_PAYLOAD(nlh,len) ((nlh)->nlmsg_len - NLMSG_SPACE((len)))

/* ===== Netlink Error Message ===== */
struct nlmsgerr {
    int         error;
    struct nlmsghdr msg;
    /*
     * followed by the message contents unless NETLINK_CAP_ACK was set
     * or the ACK indicates success (error == 0)
     * message length is aligned with NLMSG_ALIGN()
     */
    /*
     * followed by TLVs defined in enum nlmsgerr_attrs
     * if NETLINK_EXT_ACK was set
     */
};

/* ===== Netlink Attribute Header ===== */
struct nlattr {
    __u16       nla_len;
    __u16       nla_type;
};

/* ===== Netlink Attribute Macros ===== */
#define NLA_ALIGNTO     4
#define NLA_ALIGN(len)  (((len) + NLA_ALIGNTO - 1) & ~(NLA_ALIGNTO - 1))
#define NLA_HDRLEN      ((int) NLA_ALIGN(sizeof(struct nlattr)))
#define NLA_DATA(nla)   ((void*)(((char*)(nla)) + NLA_HDRLEN))
#define NLA_PAYLOAD(nla) ((int)((nla)->nla_len) - NLA_HDRLEN)
#define NLA_NEXT(nla,attrlen) ((attrlen) -= NLA_ALIGN((nla)->nla_len), \
                              (struct nlattr*)(((char*)(nla)) + NLA_ALIGN((nla)->nla_len)))
#define NLA_OK(nla,len) ((len) >= (int)sizeof(struct nlattr) && \
                        (nla)->nla_len >= sizeof(struct nlattr) && \
                        (nla)->nla_len <= (len))
#define NLA_TYPE(nla)   ((nla)->nla_type & NLA_TYPE_MASK)
#define NLA_TYPE_MASK   ~0

/* ===== Route Netlink (rtnetlink) ===== */

/* rtnetlink message types */
#define RTM_BASE        16
#define RTM_NEWLINK     (RTM_BASE+0)
#define RTM_DELLINK     (RTM_BASE+1)
#define RTM_GETLINK     (RTM_BASE+2)
#define RTM_SETLINK     (RTM_BASE+3)

#define RTM_NEWADDR     (RTM_BASE+4)
#define RTM_DELADDR     (RTM_BASE+5)
#define RTM_GETADDR     (RTM_BASE+6)

#define RTM_NEWROUTE    (RTM_BASE+8)
#define RTM_DELROUTE    (RTM_BASE+9)
#define RTM_GETROUTE    (RTM_BASE+10)

#define RTM_NEWNEIGH    (RTM_BASE+12)
#define RTM_DELNEIGH    (RTM_BASE+13)
#define RTM_GETNEIGH    (RTM_BASE+14)

#define RTM_NEWRULE     (RTM_BASE+16)
#define RTM_DELRULE     (RTM_BASE+17)
#define RTM_GETRULE     (RTM_BASE+18)

#define RTM_NEWQDISC    (RTM_BASE+20)
#define RTM_DELQDISC    (RTM_BASE+21)
#define RTM_GETQDISC    (RTM_BASE+22)

#define RTM_NEWTCLASS   (RTM_BASE+24)
#define RTM_DELTCLASS   (RTM_BASE+25)
#define RTM_GETTCLASS   (RTM_BASE+26)

#define RTM_NEWTFILTER  (RTM_BASE+28)
#define RTM_DELTFILTER  (RTM_BASE+29)
#define RTM_GETTFILTER  (RTM_BASE+30)

#define RTM_NEWACTION   (RTM_BASE+32)
#define RTM_DELACTION   (RTM_BASE+33)
#define RTM_GETACTION   (RTM_BASE+34)

#define RTM_NEWPREFIX   (RTM_BASE+36)
#define RTM_GETMULTICAST (RTM_BASE+38)

#define RTM_GETANYCAST  (RTM_BASE+46)

#define RTM_NEWNEIGHTBL (RTM_BASE+48)
#define RTM_GETNEIGHTBL (RTM_BASE+50)
#define RTM_SETNEIGHTBL (RTM_BASE+51)

#define RTM_NEWNDUSEROPT (RTM_BASE+52)

#define RTM_NEWADDRLABEL (RTM_BASE+56)
#define RTM_DELADDRLABEL (RTM_BASE+57)
#define RTM_GETADDRLABEL (RTM_BASE+58)

#define RTM_GETDCB      (RTM_BASE+62)
#define RTM_SETDCB      (RTM_BASE+63)

#define RTM_NEWNETCONF  (RTM_BASE+80)
#define RTM_DELNETCONF  (RTM_BASE+81)
#define RTM_GETNETCONF  (RTM_BASE+82)

#define RTM_NEWMDB      (RTM_BASE+84)
#define RTM_DELMDB      (RTM_BASE+85)
#define RTM_GETMDB      (RTM_BASE+86)

#define RTM_NEWNSID     (RTM_BASE+88)
#define RTM_DELNSID     (RTM_BASE+89)
#define RTM_GETNSID     (RTM_BASE+90)

#define RTM_NEWSTATS    (RTM_BASE+92)
#define RTM_GETSTATS    (RTM_BASE+94)

#define RTM_NEWCACHEREPORT (RTM_BASE+96)

#define RTM_NEWCHAIN    (RTM_BASE+100)
#define RTM_DELCHAIN    (RTM_BASE+101)
#define RTM_GETCHAIN    (RTM_BASE+102)

#define RTM_NEWNEXTHOP  (RTM_BASE+104)
#define RTM_DELNEXTHOP  (RTM_BASE+105)
#define RTM_GETNEXTHOP  (RTM_BASE+106)

#define RTM_NEWLINKPROP (RTM_BASE+108)
#define RTM_DELLINKPROP (RTM_BASE+109)
#define RTM_GETLINKPROP (RTM_BASE+110)

#define RTM_NEWVLAN     (RTM_BASE+112)
#define RTM_DELVLAN     (RTM_BASE+113)
#define RTM_GETVLAN     (RTM_BASE+114)

#define RTM_NEWNEXTHOPBUCKET (RTM_BASE+116)
#define RTM_DELNEXTHOPBUCKET (RTM_BASE+117)
#define RTM_GETNEXTHOPBUCKET (RTM_BASE+118)

#define RTM_NEWTUNNEL   (RTM_BASE+120)
#define RTM_DELTUNNEL   (RTM_BASE+121)
#define RTM_GETTUNNEL   (RTM_BASE+122)

#define __RTM_MAX       ((RTM_BASE+122+3) & ~3)
#define RTM_MAX         (__RTM_MAX - 1)

/* ===== RTAttribute Structure ===== */
struct rtattr {
    unsigned short  rta_len;
    unsigned short  rta_type;
};

/* ===== RTAttribute Macros ===== */
#define RTA_ALIGNTO     4U
#define RTA_ALIGN(len) (((len)+RTA_ALIGNTO-1) & ~(RTA_ALIGNTO-1))
#define RTA_OK(rta,len) ((len) >= (int)sizeof(struct rtattr) && \
                        (rta)->rta_len >= sizeof(struct rtattr) && \
                        (rta)->rta_len <= (len))
#define RTA_NEXT(rta,attrlen) ((attrlen) -= RTA_ALIGN((rta)->rta_len), \
                              (struct rtattr*)(((char*)(rta)) + RTA_ALIGN((rta)->rta_len)))
#define RTA_LENGTH(len) (RTA_ALIGN(sizeof(struct rtattr)) + (len))
#define RTA_SPACE(len)  RTA_ALIGN(RTA_LENGTH(len))
#define RTA_DATA(rta)   ((void*)(((char*)(rta)) + RTA_LENGTH(0)))
#define RTA_PAYLOAD(rta) ((int)((rta)->rta_len) - RTA_LENGTH(0))

/* ===== Interface Info Message ===== */
struct ifinfomsg {
    unsigned char   ifi_family;
    unsigned char   __ifi_pad;
    unsigned short  ifi_type;       /* ARPHRD_* */
    int             ifi_index;      /* Link index */
    unsigned        ifi_flags;      /* IFF_* flags */
    unsigned        ifi_change;     /* IFF_* change mask */
};

/* ===== Interface Address Message ===== */
struct ifaddrmsg {
    __u8            ifa_family;
    __u8            ifa_prefixlen;  /* The prefix length */
    __u8            ifa_flags;      /* Flags */
    __u8            ifa_scope;      /* Address scope */
    __u32           ifa_index;      /* Link index */
};

/* ===== Interface Flags ===== */
#define IFF_UP          0x1             /* interface is up */
#define IFF_BROADCAST   0x2             /* broadcast address valid */
#define IFF_DEBUG       0x4             /* turn on debugging */
#define IFF_LOOPBACK    0x8             /* is a loopback net */
#define IFF_POINTOPOINT 0x10            /* interface is has p-p link */
#define IFF_NOTRAILERS  0x20            /* avoid use of trailers */
#define IFF_RUNNING     0x40            /* interface RFC2863 OPER_UP */
#define IFF_NOARP       0x80            /* no ARP protocol */
#define IFF_PROMISC     0x100           /* receive all packets */
#define IFF_ALLMULTI    0x200           /* receive all multicast packets */
#define IFF_MASTER      0x400           /* master of a load balancer */
#define IFF_SLAVE       0x800           /* slave of a load balancer */
#define IFF_MULTICAST   0x1000          /* Supports multicast */
#define IFF_PORTSEL     0x2000          /* can set media type */
#define IFF_AUTOMEDIA   0x4000          /* auto media select active */
#define IFF_DYNAMIC     0x8000          /* dialup device with changing addresses */
#define IFF_LOWER_UP    0x10000         /* driver signals L1 up */
#define IFF_DORMANT     0x20000         /* driver signals dormant */
#define IFF_ECHO        0x40000         /* echo sent packets */

/* ===== Interface Attributes ===== */
enum {
    IFLA_UNSPEC,
    IFLA_ADDRESS,
    IFLA_BROADCAST,
    IFLA_IFNAME,
    IFLA_MTU,
    IFLA_LINK,
    IFLA_QDISC,
    IFLA_STATS,
    IFLA_COST,
    IFLA_PRIORITY,
    IFLA_MASTER,
    IFLA_WIRELESS,      /* Wireless Extension event - see wireless.h */
    IFLA_PROTINFO,      /* Protocol specific information for a link */
    IFLA_TXQLEN,
    IFLA_MAP,
    IFLA_WEIGHT,
    IFLA_OPERSTATE,
    IFLA_LINKMODE,
    IFLA_LINKINFO,
    IFLA_NET_NS_PID,
    IFLA_IFALIAS,
    IFLA_NUM_VF,        /* Number of VFs if device is SR-IOV PF */
    IFLA_VFINFO_LIST,
    IFLA_STATS64,
    IFLA_VF_PORTS,
    IFLA_PORT_SELF,
    IFLA_AF_SPEC,
    IFLA_GROUP,         /* Group the device belongs to */
    IFLA_NET_NS_FD,
    IFLA_EXT_MASK,      /* Extended info mask, VFs, etc */
    IFLA_PROMISCUITY,   /* Promiscuity count: > 0 means acts PROMISC */
    IFLA_NUM_TX_QUEUES,
    IFLA_NUM_RX_QUEUES,
    IFLA_CARRIER,
    IFLA_PHYS_PORT_ID,
    IFLA_CARRIER_CHANGES,
    IFLA_PHYS_SWITCH_ID,
    IFLA_LINK_NETNSID,
    IFLA_PHYS_PORT_NAME,
    IFLA_PROTO_DOWN,
    IFLA_GSO_MAX_SEGS,
    IFLA_GSO_MAX_SIZE,
    IFLA_PAD,
    IFLA_XDP,
    IFLA_EVENT,
    IFLA_NEW_NETNSID,
    IFLA_IF_NETNSID,
    IFLA_TARGET_NETNSID = IFLA_IF_NETNSID, /* new alias */
    IFLA_CARRIER_UP_COUNT,
    IFLA_CARRIER_DOWN_COUNT,
    IFLA_NEW_IFINDEX,
    IFLA_MIN_MTU,
    IFLA_MAX_MTU,
    IFLA_PROP_LIST,
    IFLA_ALT_IFNAME,    /* Alternative ifname */
    IFLA_PERM_ADDRESS,
    IFLA_PROTO_DOWN_REASON,
    IFLA_PARENT_DEV_NAME,
    IFLA_PARENT_DEV_BUS_NAME,
    IFLA_GRO_MAX_SIZE,
    IFLA_TSO_MAX_SIZE,
    IFLA_TSO_MAX_SEGS,
    IFLA_ALLMULTI,      /* Allmulti count: > 0 means acts ALLMULTI */
    IFLA_DEVLINK_PORT,
    IFLA_GSO_IPV4_MAX_SIZE,
    IFLA_GRO_IPV4_MAX_SIZE,
    
    __IFLA_MAX
};

#define IFLA_MAX (__IFLA_MAX - 1)

/* ===== Interface Address Attributes ===== */
enum {
    IFA_UNSPEC,
    IFA_ADDRESS,
    IFA_LOCAL,
    IFA_LABEL,
    IFA_BROADCAST,
    IFA_ANYCAST,
    IFA_CACHEINFO,
    IFA_MULTICAST,
    IFA_FLAGS,
    IFA_RT_PRIORITY,    /* u32, priority/metric for prefix route */
    IFA_TARGET_NETNSID,
    __IFA_MAX,
};

#define IFA_MAX (__IFA_MAX - 1)

/* ===== Address Families ===== */
#define AF_UNSPEC       0
#define AF_UNIX         1       /* Unix domain sockets */
#define AF_LOCAL        1       /* POSIX name for AF_UNIX */
#define AF_INET         2       /* Internet IP Protocol */
#define AF_AX25         3       /* Amateur Radio AX.25 */
#define AF_IPX          4       /* Novell IPX */
#define AF_APPLETALK    5       /* AppleTalk DDP */
#define AF_NETROM       6       /* Amateur Radio NET/ROM */
#define AF_BRIDGE       7       /* Multiprotocol bridge */
#define AF_ATMPVC       8       /* ATM PVCs */
#define AF_X25          9       /* Reserved for X.25 project */
#define AF_INET6        10      /* IP version 6 */
#define AF_ROSE         11      /* Amateur Radio X.25 PLP */
#define AF_DECnet       12      /* Reserved for DECnet project */
#define AF_NETBEUI      13      /* Reserved for 802.2LLC project */
#define AF_SECURITY     14      /* Security callback pseudo AF */
#define AF_KEY          15      /* PF_KEY key management API */
#define AF_ROUTE        AF_NETLINK
#define AF_PACKET       17      /* Packet family */
#define AF_ASH          18      /* Ash */
#define AF_ECONET       19      /* Acorn Econet */
#define AF_ATMSVC       20      /* ATM SVCs */
#define AF_RDS          21      /* RDS sockets */
#define AF_SNA          22      /* Linux SNA Project */
#define AF_IRDA         23      /* IRDA sockets */
#define AF_PPPOX        24      /* PPPoX sockets */
#define AF_WANPIPE      25      /* Wanpipe API Sockets */
#define AF_LLC          26      /* Linux LLC */
#define AF_IB           27      /* Native InfiniBand address */
#define AF_MPLS         28      /* MPLS */
#define AF_CAN          29      /* Controller Area Network */
#define AF_TIPC         30      /* TIPC sockets */
#define AF_BLUETOOTH    31      /* Bluetooth sockets */
#define AF_IUCV         32      /* IUCV sockets */
#define AF_RXRPC        33      /* RxRPC sockets */
#define AF_ISDN         34      /* mISDN sockets */
#define AF_PHONET       35      /* Phonet sockets */
#define AF_IEEE802154   36      /* IEEE802154 sockets */
#define AF_CAIF         37      /* CAIF sockets */
#define AF_ALG          38      /* Algorithm sockets */
#define AF_NFC          39      /* NFC sockets */
#define AF_VSOCK        40      /* vSockets */
#define AF_KCM          41      /* Kernel Connection Multiplexor */
#define AF_QIPCRTR      42      /* Qualcomm IPC Router */
#define AF_SMC          43      /* smc sockets: reserve number for PF_SMC protocol family that reuses AF_INET address family */
#define AF_XDP          44      /* XDP sockets */
#define AF_MCTP         45      /* Management component transport protocol */
#define AF_MAX          46      /* For now.. */

/* ===== Socket Types ===== */
#define SOCK_STREAM     1
#define SOCK_DGRAM      2
#define SOCK_RAW        3
#define SOCK_RDM        4
#define SOCK_SEQPACKET  5
#define SOCK_DCCP       6
#define SOCK_PACKET     10

/* ===== Socket Options ===== */
#define SOL_SOCKET      1
#define SOL_NETLINK     270

#define SO_DEBUG        1
#define SO_REUSEADDR    2
#define SO_TYPE         3
#define SO_ERROR        4
#define SO_DONTROUTE    5
#define SO_BROADCAST    6
#define SO_SNDBUF       7
#define SO_RCVBUF       8
#define SO_SNDBUFFORCE  32
#define SO_RCVBUFFORCE  33
#define SO_KEEPALIVE    9
#define SO_OOBINLINE    10
#define SO_NO_CHECK     11
#define SO_PRIORITY     12
#define SO_LINGER       13
#define SO_BSDCOMPAT    14
#define SO_REUSEPORT    15
#define SO_PASSCRED     16
#define SO_PEERCRED     17
#define SO_RCVLOWAT     18
#define SO_SNDLOWAT     19
#define SO_RCVTIMEO     20
#define SO_SNDTIMEO     21

/* Netlink socket options */
#define NETLINK_ADD_MEMBERSHIP      1
#define NETLINK_DROP_MEMBERSHIP     2
#define NETLINK_PKTINFO             3
#define NETLINK_BROADCAST_ERROR     4
#define NETLINK_NO_ENOBUFS          5
#define NETLINK_RX_RING             6
#define NETLINK_TX_RING             7
#define NETLINK_LISTEN_ALL_NSID     8
#define NETLINK_LIST_MEMBERSHIPS    9
#define NETLINK_CAP_ACK             10
#define NETLINK_EXT_ACK             11
#define NETLINK_GET_STRICT_CHK      12

/* ===== Interface Name Size ===== */
#define IF_NAMESIZE     16
#define IFNAMSIZ        IF_NAMESIZE

/* ===== String and Network Address Constants ===== */
#define INET_ADDRSTRLEN     16
#define INET6_ADDRSTRLEN    46

/* ===== Message Handling Macros ===== */
#define MSG_OOB         1
#define MSG_PEEK        2
#define MSG_DONTROUTE   4
#define MSG_TRYHARD     4       /* Synonym for MSG_DONTROUTE for DECnet */
#define MSG_CTRUNC      8
#define MSG_PROBE       0x10    /* Do not send. Only probe path f.e. for MTU */
#define MSG_TRUNC       0x20
#define MSG_DONTWAIT    0x40    /* Nonblocking io */
#define MSG_EOR         0x80    /* End of record */
#define MSG_WAITALL     0x100   /* Wait for a full request */
#define MSG_FIN         0x200
#define MSG_SYN         0x400
#define MSG_CONFIRM     0x800   /* Confirm path validity */
#define MSG_RST         0x1000
#define MSG_ERRQUEUE    0x2000  /* Fetch message from error queue */
#define MSG_NOSIGNAL    0x4000  /* Do not generate SIGPIPE */
#define MSG_MORE        0x8000  /* Sender will send more */
#define MSG_WAITFORONE  0x10000 /* recvmmsg(): block until 1+ packets avail */
#define MSG_SENDPAGE_NOPOLICY 0x10000 /* sendpage() internal : do no apply policy */
#define MSG_SENDPAGE_NOTLAST 0x20000 /* sendpage() internal : not the last page */
#define MSG_BATCH       0x40000 /* sendmmsg(): more messages coming */
#define MSG_EOF         MSG_FIN
#define MSG_NO_SHARED_FRAGS 0x80000 /* sendpage() internal : page frags are not shared */
#define MSG_SENDPAGE_DECRYPTED  0x100000 /* sendpage() internal : page may carry plain text and require encryption */

#define MSG_ZEROCOPY    0x4000000   /* Use user data in kernel path */
#define MSG_FASTOPEN    0x20000000  /* Send data in TCP SYN */
#define MSG_CMSG_CLOEXEC 0x40000000 /* Set close_on_exec for file descriptor received through SCM_RIGHTS */

/* ===== Generic Netlink ===== */
#define GENL_ID_CTRL        NLMSG_MIN_TYPE

struct genlmsghdr {
    __u8    cmd;
    __u8    version;
    __u16   reserved;
};

#define GENL_HDRLEN         NLMSG_ALIGN(sizeof(struct genlmsghdr))

/* Controller commands */
// enum {
//     CTRL_CMD_UNSPEC,
//     CTRL_CMD_NEWFAMILY,
//     CTRL_CMD_DELFAMILY,
//     CTRL_CMD_

struct msghdr {
    void         *msg_name;       // optional address
    unsigned int  msg_namelen;    // size of address
    struct iovec *msg_iov;        // scatter/gather array
    size_t        msg_iovlen;     // elements in msg_iov
    void         *msg_control;    // ancillary data
    size_t        msg_controllen; // ancillary data buffer len
    int           msg_flags;      // flags on received message
};

struct iovec {
    void  *iov_base;    // Starting address
    size_t iov_len;     // Number of bytes to transfer
};

#define IFLA_RTA(r)  ((struct rtattr*)(((char*)(r)) + NLMSG_ALIGN(sizeof(struct ifinfomsg))))
#define IFA_RTA(r)   ((struct rtattr*)(((char*)(r)) + NLMSG_ALIGN(sizeof(struct ifaddrmsg))))


#endif // _NETLINK_H