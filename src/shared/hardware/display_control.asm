; TH04-local PC-98 display controls. These are hardware entry points, written
; in assembly to keep BIOS calls and the GRCG register update explicit.
; Pascal large-model callers use far returns; setcolor consumes two words.

    .8086
    .model use16 large SHARED
    .code SHARED

public GRAPH_HIDE, GRAPH_SHOW, GRCG_OFF, GRCG_SETCOLOR

GRAPH_HIDE proc far
    mov ah, 41h
    int 18h
    retf
GRAPH_HIDE endp

GRAPH_SHOW proc far
    mov ah, 40h
    int 18h
    retf
GRAPH_SHOW endp

GRCG_OFF proc far
    xor al, al
    out 7Ch, al
    retf
GRCG_OFF endp

GRCG_SETCOLOR proc far
    push bp
    mov bp, sp
    push cx
    push dx
    pushf
    cli

    mov al, byte ptr ss:[bp+8]  ; mode, first Pascal argument
    out 7Ch, al
    mov ah, byte ptr ss:[bp+6]  ; color, second argument
    mov dx, 7Eh
    mov cx, 4                   ; B, R, G, I tile registers
plane_next:
    shr ah, 1
    mov al, 0
    jnc plane_write
    dec al
plane_write:
    out dx, al
    loop plane_next

    popf
    pop dx
    pop cx
    pop bp
    retf 4
GRCG_SETCOLOR endp

end
