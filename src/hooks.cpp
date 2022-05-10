#include "hooks.h"

namespace ul
{
  Hook::Hook(std::size_t const hook_size, FARPROC const hook_location, PROC const detour)
      : hook_size(hook_size), hook_backup{}, hook_content{}, hook_location{hook_location}, detour{detour}
  {
    hook_backup.reserve(hook_size);
    std::fill(hook_backup.begin(), hook_backup.end(), 0x90);
    hook_content.reserve(hook_size);
  }

  auto Hook::hooked() -> bool { return std::memcmp(hook_content.data(), hook_location, hook_size) == 0; }

  auto Hook::unhook() -> bool
  {
    DWORD old_protect;
    if (!hooked()) return false;

    if (!VirtualProtect((LPVOID)hook_location, hook_size, PAGE_EXECUTE_READWRITE, &old_protect)) {
      ::ul::error("Cannot protect 0/1");
      return false;
    }

    std::memcpy(hook_location, hook_backup.data(), hook_size);
    if (!VirtualProtect((LPVOID)hook_location, hook_size, old_protect, &old_protect)) {
      ::ul::error("Cannot protect 1/1");
      return false;
    }

    return true;
  }

  namespace x86
  {
    HookE9::HookE9(FARPROC const hook_location, PROC detour) : Hook{5, hook_location, detour} {}

    auto HookE9::hook() -> bool
    {
      DWORD old_protect;
      if (hooked()) return false;

      std::memcpy(hook_backup.data(), hook_location, hook_size);
      hook_content = std::vector<std::uint8_t>{0xe9, 0x00, 0x00, 0x00, 0x00};
      auto offset = (std::uint32_t)detour - (std::uint32_t)((std::uint32_t)hook_location + 5);
      std::memcpy(hook_content.data() + 1, &offset, sizeof(&offset));
      if (!VirtualProtect((LPVOID)hook_location, hook_size, PAGE_EXECUTE_READWRITE, &old_protect)) {
        ::ul::error("Cannot protect 0/1");
        return false;
      }

      std::memcpy(hook_location, hook_content.data(), hook_content.size());
      if (!VirtualProtect((LPVOID)hook_location, hook_size, old_protect, &old_protect)) {
        ::ul::error("Cannot protect 1/1");
        return false;
      }

      return true;
    }
  }  // namespace x86

  namespace x64
  {
    HookFF25::HookFF25(FARPROC const hook_location, PROC detour) : Hook{14, hook_location, detour} {}

    auto HookFF25::hook() -> bool
    {
      DWORD old_protect;
      if (hooked()) return false;

      std::memcpy(hook_backup.data(), hook_location, hook_size);
      hook_content = std::vector<std::uint8_t>{0xFF, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
      std::memcpy(hook_content.data() + 6, &detour, sizeof(&detour));
      if (!VirtualProtect((LPVOID)hook_location, hook_size, PAGE_EXECUTE_READWRITE, &old_protect)) {
        ::ul::error("Cannot protect 0/1");
        return false;
      }

      std::memcpy(hook_location, hook_content.data(), hook_content.size());
      if (!VirtualProtect((LPVOID)hook_location, hook_size, old_protect, &old_protect)) {
        ::ul::error("Cannot protect 1/1");
        return false;
      }

      return true;
    }

    Hook48B8::Hook48B8(FARPROC const hook_location, PROC detour) : Hook{12, hook_location, detour} {}

    auto Hook48B8::hook() -> bool
    {
      DWORD old_protect;
      if (hooked()) return false;

      std::memcpy(hook_backup.data(), hook_location, hook_size);
      hook_content = std::vector<std::uint8_t>{0x48, 0xb8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xe0};
      std::memcpy(hook_content.data() + 2, &detour, sizeof(&detour));
      if (!VirtualProtect((LPVOID)hook_location, hook_size, PAGE_EXECUTE_READWRITE, &old_protect)) {
        ::ul::error("Cannot protect 0/1");
        return false;
      }

      std::memcpy(hook_location, hook_content.data(), hook_content.size());
      if (!VirtualProtect((LPVOID)hook_location, hook_size, old_protect, &old_protect)) {
        ::ul::error("Cannot protect 1/1");
        return false;
      }

      return true;
    }

    Hook68::Hook68(FARPROC const hook_location, PROC detour) : Hook{14, hook_location, detour} {}

    auto Hook68::hook() -> bool
    {
      DWORD old_protect;
      if (hooked()) return false;

      std::memcpy(hook_backup.data(), hook_location, hook_size);
      hook_content = std::vector<std::uint8_t>{0x68, 0x00, 0x00, 0x00, 0x00, 0xc7, 0x44, 0x24, 0x04, 0x00, 0x00, 0x00, 0x00, 0xc3};
      std::memcpy(hook_content.data() + 1, &detour, sizeof(&detour) / 2);
      for (unsigned i = 0; i < sizeof(&detour) / 2; ++i)
        hook_content[i + 9] = (std::uint8_t)((DWORD_PTR)detour >> (sizeof(std::uint32_t) * 8) + i * 8);

      if (!VirtualProtect((LPVOID)hook_location, hook_size, PAGE_EXECUTE_READWRITE, &old_protect)) {
        ::ul::error("Cannot protect 0/1");
        return false;
      }

      std::memcpy(hook_location, hook_content.data(), hook_content.size());
      if (!VirtualProtect((LPVOID)hook_location, hook_size, old_protect, &old_protect)) {
        ::ul::error("Cannot protect 1/1");
        return false;
      }

      return true;
    }
  }  // namespace x64
}  // namespace ul