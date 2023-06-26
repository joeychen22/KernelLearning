/*
* PEB loader/parser.
*
* This program uses low level OS functionality to examine its 
* own memory layout. Some structs were defined manually due
* to their absence in stanard windows header packages.
*/

/* Turn off some unhelpful warnings */
#pragma warning (disable: 4820 4668 4255)

/* Make all system calls that support it use unicode variants */
#define UNICODE
#define NDEBUG /* comment out for extra-checks */

#include <Windows.h>
#include <stdio.h>
#include <assert.h>

/* Hand-rolled structs ( may cause conflict depending on your dev env ) */

struct PROCESS_BASIC_INFORMATION {
   PVOID Reserved1;
   PVOID PebBaseAddress;
   PVOID Reserved2[2];
   ULONG_PTR UniqueProcessId;
   PVOID Reserved3;
};

struct PEB_LDR_DATA {
   BYTE       Reserved1[8];
   PVOID      Reserved2[3];
   LIST_ENTRY InMemoryOrderModuleList;
};

struct PEB {
   BYTE                          Reserved1[2];
   BYTE                          BeingDebugged;
   BYTE                          Reserved2[1];
   PVOID                         Reserved3[2];
   struct PEB_LDR_DATA*          Ldr;
   PVOID  ProcessParameters;     /* PRTL_USER_PROCESS_PARAMETERS */
   BYTE                          Reserved4[104];
   PVOID                         Reserved5[52];
   PVOID PostProcessInitRoutine; /* PPS_POST_PROCESS_INIT_ROUTINE */
   BYTE                          Reserved6[128];
   PVOID                         Reserved7[1];
   ULONG                         SessionId;
};

struct UNICODE_STRING {
   USHORT Length;
   USHORT MaximumLength;
   PWSTR  Buffer;
};

struct LDR_MODULE {
   LIST_ENTRY              InLoadOrderModuleList;
   LIST_ENTRY              InMemoryOrderModuleList;
   LIST_ENTRY              InInitializationOrderModuleList;
   PVOID                   BaseAddress;
   PVOID                   EntryPoint;
   ULONG                   SizeOfImage;
   struct UNICODE_STRING  FullDllName;
   struct UNICODE_STRING  BaseDllName;
   ULONG                   Flags;
   SHORT                   LoadCount;
   SHORT                   TlsIndex;
   LIST_ENTRY              HashTableEntry;
   ULONG                   TimeDateStamp;
};


enum PROCESSINFOCLASS {
   ProcessBasicInformation = 0,
   ProcessDebugPort = 7,
   ProcessWow64Information = 26,
   ProcessImageFileName = 27
};

LPCWSTR NTDLL_NAME = L"ntdll.dll";
LPCSTR NTQUERYINFO_NAME = "NtQueryInformationProcess";

/* Entry point */

int main() {
   /* stack-locals */
   HMODULE ntdllMod;
   HANDLE thisProcess; 
   NTSTATUS ntCallRet;
   ULONG bytesReturned;
   PLIST_ENTRY li;

   /* function pointer to house result from GetProcAddress */
   NTSTATUS  (WINAPI * queryInfoProcPtr)
      (HANDLE, enum PROCESSINFOCLASS, PVOID, ULONG, PULONG);

   struct PROCESS_BASIC_INFORMATION basicInfo;
   struct PEB* pebPtr;
   struct LDR_MODULE *modPtr;

   /* retrieve pseudo-handle */
   thisProcess = GetCurrentProcess();

   /* get address to already loaded module */
   ntdllMod = LoadLibrary(NTDLL_NAME);

   /* get pointer to query function */
   queryInfoProcPtr = 
      (NTSTATUS (WINAPI *)(
      HANDLE, 
      enum PROCESSINFOCLASS, 
      PVOID, 
      ULONG, 
      PULONG)) 
      GetProcAddress(ntdllMod, NTQUERYINFO_NAME);

   assert(queryInfoProcPtr != NULL);

   /* call function on self; introspect. */
   ntCallRet = queryInfoProcPtr(
      thisProcess, 
      ProcessBasicInformation, 
      &basicInfo, 
      sizeof(basicInfo), 
      &bytesReturned);

   assert(ntCallRet == 0 && bytesReturned == sizeof(basicInfo));

   /* get peb ptr and decode some if its fields */
   pebPtr = (struct PEB*) basicInfo.PebBaseAddress;
   wprintf(L"session %x, PEB @ %p\n", pebPtr->SessionId, pebPtr);

	for(li  = pebPtr->Ldr->InMemoryOrderModuleList.Flink->Flink;
        li != pebPtr->Ldr->InMemoryOrderModuleList.Flink; 
        li  = li->Flink) {
         modPtr = (struct LDR_MODULE*) li;
    
         if (modPtr->SizeOfImage != 0){
		 wprintf(L"module: %-35S BaseAddress: %-17p size: %10x flags: %12x\n",
            modPtr->FullDllName.Buffer, 
            modPtr->BaseAddress,
            modPtr->SizeOfImage,
            modPtr->Flags);
		 }
		 else
		 {
		    /* hang the terminal and wait for a flush */
            wprintf(L"\n\nENTER exits...\n");
            getc(stdin);
            return 0; 
		 }
	  }
}