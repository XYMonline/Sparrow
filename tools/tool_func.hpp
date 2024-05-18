#pragma once
#ifndef SPARROW_TOOL_FUNC_HPP
#define SPARROW_TOOL_FUNC_HPP

#include <string>

namespace leo {
;
std::string uuid_gen();
int64_t		get_timestamp();
std::string format_timestamp(int64_t timestamp);
}

#endif // !SPARROW_TOOL_FUNC_HPP