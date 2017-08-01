#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "json_util.h"

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

boost::shared_ptr<StunClientArgs_C> Json2StunClientArgs_C(const std::string &args)
{
    boost::shared_ptr<StunClientArgs_C> pargs = boost::shared_ptr<StunClientArgs_C>(new StunClientArgs_C, StunClientArgsDeleter()); 
    stun_client_args_c_init(pargs.get());

    Document document;
    document.Parse(args.c_str());
    if (document.HasParseError()) {
        return NULL;
    }

    if (document.HasMember("family")) {
        pargs->family = document["family"].GetInt();
    }

    if (document.HasMember("remoteServerHost")) {
        pargs->remoteServerHost = clone_from_string(document["remoteServerHost"].GetString());
    }

    if (document.HasMember("remoteServerPort")) {
        pargs->remoteServerPort = document["remoteServerPort"].GetInt();
    }

    if (document.HasMember("localAddr")) {
        pargs->localAddr = clone_from_string(document["localAddr"].GetString());
    }

    if (document.HasMember("localPort")) {
        pargs->localPort = document["localPort"].GetInt();
    }

    if (document.HasMember("mode")) {
        pargs->mode = clone_from_string(document["mode"].GetString());
    }

    return pargs;
}

std::string StunClientResults_C2Json(const StunClientResults_C &results);

boost::shared_ptr<StunClientResults_C> Json2StunClientResults_C(const std::string &args);

