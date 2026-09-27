; TH04-local PC-98 graphics vertical roll, far Pascal one-word argument.
; Programs the graphics GDC display start and two line runs through 0xA2/0xA0.

    .8086
    .model use16 large SHARED
    .data
extrn graph_VramLines:word, graph_VramZoom:word

    .code SHARED
public GRAPH_SCROLLUP

GRAPH_SCROLLUP proc far
    push bp
    mov bp, sp
    mov bx, word ptr [bp+6]
    mov dx, graph_VramLines
    sub bx, dx
    sbb ax, ax
    and bx, ax
    add bx, dx             ; min(requested lines, VRAM lines)
    sub dx, bx
    mov bp, bx

    mov cx, graph_VramZoom
    shl bx, cl
    shl dx, cl
    mov cl, 4
wait_empty:
    jmp short $+2
    in al, 0A0h
    test al, cl
    jz wait_empty

    mov al, 70h
    out 0A2h, al

    mov ax, bp
    shl ax, 2
    add ax, bp
    shl ax, 1
    shl ax, 1
    shl ax, 1             ; start address = line * 40
    call gdc_param_word

    mov ax, dx
    shl ax, cl
    or ah, ch
    call gdc_param_word

    xor ax, ax
    call gdc_param_word

    mov ax, bx
    shl ax, cl
    or ah, ch
    call gdc_param_word

    pop bp
    retf 2
GRAPH_SCROLLUP endp

gdc_param_word proc near
    out 0A0h, al
    mov al, ah
    jmp short $+2
    jmp short $+2
    out 0A0h, al
    ret
gdc_param_word endp

end
