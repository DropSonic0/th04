; MAIN-owned dialog and bomb/face resource filenames.
;
; These filenames and the one-byte call counter are the target DATA owners
; used by the dialog exit/EMS preload path. They remain mutable product data,
; rather than macros copied into the C++ callers.

.8086

_TEXT segment word public 'CODE' use16
_TEXT ends

_DATA segment word public 'DATA' use16
public _number_of_calls_to_this_function
_number_of_calls_to_this_function db 0

public _dialog_kanji_buf
_dialog_kanji_buf db '  ', 0

public _dialog_fn, _dialog_fn_yuuka5_defeat_bad
_dialog_fn dd dialog_main_filename
_dialog_fn_yuuka5_defeat_bad dd dialog_yuuka5_filename
dialog_main_filename db '_DM00.TXT', 0
dialog_yuuka5_filename db '_DM04B.txt', 0

public _FACESET_REIMU_FN_1, _FACESET_MARISA_FN_1
_FACESET_REIMU_FN_1  db 'KAO0.cd2', 0
_FACESET_MARISA_FN_1 db 'KAO1.cd2', 0

public _FACESET_MUGETSU_DEFEAT_FN, _FACESET_GENGETSU_DEFEAT_FN
_FACESET_MUGETSU_DEFEAT_FN  db 'bss7.cd2', 0
_FACESET_GENGETSU_DEFEAT_FN db 'bss8.cd2', 0

public _BOMB_BG_REIMU_FN, _BOMB_BG_MARISA_FN
_BOMB_BG_REIMU_FN  db 'bb0.cdg', 0
_BOMB_BG_MARISA_FN db 'bb1.cdg', 0
_DATA ends

DGROUP group _DATA
end
