; MAIN-owned frame and slowdown counters.

.8086

_TEXT segment word public 'CODE' use16
_TEXT ends

_DATA segment word public 'DATA' use16
public _total_slow_frames, _total_frames, _total_std_frames
_total_slow_frames dd 0
_total_frames      dd 0
_total_std_frames  dw 0
_DATA ends

_BSS segment word public 'BSS' use16
public _frames_unused, _stage_frame
public _stage_frame_mod2, _stage_frame_mod4
public _stage_frame_mod8, _stage_frame_mod16
public _slowdown_factor
_frames_unused     dd ?
_stage_frame       dw ?
_stage_frame_mod2  db ?
_stage_frame_mod4  db ?
_stage_frame_mod8  db ?
_stage_frame_mod16 db ?
_slowdown_factor   dw ?
_BSS ends

DGROUP group _DATA, _BSS
end
