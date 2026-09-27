; Input detection word and BIOS shift state used by TH04 input_s.asm.

.8086

_TEXT segment word public 'CODE' use16
_TEXT ends

_BSS segment word public 'BSS' use16
public _key_det, _shiftkey
_key_det dw ?
_shiftkey db ?
_BSS ends

DGROUP group _BSS
end
