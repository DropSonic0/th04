; MAIN-owned shot-function dispatch tables.
; The four ten-entry tables are the target's level-selection data surface;
; entries point at the maintained near shot producers.

.8086

_TEXT segment word public 'CODE' use16
_TEXT ends

extrn _shot_reimu_l0:near, _shot_reimu_l1:near
extrn _shot_reimu_a_l2:near, _shot_reimu_a_l3:near, _shot_reimu_a_l4:near
extrn _shot_reimu_a_l5:near, _shot_reimu_a_l6:near, _shot_reimu_a_l7:near
extrn _shot_reimu_a_l8:near, _shot_reimu_a_l9:near
extrn _shot_reimu_b_l2:near, _shot_reimu_b_l3:near, _shot_reimu_b_l4:near
extrn _shot_reimu_b_l5:near, _shot_reimu_b_l6:near, _shot_reimu_b_l7:near
extrn _shot_reimu_b_l8:near, _shot_reimu_b_l9:near
extrn _shot_marisa_l0:near, _shot_marisa_l1:near
extrn _shot_marisa_a_l2:near, _shot_marisa_a_l3:near, _shot_marisa_a_l4:near
extrn _shot_marisa_a_l5:near, _shot_marisa_a_l6:near, _shot_marisa_a_l7:near
extrn _shot_marisa_a_l8:near, _shot_marisa_a_l9:near
extrn _shot_marisa_b_l2:near, _shot_marisa_b_l3:near, _shot_marisa_b_l4:near
extrn _shot_marisa_b_l5:near, _shot_marisa_b_l6:near, _shot_marisa_b_l7:near
extrn _shot_marisa_b_l8:near, _shot_marisa_b_l9:near

_DATA segment word public 'DATA' use16
public _SHOT_FUNCS_REIMU_A, _SHOT_FUNCS_REIMU_B
public _SHOT_FUNCS_MARISA_A, _SHOT_FUNCS_MARISA_B
_SHOT_FUNCS_REIMU_A label word
dw _shot_reimu_l0, _shot_reimu_l1
dw _shot_reimu_a_l2, _shot_reimu_a_l3, _shot_reimu_a_l4, _shot_reimu_a_l5
dw _shot_reimu_a_l6, _shot_reimu_a_l7, _shot_reimu_a_l8, _shot_reimu_a_l9
_SHOT_FUNCS_REIMU_B label word
dw _shot_reimu_l0, _shot_reimu_l1
dw _shot_reimu_b_l2, _shot_reimu_b_l3, _shot_reimu_b_l4, _shot_reimu_b_l5
dw _shot_reimu_b_l6, _shot_reimu_b_l7, _shot_reimu_b_l8, _shot_reimu_b_l9
_SHOT_FUNCS_MARISA_A label word
dw _shot_marisa_l0, _shot_marisa_l1
dw _shot_marisa_a_l2, _shot_marisa_a_l3, _shot_marisa_a_l4, _shot_marisa_a_l5
dw _shot_marisa_a_l6, _shot_marisa_a_l7, _shot_marisa_a_l8, _shot_marisa_a_l9
_SHOT_FUNCS_MARISA_B label word
dw _shot_marisa_l0, _shot_marisa_l1
dw _shot_marisa_b_l2, _shot_marisa_b_l3, _shot_marisa_b_l4, _shot_marisa_b_l5
dw _shot_marisa_b_l6, _shot_marisa_b_l7, _shot_marisa_b_l8, _shot_marisa_b_l9

public _SHOT_LEVEL_TO_POWER, _VELOCITY_192_AT_ANGLE
_SHOT_LEVEL_TO_POWER dw 6, 12, 16, 24, 32, 48, 72, 96, 128
_VELOCITY_192_AT_ANGLE label word
dw 192, 0, 192, 4, 192, 9, 191, 14, 191, 18, 190, 23, 189, 28, 189, 33
dw 188, 37, 187, 42, 186, 46, 185, 51, 183, 55, 182, 60, 180, 64, 179, 69
dw 177, 73, 175, 78, 173, 81, 171, 86, 169, 90, 167, 94, 165, 99, 162, 102
dw 159, 106, 156, 110, 154, 114, 151, 117, 148, 121, 145, 125, 142, 129, 138, 132
dw 135, 135, 132, 138, 129, 142, 125, 145, 121, 148, 117, 151, 114, 154, 110, 156
dw 106, 159, 102, 162, 99, 165, 94, 167, 90, 169, 86, 171, 81, 173, 78, 175
dw 73, 177, 69, 179, 64, 180, 60, 182, 55, 183, 51, 185, 46, 186, 42, 187
dw 37, 188, 33, 189, 28, 189, 23, 190, 18, 191, 14, 191, 9, 192, 4, 192
dw 0, 192, -5, 192, -10, 192, -15, 191, -19, 191, -24, 190, -29, 189, -33, 189
dw -38, 188, -42, 187, -47, 186, -51, 185, -56, 183, -60, 182, -65, 180, -69, 179
dw -74, 177, -78, 175, -82, 173, -87, 171, -91, 169, -95, 167, -99, 165, -103, 162
dw -107, 159, -111, 156, -114, 154, -118, 151, -122, 148, -126, 145, -129, 142, -133, 138
dw -136, 135, -139, 132, -143, 129, -146, 125, -149, 121, -152, 117, -155, 114, -157, 110
dw -160, 106, -162, 102, -165, 99, -168, 94, -170, 90, -172, 86, -174, 81, -176, 78
dw -178, 73, -180, 69, -181, 64, -183, 60, -184, 55, -186, 51, -186, 46, -188, 42
dw -189, 37, -189, 33, -190, 28, -191, 23, -192, 18, -192, 14, -192, 9, -192, 4
dw -192, 0, -192, -5, -192, -10, -192, -15, -192, -19, -191, -24, -190, -29, -189, -33
dw -189, -38, -188, -42, -186, -47, -186, -51, -184, -56, -183, -60, -181, -65, -180, -69
dw -178, -74, -176, -78, -174, -82, -172, -87, -170, -91, -168, -95, -165, -99, -162, -103
dw -160, -107, -157, -111, -155, -114, -152, -118, -149, -122, -146, -126, -143, -129, -139, -133
dw -136, -136, -133, -139, -129, -143, -126, -146, -122, -149, -118, -152, -114, -155, -111, -157
dw -107, -160, -103, -162, -99, -165, -95, -168, -91, -170, -87, -172, -82, -174, -78, -176
dw -74, -178, -69, -180, -65, -181, -60, -183, -56, -184, -51, -186, -47, -186, -42, -188
dw -38, -189, -33, -189, -29, -190, -24, -191, -19, -192, -15, -192, -10, -192, -5, -192
dw 0, -192, 4, -192, 9, -192, 14, -192, 18, -192, 23, -191, 28, -190, 33, -189
dw 37, -189, 42, -188, 46, -186, 51, -186, 55, -184, 60, -183, 64, -181, 69, -180
dw 73, -178, 78, -176, 81, -174, 86, -172, 90, -170, 94, -168, 99, -165, 102, -162
dw 106, -160, 110, -157, 114, -155, 117, -152, 121, -149, 125, -146, 129, -143, 132, -139
dw 135, -136, 138, -133, 142, -129, 145, -126, 148, -122, 151, -118, 154, -114, 156, -111
dw 159, -107, 162, -103, 165, -99, 167, -95, 169, -91, 171, -87, 173, -82, 175, -78
dw 177, -74, 179, -69, 180, -65, 182, -60, 183, -56, 185, -51, 186, -47, 187, -42
dw 188, -38, 189, -33, 189, -29, 190, -24, 191, -19, 191, -15, 192, -10, 192, -5
_DATA ends

DGROUP group _DATA
end
