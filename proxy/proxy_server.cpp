#include <thread>
#include "tcp_listen.h"
#include "sock_ntop.h"
#include "readline.h"
#include "stunclient_c.h"
#include "json_util.h"
#include "writen.h"

static void doit(int sockfd);		/* each thread executes this function */

int
main(int argc, char **argv)
{
	int				listenfd, connfd;
    pid_t           childpid;
	socklen_t		addrlen, len;
	struct sockaddr_storage	cliaddr;

	if (argc == 2) {
		listenfd = Tcp_listen(NULL, argv[1], &addrlen);
    } else if (argc == 3) {
		listenfd = Tcp_listen(argv[1], argv[2], &addrlen);
    } else {
		printf("usage: tcpserv01 [ <host> ] <service or port>\n");
        exit(1);
    }

	for ( ; ; ) {
		len = addrlen;
		connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &len);
        if (connfd < 0) {
            printf("accept error: %d, %s\n", errno, strerror(errno));
            continue;
        }
        printf("new client: %s, fd = %d\n", Sock_ntop((struct sockaddr *) &cliaddr, len), connfd);
        childpid = fork();
        if (childpid < 0) {
            printf("fork error: %d, %s\n", errno, strerror(errno));
            continue;
        }
        if (childpid == 0) {    // child proccess
            doit(connfd);
            close(connfd);  // child close 
            printf("process end, fd = %d\n", connfd);
            exit(0);
        }
        close(connfd);  // parent close
	}

    return 0;
}

static void print_results(const struct StunClientResults_C *results)
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

static void doit(int sockfd)
{
    std::string data;
    if (readline(sockfd, data, "\r\n") < 0) {
        printf("readline error: %d, %s, fd = %d\n", errno, strerror(errno), sockfd);
        return;
    }

    auto pargs = Json2StunClientArgs_C(data);
    if (pargs == NULL) {
        printf("parse json fail\n");
        return;
    }

    struct StunClientResults_C results;
    int ret = stun_client_udp_loop(pargs.get(), &results);
    if (ret != 0) {
        printf("stun_client_udp_loop error: ret=%d, errno=%d\n", ret, errno);
        return;
    }
#ifdef DEBUG
    print_results(&results);
#endif

    std::string results_json = StunClientResults_C2Json(results);
#ifdef DEBUG
    std::cout << "results_json: " << results_json << std::endl;
#endif

    results_json += "\r\n";
    writen(sockfd, results_json.data(), results_json.size()); 
}
