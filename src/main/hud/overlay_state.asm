; MAIN-owned overlay strings and state.

.8086

_TEXT segment word public 'CODE' use16
_TEXT ends

_DATA segment word public 'DATA' use16
public _overlay_fade, _boss_bgm_frame, _popup_frame
_overlay_fade   db 0
_boss_bgm_frame db 0
_popup_frame    db 0
even

public _bb_txt_seg
_bb_txt_seg dw 0

public _gStage_1, _gFINAL_STAGE, _gEXTRA_STAGE
_gStage_1     db 0BCh, 0BDh, 0AAh, 0B0h, 0AEh, 2, 0A1h, 0
_gFINAL_STAGE db 0AFh, 0B2h, 0B7h, 0AAh, 0B5h, 2, 0BCh, 0BDh, 0AAh, 0B0h, 0AEh, 0
_gEXTRA_STAGE db 0AEh, 0C1h, 0BDh, 0BBh, 0AAh, 2, 0BCh, 0BDh, 0AAh, 0B0h, 0AEh, 0

popup_hiscore_entry db 40h, 41h, 42h, 43h, 44h, 45h, 46h, 47h, 0
popup_extend       db 48h, 49h, 4Ah, 4Bh, 4Ch, 0
popup_bonus        db 58h, 59h, 5Ah, 5Bh, 0
popup_full_powerup db 50h, 51h, 52h, 53h, 54h, 55h, 56h, 57h, 0

public _POPUP_STRINGS
_POPUP_STRINGS dd popup_hiscore_entry, popup_extend, popup_bonus, popup_full_powerup

public _dissolve_sprite
_dissolve_sprite db 0
even

public _PLAYFIELD_BLANK_ROW
_PLAYFIELD_BLANK_ROW dd playfield_blank_row

public _gDEMO_PLAY
_gDEMO_PLAY db 0ADh, 0AEh, 0B6h, 0B8h, 2, 0B9h, 0B5h, 0AAh, 0C2h, 0
playfield_blank_row db 48 dup(20h), 0

; Stage/BGM title strings are kept as target-observed PC-98 code-page bytes.
title_stage_0 db 8Ch, 0B6h, 96h, 0ECh, 81h, 40h, 81h, 60h, 20h, 50h, 68h, 61h, 6Eh, 74h, 6Fh, 6Dh, 20h, 4Ch, 61h, 6Eh, 64h, 20h, 0
title_stage_1 db 8Ch, 0B6h, 96h, 0E9h, 81h, 40h, 81h, 60h, 20h, 50h, 68h, 61h, 6Eh, 74h, 6Fh, 6Dh, 20h, 4Eh, 69h, 67h, 68h, 74h, 0
title_stage_2 db 8Ch, 0CDh, 8Ah, 89h, 81h, 40h, 81h, 60h, 20h, 4Ch, 61h, 6Bh, 65h, 20h, 6Fh, 66h, 20h, 42h, 6Ch, 6Fh, 6Fh, 64h, 0
title_stage_3 db 96h, 0BBh, 97h, 48h, 81h, 40h, 81h, 60h, 20h, 44h, 61h, 72h, 6Bh, 6Eh, 65h, 73h, 73h, 20h, 0
title_stage_4 db 96h, 0B2h, 8Ch, 0B6h, 81h, 40h, 81h, 60h, 20h, 44h, 72h, 65h, 61h, 6Dh, 20h, 6Fh, 66h, 20h, 46h, 72h, 61h, 69h, 6Ch, 20h, 47h, 69h, 72h, 6Ch, 0
title_stage_5 db 8Ch, 0B6h, 91h, 7Ah, 81h, 40h, 81h, 60h, 20h, 50h, 68h, 61h, 6Eh, 74h, 61h, 73h, 6Dh, 61h, 67h, 6Fh, 72h, 69h, 61h, 20h, 0
title_stage_6 db 92h, 0C7h, 8Ch, 82h, 81h, 40h, 81h, 60h, 20h, 52h, 61h, 73h, 70h, 62h, 65h, 72h, 72h, 79h, 20h, 54h, 72h, 61h, 70h, 20h, 0
title_stage_7 db 82h, 0B7h, 82h, 0CEh, 82h, 0E7h, 82h, 0B5h, 82h, 0A2h, 8Ch, 4Eh, 82h, 0C9h, 90h, 0C3h, 82h, 0A9h, 82h, 0C8h, 9Dh, 0F7h, 82h, 0E8h, 82h, 0F0h, 81h, 40h, 81h, 60h, 20h, 50h, 75h, 63h, 6Bh, 69h, 73h, 68h, 20h, 41h, 6Eh, 67h, 65h, 6Ch, 0

title_bgm_0 db 57h, 69h, 74h, 63h, 68h, 69h, 6Eh, 67h, 20h, 44h, 72h, 65h, 61h, 6Dh, 0
title_bgm_1 db 53h, 65h, 6Ch, 65h, 6Eh, 65h, 27h, 73h, 20h, 6Ch, 69h, 67h, 68h, 74h, 0
title_bgm_2 db 91h, 95h, 8Fh, 0FCh, 90h, 0EDh, 81h, 40h, 81h, 60h, 20h, 44h, 65h, 63h, 6Fh, 72h, 61h, 74h, 69h, 6Fh, 6Eh, 20h, 42h, 61h, 74h, 74h, 6Ch, 65h, 0
title_bgm_3 db 42h, 72h, 65h, 61h, 6Bh, 20h, 74h, 68h, 65h, 20h, 53h, 61h, 62h, 62h, 61h, 74h, 68h, 0
title_bgm_4 db 8Dh, 67h, 8Bh, 0BFh, 8Bh, 0C8h, 81h, 40h, 81h, 60h, 20h, 53h, 63h, 61h, 72h, 6Ch, 65h, 74h, 20h, 50h, 68h, 6Fh, 6Eh, 65h, 6Dh, 65h, 0
title_bgm_5 db 42h, 41h, 44h, 20h, 41h, 70h, 70h, 6Ch, 65h, 21h, 21h, 0
title_bgm_6 db 97h, 0ECh, 90h, 0EDh, 81h, 40h, 81h, 60h, 20h, 50h, 65h, 72h, 64h, 69h, 74h, 69h, 6Fh, 6Eh, 20h, 63h, 72h, 69h, 73h, 69h, 73h, 20h, 0
title_bgm_7 db 83h, 41h, 83h, 8Ah, 83h, 58h, 83h, 7Dh, 83h, 47h, 83h, 58h, 83h, 65h, 83h, 89h, 0
title_bgm_8 db 90h, 0AFh, 82h, 0CCh, 8Ah, 0EDh, 81h, 40h, 81h, 60h, 20h, 43h, 61h, 73h, 6Bh, 65h, 74h, 20h, 6Fh, 66h, 20h, 53h, 74h, 61h, 72h, 20h, 0
title_bgm_9 db 4Ch, 6Fh, 74h, 75h, 73h, 20h, 4Ch, 6Fh, 76h, 65h, 0
title_bgm_10 db 96h, 0B0h, 82h, 0EAh, 82h, 0E9h, 8Bh, 0B0h, 95h, 7Ch, 81h, 40h, 81h, 60h, 20h, 53h, 6Ch, 65h, 65h, 70h, 69h, 6Eh, 67h, 20h, 54h, 65h, 72h, 72h, 6Fh, 72h, 0
title_bgm_11 db 44h, 72h, 65h, 61h, 6Dh, 20h, 4Ch, 61h, 6Eh, 64h, 0
title_bgm_12 db 97h, 48h, 96h, 0B2h, 81h, 40h, 81h, 60h, 20h, 49h, 6Eh, 61h, 6Eh, 69h, 6Dh, 61h, 74h, 65h, 20h, 44h, 72h, 65h, 61h, 6Dh, 20h, 0
title_bgm_13 db 8Bh, 0D6h, 82h, 0B6h, 82h, 0B4h, 82h, 0E9h, 82h, 0F0h, 82h, 0A6h, 82h, 0C8h, 82h, 0A2h, 97h, 56h, 8Bh, 59h, 20h, 0
title_bgm_14 db 83h, 81h, 83h, 43h, 83h, 68h, 8Ch, 0B6h, 91h, 7Ah, 81h, 40h, 81h, 60h, 20h, 49h, 63h, 65h, 6Dh, 69h, 6Ch, 6Bh, 20h, 4Dh, 61h, 67h, 69h, 63h, 20h, 0
title_bgm_15 db 82h, 0A9h, 82h, 0EDh, 82h, 0A2h, 82h, 0A2h, 88h, 0ABh, 96h, 82h, 81h, 40h, 81h, 60h, 20h, 49h, 6Eh, 6Eh, 6Fh, 63h, 65h, 6Eh, 63h, 65h, 0
title_bgm_16 db 8Fh, 0ADh, 8Fh, 97h, 0E3h, 59h, 91h, 7Ah, 8Bh, 0C8h, 81h, 40h, 81h, 60h, 20h, 43h, 61h, 70h, 72h, 69h, 63h, 63h, 69h, 6Fh, 20h, 0

public _STAGE_TITLES, _BGM_TITLES
_STAGE_TITLES dd title_stage_0, title_stage_1, title_stage_2, title_stage_3, title_stage_4, title_stage_5, title_stage_6, title_stage_7
_BGM_TITLES   dd title_bgm_0, title_bgm_1, title_bgm_2, title_bgm_3, title_bgm_4, title_bgm_5, title_bgm_6, title_bgm_7, title_bgm_8, title_bgm_9, title_bgm_10, title_bgm_11, title_bgm_12, title_bgm_13, title_bgm_14, title_bgm_15, title_bgm_16
_DATA ends

_BSS segment word public 'BSS' use16
public _stage_title_len, _stage_bgm_title_len, _boss_bgm_title_len
_stage_title_len     dw ?
_stage_bgm_title_len dw ?
_boss_bgm_title_len  dw ?

public _stage_title_id
_stage_title_id db ?

public _popup_gaiji_len, _popup_id_cur, _popup_dest_reached, _popup_shiftbuf
_popup_gaiji_len    dw ?
_popup_id_cur       db ?
_popup_dest_reached db ?
_popup_shiftbuf     db 9 dup(?)
even

public _popup_cur_tram_left, _popup_dest_tram_left, _bgm_title_id
_popup_cur_tram_left  dw ?
_popup_dest_tram_left dw ?
_bgm_title_id         db ?

public _overlay_popup_id_new, _overlay1, _overlay2, _titles_frame
_overlay_popup_id_new db ?
_overlay1             dw ?
_overlay2             dw ?
_titles_frame         db ?
even

public _overlay_popup_bonus
_overlay_popup_bonus dd ?

public _gameover_fade_frame
_gameover_fade_frame db ?
_BSS ends

DGROUP group _DATA, _BSS
end
