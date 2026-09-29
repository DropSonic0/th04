; MAIN-owned small/big boss explosion storage.

.8086

_TEXT segment word public 'CODE' use16
_TEXT ends

_DATA segment word public 'DATA' use16
public _explosion_big_frame
_explosion_big_frame dw 0
_DATA ends

_BSS segment word public 'BSS' use16
public _explosions_small, _explosions_big
; explosion_t: alive, age, center, current/delta radii, padding, angle.
_explosions_small db 32 dup(?)
_explosions_big   db 16 dup(?)
_BSS ends

DGROUP group _DATA, _BSS
end
