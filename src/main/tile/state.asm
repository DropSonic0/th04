; MAIN-owned tile ring, section lookup, and scroll-state storage.
;
; The target combination allocates a 32x25 word tile ring, a 32-entry word
; section-offset table, and the adjacent STD scroll cursors. The dimensions
; come from the maintained TH04 tile format declarations and the target
; tile/section fragments; no target bytes are copied into this owner.

.8086

_TEXT segment word public 'CODE' use16
_TEXT ends

_DATA segment word public 'DATA' use16
public _TILE_SECTION_OFFSETS
_TILE_SECTION_OFFSETS label word
@@i = 0
rept 32
    dw (@@i * 32 * 5 * word)
    @@i = (@@i + 1)
endm
_DATA ends

_BSS segment word public 'BSS' use16
public _tile_invalidate_box
_tile_invalidate_box db 4 dup(?)
public _invalidate_left_x_tile
_invalidate_left_x_tile dw ?

public _bg_render_not_bombing, _bg_render_bombing, _bg_render_bombing_func
_bg_render_not_bombing dw ?
_bg_render_bombing dw ?
_bg_render_bombing_func dw ?
                dw ?

public _tile_render_all_time
_tile_render_all_time db ?

public _halftiles_dirty, _halftiles_dirty_end
_halftiles_dirty db (32 * 50) dup(?)
_halftiles_dirty_end label byte

public _tile_ring
_tile_ring dw (32 * 25) dup(?)

public _std_map_section_id
_std_map_section_id dw ?

public _tile_row_in_section, _std_scroll_speed
_tile_row_in_section db ?
                db ?
_std_scroll_speed dw ?
_BSS ends

DGROUP group _DATA, _BSS
end
