; TH04 large-model single-file size query. Returns DX:AX and preserves the
; current physical file position, matching the existing file_* state owner.
; The DOS AH=42h interface is the source of size and error status.

.8086

_DATA segment word public 'DATA' use16
extrn file_Handle:word
_DATA ends
DGROUP group _DATA

_TEXT segment word public 'CODE' use16
assume cs:_TEXT, ds:DGROUP
public FILE_SIZE

FILE_SIZE proc far
    push bx
    push si
    push di
    mov bx, file_Handle
    cmp bx, -1
    je short size_error

    mov ax, 4201h             ; query current physical position
    xor cx, cx
    xor dx, dx
    int 21h
    jc short size_error
    push ax                   ; original low word
    push dx                   ; original high word

    mov ax, 4202h             ; seek to end
    xor cx, cx
    xor dx, dx
    int 21h
    mov si, ax
    mov di, dx

    pop cx                    ; original high word
    pop dx                    ; original low word
    mov ax, 4200h             ; restore original position
    int 21h
    mov ax, si
    mov dx, di
    jmp short size_done

size_error:
    mov ax, -1
    mov dx, ax
size_done:
    pop di
    pop si
    pop bx
    retf
FILE_SIZE endp

_TEXT ends
end
