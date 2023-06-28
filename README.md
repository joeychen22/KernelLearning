# KernelLearning
Here are some example code for who would like know the PEB(Process Environment Block) 

## Introduction
The way to loading PEB you will need to know the TEB(Thread Environment Block) structure first.[1]
PEB via Thread Environment Block (TEB) _TEB +0x030.[2]

On the the hands, ProcessEnvironmentBlock: Ptr32 _PEB Thread Information Block (TIB) _NT_TIB - first member of TEB +0x000 NtTib : _NT_TIB
fs:[30] + 0xC = PEB_LDR_Data, + 0x0C = InMemoryOrderModuleList of loaded modules.

## Code Usage
pebfinder.c will provide you current process PEB based address and module name.
```
session 1, PEB @ 000007fffffdf000
module: ntdll.dll                           BaseAddress: 19f000            size:     464e30 flags:     771cb2e0
module: kernel32.dll                        BaseAddress: 11f000            size:     4653b0 flags:       464f30
module: KERNELBASE.dll                      BaseAddress: 67000             size:     465520 flags:     771cb300
module: msvcrt.dll                          BaseAddress: 9f000             size:     466290 flags:     771cb2d0
module: ConEmuHk64.dll                      BaseAddress: 6d006500085000    size:     467430 flags:     771cb280
module: USER32.dll                          BaseAddress: 64006e000fb000    size:     467860 flags:       467540
module: GDI32.dll                           BaseAddress: 6f004300067000    size:     4678e0 flags:     771cb330
module: LPK.dll                             BaseAddress: 4a005c0000e000    size:     468770 flags:     771cb250
module: USP10.dll                           BaseAddress: cb000             size:     48b2a0 flags:       468630
module: IMM32.DLL                           BaseAddress: 2e000             size:     48c690 flags:     771cb240
module: MSCTF.dll                           BaseAddress: 10b000            size:     48c460 flags:     771cb2f0
module: api-ms-win-core-synch-l1-2-0.DLL    BaseAddress: 3000              size:     48d930 flags:     771cb1c0


ENTER exits...
```

antidebug.c will provide what will happened when calling IsDebuggerPresent. Using WinDbg to check the Kernel. 
```
0:000> u kernelBase!IsDebuggerPresent L3
KERNELBASE!IsDebuggerPresent:
00007ffb`ebf208d0 65488b042560000000 mov   rax,qword ptr gs:[60h] //64bit
00007ffb`ebf208d9 0fb64002        movzx   eax,byte ptr [rax+2]
00007ffb`ebf208dd c3  ret
```
```
0:000< u kernelbase!IsDebuggerPresent L3
KERNELBASE!IsDebuggerPresent:
751ca8d0 64a130000000    mov     eax,dword ptr fs:[00000030h]  //32bit
751ca8d6 0fb64002        movzx   eax,byte ptr [eax+2]
751ca8da c3              ret 
```

## Reference
[1]https://learn.microsoft.com/en-us/windows/win32/api/winternl/ns-winternl-teb
[2]https://stackoverflow.com/questions/14496730/mov-eax-large-fs30h# KernelLearning