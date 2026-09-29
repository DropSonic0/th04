; MAIN-owned bomb-star animation storage.
; A target bomb star is Point (4 bytes), angle, and speed (1 byte each),
; with one additional unused record following the 48 live records.

.8086

_TEXT segment word public 'CODE' use16
_TEXT ends

_BSS segment word public 'BSS' use16
public _bomb_stars
_bomb_stars db (49 * 6) dup(?)
_BSS ends

DGROUP group _BSS
end
