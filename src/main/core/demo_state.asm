; MAIN-owned demo replay pointer and filename constants.

.8086

_TEXT segment word public 'CODE' use16
_TEXT ends

_DATA segment word public 'DATA' use16
public _demo_fn, _DEMOPLAY_BINARY_OP
public _main_pf_fn, _gaiji_fn, _se_fn, _op_fn
_demo_fn            db 'DEMO0.REC', 0
_DEMOPLAY_BINARY_OP db 'op', 0
; MAIN's startup resource names.  The packfile name is the maintained
; Shift-JIS filename used by game_init_main; the remaining names are ASCII
; DOS resources used by the gaiji, sound, and OP transitions.
_main_pf_fn db 093h, 08Ch, 095h, 0FBh, 08Ch, 0B6h, 091h, 07Ah, 02Eh, 08Bh, 0BDh, 0
_gaiji_fn   db 'GAMEFT.bft', 0
_se_fn      db 'miko', 0
_op_fn      db 'op', 0
_DATA ends

_BSS segment word public 'BSS' use16
public _DemoBuf
_DemoBuf dd ?
_BSS ends

DGROUP group _DATA, _BSS
end
