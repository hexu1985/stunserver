#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "json_util.h"
#include "sock_ntop_host.h"
#include "sock_get_port.h"
#include "inet_ntop.h"
#include "inet_pton.h"

using namespace rapidjson;

std::string StunClientArgs_C2Json(const StunClientArgs_C &args)
{
    Document document;
    document.SetObject();
    Document::AllocatorType &allocator = document.GetAllocator();

    document.AddMember("family", args.family, allocator);
    if (args.remoteServerHost)
        document.AddMember("remoteServerHost", Value(args.remoteServerHost, allocator), allocator);
    document.AddMember("remoteServerPort", (int) args.remoteServerPort, allocator);
    if (args.localAddr)
        document.AddMember("localAddr", Value(args.localAddr, allocator), allocator);
    document.AddMember("localPort", (int) args.localPort, allocator);
    if (args.mode)
        document.AddMember("mode", Value(args.mode, allocator), allocator);

    StringBuffer buffer;
    Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);
    std::string str = buffer.GetString();
    return str;
}

namespace {

struct StunClientArgsDeleter {
    void operator ()(StunClientArgs_C *args)
    {
        if (args) {
            delete args->remoteServerHost;
            delete args->localAddr;
            delete args->mode;
        }
        delete args;
    }
};

char *clone_from_string(const std::string &str)
{
    char *p = new char[str.size()+1];
    memset(p, 0, str.size()+1);
    strcpy(p, str.c_str());
    return p;
}

}

std::shared_ptr<StunClientArgs_C> Json2StunClientArgs_C(const std::string &str)
{
    std::shared_ptr<StunClientArgs_C> args = std::shared_ptr<StunClientArgs_C>(new StunClientArgs_C, StunClientArgsDeleter()); 
    stun_client_args_c_init(args.get());

    Document document;
    document.Parse(str.c_str());
    if (document.HasParseError()) {
        return NULL;
    }

    if (document.HasMember("family")) {
        args->family = document["family"].GetInt();
    }

    if (document.HasMember("remoteServerHost")) {
        args->remoteServerHost = clone_from_string(document["remoteServerHost"].GetString());
    }

    if (document.HasMember("remoteServerPort")) {
        args->remoteServerPort = document["remoteServerPort"].GetInt();
    }

    if (document.HasMember("localAddr")) {
        args->localAddr = clone_from_string(document["localAddr"].GetString());
    }

    if (document.HasMember("localPort")) {
        args->localPort = document["localPort"].GetInt();
    }

    if (document.HasMember("mode")) {
        args->mode = clone_from_string(document["mode"].GetString());
    }

    return args;
}

std::string StunClientResults_C2Json(const StunClientResults_C &results)
{
    Document document;
    document.SetObject();
    Document::AllocatorType &allocator = document.GetAllocator();

    document.AddMember("fBindingTestSuccess", results.fBindingTestSuccess, allocator);
    document.AddMember("addrLocalHost", Value(Sock_ntop_host((const sockaddr *) &results.addrLocal, results.addrLocalLen), allocator), allocator);
    document.AddMember("addrLocalPort", (int) ntohs(sock_get_port((const sockaddr *) &results.addrLocal, results.addrLocalLen)), allocator);
    document.AddMember("addrMappedHost", Value(Sock_ntop_host((const sockaddr *) &results.addrMapped, results.addrMappedLen), allocator), allocator);
    document.AddMember("addrMappedPort", (int) ntohs(sock_get_port((const sockaddr *) &results.addrMapped, results.addrMappedLen)), allocator);
    document.AddMember("fBehaviorTestSuccess", results.fBehaviorTestSuccess, allocator);
    document.AddMember("natType", (int) results.natType, allocator);

    StringBuffer buffer;
    Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);
    std::string str = buffer.GetString();
    return str;
}

namespace {

void sock_pton(struct sockaddr_in *addr, const std::string &ip, int port)
{
    addr->sin_family = AF_INET;
    addr->sin_port = htons((uint16_t) port);
    inet_pton(AF_INET, ip.c_str(), &addr->sin_addr); 
}

}

std::shared_ptr<StunClientResults_C> Json2StunClientResults_C(const std::string &str)
{
    std::shared_ptr<StunClientResults_C> results = std::shared_ptr<StunClientResults_C>(new StunClientResults_C); 
    memset(results.get(), 0, sizeof(StunClientResults_C));

    Document document;
    document.Parse(str.c_str());
    if (document.HasParseError()) {
        return NULL;
    }

    if (document.HasMember("fBindingTestSuccess")) {
        results->fBindingTestSuccess = document["fBindingTestSuccess"].GetInt();
    }

    std::string addrLocalHost = "0.0.0.0";
    if (document.HasMember("addrLocalHost")) {
        addrLocalHost = document["addrLocalHost"].GetString();
    }

    int addrLocalPort = 0;
    if (document.HasMember("addrLocalPort")) {
        addrLocalPort = document["addrLocalPort"].GetInt();
    }

    sock_pton((sockaddr_in *) &results->addrLocal, addrLocalHost, addrLocalPort); 
    results->addrLocalLen = sizeof(sockaddr_in); 

    std::string addrMappedHost = "0.0.0.0";
    if (document.HasMember("addrMappedHost")) {
        addrMappedHost = document["addrMappedHost"].GetString();
    }

    int addrMappedPort = 0;
    if (document.HasMember("addrMappedPort")) {
        addrMappedPort = document["addrMappedPort"].GetInt();
    }

    sock_pton((sockaddr_in *) &results->addrMapped, addrMappedHost, addrMappedPort); 
    results->addrMappedLen = sizeof(sockaddr_in); 

    if (document.HasMember("fBehaviorTestSuccess")) {
        results->fBehaviorTestSuccess = document["fBehaviorTestSuccess"].GetInt();
    }

    if (document.HasMember("natType")) {
        results->natType = (NatType_C) document["natType"].GetInt();
    }

    return results;
}

