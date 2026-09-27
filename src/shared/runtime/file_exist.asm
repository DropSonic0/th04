; TH04-local DOS file-existence probe. Far Pascal receives a far filename
; pointer and consumes its four stack bytes. An open handle is always closed.

    .8086
    .model use16 large SHARED
    .code SHARED

public FILE_EXIST

FILE_EXIST proc far
    push bp
    mov bp, sp
    push ds
    push bx

    mov dx, word ptr ss:[bp+6] ; filename offset
    mov ax, word ptr ss:[bp+8] ; filename segment
    mov ds, ax
    mov ax, 3D00h             ; DOS read-only open
    int 21h
    jc not_found

    mov bx, ax
    mov ah, 3Eh               ; Close the temporary handle
    int 21h
    jc not_found
    mov ax, 1
    jmp done
not_found:
    xor ax, ax
done:
    pop bx
    pop ds
    pop bp
    retf 4
FILE_EXIST endp

end
