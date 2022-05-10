#pragma once

#include <Windows.h>
#include <string>

#include <functional>
#include <string_view>
#include <vector>

namespace ul::test
{
    auto get_lines(std::string&&path) -> std::vector<std::string>;
    void with_process(std::string_view&& command_line, std::function<void(PROCESS_INFORMATION const *)> callback);
    extern unsigned char dll_sample_injectee_x64[10752];
    extern unsigned char dll_sample_injectee_x86[8704];
}