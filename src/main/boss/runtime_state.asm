; Inferred MAIN runtime state declared by maintained boss translation units.
; These are genuine typed storage owners, not placeholder returns. Widths come
; from the declarations and the 16-bit PC-98 ABI; absolute target placement is
; intentionally left to the later aggregate layout replay.

.8086

_BSS segment word public 'BSS' use16

public _yuuka6_bg_state, _yuuka6_bg_fade, _yuuka6_bg_palette_latch
_yuuka6_bg_state          db ?
_yuuka6_bg_fade           db ?
_yuuka6_bg_palette_latch  db ?

public _elly_scythe_motion, _elly_scythe_flag
_elly_scythe_motion db 12 dup(?)
_elly_scythe_flag   db ?

public _gengetsu_damage_flash_cycle, _boss_bomb_invincibility_frames
public _mugetsu_damage_flash_cycle, _reimu_trail_visible, _yuuka5_move_state
_gengetsu_damage_flash_cycle      db ?
_boss_bomb_invincibility_frames   db ?
_mugetsu_damage_flash_cycle       db ?
_reimu_trail_visible              db ?
_yuuka5_move_state                db ?

public _yuuka6_mirror_damage, _yuuka6_mirror_damage_flash_cycle
public _yuuka6_mirror_pos, _yuuka6_mirror_state, _yuuka6_damage_flash_cycle
public _yuuka6_aux_flag
_yuuka6_mirror_damage             db ?
_yuuka6_mirror_damage_flash_cycle db ?
_yuuka6_mirror_pos                db 4 dup(?)
_yuuka6_mirror_state              db ?
_yuuka6_damage_flash_cycle        db ?
_yuuka6_aux_flag                  db ?

public _elly_orbit_frame, _elly_scythe_mode, _elly_scythe_turn
public _elly_scythe_frame, _elly_scythe_angle, _elly_scythe_speed
public _elly_pattern_group
_elly_orbit_frame  dw ?
_elly_scythe_mode  db ?
_elly_scythe_turn  db ?
_elly_scythe_frame dw ?
_elly_scythe_angle db ?
_elly_scythe_speed db ?
_elly_pattern_group db ?

public _kurumi_special_turn_toggle, _kurumi_unknown_state
_kurumi_special_turn_toggle db ?
_kurumi_unknown_state       db ?

public _marisa_bit_angle_speed, _marisa_pattern_variant
public _marisa_palette_direction, _marisa_prev_bits_alive
public _marisa_prev_mode, _marisa_bitless_cycle
_marisa_bit_angle_speed db ?
_marisa_pattern_variant db ?
_marisa_palette_direction db ?
_marisa_prev_bits_alive db ?
_marisa_prev_mode db ?
_marisa_bitless_cycle db ?

public _mugetsu_transition_func, _mugetsu_anchor, _mugetsu_gather_frame_offset
_mugetsu_transition_func dw ?
_mugetsu_anchor            db 4 dup(?)
_mugetsu_gather_frame_offset dw ?

public _reimu_pattern_angle_delta, _reimu_orbs_visible
_reimu_pattern_angle_delta db ?
_reimu_orbs_visible        db ?

public _yuuka5_cloud_accum, _yuuka5_cloud_step, _yuuka5_sweep_x
public _yuuka5_palette_tone
_yuuka5_cloud_accum  db ?
_yuuka5_cloud_step    db ?
_yuuka5_sweep_x      dw ?
_yuuka5_palette_tone db ?

public _yuuka6_aux_state, _yuuka6_aux_pos, _yuuka6_pattern_prev
_yuuka6_aux_state   db ?
_yuuka6_aux_pos     db 4 dup(?)
_yuuka6_pattern_prev db ?

_BSS ends

DGROUP group _BSS
end
