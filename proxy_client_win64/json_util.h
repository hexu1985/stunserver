#ifndef __json_util_h
#define __json_util_h

#include <string>
#include <memory>
#include "stunclient_c.h"

std::string StunClientArgs_C2Json(const StunClientArgs_C &args);

std::shared_ptr<StunClientArgs_C> Json2StunClientArgs_C(const std::string &args);

std::string StunClientResults_C2Json(const StunClientResults_C &results);

std::shared_ptr<StunClientResults_C> Json2StunClientResults_C(const std::string &args);

#endif
