#include "addresses.h"

#include <intrin.h>
#include <windows.h>

#include <cstdint>
#include <format>

#include "log.h"

namespace ul
{
  auto rva_to_offset(std::uint32_t const rva, std::ptrdiff_t const base) -> DWORD
  {
    auto nt_header = (PIMAGE_NT_HEADERS)(base + ((PIMAGE_DOS_HEADER)base)->e_lfanew);
    auto section_header = (PIMAGE_SECTION_HEADER)((UINT_PTR)(&nt_header->OptionalHeader) + nt_header->FileHeader.SizeOfOptionalHeader);

    if (rva < section_header[0].PointerToRawData) return rva;

    for (auto i = 0; i < nt_header->FileHeader.NumberOfSections; i++) {
      if (rva >= section_header[i].VirtualAddress && rva < (section_header[i].VirtualAddress + section_header[i].SizeOfRawData)) {
        return (rva - section_header[i].VirtualAddress + section_header[i].PointerToRawData);
      }
    }

    return 0;
  }

  auto get_module_export(std::string_view&& module_name, std::string_view&& procedure_name) -> PVOID
  {
    auto handle = GetModuleHandleA(module_name.data());
    if (handle == nullptr) {
      ::ul::error("Cannot GetModuleHandleA");
      return nullptr;
    }

    auto procedure = GetProcAddress(handle, procedure_name.data());
    if (procedure == nullptr) {
      ::ul::error(std::format("Cannot GetProcAddress for {}", procedure_name));
    }

    return procedure;
  }

  auto walk_exports(std::ptrdiff_t const base, on_export callback) -> bool
  {
    if (((PIMAGE_DOS_HEADER)base)->e_magic != IMAGE_DOS_SIGNATURE) {
      ::ul::error("DOS header not found");
      return false;
    }

    auto nt_header = base + ((PIMAGE_DOS_HEADER)base)->e_lfanew;
    if (((PIMAGE_NT_HEADERS)nt_header)->Signature != IMAGE_NT_SIGNATURE) {
      ::ul::error("NT header not found");
      return false;
    }

    auto export_dir = (std::ptrdiff_t) & ((PIMAGE_NT_HEADERS)nt_header)->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
    export_dir = base + ::ul::rva_to_offset(((PIMAGE_DATA_DIRECTORY)export_dir)->VirtualAddress, base);
    auto names = base + ::ul::rva_to_offset(((PIMAGE_EXPORT_DIRECTORY)export_dir)->AddressOfNames, base);
    auto addresses = base + ::ul::rva_to_offset(((PIMAGE_EXPORT_DIRECTORY)export_dir)->AddressOfFunctions, base);
    auto ordinals = base + ::ul::rva_to_offset(((PIMAGE_EXPORT_DIRECTORY)export_dir)->AddressOfNameOrdinals, base);
    for (DWORD i = 0; i < ((PIMAGE_EXPORT_DIRECTORY)export_dir)->NumberOfFunctions; ++i) {
      addresses = base + ::ul::rva_to_offset(((PIMAGE_EXPORT_DIRECTORY)export_dir)->AddressOfFunctions, base);
      auto ordinal = *(std::uint16_t*)(ordinals);
      addresses += (ordinal * sizeof(std::uint32_t));
      auto offset = (std::ptrdiff_t)::ul::rva_to_offset(*(std::uint32_t*)(addresses), base);
      auto address = (PVOID)(base + offset);
      auto name = std::string{(char*)(base + ::ul::rva_to_offset(*(std::uint32_t*)(names), base))};
      if (callback(::ul::Export{ordinal, offset, address, name}) == ::ul::walk_t::WALK_STOP) break;

      names += sizeof(std::uint32_t);
      ordinals += sizeof(std::uint16_t);
    }

    return true;
  }

  auto walk_exports(std::string_view&& path, on_export callback) -> bool
  {
    auto base = (std::ptrdiff_t)GetModuleHandle(path.data());
    if (!base) {
      ::ul::error("Cannot get module handle");
      return false;
    }

    if (((PIMAGE_DOS_HEADER)base)->e_magic != IMAGE_DOS_SIGNATURE) {
      ::ul::error("DOS header not found");
      return false;
    }

    auto nt_header = base + ((PIMAGE_DOS_HEADER)base)->e_lfanew;
    if (((PIMAGE_NT_HEADERS)nt_header)->Signature != IMAGE_NT_SIGNATURE) {
      ::ul::error("NT header not found");
      return false;
    }

    // Optional header is a PIMAGE_OPTIONAL_HEADER32 or a PIMAGE_OPTIONAL_HEADER64
    auto export_dir = base + ((PIMAGE_NT_HEADERS)nt_header)->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
    auto functions = base + ((PIMAGE_EXPORT_DIRECTORY)export_dir)->AddressOfFunctions;
    auto names = base + ((PIMAGE_EXPORT_DIRECTORY)export_dir)->AddressOfNames;
    auto ordinals = base + ((PIMAGE_EXPORT_DIRECTORY)export_dir)->AddressOfNameOrdinals;
    for (DWORD i = 0; i < ((PIMAGE_EXPORT_DIRECTORY)export_dir)->NumberOfFunctions; ++i) {
      auto ordinal = ((std::uint16_t*)ordinals)[i];
      auto offset = (std::ptrdiff_t)((std::uint32_t*)functions)[ordinal];
      auto address = ((PVOID)(base + offset));
      auto name = std::string{((char*)(base) + ((std::uint32_t*)names)[i])};
      if (!address) {
        ::ul::warning("Cannot find exported address. Continue");
        break;
      }

      if (name.empty()) {
        ::ul::warning("Cannot find exported name. Continue");
        break;
      }

      if (callback(::ul::Export{ordinal, offset, address, name}) == ::ul::walk_t::WALK_STOP) break;
    }

    return true;
  }

  auto with_export(std::ptrdiff_t const base, std::string_view&& export_name, on_export callback) -> bool
  {
    auto found = false;
    auto can_walk = ::ul::walk_exports(base, [&](::ul::Export const& xport) {
      if (xport.name != export_name) return ::ul::walk_t::WALK_CONTINUE;

      callback(std::move(xport));
      found = true;
      return ::ul::walk_t::WALK_STOP;
    });

    return can_walk && found;
  }

  auto with_export(std::string_view&& path, std::string_view&& export_name, on_export callback) -> bool
  {
    auto found = false;
    auto can_walk = ::ul::walk_exports(std::move(path), [&](::ul::Export const& xport) {
      if (xport.name != export_name) return ::ul::walk_t::WALK_CONTINUE;

      callback(std::move(xport));
      found = true;
      return ::ul::walk_t::WALK_STOP;
    });

    return can_walk && found;
  }

  auto find_by_hash() -> void {}

  auto backwards_to_base_image_address(std::ptrdiff_t from) -> std::ptrdiff_t
  {
    std::ptrdiff_t header = 0;

    for (;;) {
      if (((PIMAGE_DOS_HEADER)from)->e_magic == IMAGE_DOS_SIGNATURE) {
        header = ((PIMAGE_DOS_HEADER)from)->e_lfanew;
        if (header >= sizeof(IMAGE_DOS_HEADER) && header < 0x400) {
          header += from;
          if (((PIMAGE_NT_HEADERS)header)->Signature == IMAGE_NT_SIGNATURE) break;
        }
      }

      from--;
    }

    return from;
  }

  auto get_peb() -> std::ptrdiff_t  // __PPEB from nt.h
  {
#ifdef _WIN64
    return __readgsqword(0x60);
#else
    return __readfsdword(0x30);
#endif
  }
}  // namespace ul