; MAIN-owned gather-circle and gather-template storage.
; Extents follow the target gather.inc structures for TH04.

.8086

_TEXT segment word public 'CODE' use16
_TEXT ends

_BSS segment word public 'BSS' use16
public _gather_circles, _gather_template
; gather_t is 42 bytes in TH04; 16 live plus 2 unused records are reserved.
_gather_circles db (18 * 42) dup(?)
; gather_template_t is two Points, two words, and two bytes (14 bytes).
_gather_template db 14 dup(?)
_BSS ends

DGROUP group _BSS
end
