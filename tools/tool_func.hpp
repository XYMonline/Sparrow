#pragma once
#ifndef SPARROW_TOOL_FUNC_HPP
#define SPARROW_TOOL_FUNC_HPP

#include <string>
#include <tuple>

namespace leo {
;
std::string uuid_gen();
int64_t		get_timestamp();
std::string format_timestamp(int64_t timestamp);
std::tuple<std::string, std::string> uri2host_port(const std::string& uri);
std::string calculateSHA512(const std::string& data);
}

#endif // !SPARROW_TOOL_FUNC_HPP