#include "utils.h"
#include <Windows.h>
#include <vector>
#include <locale> // ctype

namespace ul
{
  auto base_name(std::string const& path) -> std::string { return path.substr(path.find_last_of("/\\") + 1); }

  auto from_ansi_wstring_to_string(std::wstring in) -> std::string
  {
    // ctype narrow
    int sz = WideCharToMultiByte(CP_ACP, 0, &in[0], static_cast<int>(in.size()), 0, 0, 0, 0);
    auto out = std::string(sz, 0);
    WideCharToMultiByte(CP_ACP, 0, &in[0], (int)in.size(), &out[0], sz, 0, 0);
    return out;
  }

  auto from_string_to_wstring(std::string in) -> std::wstring
  {
    std::vector<wchar_t> buf(in.size());
    std::use_facet<std::ctype<wchar_t>>(std::locale()).widen(in.data(), in.data() + in.size(), buf.data());
    return std::wstring(buf.data(), buf.size());
  }
}  // namespace ul