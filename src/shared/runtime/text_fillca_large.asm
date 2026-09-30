; TH04 large-model text_fillca support.
;
; The pinned ReC98 masters.lib member is assembled for the small/near model
; and returns with RET 4.  TH04's large-model declaration calls TEXT_FILLCA
; as a far Pascal function; the target support member therefore returns with
; RETF 4.  Keep the algorithmic body local so the maintained build owns the
; ABI boundary without claiming byte identity for MAIN.

    .8086
    .model use16 large SHARED
    .code SHARED

public TEXT_FILLCA

TEXT_FILLCA proc far
    mov bx, bp
    mov bp, sp
    push di

    ; In the large model the far return address occupies four bytes.  Pascal
    ; arguments are therefore atrb at [BP+4], followed by chr at [BP+6].
    xor ax, ax
    mov es, ax
    mov al, es:[0712h]
    inc ax
    mov dx, ax
    shl dx, 1
    shl dx, 1
    add dx, ax
    mov cl, 4
    shl dx, cl
    mov cx, dx

    mov ax, 0A000h
    mov es, ax
    xor di, di
    mov ax, ss:[bp+6]
    rep stosw

    mov cx, dx
    mov di, 2000h
    mov ax, ss:[bp+4]
    rep stosw

    pop di
    mov bp, bx
    retf 4
TEXT_FILLCA endp

end
