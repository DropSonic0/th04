; MAIN-owned boss state, resource names, and defeat/drop tables.

.8086

_TEXT segment word public 'CODE' use16
_TEXT ends

_DATA segment word public 'DATA' use16
public _BOSS_ITEM_DROPS, _boss_phase_timed_out
; TH04 item kinds: power=0, point=1, big-power=3.
_BOSS_ITEM_DROPS db 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1
_boss_phase_timed_out db 0

public _aSt00_bmt, _aSt00bk_cdg, _aSt00_bb
public _aSt01_bmt, _aSt01bk_cdg, _aSt01_bb
public _aSt02_bmt, _aSt02bk_cdg, _aSt02_bb
public _aSt03_bmt, _aSt03bk_cdg, _aSt03bk2_cdg, _aSt03_bb
public _aSt04bk_cdg, _aSt04_bb, _aSt04_cdg, _aSt05_bb
_aSt00_bmt    db 'st00.bmt', 0
_aSt00bk_cdg  db 'st00bk.cdg', 0
_aSt00_bb     db 'st00.bb', 0
_aSt01_bmt    db 'st01.bmt', 0
_aSt01bk_cdg  db 'st01bk.cdg', 0
_aSt01_bb     db 'st01.bb', 0
_aSt02_bmt    db 'st02.bmt', 0
_aSt02bk_cdg  db 'st02bk.cdg', 0
_aSt02_bb     db 'st02.bb', 0
_aSt03_bmt    db 'st03.bmt', 0
_aSt03bk_cdg  db 'st03bk.cdg', 0
_aSt03bk2_cdg db 'st03bk2.cdg', 0
_aSt03_bb     db 'st03.bb', 0
_aSt04bk_cdg  db 'st04bk.cdg', 0
_aSt04_bb     db 'st04.bb', 0
_aSt04_cdg    db 'st04.cdg', 0
_aSt05_bb     db 'st05.bb', 0
_DATA ends

_BSS segment word public 'BSS' use16
; boss_stuff_t: PlayfieldMotion (12 bytes), followed by
; hp/sprite/phase/frame/damage/mode/angle/phase-state/end-hp (24 bytes).
; Keep the phase-state field public at its target-relative offset so the
; decompiled phase transitions can share the aggregate without a second,
; conflicting storage owner.
public _boss, _boss_phase_state
_boss label byte
db 21 dup(?)
_boss_phase_state db ?
db 2 dup(?)

public _boss_update, _boss_fg_render, _boss_update_func
public _boss_bg_render_func, _boss_fg_render_func
_boss_update        dd ?
_boss_fg_render     dw ?
_boss_bg_render_func dw ?
_boss_update_func   dd ?
_boss_fg_render_func dw ?

public _boss_statebyte, _bb_boss_seg, _boss_hitbox_radius
_boss_statebyte      db 16 dup(?)
_bb_boss_seg          dw ?
_boss_hitbox_radius   db 4 dup(?)

public _tiles_bb_col, _boss_backdrop_colorfill, _tiles_bb_seg
_tiles_bb_col           db ?
even
_boss_backdrop_colorfill dw ?
_tiles_bb_seg             dw ?
_BSS ends

DGROUP group _DATA, _BSS
end
