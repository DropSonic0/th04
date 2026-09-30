; TH04 large-model super_convert_tiny support.
;
; The pinned masters.lib member returns with a near RET 2, while the MAIN
; target boundary uses RETF 2.  This keeps the recovered conversion body
; under the large-model far Pascal ABI without claiming MAIN byte identity.

    .8086
    .model use16 large SHARED

    .data
extrn super_buffer:word
extrn super_patnum:word
extrn super_patdata:word
extrn super_patsize:word

    .code SHARED
public SUPER_CONVERT_TINY

SUPER_CONVERT_TINY proc far
    push bp
    mov bp, sp
    push ds
    push si
    push di

    xor di, di
    mov ax, 4
    push ax              ; color counter

    mov bx, ss:[bp+6]    ; far Pascal: num
    cmp bx, super_patnum
    jb short sc_pattern_index_ok
    jmp sc_fail
sc_pattern_index_ok:
    shl bx, 1
    mov cx, super_patdata[bx]
    jcxz short sc_fail

    mov ax, super_patsize[bx]
    mul ah               ; AX = bytes_per_row * height
    mov bp, ax
    mov es, super_buffer
    mov ds, cx
    mov bh, 0FFh

sc_color_loop:
    xor si, si
    mov ax, bx
    not ax
    mov al, 80h
    and ah, 0Fh
    stosw
    mov cx, bp
    shr cx, 1
    rep movsw
    sub di, bp

    mov bl, 4
    mov cx, bp
    shr cx, 1

sc_bit_loop:
    ror bh, 1
    sbb dx, dx
sc_and_loop:
    lodsw
    xor ax, dx
    and es:[di], ax
    inc di
    inc di
    loop sc_and_loop

    sub di, bp
    mov cx, bp
    shr cx, 1
    dec bl
    jnz short sc_bit_loop

    lea dx, [di-2]
    xor ax, ax
    repe scasw
    mov di, dx
    jz short sc_next_color
    lea di, [di+bp+2]
    pop ax
    dec ax
    push ax
    jns short sc_next_color
    jmp sc_fail

sc_next_color:
    sub bh, 11h
    jnc short sc_color_loop

    mov cx, di
    shr cx, 1
    push ds
    push es
    pop ds
    pop es
    xor ax, ax
    mov di, ax
    mov si, ax
    rep movsw
    stosw
    clc
    xor ax, ax
    jmp sc_return

sc_fail:
    mov ax, 0FFF3h       ; InvalidData
    stc

sc_return:
    pop di               ; color counter
    pop di
    pop si
    pop ds
    pop bp
    retf 2
SUPER_CONVERT_TINY endp

end
