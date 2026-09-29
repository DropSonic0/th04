; MAIN-owned homing target point.

.8086

_TEXT segment word public 'CODE' use16
_TEXT ends

_BSS segment word public 'BSS' use16
public _homing_target, _homing_target_x, _homing_target_y
_homing_target label byte
_homing_target_x dw ?
_homing_target_y dw ?
_BSS ends

DGROUP group _BSS
end
