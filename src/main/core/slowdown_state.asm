; MAIN-owned turbo-mode state.

.8086

_TEXT segment word public 'CODE' use16
_TEXT ends

_DATA segment word public 'DATA' use16
_DATA ends

_BSS segment word public 'BSS' use16
public _turbo_mode
_turbo_mode db ?
_BSS ends

DGROUP group _DATA, _BSS
end
