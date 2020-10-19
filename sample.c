/* https://ja.coder.work/so/c%2B%2B/866182 */
/* https://ja.coder.work/so/c%2B%2B/731262 */
#include <windows.h>
#include <stdio.h>

//unsigned long sz = sizeof(unsigned char)*1024; // 1KB
//unsigned long sz = sizeof(unsigned char)*1024*1024; // 1MB
unsigned long sz = sizeof(unsigned char)*1024*1024*1024; // 1GB

static const char map_name[] = "FileMapping1";
static const char event1_name[] = "EventName1";
static const char event2_name[] = "EventName2";

int main() { 
    HANDLE mapping = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, map_name);


    DWORD dwAccess = (PROCESS_QUERY_INFORMATION | PROCESS_SET_QUOTA);
    DWORD dwProcessID = GetCurrentProcessId();
    HANDLE hProcess;

    if ( (hProcess = OpenProcess(dwAccess,FALSE,dwProcessID)) != NULL ){
      //unsigned long szex = sz; // OK@1KB
      //unsigned long szex = sz+(sz>>5); // OK@1MB
      //unsigned long szex = sz+(sz>>6); // NG@1MB
      //unsigned long szex = sz+(sz>>15); // OK@1GB
      //unsigned long szex = sz+(sz>>16); // NG@1GB
      unsigned long szex = sz+32*1024; // OK@1KB/1MB/1GB
      printf("[sz,(sz>>N)]=[%u,%u]\n",sz,(szex-sz));
      if(!SetProcessWorkingSetSize(hProcess, szex, szex))// ok
        printf("Error Changing Working Set Size\n");
      CloseHandle(hProcess);
    }


    if (NULL == mapping) {
        printf("Calling CreateFile\n");
        HANDLE file = (HANDLE*) -1; 
        mapping = CreateFileMapping(file, NULL, PAGE_READWRITE, 0, sz, map_name);

        printf("Closing file handle\n");
        CloseHandle(file);
    }

    printf("Mapping view of file\n");
    char *memory = (char *)MapViewOfFile(mapping, FILE_MAP_ALL_ACCESS, 0, 0, sz);
    if (memory == NULL) {
        printf("Mapping Failed.\n");
        return 1;
    }
    printf("Mapping succeeded\n");

    // Protect memory from being swapped
    if(!VirtualLock(memory , sz)) {
      printf("Memory swap protection failed\n");
      return 2;
    }           
    printf("VirtualLock succeeded\n");

    HANDLE event = CreateEvent(NULL, FALSE, FALSE, event1_name);

    if (GetLastError()==ERROR_ALREADY_EXISTS) {
        printf("Waiting to receive string:\n");
        WaitForSingleObject(event, INFINITE);
        printf("Received: %s",memory);
        HANDLE event2 = CreateEvent(NULL, FALSE, FALSE, event2_name);
        SetEvent(event2);
    }
    else {
        char string[] = "This is the shared string";
        printf("Sending string: %s\n",string);
        strncpy(memory, string, sizeof(string));
        SetEvent(event);
        HANDLE event2 = CreateEvent(NULL, FALSE, FALSE, event2_name);
        WaitForSingleObject(event2, INFINITE);
    }   
    return 0;
}
