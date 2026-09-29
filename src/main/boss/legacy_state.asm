; MAIN-owned state shared by stage and boss translation units.
; These names and widths are observed in the target MAIN DATA/BSS fragments;
; derived boss entity views continue to use the custom-entity arena.

.8086

_TEXT segment word public 'CODE' use16
_TEXT ends

_DATA segment word public 'DATA' use16
public _MIDBOSS3_FLY_ANGLES, _YUUKA6_PHASE2_FLY_ANGLES, _MARISA_BIT_HP
_MIDBOSS3_FLY_ANGLES db 18h, 68h, 98h, -18h, 00h, 60h, -60h, 40h, -20h, 80h, 20h, 60h
_YUUKA6_PHASE2_FLY_ANGLES db 60h, 00h, 70h, -20h, 80h, 20h, 70h, -70h, -10h, 10h
_MARISA_BIT_HP dw 220, 400, 280, 450
_DATA ends

_BSS segment word public 'BSS' use16
public _bits_alive, _bit_fire, _bit_center_x, _bit_center_y
_bits_alive db ?
                db 2 dup(?)
_bit_fire dw ?
_bit_center_x dw 4 dup(?)
_bit_center_y dw 4 dup(?)

public _stage_render, _stage_vm, _enemy_cur
_stage_render dw ?
_stage_vm dd ?
_enemy_cur dw ?

public _miss_explosion_angle, _miss_explosion_radius
_miss_explosion_angle db ?
_miss_explosion_radius dw ?

public _mugetsu_phase2_mode
_mugetsu_phase2_mode db ?

public _yuuka6_sprite_flag, _yuuka6_phase2_fly_path, _yuuka6_anim_frame
_yuuka6_sprite_flag db ?
_yuuka6_phase2_fly_path db ?
_yuuka6_anim_frame dw ?

public _midboss3_patterns_done
_midboss3_patterns_done db ?

public _orb_patnum_base
_orb_patnum_base db ?

public _palette_changed
_palette_changed db ?
_BSS ends

DGROUP group _DATA, _BSS
end
