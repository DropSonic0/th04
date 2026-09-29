; Evidence-backed original-style TH04 assembly.
; The independently attested TH05 target preserves the same packed-box
; invalidation architecture. Legal TC4J source probes expand the two 32-bit
; memory shifts instead of emitting this target form.

.186
.model use16 large
locals

F_FREE = 0
BSF_GRAZED = 1
PELLET_COUNT = 240
BULLET16_COUNT = 200
BULLET_COUNT = 440
GATHER_COUNT = 16
PELLET_W = 8
PELLET_H = 8
BULLET16_W = 16
BULLET16_H = 16

Point struc
	x dw ?
	y dw ?
Point ends
motion_t struc
	cur Point <?>
	prev Point <?>
	velocity Point <?>
motion_t ends
bullet_t struc
	flag db ?
	age db ?
	pos motion_t <?>
	from_group db ?
	unused db ?
	speed_cur db ?
	angle db ?
	spawn_flag db ?
	move_flag db ?
	special_motion db ?
	speed_final db ?
	decelerate_time db ?
	decelerate_speed_delta db ?
	patnum dw ?
bullet_t ends
gather_t struc
	G_flag db ?
	G_col db ?
	G_center motion_t <?>
	G_radius_cur dw ?
	G_ring_points dw ?
	G_angle_cur db ?
	G_angle_delta db ?
	G_bullet_template db 18 dup(?)
	G_radius_prev dw ?
	G_radius_delta dw ?
gather_t ends

extrn _bullets:byte
extrn _bullet_zap_active:byte
extrn _bullet_clear_time:byte
extrn _tile_invalidate_box:byte
extrn _gather_circles:byte
TILES_INVALIDATE_AROUND procdesc pascal near center:dword

TILE_TEXT segment word public 'CODE' use16
TILE_TEXT ends
main_01 group TILE_TEXT
TILE_TEXT segment word public 'CODE' use16
assume cs:main_01

public @bullets_and_gather_invalidate$qv
@bullets_and_gather_invalidate$qv proc near
	push	si
	push	di
	mov	si, offset _bullets
	mov	di, BULLET_COUNT
	cmp	_bullet_zap_active, 0
	jnz	short @@pellets_decaying
	cmp	_bullet_clear_time, 0
	jnz	short @@pellets_decaying
	mov	_tile_invalidate_box, (PELLET_W shl 16) or PELLET_H
	mov	di, PELLET_COUNT

@@pellet_loop:
	cmp	[si+bullet_t.flag], F_FREE
	jz	short @@pellet_next
	call	tiles_invalidate_around pascal, dword ptr [si+bullet_t.pos.prev]
@@pellet_next:
	add	si, size bullet_t
	dec	di
	jnz	short @@pellet_loop
	mov	di, BULLET16_COUNT

@@pellets_decaying:
	mov	_tile_invalidate_box, (BULLET16_W shl 16) or BULLET16_H
@@bullet16_loop:
	cmp	[si+bullet_t.flag], F_FREE
	jz	short @@bullet16_next
	cmp	[si+bullet_t.spawn_flag], BSF_GRAZED
	jbe	short @@bullet16_not_grazed
	shl	_tile_invalidate_box, 1
	call	tiles_invalidate_around pascal, dword ptr [si+bullet_t.pos.prev]
	shr	_tile_invalidate_box, 1
	jmp	short @@bullet16_next
@@bullet16_not_grazed:
	call	tiles_invalidate_around pascal, dword ptr [si+bullet_t.pos.prev]
@@bullet16_next:
	add	si, size bullet_t
	dec	di
	jnz	short @@bullet16_loop
	mov	si, offset _gather_circles
	mov	di, GATHER_COUNT

@@gather_loop:
	cmp	[si+gather_t.G_flag], F_FREE
	jz	short @@gather_next
	mov	ax, [si+gather_t.G_radius_cur]
	shr	ax, 3
	add	ax, 16
	mov	_tile_invalidate_box.x, ax
	mov	_tile_invalidate_box.y, ax
	call	tiles_invalidate_around pascal, dword ptr [si+gather_t.G_center.prev]
@@gather_next:
	add	si, size gather_t
	dec	di
	jnz	short @@gather_loop
	pop	di
	pop	si
	retn
@bullets_and_gather_invalidate$qv endp
	even
TILE_TEXT ends
end
