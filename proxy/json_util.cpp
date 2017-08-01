#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "json_util.h"

using namespace rapidjson;

std::string StunClientArgs_C2json(const StunClientArgs_C &args)
{
    Document document;
    document.SetObject();
    Document::AllocatorType &allocator = document.GetAllocator();

    document.AddMember("family", args.family, allocator);

    StringBuffer buffer;
    Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);
    std::string str = buffer.GetString();
    return str;
}

boost::shared_ptr<StunClientArgs_C> json2StunClientArgs_C(const std::string &args);

std::string StunClientResults_C2json(const StunClientResults_C &results);

boost::shared_ptr<StunClientResults_C> json2StunClientResults_C(const std::string &args);

