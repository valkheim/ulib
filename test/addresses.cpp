#include <gtest/gtest.h>
#include "ulib.h"
#include "test.h"

TEST(addresses, test_rva_to_offset)
{
#if _WIN64
    auto base = (UINT_PTR)&::ul::test::dll_sample_injectee_x64;
#else
    auto base = (UINT_PTR)&::ul::test::dll_sample_injectee_x86;
#endif
    auto nt_header = (PIMAGE_NT_HEADERS)(base + ((PIMAGE_DOS_HEADER)base)->e_lfanew);

    auto export_dir_rva = (UINT_PTR)&nt_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
#if _WIN64
    ASSERT_EQ(((PIMAGE_DATA_DIRECTORY)export_dir_rva)->VirtualAddress, 0x2850);
#else
    ASSERT_EQ(((PIMAGE_DATA_DIRECTORY)export_dir_rva)->VirtualAddress, 0x2540);
#endif

    auto export_dir_offset = ::ul::rva_to_offset(((PIMAGE_DATA_DIRECTORY)export_dir_rva)->VirtualAddress, base);
#if _WIN64
    ASSERT_EQ(export_dir_offset, 0x1C50);
#else
    ASSERT_EQ(export_dir_offset, 0x1740);
#endif

    auto export_dir = base + export_dir_offset; 
    auto names_offset = ::ul::rva_to_offset(((PIMAGE_EXPORT_DIRECTORY)export_dir)->AddressOfNames, base);
#if _WIN64
    ASSERT_EQ(names_offset, 0x1C7C);
#else
    ASSERT_EQ(names_offset, 0x176C);
#endif

    auto first_export_name_rva = *(std::uint32_t *)(base + names_offset + 0 * sizeof(DWORD));
#if _WIN64
    ASSERT_EQ(first_export_name_rva, 0x0000289A);
#else
    ASSERT_EQ(first_export_name_rva, 0X0000258A);
#endif

    auto first_export_name_offset = ::ul::rva_to_offset(first_export_name_rva, base);
#if _WIN64
    ASSERT_EQ(first_export_name_offset, 0x0001C9A);
#else
    ASSERT_EQ(first_export_name_offset, 0x000178A);
#endif
    ASSERT_EQ(std::string{(char *)(base + first_export_name_offset)}, "example");

    auto addresses_offset = ::ul::rva_to_offset(((PIMAGE_EXPORT_DIRECTORY)export_dir)->AddressOfFunctions, base);
#if _WIN64
    ASSERT_EQ(addresses_offset, 0x1C78);
    ASSERT_EQ(*(std::uint32_t *)(base + addresses_offset), 0x0000112C); // address of example entrypoint
#else
    ASSERT_EQ(addresses_offset, 0x1768);
    ASSERT_EQ(*(std::uint32_t *)(base + addresses_offset), 0x000010FB); // address of example entrypoint
#endif


    auto ordinals_dir_offset = ::ul::rva_to_offset(((PIMAGE_EXPORT_DIRECTORY)export_dir)->AddressOfNameOrdinals, base);
#if _WIN64
    ASSERT_EQ(ordinals_dir_offset, 0x1C80);
#else
    ASSERT_EQ(ordinals_dir_offset, 0x1770);
#endif
    ASSERT_EQ(*(std::uint16_t *)(base + ordinals_dir_offset), 0x0000); // example ordinal
}

TEST(addresses, walk_exports)
{
#if _WIN64
    auto dll_path = R"(C:\Windows\system32\ntdll.dll)";
#else
    auto dll_path = R"(C:\Windows\SysWOW64\ntdll.dll)";
#endif
    auto dll_file = CreateFileA(dll_path, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    ASSERT_TRUE(dll_file != INVALID_HANDLE_VALUE);

    auto dll_size = GetFileSize(dll_file, NULL);
    ASSERT_TRUE(dll_size != INVALID_FILE_SIZE);
    ASSERT_TRUE(dll_size != 0);

    auto dll_content = HeapAlloc(GetProcessHeap(), 0, dll_size);
    ASSERT_TRUE(!!dll_content);

    DWORD read;
    auto ok = ReadFile(dll_file, dll_content, dll_size, &read, nullptr);
    ASSERT_TRUE(ok != FALSE);

    // manual resolution
    auto from_memory = std::vector<std::string>{};
    ASSERT_TRUE(::ul::walk_exports((UINT_PTR)dll_content, [&](::ul::Export const &xport) -> ::ul::walk_t {
        from_memory.emplace_back(std::move(xport.name));
        return ::ul::walk_t::WALK_CONTINUE;
    }));

    // already loaded dll
    auto from_disk = std::vector<std::string>{};
    ASSERT_TRUE(::ul::walk_exports(dll_path, [&](::ul::Export const &xport) -> ::ul::walk_t {
        from_disk.emplace_back(std::move(xport.name));
        return ::ul::walk_t::WALK_CONTINUE;
    }));

    ASSERT_FALSE(from_memory.empty());
    ASSERT_EQ(from_memory.size(), from_disk.size());
    for (unsigned i = 0 ; i < from_memory.size() ; ++i)
    {
        auto from_memory_name = from_memory[i];
        auto from_disk_name = from_disk[i];
        ASSERT_EQ(from_memory_name, from_disk_name);
    }

    HeapFree(GetProcessHeap(), 0, dll_content);
}

TEST(addresses, test_with_export_from_memory)
{
#if _WIN64
    auto base = (UINT_PTR)&::ul::test::dll_sample_injectee_x64;
#else
    auto base = (UINT_PTR)&::ul::test::dll_sample_injectee_x86;
#endif

    ::ul::Export got;
    ASSERT_TRUE(ul::with_export(base, "example", [&](::ul::Export const &xport) -> ::ul::walk_t {
        got = xport;
        return ::ul::walk_t::WALK_STOP;
    }));
    ASSERT_EQ(got.name, "example");
}

TEST(addresses, test_with_export_from_file)
{
#if _WIN64
    auto dll_path = R"(C:\Windows\system32\ntdll.dll)";
#else
    auto dll_path = R"(C:\Windows\SysWOW64\ntdll.dll)";
#endif

    ::ul::Export from_parsing;
    ASSERT_TRUE(ul::with_export(dll_path, "NtOpenFile", [&](::ul::Export const &xport) -> ::ul::walk_t {
        from_parsing = xport;
        return ::ul::walk_t::WALK_STOP;
    }));
    ASSERT_EQ(from_parsing.name, "NtOpenFile");
    auto from_api = ::ul::get_module_export(dll_path, "NtOpenFile");
    ASSERT_EQ(from_parsing.address, from_api);
}

TEST(addresses, test_backwards_to_base_image_address)
{
#if _WIN64
    auto base = (UINT_PTR)&::ul::test::dll_sample_injectee_x64;
#else
    auto base = (UINT_PTR)&::ul::test::dll_sample_injectee_x86;
#endif

    auto got = ::ul::backwards_to_base_image_address(base + 0x100);
    ASSERT_EQ(got, base);
}