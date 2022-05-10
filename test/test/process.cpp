#include "test.h"


namespace ul::test
{
    void with_process(std::string_view&& command_line, std::function<void(PROCESS_INFORMATION const *)> callback)
    {
        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));
        CreateProcess(NULL, (LPSTR)command_line.data(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
        callback(&pi);
        TerminateProcess(pi.hProcess, 0);
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
}