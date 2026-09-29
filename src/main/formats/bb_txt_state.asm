; MAIN-owned BB text resource filenames.

.8086

_TEXT segment word public 'CODE' use16
_TEXT ends

_DATA segment word public 'DATA' use16
public _bb_txt_fn, _bb_txt2_fn
_bb_txt_fn  db 'txt.bb', 0
_bb_txt2_fn db 'txt2.bb', 0
_DATA ends

DGROUP group _DATA
end
