; MAIN-owned dialog callback, script pointer, cursor, and side state.

.8086

_TEXT segment word public 'CODE' use16
_TEXT ends

_DATA segment word public 'DATA' use16
_DATA ends

_BSS segment word public 'BSS' use16
public _std_update, _dialog_p, _dialog_cursor, _dialog_side
_std_update    dw ?
_dialog_p      dd ?
_dialog_cursor dw 2 dup(?)
_dialog_side   dw ?
_BSS ends

DGROUP group _DATA, _BSS
end
