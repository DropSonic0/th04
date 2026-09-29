; MAIN-owned Yuuka 6 background-shape state.
; Shape extents and scalar widths are observed in the target MAIN BSS.

.8086

_TEXT segment word public 'CODE' use16
_TEXT ends

_BSS segment word public 'BSS' use16
public _bg_shapes, _bg_shape_patnum, _bg_shape_flyout_speed, _bg_shape_clip
; yuuka6_bg_shape_t is a Point (4 bytes), angle, and speed byte.
_bg_shapes db (57 * 6) dup(?)
_bg_shape_patnum dw ?
_bg_shape_flyout_speed dw ?
_bg_shape_clip dw ?
_BSS ends

DGROUP group _BSS
end
