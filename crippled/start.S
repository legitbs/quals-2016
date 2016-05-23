format elf

section '.cs' executable
extrn main

public _start
_start:
	mov ebp, esp

	;open up our file
	mov eax, 5
	mov ebx, Filename
	xor ecx, ecx
	xor edx, edx
	int 0x80
	
	;need to setup a kill timer
	mov eax, 259
	mov ebx, 1	;CLOCK_REALTIME

	;setup 64 bytes of stack to be 0
	mov ecx, 16
pushloop:
	push 0
	loop pushloop

	;setup the sigevent structure to send sigkill when timer fires
	mov ecx, esp
	mov [ecx+4], dword 9

	;point to the stack for the return value
	push 0
	mov edx, esp
	int 0x80

	;start the timer
	mov eax, 260
	mov ebx, [esp]
	xor ecx, ecx
	push 0
	push 10		;10 second max execution
	push 0
	push 0
	mov edx, esp
	xor esi, esi
	int 0x80

	;make sure no core
	;setrlimit(RLIMIT_CORE, [0, 0]);
	push 0
	push 0
	mov eax, 75
	mov ebx, 4
	mov ecx, esp
	int 0x80

	;limit to 2k of output data, the crippled loader has a 1k limit on return
	;setrlimit(RLIMIT_FSIZE, [2048, 2048]);
	push 2048
	push 2048
	mov eax, 75
	mov ebx, 1
	mov ecx, esp
	int 0x80

	;only allow read, write, _exit, and sigreturn
	;prctl(PR_SET_SECCOMP, SECCOMP_SET_MODE_STRICT, 0, 0, 0);
	mov eax, 172
	mov ebx, 22
	mov ecx, 1
	xor edx, edx
	xor esi, esi
	xor edi, edi
	int 0x80

	;reset the stack position and call main
	;call main
	mov esp, ebp
	xor ebp, ebp
	push 0
	push 0
	call main

	;exit
	mov eax, 0x1
	int 0x80

public write
write:
	mov eax, 4
	mov ebx, [esp+4]
	mov ecx, [esp+8]
	mov edx, [esp+12]
	int 0x80
	ret

section '.ds'
Filename:
db "/home/crippled/flag", 0
