## Installation
compiling server
```bash
make server
```

compiling client
```bash
make client
```

<br>
<br>

## 🔧 1. socket()
**Description**:
The `socket()` system call is the first step in network programming. It creates an endpoint for communication and returns a file descriptor that represents this socket.

- `domain`: Specifies the communication domain. Common values:
    - AF_INET for IPv4
    - AF_INET6 for IPv6
- `type`: Specifies the communication semantics:
    - `SOCK_STREAM` for TCP (reliable, connection-oriented)
    - `SOCK_DGRAM` for UDP (unreliable, connectionless)
- `protocol`: Usually 0 to select the default protocol for the given type.

**Returns**:
On success, returns a non-negative file descriptor. On failure, returns `-1` and sets `errno`.
**Why this comes first**:
Every network operation begins with a socket. Without it, you cannot bind, connect, send, or receive data.

<br>
<br>

## 🔧 2. bind()
Bind a socket to a specific IP address and port.

Prototype: 
```c
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

**Description**:
The `bind()` call assigns a local protocol address (IP and port) to a socket. This is mandatory for servers, and optional for clients (unless you want to specify the local IP/port explicitly).

- `sockfd`: The socket file descriptor returned by `socket()`.
- `addr`: A pointer to a `struct sockaddr_in` (usually cast to `struct sockaddr *`) that holds the IP address and port.
- `addrlen`: Size of the `addr` structure.

**Returns**:
`0` on success, `-1` on failure (with `errno` set).

**Why use it**:
Without `bind()`, the socket has no local identity. For TCP servers, this is required before `listen()`.

<br>
<br>

## 🔧 3. connect()
Establish a connection to a remote socket (client-side).

**Prototype**: 
```c
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

**Description**:
Used by clients, `connect()` initiates a connection to a server at the specified address and port.
- `sockfd`: The socket file descriptor.
- `addr`: Destination address (typically filled with struct sockaddr_in).
- `addrlen`: Size of the address structure.

**Returns**:
`0` on success. `-1` on error (with `errno` set). On error, you might get `ECONNREFUSED`, `ETIMEDOUT`, etc.

**Use Case**:
After connecting, the socket is ready for `send()` and `recv()`. It also enables `read()`/`write()` like a file descriptor.

<br>
<br>

## 🔧 4. listen()
Mark the socket as passive (server-side).

<br>

### ✅ What the kernel does after `listen()`:
Once you've called:
```c
listen(sockfd, backlog);
```

The kernel:
1. Marks the socket as passive (ready to accept connections).
2. Creates an internal queue (called the pending connection queue).
3. Starts accepting SYN packets (TCP connection requests).
4. For each request:
    - Handles the TCP 3-way handshake.
    - Once complete, it places the fully established connection into the queue.

<br>


| Queue Type       | What it's for                                        |
| ---------------- | ---------------------------------------------------- |
| **SYN queue**    | Partially completed handshakes (SYN received)        |
| **Accept queue** | Fully established connections (ready for `accept()`) |

---

How backlog affects this
```c
listen(sockfd, 10);
```
- The backlog controls the maximum number of fully completed (ready-to-accept) connections the kernel will queue.
- If the queue fills up:
    - New connection attempts are ignored or refused (RST is sent).
    - The client might retry later.

---

✅ Kernel manages all of this
You do not implement the queue yourself. The kernel:
- Stores the queue in socket state
- Manages timeouts, retransmissions
- Ensures accept() only returns connections that passed the handshake

You just:
```c
int client_fd = accept(sockfd, ...);
```
…and get a fresh, connected socket to use.

<br>

### What is backlog?
`backlog` is an integer that sets the maximum number of pending client connections the kernel will queue after the handshake is completed but before your program accepts them with `accept()`.

🧠 Think of backlog like a waiting room
- Your server is the **doctor**.
- `accept()` is when the doctor calls the next patient (client).
- `backlog` is how many patients can wait in the reception area.
- If it's full and more people (clients) come, they're turned away.

---

### 🔄 What happens during a connection?
1. A client sends a TCP SYN.
2. Kernel starts the 3-way handshake.
3. After handshake, connection is placed in the accept queue.
4. You call accept() to pull it from the queue.
5. If too many connections arrive before you call accept(), the queue fills up.

---

### 📦 What if the backlog is too small?
- Excess connections are dropped or reset (RST is sent).
- Clients may retry.
- You risk denying service to legitimate clients under load.

<br>

| Protocol | `listen()` | `accept()` | `connect()` | How data is handled                   |
| -------- | ---------- | ---------- | ----------- | ------------------------------------- |
| **TCP**  | ✅ Yes     | ✅ Yes     | ✅ Optional | Requires connections, handshake, etc. |
| **UDP**  | ❌ No      | ❌ No      | ✅ Optional | Connectionless — just send/receive    |


<br>
<br>

## 🔧 5. accept()
accept() — at a high level:
Waits for an incoming connection on a listening socket, and creates a new connected socket for communicating with the client.

<br>

🔍 What `accept()` really does in detail:
1. Takes a listening socket
- You must have already:
    - Created it: `socket()`
    - Bound it: `bind()`
    - Marked it as listening: `listen()`

---

2. Waits for a connection
- If no pending connection is in the backlog queue:
- If the socket is blocking (default), `accept()` blocks (waits).
- If non-blocking, it returns `-1` and sets `errno = EAGAIN` or `EWOULDBLOCK`.

---

3. Dequeues a connection
- From the backlog queue established by `listen(sockfd, backlog)`.
- Each incoming TCP SYN is added to this queue.
- `accept()` removes the next completed connection from that queue.

---

4. Creates a new socket
- The new file descriptor (say, FD 4) refers to:
- A new socket specifically connected to the client.
- This socket can be used for `read()`, `write()`, `send()`, `recv()`.
- The original sockfd continues to listen for other connections.

---

5. Fills in `addr` (optional)
- If you pass non-NULL `addr` and `addrlen`, the kernel will fill in the client’s address (e.g., IP and port for TCP).
- Usually used to log or identify the peer.

---

6. Returns new socket FD
- On success → returns the new FD (≥ 0)
- On failure → returns -1, sets errno

---

✅ Resulting state after accept()
| FD       | Type             | Purpose                           |
| -------- | ---------------- | --------------------------------- |
| `sockfd` | Listening socket | Keep using to accept more clients |
| `newfd`  | Connected socket | Use this to talk to the client    |

---

***Example workflow***
```c
int server_fd = socket(AF_INET, SOCK_STREAM, 0);
bind(server_fd, ...);
listen(server_fd, 5);

int client_fd = accept(server_fd, ...);
// now client_fd is a connected socket
```

---

🧠 Bonus: What accept() does not do
- It does not authenticate the client
- It does not create a new thread or process
- It does not automatically make the socket non-blocking (use fcntl() or accept4() with SOCK_NONBLOCK)


<br>
<br>

## 🔧 6. send() and recv()
Once a connection is established (via connect() for clients or accept() for servers), you can start sending and receiving data.

### 🧪 How it works under the hood
| User Function | Syscall Used (x86\_64)    | Purpose                             |
| ------------- | ------------------------- | ----------------------------------- |
| `send()`      | `sendto()` (syscall 44)   | Sends data on a connected socket    |
| `recv()`      | `recvfrom()` (syscall 45) | Receives data on a connected socket |

Although `send()` and `recv()` are their own libc wrappers, they internally use the same syscalls as `sendto()` and `recvfrom()`, just with NULL destination/source address arguments for connected sockets.


Most Useful flags (Bitmask values):
| Flag           | Direction | Meaning                                                              |
| -------------- | --------- | -------------------------------------------------------------------- |
| `MSG_DONTWAIT` | both      | Make the call non-blocking (even if socket is blocking)              |
| `MSG_NOSIGNAL` | send      | Don't raise `SIGPIPE` if peer closed connection (useful for clients) |
| `MSG_PEEK`     | recv      | Look at incoming data without removing it from the queue             |
| `MSG_WAITALL`  | recv      | Wait until the full `len` bytes are received, unless error occurs    |
| `MSG_OOB`      | both      | Send or receive "out-of-band" (urgent) data                          |


<br>


***Examples***:
1. Prevent SIGPIPE on send()
When sending to a closed socket, Linux may send a SIGPIPE. You can prevent this:
```c
send(sockfd, buf, len, MSG_NOSIGNAL);
```

2. Peek at data without consuming it:
```c
recv(sockfd, buf, len, MSG_PEEK);  // You can read the same data again later
```

3. Non-blocking read, even if socket is blocking:
```c
recv(sockfd, buf, len, MSG_DONTWAIT);  // Returns -1 + EAGAIN if no data now
```

<br>
<br>

## 🔧 7. close()
Close the socket. (Usually just wraps close() from unistd.h.)

<br>
<br>

## Bonus (after the above):
- `setsockopt()` / `getsockopt()`

- `shutdown()`

- Support structures like `sockaddr_in`, `inet_addr()`, etc.

<br>
<br>

## Client/Server connection
| Role       | What it does                               |
| ---------- | ------------------------------------------ |
| **Server** | Creates socket → binds → listens → accepts |
| **Client** | Creates socket → **connects** to server    |


For UDP implementation => https://www.geeksforgeeks.org/computer-networks/udp-client-server-using-connect-c-implementation/
