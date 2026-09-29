; MAIN-owned checkerboard VRAM descriptor.
; The descriptor is five bytes/words in the target DATA owner.

.8086

_TEXT segment word public 'CODE' use16
_TEXT ends

_DATA segment word public 'DATA' use16
public _checkerboard
_checkerboard dw 0AF30h, 04B0h, 09B0h
              db 4, 2
_DATA ends

DGROUP group _DATA
end
