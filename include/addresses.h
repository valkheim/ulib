#pragma once

#include <Windows.h>

#include <functional>
#include <string>
#include <string_view>
#include <vector>

#include "utils.h"

namespace ul
{
  struct Export {
    std::uint16_t ordinal;
    std::ptrdiff_t offset;
    void* address;
    std::string name;
  };

  using Exports = std::vector<::ul::Export>;
  using on_export = std::function<::ul::walk_t(::ul::Export const&)>;

  auto rva_to_offset(std::uint32_t const rva, std::ptrdiff_t const base) -> DWORD;
  auto get_module_export(std::string_view&& module_name, std::string_view&& procedure_name) -> PVOID;
  [[nodiscard]] auto walk_exports(std::ptrdiff_t const base, on_export callback) -> bool;
  [[nodiscard]] auto walk_exports(std::string_view&& path, on_export callback) -> bool;
  [[nodiscard]] auto with_export(std::ptrdiff_t const base, std::string_view&& export_name, on_export callback) -> bool;
  [[nodiscard]] auto with_export(std::string_view&& path, std::string_view&& export_name, on_export callback) -> bool;
  auto find_by_hash() -> void;
  auto backwards_to_base_image_address(std::ptrdiff_t uiLibraryAddress) -> std::ptrdiff_t;
  auto get_peb() -> std::ptrdiff_t;
}  // namespace ul