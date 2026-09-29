	.8086
	.model use16 large _TEXT
	locals

Point struc
	x dw ?
	y dw ?
Point ends
motion_t struc
	cur Point <?>
	prev Point <?>
	velocity Point <?>
motion_t ends

CIRCLE_TEXT segment word public 'CODE' use16
CIRCLE_TEXT ends
main_01 group CIRCLE_TEXT

CIRCLE_TEXT segment word public 'CODE' use16
assume cs:main_01
public @PlayfieldMotion@update_seg1$qv
@PlayfieldMotion@update_seg1$qv proc near
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
@PlayfieldMotion@update_seg1$qv endp
CIRCLE_TEXT ends
end
