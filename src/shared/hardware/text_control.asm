; TH04-local PC-98 text controls. All public calls are far Pascal entries.

    .8086
    .model use16 large SHARED
    .code SHARED

public KEY_BEEP_OFF, TEXT_CURSOR_HIDE, TEXT_SYSTEMLINE_HIDE

KEY_BEEP_OFF proc far
    push es
    xor ax, ax
    mov es, ax
    or byte ptr es:[0500h], 20h
    pop es
    retf
KEY_BEEP_OFF endp

TEXT_CURSOR_HIDE proc far
    push dx
    mov dh, '5'
    mov dl, 'h'
    call emit_escape
    pop dx
    retf
TEXT_CURSOR_HIDE endp

TEXT_SYSTEMLINE_HIDE proc far
    push dx
    mov dh, '1'
    mov dl, 'h'
    call emit_escape
    pop dx
    retf
TEXT_SYSTEMLINE_HIDE endp

; PC-98 text BIOS direct output: ESC [ > followed by the selected command.
emit_escape proc near
    mov al, 27
    int 29h
    mov al, '['
    int 29h
    mov al, '>'
    int 29h
    mov al, dh
    int 29h
    mov al, dl
    int 29h
    ret
emit_escape endp

end
