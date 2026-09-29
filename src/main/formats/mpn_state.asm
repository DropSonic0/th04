; MAIN-owned .MPN slot storage and load-time palette flag.

.8086

_TEXT segment word public 'CODE' use16
_TEXT ends

_DATA segment word public 'DATA' use16
public _mpn_show_palette_on_load
_mpn_show_palette_on_load db 1
_DATA ends

_BSS segment word public 'BSS' use16
public _mpn_slots
; mpn_t: far image pointer (4), count (2), Palette8 (48),
; and the ten-byte target-reserved tail (total 64 bytes).
_mpn_slots db 64 * 8 dup(?)
_BSS ends

DGROUP group _DATA, _BSS
end
