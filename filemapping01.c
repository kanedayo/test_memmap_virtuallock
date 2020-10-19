/* filemapping01.c */
/* http://www.kumei.ne.jp/c_lang/intro2/no_114.htm */

#include <stdio.h>
#include <windows.h>
#include <conio.h>

int FRead(LPSTR);
int FWrite(LPSTR);

HANDLE hFMWrite;


int main()
{
    
    static LPSTR lpAddress1, lpAddress2;

    char szNo[8];
    BOOL bEnd = FALSE;

    while (1) {
        printf("***************\n");
        printf("1. Writre\n");
        printf("2. Read\n");
        printf("0. End\n");
        printf("***************\n");
        printf("No. = ");
        gets(szNo);

        switch (szNo[0]) {
            case '1':
                FWrite(lpAddress1);
                break;
            case '2':
                FRead(lpAddress2);
                break;
            case '0':
                bEnd = TRUE;
                break;
            default:
                printf("Wrong Number !\n\n");
                break;
        }
        if (bEnd)
            break;
    }
    

    if (hFMWrite) {
        if (CloseHandle(hFMWrite) == 0)
            printf("ERROR wirte-handle close\n");
        else
            printf("Success wirte-handle close\n");
    }

    return 0;
}

int FWrite(LPSTR lpStr)
{
    char szStr[1024];

    if (!hFMWrite)
        CloseHandle(hFMWrite);
    
    
    hFMWrite = CreateFileMapping(
       (HANDLE)-1, 
       NULL,
       PAGE_READWRITE,
       0, 
       1024,
       "NEKODEMOWAKARU");
    if (hFMWrite == NULL)
        return -1;
    if (GetLastError() == ERROR_ALREADY_EXISTS)
        printf("Already exist Mappingfile\n");

    lpStr = (LPSTR)MapViewOfFile(hFMWrite,
       FILE_MAP_ALL_ACCESS, 0, 0, 0);
    if (lpStr == NULL)
        return -2;

    printf("Write==");
    gets(szStr);
    strcpy(lpStr, szStr);
    strcat(lpStr, "\n");

    UnmapViewOfFile(lpStr);
    
    printf("Write Done\n");
    return 0;
}

int FRead(LPSTR lpStr)
{
    HANDLE hFM;

    hFM = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, "NEKODEMOWAKARU");
        
    lpStr = (LPSTR)MapViewOfFile(hFM,
            FILE_MAP_ALL_ACCESS, 0, 0, 0);

    if (lpStr == NULL) {
        printf("ERROR Rx\n");
        CloseHandle(hFM);
        return -1;
    }
    printf(lpStr);
    if (UnmapViewOfFile(lpStr) == 0) {
        printf("ERROR: Address Mapping for Write\n");
    } else {
        printf("SUCCESS: Address Mapping for Write\n");
        lpStr = NULL;
    }

    CloseHandle(hFM);

    return 0;
}
