; MAIN-owned playfield scrolling state. The initialized stage setup assigns
; semantic values before the first gameplay use; these are BSS owners.

.8086

_TEXT segment word public 'CODE' use16
_TEXT ends

_BSS segment word public 'BSS' use16
public _scroll_subpixel_line, _scroll_speed
public _scroll_line, _scroll_last_delta, _scroll_active
_scroll_subpixel_line db ?
_scroll_speed         db ?
_scroll_line          dw ?
_scroll_last_delta    dw ?
_scroll_active        db ?
evendata
_BSS ends

DGROUP group _BSS
end
