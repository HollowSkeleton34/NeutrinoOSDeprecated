section .boot

global __default_drive
__default_drive: db 0

global __num_sectors
__num_sectors: dd 27

bits 16
global boot
boot:
	mov ax, 0x2401
	int 0x15

	mov ax, 0x3
	int 0x10

.load_kernel:
	mov [__default_drive],dl

	mov ah, 0x2    ;read sectors
	mov al, [__num_sectors]      ;sectors to read
	mov ch, 0      ;cylinder idx
	mov dh, 0      ;head idx
	mov cl, 2      ;sector idx
	mov dl, [__default_drive] ;disk idx
	mov bx, target ;target pointer
	int 0x13

.read_drives:
	mov eax, 0
	mov [__drive_count], eax

	mov eax, __drive_data
	mov [__temppointer], eax
	mov dl, 0x80
	mov ah, 0x08
	mov di, 0
.drive_loop:
	mov ah, 0x08
	int 0x13

	jc .drives_found

	; increment drive count
	mov eax, [__drive_count]
	inc eax
	mov [__drive_count], eax

	mov ebx, [__temppointer]

	mov [ebx], dh

	mov ax, cx
	and ax, 0x3F
	mov [ebx+8], ax
	
	mov ax, cx
	shr ax, 6
	and ax, 0x3FF
	mov [ebx+4], ax

	inc dl
	add ebx, 12

	mov [__temppointer], ebx
	jmp .drive_loop
.drives_found:
	mov [__temppointer], dword 0

.get_e820:
	mov di, word __e820_data

.e820_loop:
	mov eax, 0xe820
	mov ebx, [__tempcont]
	mov ecx, 20
	mov edx, 0x534D4150
	int 0x15

	mov [__tempcont], ebx

	jc .e820_found
	or ebx, ebx
	jz .e820_found

	mov eax, [__e820_size]
	inc eax
	mov [__e820_size], eax

	mov ax, di
	add ax, 20
	mov di, ax

	jmp .e820_loop

.e820_found:
	
	cli
	lgdt [gdt32_pointer]
	mov eax, cr0
	or eax,0x1
	mov cr0, eax
	mov ax, DATA_SEG
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	jmp CODE_SEG:stage2


gdt32_start:
	dq 0x0
gdt32_code:
	dw 0xFFFF
	dw 0x0
	db 0x0
	db 10011010b
	db 11001111b
	db 0x0
gdt32_data:
	dw 0xFFFF
	dw 0x0
	db 0x0
	db 10010010b
	db 11001111b
	db 0x0
gdt32_end:
gdt32_pointer:
	dw gdt32_end - gdt32_start
	dd gdt32_start
CODE_SEG equ gdt32_code - gdt32_start
DATA_SEG equ gdt32_data - gdt32_start

times 510 - ($-$$) db 0
dw 0xaa55

target:
bits 32

stage2:
	mov esp,kernel_stack_top
	cli
	extern main
	call main
	
	jmp $
	
	hlt

__temppointer: dd 0
__tempcont: dd 0

global __e820_size
__e820_size: dd 0
global __e820_data
__e820_data:
	times 20 * 16 db 0

global __drive_count
__drive_count: dd 0
global __drive_data
__drive_data:
	times 12 * 16 db 0

section .bss
align 4
kernel_stack_bottom: equ $
	resb 16384 ; 16 KB
kernel_stack_top:
