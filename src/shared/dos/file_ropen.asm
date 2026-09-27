; Open one file for reading through the MASTER single-file state.
; Near Pascal ABI: one near filename pointer, callee removes two bytes.

.8086

extrn DOS_ROPEN:near

_DATA segment word public 'DATA' use16
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

public FILE_ROPEN

ifdef TH04_LARGE_PRODUCT
FILENAME_OFF equ 6
FILE_ROPEN proc far
else
FILENAME_OFF equ 4
FILE_ROPEN proc near
endif

    push bp
    mov bp, sp

    xor ax, ax
    mov bx, file_Handle
    cmp bx, -1
    jne short ropen_done

ifdef TH04_LARGE_PRODUCT
    push ds
    lds dx, dword ptr [bp+FILENAME_OFF]
    mov ax, 3D00h
    int 21h
    pop ds
else
    push word ptr [bp+FILENAME_OFF]
    call DOS_ROPEN
endif
    sbb bx, bx
    or ax, bx
    mov file_Handle, ax

    xor ax, ax
    mov file_InReadBuf, ax
    mov word ptr file_BufferPos, ax
    mov word ptr file_BufferPos+2, ax
    mov file_BufPtr, ax
    mov file_Eof, ax
    mov file_ErrorStat, ax

    lea ax, [bx+1]

ropen_done:
    pop bp
ifdef TH04_LARGE_PRODUCT
    retf 4
else
    ret 2
endif
FILE_ROPEN endp

even
_TEXT ends
end
