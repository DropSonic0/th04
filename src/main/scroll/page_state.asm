; Per-VRAM-page scroll-line state historically adjacent to tile invalidation
; storage. Kept separate from the playfield scrolling state so a future native
; link manifest can preserve both physical BSS ownership positions.

.8086

_TEXT segment word public 'CODE' use16
_TEXT ends

_BSS segment word public 'BSS' use16
public _scroll_line_on_page
_scroll_line_on_page dw 2 dup(?)
_BSS ends

DGROUP group _BSS
end
