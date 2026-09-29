; MAIN-owned .STD segment and filename pointer.
;
; These are the target-observed DATA/BSS owners from formats/std[data].asm and
; formats/std[bss].asm. std_load() mutates the stage digit in the far filename
; before opening it, so the pointer and its backing text stay in MAIN DATA.

.8086

_TEXT segment word public 'CODE' use16
_TEXT ends

_DATA segment word public 'DATA' use16
public _std_fn
_std_fn dd std_filename
std_filename db 'ST00.STD', 0
_DATA ends

_BSS segment word public 'BSS' use16
public _std_seg, _std_enemy_scripts, _std_ip
_std_seg           dw 0
_std_enemy_scripts dw 32 dup(?)
_std_ip            dd ?
_BSS ends

DGROUP group _DATA, _BSS
end
