; MAIN-owned random-number ring and cursor.

.8086

_TEXT segment word public 'CODE' use16
_TEXT ends

_BSS segment word public 'BSS' use16
public _randring, _randring_p
_randring   db 256 dup(?)
_randring_p dw ?
_BSS ends

DGROUP group _BSS
end
