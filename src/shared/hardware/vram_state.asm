; Four far VRAM plane pointers used by the TH04 shared graphics runtime.
; vram_planes_set() initializes them before game drawing begins.

.8086

_TEXT segment word public 'CODE' use16
_TEXT ends

_BSS segment word public 'BSS' use16
public _VRAM_PLANE, _VRAM_PLANE_B, _VRAM_PLANE_R
public _VRAM_PLANE_G, _VRAM_PLANE_E

_VRAM_PLANE label dword
_VRAM_PLANE_B dd ?
_VRAM_PLANE_R dd ?
_VRAM_PLANE_G dd ?
_VRAM_PLANE_E dd ?
_BSS ends

DGROUP group _BSS
end
