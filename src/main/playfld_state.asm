; MAIN-owned scroll and playfield shake state.
; Widths and alignment follow the target playfld BSS fragment.

.8086

_TEXT segment word public 'CODE' use16
_TEXT ends

_DATA segment word public 'DATA' use16
public _playfield_shake_redraw_time
_playfield_shake_redraw_time db 0
                db ?
_DATA ends

_BSS segment word public 'BSS' use16
public _playfield_shake_x, _playfield_shake_y, _playfield_shake_anim_time
_playfield_shake_x dw ?
_playfield_shake_y dw ?
_playfield_shake_anim_time dw ?
                dw ?
_BSS ends

DGROUP group _BSS
end
