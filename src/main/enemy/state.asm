; MAIN-owned enemy accounting counters.

.8086

_TEXT segment word public 'CODE' use16
_TEXT ends

_DATA segment word public 'DATA' use16
public _enemies_gone, _enemies_killed
_enemies_gone   dw 0
_enemies_killed dw 0
_DATA ends

_BSS segment word public 'BSS' use16
public _enemies
; TH04 enemy records are 64 bytes under the 16-bit target packing.
_enemies db (32 * 64) dup(?)
_BSS ends

DGROUP group _DATA, _BSS
end
