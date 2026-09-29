; MAIN-owned player, shot, and shot-laser state.
;
; Extents are derived from the maintained Shot/PlayfieldMotion declarations
; and the target shots/shots_alive/option fragments. Function bodies remain in
; their own translation units; this file owns storage only.

.8086

_TEXT segment word public 'CODE' use16
_TEXT ends

_DATA segment word public 'DATA' use16
public _shot_laser_ring_cycle
_shot_laser_ring_cycle db 0

public _shots_hittest_against_boss
_shots_hittest_against_boss db 0

public _SHOT_LASER_DOTS
_SHOT_LASER_DOTS db 00011000b, 00111100b, 01111110b, 10111101b, 11111111b
_DATA ends

_BSS segment word public 'BSS' use16
public _player_pos
_player_pos db 12 dup(?)

public _player_option_pos_cur, _player_option_pos_prev, _player_option_patnum
_player_option_pos_cur  db 4 dup(?)
_player_option_pos_prev db 4 dup(?)
_player_option_patnum   dw ?

public _player_is_hit, _player_invincibility_time, _power, _shot_level
public _shot_time, _player_respawn_motion_time, _miss_time
_player_is_hit               db ?
_player_invincibility_time   db ?
_power                       db ?
_shot_level                  db ?
_shot_time                   db ?
_player_respawn_motion_time  db ?
_miss_time                   db ?

; Shot is 18 bytes in TH04: flag/age, three Points, patnum, damage, angle.
public _shots, _shot_ptr, _shot_last_id
_shots       db (18 * 68) dup(?)
_shot_ptr    dw ?
_shot_last_id db ?
                db ?

public _shot_hitbox_center, _shot_hitbox_radius, _shots_alive
public _shots_alive_count
_shot_hitbox_center db 4 dup(?)
_shot_hitbox_radius db 4 dup(?)
; shot_alive_t is a Point plus a near pointer (8 bytes).
_shots_alive       db (8 * 68) dup(?)
_shots_alive_count dw ?

public _shot_laser_time, _shot_laser_style, _shot_laser_bottomcenter
_shot_laser_time        dw ?
_shot_laser_style       db ?
                db ?
_shot_laser_bottomcenter db 12 dup(?)

public playchar_shot_func, playchar_shot_funcs, _playchar_shot_func, _playchar_shot_funcs
_playchar_shot_func label word
playchar_shot_func  dw ?
_playchar_shot_funcs label word
playchar_shot_funcs dw ?

public _reimu_shot_cycle
_reimu_shot_cycle db ?

; Inferred scalar state declared by the maintained player/shots units. These
; names are real storage owners, but their original absolute offsets remain
; unaccepted until the complete MAIN layout is replayed.
public _player_input_prev, _byte_25980, _byte_259A7
_player_input_prev dw ?
_byte_25980        db ?
_byte_259A7        db ?
_BSS ends

DGROUP group _DATA, _BSS
end
