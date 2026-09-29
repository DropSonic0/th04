; MAIN-owned game-over text and chain target.

.8086

_TEXT segment word public 'CODE' use16
_TEXT ends

_DATA segment word public 'DATA' use16
public _gGAMEOVER, _gCONTINUE_QUESTION, _gYES, _gNO, _gCREDIT
public _gameover_erase_in, _gameover_erase_out
_gGAMEOVER          db 0B0h, 0AAh, 0B6h, 0AEh, 0B8h, 0BFh, 0AEh, 0BBh, 0, 0
_gCONTINUE_QUESTION db 0ACh, 0B8h, 0B7h, 0BDh, 0B2h, 0B7h, 0BEh, 0AEh, 8, 0, 0
_gYES               db 0C2h, 0AEh, 0BCh, 0
_gNO                db 0B7h, 0B8h, 0
_gCREDIT            db 0ACh, 0BBh, 0AEh, 0ADh, 0B2h, 0BDh, 0
; Two-character blank strings used while sliding the game-over glyph.
_gameover_erase_in   db '  ', 0
_gameover_erase_out  db '  ', 0

public _maine_binary
_maine_binary db 'maine', 0
_DATA ends

DGROUP group _DATA
end
