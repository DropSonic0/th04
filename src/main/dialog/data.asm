; MAIN-owned dialog box mask tiles.

.8086

_TEXT segment word public 'CODE' use16
_TEXT ends

_DATA segment word public 'DATA' use16
public _BOX_TILES
_BOX_TILES label word
	; Tile 0
	dw 1000100010001000b
	dw  100010001000100b
	dw   10001000100010b
	dw    1000100010001b
	; Tile 1
	dw 1100110011001100b
	dw  110011001100110b
	dw   11001100110011b
	dw 1001100110011001b
	; Tile 2
	dw 1110111011101110b
	dw 111011101110111b
	dw 1011101110111011b
	dw 1101110111011101b
_DATA ends

DGROUP group _DATA
end
