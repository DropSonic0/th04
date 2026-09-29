; MAIN-owned play-character/performance state.
; The byte widths and public order follow the target play BSS fragment.

.8086

_TEXT segment word public 'CODE' use16
_TEXT ends

_BSS segment word public 'BSS' use16
public _playperf, _playperf_max, _playperf_min, _playchar
_playperf db ?
_playperf_max db ?
_playperf_min db ?
_playchar db ?
_BSS ends

DGROUP group _BSS
end
