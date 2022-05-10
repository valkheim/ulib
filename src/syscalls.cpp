#include "syscalls.h"

#include <windows.h>

#include <algorithm>
#include <array>
#include <format>
#include <sstream>

#include "addresses.h"
#include "log.h"

namespace ul
{
  auto operator<<(std::ostream &os, Syscall const &syscall) -> std::ostream &
  {
    return os << syscall.name << ", " << syscall.number << std::hex << ", 0x" << syscall.number;
  }

  auto Syscall::to_string() const -> std::string
  {
    std::stringstream ss;
    ss << (*this);
    return ss.str();
  }

  auto walk_syscalls_x64(on_syscall &&callback) -> bool
  {
    /*
    clang-format off
    .text:000000018009D370                         ; NTSTATUS __stdcall NtOpenFile(PHANDLE FileHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, PIO_STATUS_BLOCK IoStatusBlock, ULONG ShareAccess, ULONG OpenOptions)
    .text:000000018009D370                                         public NtOpenFile
    .text:000000018009D370                         NtOpenFile      proc near               ; CODE XREF: LdrpMapResourceFile+10D↑p
    .text:000000018009D370                                                                 ; RtlpCheckRelativeDrive+FA↑p ...
    .text:000000018009D370
    .text:000000018009D370                         ShareAccess     = dword ptr  28h
    .text:000000018009D370                         OpenOptions     = dword ptr  30h
    .text:000000018009D370
    .text:000000018009D370 4C 8B D1                                mov     r10, rcx        ; NtOpenFile
    .text:000000018009D373 B8 33 00 00 00                          mov     eax, 33h ; '3'
    .text:000000018009D378 F6 04 25 08 03 FE 7F 01                 test    byte ptr ds:7FFE0308h, 1
    .text:000000018009D380 75 03                                   jnz     short loc_18009D385
    .text:000000018009D382 0F 05                                   syscall                 ; Low latency system call
    .text:000000018009D384 C3                                      retn
    .text:000000018009D385                         ; ---------------------------------------------------------------------------
    .text:000000018009D385
    .text:000000018009D385                         loc_18009D385:                          ; CODE XREF: NtOpenFile+10↑j
    .text:000000018009D385 CD 2E                                   int     2Eh             ; DOS 2+ internal - EXECUTE COMMAND
    .text:000000018009D385                                                                 ; DS:SI -> counted CR-terminated command string
    .text:000000018009D387 C3                                      retn
    .text:000000018009D387                         NtOpenFile      endp
    .text:000000018009D387
    .text:000000018009D387                         ; ---------------------------------------------------------------------------
    .text:000000018009D388 0F 1F 84 00 00 00 00 00                 align 10h
    clang-format on
    */
    constexpr auto module_name = R"(C:\windows\system32\ntdll.dll)";
    auto function_code = std::array<std::uint8_t, g_regular_syscall_function_size_x64>{0};
    return ::ul::walk_exports(module_name, [&](::ul::Export const &xport) -> ::ul::walk_t {
      if (!xport.name.starts_with("Nt")) {
        return ::ul::walk_t::WALK_CONTINUE;
      }

      std::memcpy(function_code.data(), xport.address, g_regular_syscall_function_size_x64);
      if (function_code[3] != 0xB8) {
        return ::ul::walk_t::WALK_CONTINUE;
      }

      return callback(Syscall{false, function_code[4], xport.address, xport.name});
    });
  }

  auto walk_syscalls_x86(on_syscall &&callback) -> bool
  {
    /*
    clang-format off
    .text:4B2F2CB0                         ; NTSTATUS __stdcall NtOpenFile(PHANDLE FileHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, PIO_STATUS_BLOCK IoStatusBlock, ULONG ShareAccess, ULONG OpenOptions)
    .text:4B2F2CB0                                         public _NtOpenFile@24
    .text:4B2F2CB0                         _NtOpenFile@24  proc near               ; CODE XREF: LdrpNtOpenFileUnredirected(x,x,x)+1C↑p
    .text:4B2F2CB0                                                                 ; LdrpNtOpenFileUnredirected(x,x,x)+AD↑p ...
    .text:4B2F2CB0
    .text:4B2F2CB0                         FileHandle      = dword ptr  4
    .text:4B2F2CB0                         DesiredAccess   = dword ptr  8
    .text:4B2F2CB0                         ObjectAttributes= dword ptr  0Ch
    .text:4B2F2CB0                         IoStatusBlock   = dword ptr  10h
    .text:4B2F2CB0                         ShareAccess     = dword ptr  14h
    .text:4B2F2CB0                         OpenOptions     = dword ptr  18h
    .text:4B2F2CB0
    .text:4B2F2CB0 B8 33 00 00 00                          mov     eax, 33h ; '3'  ; NtOpenFile
    .text:4B2F2CB5 BA 30 87 30 4B                          mov     edx, offset _Wow64SystemServiceCall@0 ; Wow64SystemServiceCall()
    .text:4B2F2CBA FF D2                                   call    edx ; Wow64SystemServiceCall() ; Wow64SystemServiceCall()
    .text:4B2F2CBC C2 18 00                                retn    18h
    .text:4B2F2CBC                         _NtOpenFile@24  endp
    .text:4B2F2CBC
    .text:4B2F2CBC                         ; ---------------------------------------------------------------------------
    .text:4B2F2CBF 90                                      align 10h
    clang-format on
    */
    constexpr auto module_name = R"(C:\Windows\SysWOW64\ntdll.dll)";
    auto function_code = std::array<std::uint8_t, g_regular_syscall_function_size_x86>{0};
    return ::ul::walk_exports(module_name, [&](::ul::Export const &xport) -> ::ul::walk_t {
      if (!xport.name.starts_with("Nt")) {
        return ::ul::walk_t::WALK_CONTINUE;
      }

      std::memcpy(function_code.data(), xport.address, g_regular_syscall_function_size_x86);
      if (function_code[0] != 0xB8) {
        return ::ul::walk_t::WALK_CONTINUE;
      }

      return callback(Syscall{false, function_code[1], xport.address, xport.name});
    });
  }

  auto walk_syscalls(on_syscall callback) -> bool
  {
#if _WIN64
    return walk_syscalls_x64(std::move(callback));
#else
    return walk_syscalls_x86(std::move(callback));
#endif
  }

  void show_syscall(Syscall const &syscall)
  {
    auto function_code = std::array<std::uint8_t, g_regular_syscall_function_size>{0};
    std::memcpy(function_code.data(), syscall.address, g_regular_syscall_function_size);

    auto label = std::format("{:x}", syscall.number) + "-- " + syscall.name;
    ::ul::hexdump(label, function_code.data(), sizeof(function_code));
  }

  void show_syscalls()
  {
    ::ul::walk_syscalls([](::ul::Syscall const &syscall) -> ::ul::walk_t {
      show_syscall(std::move(syscall));
      return ::ul::walk_t::WALK_CONTINUE;
    });
  }

  auto with_syscall(std::string_view &&requested_name, std::function<void(::ul::Syscall const &)> callback) -> bool
  {
    auto found = false;
    ::ul::walk_syscalls([&](::ul::Syscall const &syscall) -> ::ul::walk_t {
      if (syscall.name != requested_name) {
        return ::ul::walk_t::WALK_CONTINUE;
      }

      callback(std::move(syscall));
      found = true;
      return ::ul::walk_t::WALK_STOP;
    });

    return found;
  }

  auto get_syscalls() -> Syscalls
  {
    auto syscalls = Syscalls{};
    ::ul::walk_syscalls([&](::ul::Syscall const &syscall) -> ::ul::walk_t {
      syscalls.emplace_back(syscall);
      return ::ul::walk_t::WALK_CONTINUE;
    });
    return syscalls;
  }

}  // namespace ul