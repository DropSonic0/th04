; MAIN-owned bullet, pellet, clear-zap, and thick-laser state.
;
; TH04 bullet_t and pellet_render_t are reconstructed from the maintained
; headers and their target structure declarations. This owner deliberately
; leaves gameplay-distribution tables to a later semantic source unit.

.8086

_TEXT segment word public 'CODE' use16
_TEXT ends

_DATA segment word public 'DATA' use16
public _group_fixedspeed
_group_fixedspeed db 0, 0
_DATA ends

_BSS segment word public 'BSS' use16
; TH04 bullet_t is 26 bytes; 240 pellets + 200 regular bullets.
public _bullets, _pellets, _bullets16
_bullets label byte
_pellets db (26 * 240) dup(?)
_bullets16 db (26 * 200) dup(?)

public _pellets_render
_pellets_render db (4 * 240) dup(?)

public _bullet_special, _bullet_special_active
public _bullet_special_turns_max, _bullet_special_speed_delta
public _bullet_template_special_angle
_bullet_special label byte
_bullet_special_turns_max label byte
_bullet_special_speed_delta label byte
_bullet_special_active db ?
_bullet_template_special_angle db ?

public _bullet_zap, _bullet_zap_active, _bullet_clear_time
_bullet_zap_active label byte
_bullet_zap        db ?
_bullet_clear_time db ?
                db ?

public _stage_graze, _graze_score, _pellets_render_count
_stage_graze          dw ?
_graze_score          dw ?
_pellets_render_count  dw ?

public _bullet_template
_bullet_template db 18 dup(?)

public _bullet_template_tune, _bullets_add_regular, _bullets_add_special
_bullet_template_tune dw ?
_bullets_add_regular  dw ?
_bullets_add_special  dw ?

public _group_i_spread_angle, _group_i_absolute_angle
_group_i_spread_angle db ?
                db ?
_group_i_absolute_angle db ?
                db ?

; TH04 thicklaser_t is 24 bytes; the target keeps one template and two slots.
public _thicklaser_template, _thicklasers
_thicklaser_template db 24 dup(?)
_thicklasers          db (24 * 2) dup(?)
_BSS ends

DGROUP group _BSS
end
