; TH04 background snapshot plane segments. Zero means no snapshot is allocated.
; Keep B, R, G, E in the 8-byte order consumed by bgimage.cpp.

.8086

_TEXT segment word public 'CODE' use16
_TEXT ends

_DATA segment word public 'DATA' use16
public _bgimage
_bgimage dw 0, 0, 0, 0
_DATA ends

DGROUP group _DATA
end
