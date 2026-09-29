; MAIN-owned player bomb dispatch state.
; The byte/word widths and public ownership follow the target bomb BSS
; fragment; bomb behavior itself remains in the maintained C++/ASM units.

.8086

_TEXT segment word public 'CODE' use16
_TEXT ends

_BSS segment word public 'BSS' use16
public _bombing, _bomb_frame, _playchar_bomb_func
_bombing db ?
_bomb_frame db ?
public _player_bomb_func
_player_bomb_func dw ?
_playchar_bomb_func dw ?

public _bombing_disabled
_bombing_disabled db ?

; BombPaletteColor is three byte channels in the maintained C++ owner.
public _bomb_palette_color_backup
_bomb_palette_color_backup db 3 dup(?)
_BSS ends

DGROUP group _BSS
end
