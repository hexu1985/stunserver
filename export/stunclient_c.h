#ifndef STUNCLIENT_C_H
#define STUNCLIENT_C_H

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "nattype_c.h"

struct StunClientArgs_C {
    int family;                     // 4 or 6: IPv4 or IPv6
    const char *remoteServerHost;   // stun server host
    uint16_t remoteServerPort;      // stun server port
    const char *localAddr;          // stun client addr
    uint16_t localPort;             // stun client port
    const char *mode;               // Mode option must be 'full', 'basic', 'behavior', or 'filtering'
};

struct StunClientResults_C {
    // basic binding test will set these results
    int fBindingTestSuccess;        // if success set 1, else set 0
    struct sockaddr_storage addrLocal;     // local address
    socklen_t addrLocalLen;
    struct sockaddr_storage addrMapped;    // mapped address from PP for local address
    socklen_t addrMappedLen;
    // -----------------------------------------

    // behavior state test --------------------------
    int fBehaviorTestSuccess;       // if success set 1, else set 0
    enum NatType_C natType;
    // -----------------------------------------
};

#ifdef __cplusplus
extern "C" {
#endif

void stun_client_args_c_init(struct StunClientArgs_C *args);
int stun_client_udp_loop(const struct StunClientArgs_C *args, struct StunClientResults_C *results);

#ifdef __cplusplus
}
#endif

#endif
