; MAIN-owned expanding-circle storage.

.8086

_TEXT segment word public 'CODE' use16
_TEXT ends

_BSS segment word public 'BSS' use16
public _circles, _circles_color
; circle_t: flag, age, Point center, current radius, radius delta (10 bytes).
_circles       db 160 dup(?)
_circles_color db ?
_BSS ends

DGROUP group _BSS
end
