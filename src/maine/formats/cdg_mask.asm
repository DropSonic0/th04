; First-word bit mask for each horizontal CDG pixel alignment (0..15).
; The low byte clears before the high byte as the image shifts right.

.8086

_TEXT segment word public 'CODE' use16
_TEXT ends

_DATA segment word public 'DATA' use16
public DOTS16_MASK_UNALIGNED
DOTS16_MASK_UNALIGNED dw 0FFFFh, 0FF7Fh, 0FF3Fh, 0FF1Fh
                      dw 0FF0Fh, 0FF07h, 0FF03h, 0FF01h
                      dw 0FF00h, 07F00h, 03F00h, 01F00h
                      dw 00F00h, 00700h, 00300h, 00100h
_DATA ends

DGROUP group _DATA
end
