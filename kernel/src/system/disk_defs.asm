[bits 32]

global read_drive_parameters
read_drive_parameters:
    push ebp
    mov ebp, esp
    mov dl, [ebp+4]
    mov ebp, [ebp+8]

    mov ax, 0
    mov es, ax
    mov di, ax
    mov ah, 0x09

    int 0x13

    mov [ebp+4], dh
    mov ax, cx
    and ax, 0x1F
    mov [ebp+12], ax
    mov ax, cx
    and ax, 0x3FF
    mov [ebp+8], ax
    

    pushfd
    pop eax
    and eax, 1

    pop ebp

    ret