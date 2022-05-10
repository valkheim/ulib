#include "log.h"

#include <iostream>

namespace ul
{
  auto error(const std::string_view& message) -> DWORD
  {
    auto error_code = GetLastError();
    auto error_desc = std::system_category().message(error_code);
    std::cerr << "[x] " << message << " (0x" << std::hex << error_code << ": " << error_desc << ")" << std::endl;
    return error_code;
  }

  void warning(const std::string_view& message)
  {
    // std::cout << "[!] " << message << std::endl;
  }

  void info(const std::string_view& message) { std::cout << "[+] " << message << std::endl; }

  VOID hexdump(PVOID const buffer, SIZE_T const buffer_size)
  {
    constexpr auto width = 0x10;
    auto walk = (PBYTE)buffer;
    for (SIZE_T i = 0; i < buffer_size; i += width) {
#if _WIN64
      printf(" | %08llx: ", i);
#else
      printf(" | %08lx: ", i);
#endif
      for (SIZE_T j = 0; j < width; j++)
        if (i + j < buffer_size)
          printf("%02x ", walk[i + j]);
        else
          printf("   ");

      printf(" | ");
      for (SIZE_T j = 0; j < width; j++)
        if (i + j < buffer_size)
          printf("%c", isprint(walk[i + j]) ? walk[i + j] : '.');
        else
          printf(" ");

      printf(" |\n");
    }
  }

  VOID hexdump(std::string_view&& caption, PVOID const buffer, SIZE_T const buffer_size)
  {
    printf(" ,---------: %s\n", caption.data());
    hexdump(buffer, buffer_size);
    printf(" `---\n");
  }
}  // namespace ul