; Set TH04's 640x400 PC-98 color mode and matching VRAM/clip defaults.
; Far Pascal entry used by the large-model product build.

    .8086
    .model use16 large SHARED

    .data
extrn graph_VramSeg:word, graph_VramWords:word, graph_VramLines:word
extrn graph_VramZoom:word
extrn ClipXL:word, ClipXW:word, ClipXR:word
extrn ClipYT:word, ClipYH:word, ClipYB:word
extrn ClipYT_seg:word, ClipYB_adr:word

    .code SHARED
public GRAPH_400LINE

GRAPH_400LINE proc far
    mov ah, 42h
    mov ch, 0C0h             ; 640x400 color, first page
    int 18h

    mov ax, 0A800h
    mov graph_VramSeg, ax
    mov ClipYT_seg, ax
    mov graph_VramWords, 16000
    xor ax, ax
    mov ClipXL, ax
    mov ClipYT, ax

    mov es, ax
    mov ah, byte ptr es:[054Dh]
    and ah, 4
    add ah, 3Fh
    and ah, 40h
    mov graph_VramZoom, ax

    mov ax, 639
    mov ClipXR, ax
    mov ClipXW, ax
    mov ax, 400
    mov graph_VramLines, ax
    dec ax
    mov ClipYB, ax
    mov ClipYH, ax
    mov ClipYB_adr, 31920
    retf
GRAPH_400LINE endp

end
