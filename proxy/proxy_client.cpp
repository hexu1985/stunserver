#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <getopt.h>

#include <iostream>

#include "stunclient_c.h"
#include "sock_ntop.h"
#include "json_util.h"
#include "writen.h"
#include "readline.h"

void print_results(const struct StunClientResults_C *results)
{
    printf("\n======================================\n");
    printf("results:\n");
    printf("fBindingTestSuccess: %d\n", results->fBindingTestSuccess);
    printf("addrLocal: %s\n", Sock_ntop((const struct sockaddr *) &results->addrLocal, results->addrLocalLen));
    printf("addrMapped: %s\n", Sock_ntop((const struct sockaddr *) &results->addrMapped, results->addrMappedLen));
    printf("fBehaviorTestSuccess: %d\n", results->fBehaviorTestSuccess);
    printf("natType: %d\n", (int) results->natType);
    printf("\n======================================\n");
}

void usage(char *cmd)
{
    printf("usage: %s OPTIONS server [port]\n", cmd);
    printf("\n");
    printf("OPTIONS\n");
    printf("\t--mode MODE (Mode option must be 'full', 'basic', 'behavior', or 'filtering')\n");
    printf("\t--localaddr INTERFACE\n");
    printf("\t--localport PORTNUMBER\n");
    printf("\t--family IPVERSION\n");
    printf("\n");
}

struct option longopts[] = { 
    {"mode", 1, NULL, 'm'},
    {"localaddr", 1, NULL, 'a'},
    {"localport", 1, NULL, 'p'},
    {"family", 1, NULL, 'f'},
    {"help", 0, NULL, 'h'},
    {0,0,0,0}
};

int main(int argc, char **argv)
{
    int opt;
    struct StunClientArgs_C args;
    stun_client_args_c_init(&args);

    while((opt = getopt_long(argc, argv, "m:a:p:f:h", longopts, NULL)) != -1) {
        switch(opt) {
            case 'm':
                printf("mode: %s(%p)\n", optarg, optarg);
                args.mode = optarg;
                break;
            case 'a':
                printf("localAddr: %s(%p)\n", optarg, optarg);
                args.localAddr = optarg;
                break;
            case 'p':
                printf("localPort: %s(%p)\n", optarg, optarg);
                args.localPort = atoi(optarg);
                break;
            case 'f':
                printf("family: %s(%p)\n", optarg, optarg);
                args.family = atoi(optarg);
                break;
            case 'h':
                usage(argv[0]);
                exit(1);
            case '?':
                printf("unknown option: %c\n", optopt);
                break;
        }   
    }   
    int i = 1;
    for(; optind < argc; optind++) {
        if (i == 1) {
            printf("remoteServerHost: %s(%p)\n", argv[optind], argv[optind]);
            args.remoteServerHost = argv[optind];
        } 
        if (i == 2) {
            printf("remoteServerPort: %s(%p)\n", argv[optind], argv[optind]);
            args.remoteServerPort = atoi(argv[optind]);
        }
        i++;
    }

	int					sockfd;
	struct sockaddr_in	servaddr;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("socket error: %d, %s\n", errno, strerror(errno));
        exit(1);
    }

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons((uint16_t) 9999);
	if (inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr) < 0) {
        printf("inet_pton error: %d, %s\n", errno, strerror(errno));
        exit(1);
    }

	if (connect(sockfd, (sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        printf("connect error: %d, %s\n", errno, strerror(errno));
        exit(1);
    }

    std::string args_json = StunClientArgs_C2Json(args);
    std::cout << "args_json: " << args_json << std::endl;
    args_json += "\r\n";

    if (writen(sockfd, args_json.data(), args_json.size()) != args_json.size()) {
        printf("writen error: %d, %s\n", errno, strerror(errno));
        exit(1);
    }

    std::string data;
    if (readline(sockfd, data, "\r\n") < 0) {
        printf("readline error: %d, %s\n", errno, strerror(errno));
        exit(1);
    }

    std::cout << "results_json: " << data << std::endl;
    auto presults = Json2StunClientResults_C(data); 

    print_results(presults.get());

    close(sockfd);
    return 0;
}
