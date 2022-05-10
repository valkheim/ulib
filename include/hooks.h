#pragma once

#include <Windows.h>

#include <vector>

#include "log.h"

namespace ul
{
  class IHook
  {
   public:
    virtual auto hook() -> bool = 0;
    virtual auto hooked() -> bool = 0;
    virtual auto unhook() -> bool = 0;
  };

  class Hook : public IHook
  {
   protected:
    std::size_t const hook_size;
    std::vector<std::uint8_t> hook_backup;
    std::vector<std::uint8_t> hook_content;
    FARPROC const hook_location;
    PROC const detour;

   public:
    Hook(std::size_t const hook_size, FARPROC const hook_location, PROC const detour);
    virtual auto hook() -> bool = 0;
    auto hooked() -> bool override;
    auto unhook() -> bool override;
  };

  namespace x86
  {
    class HookE9 : public Hook
    {
      /*
        E9          jmp +5
        XX XX XX XX
      */

     public:
      HookE9(FARPROC const hook_location, PROC detour);
      auto hook() -> bool override;
    };
  }  // namespace x86

  namespace x64
  {
    class HookFF25 : public Hook
    {
      /*
          FF 25 XX XX XX XX    is a will jmp at *(rip+XX XX XX XX)
          YY YY YY YY YY YY YY is a placeholder for the hook function
      */

     public:
      HookFF25(FARPROC const hook_location, PROC detour);
      auto hook() -> bool override;
    };

    class Hook48B8 : public Hook
    {
      /*
          48 B8 XX XX XX XX,XX XX XX XX is a mov rax imm64
          FF E0 is a jmp rax
      */

     public:
      Hook48B8(FARPROC const hook_location, PROC detour);
      auto hook() -> bool override;
    };

    class Hook68 : public Hook
    {
      /*
          68 XX XX XX XX          push 4 bytes low address
          C7 44 24 04 00 00 00 00 mov dword ptr[rsp+4], 4 bytes high address
          C3                      ret
      */
     public:
      Hook68(FARPROC const hook_location, PROC detour);
      auto hook() -> bool override;
    };
  }  // namespace x64
}  // namespace ul