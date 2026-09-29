; MAIN-owned HUD previous-value state.

.8086

_TEXT segment word public 'CODE' use16
_TEXT ends

_DATA segment word public 'DATA' use16
public _hud_hp_bar_value_prev
_hud_hp_bar_value_prev dw 0
_DATA ends

DGROUP group _DATA
end
