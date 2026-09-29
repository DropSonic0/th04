; MAIN-owned stage-clear bonus text and gaiji tables.
; The Shift-JIS/gaiji byte sequences and pointer order are observed in the
; target DATA owner; formatting behavior remains in stage/bonus.cpp.

.8086

_TEXT segment word public 'CODE' use16
_TEXT ends

_DATA segment word public 'DATA' use16
public _STAGE_CLEAR_BONUS_DESC
_STAGE_CLEAR_BONUS_DESC label dword
dd _aBOSS_FINAL_TIMEOUT, _aPENALTY_6, _aPENALTY_5, _aPENALTY_4
dd _aPENALTY_CONT_1, _aPENALTY_CONT_2, _aPENALTY_CONT_3
dd _aBONUS_EASY, _aBONUS_NORMAL, _aBONUS_HARD, _aBONUS_LUNATIC

public _gpCLEAR_BONUS, _gpCONGRATULATION
_gpCLEAR_BONUS db 4Dh, 4Eh, 4Fh, 2, 58h, 59h, 5Ah, 5Bh, 0
_gpCONGRATULATION db 5Ch, 5Dh, 5Eh, 5Fh, 60h, 61h, 62h, 63h, 64h, 0

public _aBOMB_EXTEND, _aBONUS_TOTAL, _aBONUS_POINT, _aGRAZEX50
public _aBONUS_DREAM, _aPOWERX50, _aBONUS_STAGE
public _aBONUS_TOTAL_2, _aBONUS_POINT_2, _aPLAYER_REM_30000
public _aPLAYER_REM_10000, _aGRAZEX50_2, _aBONUS_DREAM_2, _aPOWERX50_2
public _aALL_CLEAR
_aBOMB_EXTEND db 81h,40h,81h,40h,81h,40h,81h,40h,81h,40h,82h,61h,82h,8Fh,82h,8Dh,82h,82h,81h,40h,82h,64h,82h,98h,82h,94h,82h,85h,82h,8Eh,82h,84h,81h,49h,81h,49h,0
_aBONUS_TOTAL db 81h,40h,81h,40h,81h,40h,82h,73h,82h,6Eh,82h,73h,82h,60h,82h,6Bh,0
_aBONUS_POINT db 82h,6Fh,82h,6Eh,82h,68h,82h,6Dh,82h,73h,81h,40h,82h,61h,82h,8Fh,82h,8Eh,82h,95h,82h,93h,81h,40h,81h,40h,81h,40h,81h,40h,81h,40h,81h,40h,81h,7Eh,0
_aGRAZEX50 db 83h,4Ah,83h,58h,83h,8Ah,92h,65h,90h,94h,81h,40h,81h,7Eh,81h,40h,81h,40h,82h,54h,82h,4Fh,0
_aBONUS_DREAM db 82h,63h,82h,71h,82h,64h,82h,60h,82h,6Ch,81h,40h,82h,61h,82h,8Fh,82h,8Eh,82h,95h,82h,93h,0
_aPOWERX50 db 82h,6Fh,82h,6Eh,82h,76h,82h,64h,82h,71h,81h,40h,81h,7Eh,81h,40h,81h,40h,82h,54h,82h,4Fh,0
_aBONUS_STAGE db 82h,72h,82h,73h,82h,60h,82h,66h,82h,64h,81h,40h,82h,61h,82h,8Fh,82h,8Eh,82h,95h,82h,93h,0

_aBONUS_TOTAL_2 db 81h,40h,81h,40h,81h,40h,82h,73h,82h,6Eh,82h,73h,82h,60h,82h,6Bh,0
_aBONUS_POINT_2 db 82h,6Fh,82h,6Eh,82h,68h,82h,6Dh,82h,73h,81h,40h,82h,61h,82h,8Fh,82h,8Eh,82h,95h,82h,93h,81h,40h,81h,40h,81h,40h,81h,40h,81h,40h,81h,40h,81h,7Eh,0
_aPLAYER_REM_30000 db 8Eh,63h,82h,0E8h,90h,6Ch,90h,94h,81h,40h,81h,7Eh,82h,52h,82h,4Fh,82h,4Fh,82h,4Fh,82h,4Fh,0
_aPLAYER_REM_10000 db 8Eh,63h,82h,0E8h,90h,6Ch,90h,94h,81h,40h,81h,7Eh,82h,50h,82h,4Fh,82h,4Fh,82h,4Fh,82h,4Fh,0
_aGRAZEX50_2 db 83h,4Ah,83h,58h,83h,8Ah,92h,65h,90h,94h,81h,40h,81h,7Eh,81h,40h,81h,40h,82h,54h,82h,4Fh,0
_aBONUS_DREAM_2 db 82h,63h,82h,71h,82h,64h,82h,60h,82h,6Ch,81h,40h,82h,61h,82h,8Fh,82h,8Eh,82h,95h,82h,93h,0
_aPOWERX50_2 db 82h,6Fh,82h,6Eh,82h,76h,82h,64h,82h,71h,81h,40h,81h,7Eh,81h,40h,81h,40h,82h,54h,82h,4Fh,0
_aALL_CLEAR db 82h,60h,82h,6Bh,82h,6Bh,81h,40h,82h,62h,82h,8Ch,82h,85h,82h,81h,82h,92h,81h,40h,81h,40h,0

_aBOSS_FINAL_TIMEOUT db 88h,0ABh,97h,0ECh,83h,7Bh,83h,58h,91h,0DEh,8Eh,0A1h,8Eh,0B8h,94h,73h,81h,49h,81h,49h,81h,40h,81h,40h,81h,40h,81h,40h,81h,40h,81h,40h,81h,40h,81h,40h,81h,7Eh,81h,40h,82h,4Fh,81h,44h,82h,4Fh,0
_aPENALTY_6 db 83h,76h,83h,8Ch,83h,43h,83h,84h,81h,5Bh,90h,94h,83h,79h,83h,69h,83h,8Bh,83h,65h,83h,42h,81h,69h,8Fh,89h,8Ah,0FAh,82h,55h,90h,6Ch,81h,6Ah,81h,7Eh,81h,40h,82h,4Fh,81h,44h,82h,52h,0
_aPENALTY_5 db 83h,76h,83h,8Ch,83h,43h,83h,84h,81h,5Bh,90h,94h,83h,79h,83h,69h,83h,8Bh,83h,65h,83h,42h,81h,69h,8Fh,89h,8Ah,0FAh,82h,54h,90h,6Ch,81h,6Ah,81h,7Eh,81h,40h,82h,4Fh,81h,44h,82h,54h,0
_aPENALTY_4 db 83h,76h,83h,8Ch,83h,43h,83h,84h,81h,5Bh,90h,94h,83h,79h,83h,69h,83h,8Bh,83h,65h,83h,42h,81h,69h,8Fh,89h,8Ah,0FAh,82h,53h,90h,6Ch,81h,6Ah,81h,7Eh,81h,40h,82h,4Fh,81h,44h,82h,56h,0
_aPENALTY_CONT_1 db 83h,52h,83h,93h,83h,65h,83h,42h,83h,6Ah,83h,85h,81h,5Bh,83h,79h,83h,69h,83h,8Bh,83h,65h,83h,42h,81h,69h,82h,50h,89h,0F1h,81h,6Ah,81h,40h,81h,7Eh,81h,40h,82h,4Fh,81h,44h,82h,57h,0
_aPENALTY_CONT_2 db 83h,52h,83h,93h,83h,65h,83h,42h,83h,6Ah,83h,85h,81h,5Bh,83h,79h,83h,69h,83h,8Bh,83h,65h,83h,42h,81h,69h,82h,51h,89h,0F1h,81h,6Ah,81h,40h,81h,7Eh,81h,40h,82h,4Fh,81h,44h,82h,55h,0
_aPENALTY_CONT_3 db 83h,52h,83h,93h,83h,65h,83h,42h,83h,6Ah,83h,85h,81h,5Bh,83h,79h,83h,69h,83h,8Bh,83h,65h,83h,42h,81h,69h,82h,52h,89h,0F1h,81h,6Ah,81h,40h,81h,7Eh,81h,40h,82h,4Fh,81h,44h,82h,53h,0
_aBONUS_EASY db 93h,0EFh,88h,0D5h,93h,78h,83h,7Bh,81h,5Bh,83h,69h,83h,58h,81h,69h,82h,64h,82h,81h,82h,93h,82h,99h,81h,6Ah,81h,40h,81h,40h,81h,40h,81h,40h,81h,7Eh,81h,40h,82h,4Fh,81h,44h,82h,54h,0
_aBONUS_NORMAL db 93h,0EFh,88h,0D5h,93h,78h,83h,7Bh,81h,5Bh,83h,69h,83h,58h,81h,69h,82h,6Dh,82h,8Fh,82h,92h,82h,8Dh,82h,81h,82h,8Ch,81h,6Ah,81h,40h,81h,40h,81h,7Eh,81h,40h,82h,50h,81h,44h,82h,4Fh,0
_aBONUS_HARD db 93h,0EFh,88h,0D5h,93h,78h,83h,7Bh,81h,5Bh,83h,69h,83h,58h,81h,69h,82h,67h,82h,81h,82h,92h,82h,84h,81h,6Ah,81h,40h,81h,40h,81h,40h,81h,40h,81h,7Eh,81h,40h,82h,50h,81h,44h,82h,51h,0
_aBONUS_LUNATIC db 93h,0EFh,88h,0D5h,93h,78h,83h,7Bh,81h,5Bh,83h,69h,83h,58h,81h,69h,82h,6Bh,82h,95h,82h,8Eh,82h,81h,82h,94h,82h,89h,82h,83h,81h,6Ah,81h,40h,81h,7Eh,81h,40h,82h,50h,81h,44h,82h,53h,0
_DATA ends

DGROUP group _DATA
end
