#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include <getopt.h>

#include <iostream>

#include "stunclient_c.h"
#include "sock_ntop.h"
#include "json_util.h"


void print_results(const struct StunClientResults_C *results)
{
    printf("\n======================================\n");
    printf("results:\n");
    printf("fBindingTestSuccess: %d\n", results->fBindingTestSuccess);
    printf("addrLocal: %s\n", sock_ntop((const struct sockaddr *) &results->addrLocal, results->addrLocalLen));
    printf("addrMapped: %s\n", sock_ntop((const struct sockaddr *) &results->addrMapped, results->addrMappedLen));
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

int main(int argc, char *argv[])
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

    int ret;
    struct StunClientResults_C results;
    std::string args_json = StunClientArgs_C2json(args);
    std::cout << "args_json: " << args_json << std::endl;
    ret = stun_client_udp_loop(&args, &results);
    if (ret != 0) {
        printf("stun_client_udp_loop error: ret=%d, errno=%d\n", ret, errno);
        exit(ret);
    }

    print_results(&results);

    return 0;
}

