#include <assert.h>
#include <iostream>
#include <string>
#include <atomic>
#include <mutex>

#ifdef USE_WINSOCK
#include <winsock2.h>
#include <WS2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include "stunclient_c.h"
#include "sock_ntop.h"
#include "json_util.h"
#include "writen.h"
#include "readline.h"
#include "inet_ntop.h"
#include "inet_pton.h"

#define WSVERS      MAKEWORD(2, 0)

namespace {

std::atomic<int> proxy_port {9999};

struct WSADATA_Guard { 
    WSADATA_Guard() {
        if (WSAStartup(WSVERS, &wsadata) != 0) {
            printf("WSAStartup failed\n");
            exit(1);
        }
    }

    ~WSADATA_Guard() {
        WSACleanup();
    }

    WSADATA wsadata;
};

std::shared_ptr<WSADATA_Guard> wsadata_guard; 

std::once_flag init_wsadata_guard_flag;

void init_wsadata_guard()
{
    wsadata_guard.reset(new WSADATA_Guard);
}

}   // namespace {

std::shared_ptr<WSADATA_Guard> get_wsadata_guard()
{
    std::call_once(init_wsadata_guard_flag, &init_wsadata_guard);
    return wsadata_guard;
}

void set_proxy_port(int port)
{
    proxy_port = port;
}

int get_proxy_port()
{
    return proxy_port;
}

void stun_client_args_c_init(StunClientArgs_C *args)
{
    assert(args);

    memset(args, 0x0, sizeof(*args));
    args->family = 0;
    args->remoteServerHost = NULL;
    args->remoteServerPort = 0;
    args->localAddr = NULL;
    args->localPort = 0;
    args->mode = NULL;
}

int stun_client_udp_loop(const struct StunClientArgs_C *args, struct StunClientResults_C *results_c)
{
	int					sockfd;
	struct sockaddr_in	servaddr;

    auto guard = get_wsadata_guard();

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("socket error: %d, %s\n", errno, strerror(errno));
        return -1;
    }

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons((uint16_t) get_proxy_port());
	if (inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr) < 0) {
        printf("inet_pton error: %d, %s\n", errno, strerror(errno));
        return -2;
    }

	if (connect(sockfd, (sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        printf("connect error: %d, %s\n", errno, strerror(errno));
        return -3;
    }

    std::string args_json = StunClientArgs_C2Json(*args);
#ifdef DEBUG
    std::cout << "args_json: " << args_json << std::endl;
#endif
    args_json += "\r\n";

    if (writen(sockfd, args_json.data(), args_json.size()) != (int) args_json.size()) {
        printf("writen error: %d, %s\n", errno, strerror(errno));
        return -4;
    }

    std::string data;
    if (readline(sockfd, data, "\r\n") < 0) {
        printf("readline error: %d, %s\n", errno, strerror(errno));
        return -5;
    }

#ifdef DEBUG
    std::cout << "results_json: " << data << std::endl;
#endif
    auto presults = Json2StunClientResults_C(data); 
    if (presults == NULL) {
        printf("Json2StunClientResults_C error: data: %s\n", data.c_str());
        return -5;
    }

    *results_c = *presults;

    return 0;
}

