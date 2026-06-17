#include <windows.h>
#include <iostream>

/*

First refer the coursework for this program

Compilation instructions :
x86_64-w64-mingw32-g++ token_tracker.cpp -o token_tracker.exe -O0 -g -static

*/

int main() {
    std::cout << "[*] Chapter 7: Windows Security Model (Access Tokens)" << std::endl;

    HANDLE hToken;
    // 1. Open the Access Token of the current process
    // We need TOKEN_ADJUST_PRIVILEGES to enable them, and TOKEN_QUERY to read them.
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
        std::cout << "[-] OpenProcessToken failed. Error: " << GetLastError() << std::endl;
        return 1;
    }

    std::cout << "[+] Successfully opened Process Token." << std::endl;

    // 2. Lookup the LUID (Locally Unique Identifier) for SeDebugPrivilege
    // This privilege is required to debug or access other processes' memory.
    LUID luid;
    if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid)) {
        std::cout << "[-] LookupPrivilegeValue failed. Error: " << GetLastError() << std::endl;
        CloseHandle(hToken);
        return 1;
    }

    // 3. Prepare to enable the privilege
    TOKEN_PRIVILEGES tp;
    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; // This tells the OS we want it ON

    std::cout << "[*] Attempting to enable SeDebugPrivilege..." << std::endl;
    
    // 4. Adjust the token
    if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL)) {
        std::cout << "[-] AdjustTokenPrivileges failed. Error: " << GetLastError() << std::endl;
    } else {
        // Even if the API succeeds, we must check GetLastError() to see if the privilege was actually assigned
        if (GetLastError() == ERROR_NOT_ALL_ASSIGNED) {
            std::cout << "[-] Failed to enable privilege (Do you have Administrator rights?)." << std::endl;
        } else {
            std::cout << "[+] SeDebugPrivilege successfully enabled!" << std::endl;
        }
    }

    std::cout << "\n[*] PAUSED. Open Process Explorer, find this process, right-click -> Properties -> Security." << std::endl;
    std::cout << "[*] Observe the privileges list. You should see SeDebugPrivilege marked as 'Enabled'." << std::endl;
    std::cout << "[*] Press Enter to exit." << std::endl;
    std::cin.get();

    CloseHandle(hToken);
    return 0;
}