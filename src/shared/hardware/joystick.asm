; TH04-local PC-98 joystick interface. The OP/MAIN/MAINE input producers
; already consume these public names. This is a product semantic owner, not
; an exact byte claim. YM2203 register 7 enables joystick input; register 14
; reads the active-low first controller on the PC-98 sound-board ports.

    .8086
    .model use16 large SHARED

    .data
public js_bexist, _js_bexist, js_stat, _js_stat
js_bexist label word
_js_bexist dw 0
js_stat label word
_js_stat dw 0, 0

    .code SHARED
public JS_START, JS_END, JS_SENSE

JS_START proc far
    push bx
    push cx
    push dx
    mov cx, 256
    mov dx, 188h
detect_board:
    in al, dx
    inc al
    jnz board_present
    loop detect_board
    xor ax, ax
    jmp short start_done

board_present:
    pushf
    cli
    mov bh, 7
    call sound_read
    and al, 3Fh
    or al, 80h
    mov bl, al
    call sound_write
    popf
    mov ax, 1
start_done:
    mov js_bexist, ax
    pop dx
    pop cx
    pop bx
    retf
JS_START endp

JS_END proc far
    mov ax, 0C00h             ; DOS flush keyboard buffer, no input read
    int 21h
    retf
JS_END endp

JS_SENSE proc far
    push bx
    push dx
    pushf
    cli
    mov bh, 0Fh              ; select sound register 15
    mov bl, 80h              ; first joystick
    call sound_write
    mov dx, 188h
    mov al, 0Eh              ; select sound register 14
    out dx, al
    add dx, 2
    in al, dx
    not al                   ; active-low buttons and directions
    and ax, 003Fh
    popf
    pop dx
    pop bx
    retf
JS_SENSE endp

; YM2203's busy flag is status port 188h bit 7.
sound_ready proc near
    mov dx, 188h
ready_loop:
    in al, dx
    test al, 80h
    jnz ready_loop
    ret
sound_ready endp

sound_write proc near
    call sound_ready
    mov al, bh
    out dx, al
    call sound_ready
    add dx, 2
    mov al, bl
    out dx, al
    ret
sound_write endp

sound_read proc near
    call sound_ready
    mov al, bh
    out dx, al
    call sound_ready
    add dx, 2
    in al, dx
    ret
sound_read endp

end
