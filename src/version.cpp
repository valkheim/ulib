#pragma comment(lib, "Version.lib")

#include "version.h"

#include <Windows.h>

#include <cassert>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

#include "log.h"

namespace ul
{
  auto operator<<(std::ostream &os, WindowsBuildVersion const &build) -> std::ostream &
  {
    return os << build.a << "." << build.b << "." << build.c << "." << build.d;
  }

  auto WindowsBuildVersion::to_string() const -> std::string
  {
    std::stringstream ss;
    ss << (*this);
    return ss.str();
  }

  auto get_ulib_version() -> std::string
  {
    static constexpr auto version = "1.0.0";
    return std::string(version);
  }

  auto get_windows_build_version() -> std::optional<WindowsBuildVersion>
  {
    constexpr auto system = L"kernel32.dll";
    DWORD dummy;
    const auto cbInfo = ::GetFileVersionInfoSizeExW(FILE_VER_GET_NEUTRAL, system, &dummy);
    if (cbInfo == 0) {
      ::ul::error("Cannot GetFileVersionInfoSizeExW");
      return {};
    }

    std::vector<char> buffer(cbInfo);
    if (::GetFileVersionInfoExW(FILE_VER_GET_NEUTRAL, system, dummy, static_cast<DWORD>(buffer.size()), &buffer[0]) == 0) {
      ::ul::error("Cannot GetFileVersionInfoExW");
      return {};
    }

    void *p = nullptr;
    UINT size = 0;
    if (::VerQueryValueW(buffer.data(), L"\\", &p, &size) == 0) {
      ::ul::error("Cannot VerQueryValueW");
      return {};
    }

    if (p == nullptr || size < sizeof(VS_FIXEDFILEINFO)) {
      ::ul::error("Cannot VerQueryValueW (bad outputs)");
      return {};
    }

    auto pFixed = static_cast<const VS_FIXEDFILEINFO *>(p);
    return WindowsBuildVersion{HIWORD(pFixed->dwFileVersionMS), LOWORD(pFixed->dwFileVersionMS), HIWORD(pFixed->dwFileVersionLS),
                               LOWORD(pFixed->dwFileVersionLS)};
  }
};  // namespace ul