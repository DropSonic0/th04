; TH04 large-model grc_setclip support.
;
; The pinned ReC98 masters.lib member uses the near-return body even though
; the documented interface is far Pascal.  MAIN's target boundary ends in
; RETF 8, so keep the recovered clipping algorithm under a far ABI owner.
; This is a runtime ABI repair, not an authored MAIN exactness claim.

    .8086
    .model use16 large SHARED

    .data
extrn graph_VramWidth:word
extrn graph_VramLines:word
extrn graph_VramSeg:word
extrn ClipXL:word
extrn ClipXW:word
extrn ClipXR:word
extrn ClipYT:word
extrn ClipYH:word
extrn ClipYB:word
extrn ClipYT_seg:word
extrn ClipYB_adr:word

    .code SHARED

public GRC_SETCLIP

GRC_SETCLIP proc far
    push bp
    mov bp, sp

    ; Far Pascal arguments: xl=[BP+12], yt=[BP+10],
    ; xr=[BP+8], yb=[BP+6].
    mov ax, ss:[bp+12]
    mov bx, ss:[bp+8]
    test ax, bx
    jns short grc_setclip_x_sign_ok
    jmp grc_setclip_error
grc_setclip_x_sign_ok:
    cmp ax, bx
    jl short grc_setclip_x_ordered
    xchg ax, bx
grc_setclip_x_ordered:
    cmp ax, 8000h
    sbb dx, dx
    and ax, dx

    mov cx, graph_VramWidth
    shl cx, 1
    shl cx, 1
    shl cx, 1
    dec cx
    sub bx, cx
    sbb dx, dx
    and bx, dx
    add bx, cx
    sub bx, ax
    jnl short grc_setclip_x_range_ok
    jmp grc_setclip_error
grc_setclip_x_range_ok:

    mov ClipXL, ax
    mov ClipXW, bx
    add ax, bx
    mov ClipXR, ax

    mov ax, ss:[bp+10]
    mov bx, ss:[bp+6]
    test ax, bx
    jns short grc_setclip_y_sign_ok
    jmp grc_setclip_error
grc_setclip_y_sign_ok:
    cmp ax, bx
    jl short grc_setclip_y_ordered
    xchg ax, bx
grc_setclip_y_ordered:
    cmp ax, 8000h
    sbb dx, dx
    and ax, dx

    mov cx, graph_VramLines
    dec cx
    sub bx, cx
    sbb dx, dx
    and bx, dx
    add bx, cx
    sub bx, ax
    jnl short grc_setclip_y_range_ok
    jmp grc_setclip_error
grc_setclip_y_range_ok:

    mov ClipYT, ax
    mov cx, ax
    mov ClipYH, bx
    add ax, bx
    mov ClipYB, ax

    mov ax, graph_VramWidth
    xchg ax, bx
    mul bx
    mov ClipYB_adr, ax

    mov ax, bx
    shr ax, 1
    shr ax, 1
    shr ax, 1
    shr ax, 1
    mul cx
    add ax, graph_VramSeg
    mov ClipYT_seg, ax

    mov ax, 1
    pop bp
    retf 8

grc_setclip_error:
    xor ax, ax
    pop bp
    retf 8
GRC_SETCLIP endp

end
