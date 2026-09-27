; Generated 256-entry bit-reversal table for sprite horizontal flips.
; hflip_lut_generate() fills this table before the first consumer uses it.

.8086

_TEXT segment word public 'CODE' use16
_TEXT ends

_BSS segment word public 'BSS' use16
public _hflip_lut
_hflip_lut db 256 dup(?)
_BSS ends

DGROUP group _BSS
end
