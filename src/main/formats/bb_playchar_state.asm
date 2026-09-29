; MAIN-owned play-character bomb backdrop resources.
; These names and strings are observed in the target bb_playchar data/BSS
; fragments; loading and rendering remain in their format translation units.

.8086

_TEXT segment word public 'CODE' use16
_TEXT ends

_DATA segment word public 'DATA' use16
public _bb_playchar_bb_fn, _bb_playchar_cdg_fn
_bb_playchar_bb_fn dd bb_playchar_bb_name
_bb_playchar_cdg_fn dd bb_playchar_cdg_name
bb_playchar_bb_name db 'BB0.BB', 0
bb_playchar_cdg_name db 'BB0.CDG', 0
                db 0
_DATA ends

_BSS segment word public 'BSS' use16
public _bb_playchar_seg
_bb_playchar_seg dw ?
_BSS ends

DGROUP group _DATA, _BSS
end
