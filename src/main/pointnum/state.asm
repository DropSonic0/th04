; MAIN-owned point-number rings and per-frame pointer list.

.8086

_TEXT segment word public 'CODE' use16
_TEXT ends

_DATA segment word public 'DATA' use16
public _FIVE_DIGIT_POWERS_OF_10
_FIVE_DIGIT_POWERS_OF_10 dw 10000, 1000, 100, 10, 1
_DATA ends

_BSS segment word public 'BSS' use16
; pointnum_t is 16 bytes in TH04; 200 white + 200 yellow entries.
public _pointnums
_pointnums db (16 * 400) dup(?)

public _pointnum_white_p, _pointnum_yellow_p
_pointnum_white_p  db ?
_pointnum_yellow_p db ?

public _pointnums_alive, _pointnum_first_yellow_alive
_pointnums_alive          dw 401 dup(?)
_pointnum_first_yellow_alive dw ?

public _pointnum_times_2
_pointnum_times_2 db 0
_BSS ends

DGROUP group _DATA, _BSS
end
