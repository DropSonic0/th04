; MAIN's large-model DOS_PUTS2 entry takes a far string pointer.
; The shared near implementation is retained for tiny/small-model users;
; MAIN must preserve the far Pascal ABI declared by src/shared/runtime/api.hpp.

    .8086
    .model use16 large MAIN
    .code SHARED

public DOS_PUTS2

DOS_PUTS2 proc far
    push bp
    mov bp, sp
    push ds
    push si
    mov si, ss:[bp+6]
    mov ax, ss:[bp+8]
    mov ds, ax

next_character:
    lodsb
    or al, al
    jz short done
    cmp al, 0ah
    jne short emit_character
    push ax
    mov ah, 2
    mov dl, 0dh
    int 21h
    pop ax

emit_character:
    mov dl, al
    mov ah, 2
    int 21h
    jmp short next_character

done:
    pop si
    pop ds
    pop bp
    retf 4
DOS_PUTS2 endp

end
