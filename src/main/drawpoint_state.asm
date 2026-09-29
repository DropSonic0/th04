; MAIN-owned shared drawpoint.

.8086

_TEXT segment word public 'CODE' use16
_TEXT ends

_BSS segment word public 'BSS' use16
public _drawpoint, _drawpoint_x, _drawpoint_y
_drawpoint label byte
_drawpoint_x dw ?
_drawpoint_y dw ?
_BSS ends

DGROUP group _BSS
end
