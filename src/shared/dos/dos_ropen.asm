; Read-only DOS open helper and its sharing-mode state.
; DOS_ROPEN and FONTFILE_OPEN use the product's Pascal pointer distance:
; MAIN is large-model (far filename), while OP/MAINE retain the near ABI.

.8086

_DATA segment word public 'DATA' use16
public file_sharingmode, _file_sharingmode
_file_sharingmode label word
file_sharingmode dw 0
_DATA ends
DGROUP group _DATA

_TEXT segment word public 'CODE' use16
assume cs:_TEXT, ds:DGROUP

public DOS_ROPEN, FONTFILE_OPEN

FileNotFound equ -2

ifdef TH04_LARGE_PRODUCT
DOS_ROPEN label far
else
DOS_ROPEN label near
endif
ifdef TH04_LARGE_PRODUCT
FONTFILE_OPEN proc far
else
FONTFILE_OPEN proc near
endif
    mov bx, sp
    mov ah, 3Dh
    mov al, byte ptr file_sharingmode
ifdef TH04_LARGE_PRODUCT
    lds dx, dword ptr ss:[bx+4]
else
    mov dx, ss:[bx+2]
endif
    int 21h
    jc short open_error
ifdef TH04_LARGE_PRODUCT
    retf 4
else
    ret 2
endif

even
open_error:
    mov ax, FileNotFound
ifdef TH04_LARGE_PRODUCT
    retf 4
else
    ret 2
endif
FONTFILE_OPEN endp

_TEXT ends
end
