	.8086
	.model use16 large
	locals

; motion_t is three packed Points in the target 16-bit ABI.
Point struc
	x dw ?
	y dw ?
Point ends
motion_t struc
	cur Point <?>
	prev Point <?>
	velocity Point <?>
motion_t ends

MOTION_3_TEXT segment word public 'CODE' use16
MOTION_3_TEXT ends
main_03 group MOTION_3_TEXT
MOTION_3_TEXT segment word public 'CODE' use16
assume cs:main_03

public @PlayfieldMotion@update_seg3$qv
@PlayfieldMotion@update_seg3$qv proc near
	mov	bx, sp
	mov	bx, ss:[bx+2]
	mov	ax, word ptr [bx+motion_t.cur]
	mov	word ptr [bx+motion_t.prev], ax
	add	ax, word ptr [bx+motion_t.velocity]
	mov	word ptr [bx+motion_t.cur], ax
	add	bx, Point.y
	mov	dx, word ptr [bx+motion_t.cur]
	mov	word ptr [bx+motion_t.prev], dx
	add	dx, word ptr [bx+motion_t.velocity]
	mov	word ptr [bx+motion_t.cur], dx
	ret	2
@PlayfieldMotion@update_seg3$qv endp
MOTION_3_TEXT ends
end
