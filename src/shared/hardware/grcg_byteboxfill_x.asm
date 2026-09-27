; TH04-local GRCG byte-aligned rectangle. The caller owns GRCG color/mode.
; Clips Y to the current graphics rectangle; X is a VRAM byte coordinate.

    .8086
    .model use16 large SHARED
    .data
extrn ClipYT:word, ClipYH:word, ClipYT_seg:word

    .code SHARED
public GRCG_BYTEBOXFILL_X

GRCG_BYTEBOXFILL_X proc far
    push bp
    mov bp, sp
    push si
    push di
    push bx
    push cx
    push dx

    mov si, word ptr [bp+10] ; top relative to clipped origin
    sub si, ClipYT
    jns top_ready
    xor si, si
top_ready:
    mov ax, word ptr [bp+6]  ; bottom relative to clipped origin
    sub ax, ClipYT
    cmp ax, ClipYH
    jle bottom_ready
    mov ax, ClipYH
bottom_ready:
    cmp ax, si
    jl box_done
    mov dx, ax

    mov bx, word ptr [bp+12] ; left byte
    mov cx, word ptr [bp+8]  ; right byte
    cmp cx, bx
    jl box_done
    sub cx, bx
    inc cx
    mov dx, cx               ; width in bytes

    mov ax, si
    shl ax, 2
    add ax, si
    add ax, ClipYT_seg
    mov es, ax

    mov ax, word ptr [bp+6]
    sub ax, ClipYT
    cmp ax, ClipYH
    jle count_bottom_ready
    mov ax, ClipYH
count_bottom_ready:
    sub ax, si
    inc ax
    mov si, ax               ; row count

row_next:
    mov di, bx
    mov cx, dx
    mov al, 0FFh
    rep stosb
    mov ax, es
    add ax, 5                ; 80 VRAM bytes = five paragraphs
    mov es, ax
    dec si
    jnz row_next

box_done:
    pop dx
    pop cx
    pop bx
    pop di
    pop si
    pop bp
    retf 8
GRCG_BYTEBOXFILL_X endp

end
