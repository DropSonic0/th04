; TH04-local default 640x400 PC-98 clipping rectangle. Each alias names
; one shared word in DGROUP; both C and Pascal producers use these globals.

    .8086
    .model use16 large SHARED
    .data

public ClipXL, _ClipXL, ClipXW, _ClipXW, ClipXR, _ClipXR
public ClipYT, _ClipYT, ClipYH, _ClipYH, ClipYB, _ClipYB
public ClipYT_seg, _ClipYT_seg, ClipYB_adr, _ClipYB_adr

ClipXL label word
_ClipXL dw 0
ClipXW label word
_ClipXW dw 639
ClipXR label word
_ClipXR dw 639
ClipYT label word
_ClipYT dw 0
ClipYH label word
_ClipYH dw 399
ClipYB label word
_ClipYB dw 399
ClipYT_seg label word
_ClipYT_seg dw 0A800h
ClipYB_adr label word
_ClipYB_adr dw 31920

end
