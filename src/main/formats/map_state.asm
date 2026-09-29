; MAIN-owned .MAP segment and filename pointer.
;
; The target's formats/map data surface stores the loaded segment as a word
; and keeps the stage filename as a far pointer to the mutable ST00.MAP text.
; The filename is intentionally a real data owner because map_load() rewrites
; its stage digit before opening the file.

.8086

_TEXT segment word public 'CODE' use16
_TEXT ends

_DATA segment word public 'DATA' use16
public _map_fn
_map_fn dd map_filename
map_filename db 'ST00.MAP', 0
_DATA ends

_BSS segment word public 'BSS' use16
public _map_seg
_map_seg dw ?
_BSS ends

DGROUP group _DATA, _BSS
end
