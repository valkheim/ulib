#pragma once

#include <Windows.h>

#define STATUS_INFO_LENGTH_MISMATCH 0xc0000004

typedef LONG KPRIORITY;

typedef struct _CLIENT_ID {
  DWORD UniqueProcess;
  DWORD UniqueThread;
} CLIENT_ID, *PCLIENT_ID;

typedef enum _THREAD_STATE {
  StateInitialized,
  StateReady,
  StateRunning,
  StateStandby,
  StateTerminated,
  StateWait,
  StateTransition,
  StateUnknown
} THREAD_STATE;

typedef struct _SYSTEM_THREAD {
  LARGE_INTEGER KernelTime;
  LARGE_INTEGER UserTime;
  LARGE_INTEGER CreateTime;
  ULONG WaitTime;
  PVOID StartAddress;
  CLIENT_ID ClientId;
  KPRIORITY Priority;
  KPRIORITY BasePriority;
  ULONG ContextSwitchCount;
  THREAD_STATE State;
  LONG WaitReason;
} SYSTEM_THREAD, *PSYSTEM_THREAD;

typedef struct _UNICODE_STRING {
  USHORT Length;
  USHORT MaximumLength;
  PWSTR Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

typedef struct _VM_COUNTERS {
#ifdef _WIN64
  SIZE_T PeakVirtualSize;
  SIZE_T PageFaultCount;
  SIZE_T PeakWorkingSetSize;
  SIZE_T WorkingSetSize;
  SIZE_T QuotaPeakPagedPoolUsage;
  SIZE_T QuotaPagedPoolUsage;
  SIZE_T QuotaPeakNonPagedPoolUsage;
  SIZE_T QuotaNonPagedPoolUsage;
  SIZE_T PagefileUsage;
  SIZE_T PeakPagefileUsage;
  SIZE_T VirtualSize;
#else
  SIZE_T PeakVirtualSize;
  SIZE_T VirtualSize;
  ULONG PageFaultCount;
  SIZE_T PeakWorkingSetSize;
  SIZE_T WorkingSetSize;
  SIZE_T QuotaPeakPagedPoolUsage;
  SIZE_T QuotaPagedPoolUsage;
  SIZE_T QuotaPeakNonPagedPoolUsage;
  SIZE_T QuotaNonPagedPoolUsage;
  SIZE_T PagefileUsage;
  SIZE_T PeakPagefileUsage;
#endif
} VM_COUNTERS;

typedef struct _SYSTEM_PROCESS_INFORMATION {
  ULONG NextEntryDelta;
  ULONG ThreadCount;
  ULONG Reserved1[6];
  LARGE_INTEGER CreateTime;
  LARGE_INTEGER UserTime;
  LARGE_INTEGER KernelTime;
  UNICODE_STRING ProcessName;
  KPRIORITY BasePriority;
  ULONG ProcessId;
  ULONG InheritedFromProcessId;
  ULONG HandleCount;
  ULONG Reserved2[2];
  VM_COUNTERS VmCounters;
#if _WIN32_WINNT >= 0x500
  IO_COUNTERS IoCounters;
#endif
  SYSTEM_THREAD Threads[ANYSIZE_ARRAY];
} SYSTEM_PROCESS_INFORMATION, *PSYSTEM_PROCESS_INFORMATION;

typedef struct _SYSTEM_MODULE {
  ULONG Reserved1;
  ULONG Reserved2;
  ULONG Reserved3;
  PVOID ImageBaseAddress;
  ULONG ImageSize;
  ULONG Flags;
  WORD Id;
  WORD Rank;
  WORD LoadCount;
  WORD NameOffset;
  CHAR Name[MAX_PATH - 4];
} SYSTEM_MODULE, *PSYSTEM_MODULE;

typedef struct _SYSTEM_MODULE_INFORMATION {
  ULONG ModulesCount;
  SYSTEM_MODULE Modules[ANYSIZE_ARRAY];
} SYSTEM_MODULE_INFORMATION, *PSYSTEM_MODULE_INFORMATION;

typedef enum _SYSTEM_INFORMATION_CLASS { SystemProcessAndThreadInformation = 0x05, SystemModuleInformation = 0x0b } SYSTEM_INFORMATION_CLASS;

typedef struct _UNICODE_STR {
  USHORT Length;
  USHORT MaximumLength;
  PWSTR pBuffer;
} UNICODE_STR, *PUNICODE_STR;

// WinDbg> dt -v ntdll!_LDR_DATA_TABLE_ENTRY
//__declspec( align(8) )
typedef struct _LDR_DATA_TABLE_ENTRY {
  // LIST_ENTRY InLoadOrderLinks; // As we search from PPEB_LDR_DATA->InMemoryOrderModuleList we dont use the first entry.
  LIST_ENTRY InMemoryOrderModuleList;
  LIST_ENTRY InInitializationOrderModuleList;
  PVOID DllBase;
  PVOID EntryPoint;
  ULONG SizeOfImage;
  UNICODE_STR FullDllName;
  UNICODE_STR BaseDllName;
  ULONG Flags;
  SHORT LoadCount;
  SHORT TlsIndex;
  LIST_ENTRY HashTableEntry;
  ULONG TimeDateStamp;
} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;

// WinDbg> dt -v ntdll!_PEB_LDR_DATA
typedef struct _PEB_LDR_DATA  //, 7 elements, 0x28 bytes
{
  DWORD dwLength;
  DWORD dwInitialized;
  LPVOID lpSsHandle;
  LIST_ENTRY InLoadOrderModuleList;
  LIST_ENTRY InMemoryOrderModuleList;
  LIST_ENTRY InInitializationOrderModuleList;
  LPVOID lpEntryInProgress;
} PEB_LDR_DATA, *PPEB_LDR_DATA;

// WinDbg> dt -v ntdll!_PEB_FREE_BLOCK
typedef struct _PEB_FREE_BLOCK  // 2 elements, 0x8 bytes
{
  struct _PEB_FREE_BLOCK* pNext;
  DWORD dwSize;
} PEB_FREE_BLOCK, *PPEB_FREE_BLOCK;

// struct _PEB is defined in Winternl.h but it is incomplete
// WinDbg> dt -v ntdll!_PEB
typedef struct __PEB  // 65 elements, 0x210 bytes
{
  BYTE bInheritedAddressSpace;
  BYTE bReadImageFileExecOptions;
  BYTE bBeingDebugged;
  BYTE bSpareBool;
  LPVOID lpMutant;
  LPVOID lpImageBaseAddress;
  PPEB_LDR_DATA pLdr;
  LPVOID lpProcessParameters;
  LPVOID lpSubSystemData;
  LPVOID lpProcessHeap;
  PRTL_CRITICAL_SECTION pFastPebLock;
  LPVOID lpFastPebLockRoutine;
  LPVOID lpFastPebUnlockRoutine;
  DWORD dwEnvironmentUpdateCount;
  LPVOID lpKernelCallbackTable;
  DWORD dwSystemReserved;
  DWORD dwAtlThunkSListPtr32;
  PPEB_FREE_BLOCK pFreeList;
  DWORD dwTlsExpansionCounter;
  LPVOID lpTlsBitmap;
  DWORD dwTlsBitmapBits[2];
  LPVOID lpReadOnlySharedMemoryBase;
  LPVOID lpReadOnlySharedMemoryHeap;
  LPVOID lpReadOnlyStaticServerData;
  LPVOID lpAnsiCodePageData;
  LPVOID lpOemCodePageData;
  LPVOID lpUnicodeCaseTableData;
  DWORD dwNumberOfProcessors;
  DWORD dwNtGlobalFlag;
  LARGE_INTEGER liCriticalSectionTimeout;
  DWORD dwHeapSegmentReserve;
  DWORD dwHeapSegmentCommit;
  DWORD dwHeapDeCommitTotalFreeThreshold;
  DWORD dwHeapDeCommitFreeBlockThreshold;
  DWORD dwNumberOfHeaps;
  DWORD dwMaximumNumberOfHeaps;
  LPVOID lpProcessHeaps;
  LPVOID lpGdiSharedHandleTable;
  LPVOID lpProcessStarterHelper;
  DWORD dwGdiDCAttributeList;
  LPVOID lpLoaderLock;
  DWORD dwOSMajorVersion;
  DWORD dwOSMinorVersion;
  WORD wOSBuildNumber;
  WORD wOSCSDVersion;
  DWORD dwOSPlatformId;
  DWORD dwImageSubsystem;
  DWORD dwImageSubsystemMajorVersion;
  DWORD dwImageSubsystemMinorVersion;
  DWORD dwImageProcessAffinityMask;
  DWORD dwGdiHandleBuffer[34];
  LPVOID lpPostProcessInitRoutine;
  LPVOID lpTlsExpansionBitmap;
  DWORD dwTlsExpansionBitmapBits[32];
  DWORD dwSessionId;
  ULARGE_INTEGER liAppCompatFlags;
  ULARGE_INTEGER liAppCompatFlagsUser;
  LPVOID lppShimData;
  LPVOID lpAppCompatInfo;
  UNICODE_STR usCSDVersion;
  LPVOID lpActivationContextData;
  LPVOID lpProcessAssemblyStorageMap;
  LPVOID lpSystemDefaultActivationContextData;
  LPVOID lpSystemAssemblyStorageMap;
  DWORD dwMinimumStackCommit;
} _PEB, *_PPEB;

typedef struct {
  WORD offset : 12;
  WORD type : 4;
} IMAGE_RELOC, *PIMAGE_RELOC;

typedef struct _OBJECT_ATTRIBUTES {
  ULONG Length;
  HANDLE RootDirectory;
  PUNICODE_STRING ObjectName;
  ULONG Attributes;
  PVOID SecurityDescriptor;
  PVOID SecurityQualityOfService;
} OBJECT_ATTRIBUTES, *POBJECT_ATTRIBUTES;

using _NtCreateSection = NTSTATUS(NTAPI*)(OUT PHANDLE SectionHandle, IN ULONG DesiredAccess, IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
                                          IN PLARGE_INTEGER MaximumSize OPTIONAL, IN ULONG PageAttributess, IN ULONG SectionAttributes,
                                          IN HANDLE FileHandle OPTIONAL);
using _NtMapViewOfSection = NTSTATUS(NTAPI*)(HANDLE SectionHandle, HANDLE ProcessHandle, PVOID* BaseAddress, ULONG_PTR ZeroBits, SIZE_T CommitSize,
                                             PLARGE_INTEGER SectionOffset, PSIZE_T ViewSize, DWORD InheritDisposition, ULONG AllocationType,
                                             ULONG Win32Protect);
using _RtlCreateUserThread = NTSTATUS(NTAPI*)(IN HANDLE ProcessHandle, IN PSECURITY_DESCRIPTOR SecurityDescriptor OPTIONAL,
                                              IN BOOLEAN CreateSuspended, IN ULONG StackZeroBits, IN OUT PULONG StackReserved,
                                              IN OUT PULONG StackCommit, IN PVOID StartAddress, IN PVOID StartParameter OPTIONAL,
                                              OUT PHANDLE ThreadHandle, OUT PCLIENT_ID ClientID);
using _NtQuerySystemInformation = NTSTATUS(WINAPI*)(__in SYSTEM_INFORMATION_CLASS SystemInformationClass, __inout PVOID SystemInformation,
                                                    __in ULONG SystemInformationLength, __out_opt PULONG ReturnLength);