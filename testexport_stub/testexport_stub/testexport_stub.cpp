// testexport_stub.cpp : Defines the entry point for the console application.
//


#include "stdafx.h"
#include <iostream>
#include "sock_ntop.h"
#include "stunclient_c.h" 

using namespace std;

#pragma comment(lib,"export_stub.lib")

void print_results(const struct StunClientResults_C *results)
{
	printf("\n======================================\n");
	printf("results:\n");
	printf("fBindingTestSuccess: %d\n", results->fBindingTestSuccess);
	printf("addrLocal: %s\n", Sock_ntop((const struct sockaddr *) &results->addrLocal, results->addrLocalLen));
	printf("addrMapped: %s\n", Sock_ntop((const struct sockaddr *) &results->addrMapped, results->addrMappedLen));
	printf("fBehaviorTestSuccess: %d\n", results->fBehaviorTestSuccess);
	printf("natType: %d\n", (int)results->natType);
	printf("\n======================================\n");
}

int _tmain(int argc, _TCHAR* argv[])
{
	struct StunClientArgs_C args;
	stun_client_args_c_init(&args);

	args.mode = "behavior";
	set_proxy_port(9999);
	args.remoteServerHost = "stun.ekiga.net";

	int ret;
	struct StunClientResults_C results;
	ret = stun_client_udp_loop(&args, &results);
	if (ret != 0) {
		printf("stun_client_udp_loop error: ret=%d, errno=%d\n", ret, errno);
		exit(ret);
	}

	print_results(&results);
	cout << "entry any key and newline: ";
	char c;
	cin >> c;
	return 0;
}

