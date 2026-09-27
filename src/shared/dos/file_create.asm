; Create one file for writing through the MASTER single-file state.
; Near Pascal ABI: one near filename pointer, callee removes two bytes.

.8086

extrn DOS_AXDX:near

_DATA segment word public 'DATA' use16
extrn file_Buffer:dword
extrn file_BufferSize:word
extrn file_BufferPos:dword
extrn file_BufPtr:word
extrn file_InReadBuf:word
extrn file_Eof:word
extrn file_ErrorStat:word
extrn file_Handle:word
_DATA ends
DGROUP group _DATA

_TEXT segment word public 'CODE' use16
assume cs:_TEXT, ds:DGROUP

public FILE_CREATE

ifdef TH04_LARGE_PRODUCT
FILENAME_OFF equ 6
FILE_CREATE proc far
else
FILENAME_OFF equ 4
FILE_CREATE proc near
endif
    push bp
    mov bp, sp

    mov ax, 0
    mov bx, file_Handle
    cmp bx, -1
    jne short create_done

ifdef TH04_LARGE_PRODUCT
    push ds
    lds dx, dword ptr [bp+FILENAME_OFF]
    mov cx, 20h
    mov ah, 3Ch
    int 21h
    pop ds
    sbb dx, dx
    xor ax, dx
    sub ax, dx
else
    mov cx, 20h
    mov ah, 3Ch
    push ax
    push word ptr [bp+FILENAME_OFF]
    call DOS_AXDX
endif
    or ax, dx
    mov file_Handle, ax

    xor ax, ax
    mov file_InReadBuf, ax
    mov file_BufPtr, ax
    mov file_Eof, ax
    mov file_ErrorStat, ax
    mov word ptr file_BufferPos, ax
    mov word ptr file_BufferPos+2, ax

    mov ax, dx
    inc ax

create_done:
    pop bp
ifdef TH04_LARGE_PRODUCT
    retf 4
else
    ret 2
endif
FILE_CREATE endp

even
_TEXT ends
end
