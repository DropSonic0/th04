; TH04 CDG runtime slots: 64 entries of the 16-byte cdg_t used by cdg_load.asm.
; The loader sets the no-alpha mode and image count for each operation.

.8086

_TEXT segment word public 'CODE' use16
_TEXT ends

_DATA segment word public 'DATA' use16
public _cdg_noalpha
_cdg_noalpha db 0
_DATA ends

_BSS segment word public 'BSS' use16
public _cdg_slots, cdg_images_to_load
_cdg_slots db (64 * 16) dup(?)
cdg_images_to_load db ?
_BSS ends

DGROUP group _DATA, _BSS
end
