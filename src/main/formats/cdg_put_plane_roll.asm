	.386
	.model use16 large SHARED
	locals

	ROW_SIZE = 80
	PLANE_SIZE = 32000
	CDG_SLOT_COUNT = 64

	cdg_t struc
	CDG_plane_size dw ?
	pixel_w dw ?
	pixel_h dw ?
	offset_at_bottom_left dw ?
	vram_dword_w dw ?
	image_count db ?
	plane_layout db ?
	seg_alpha dw ?
	seg_colors dw ?
	cdg_t ends

	extrn _cdg_slots:byte

	.code SHARED

public @CDG_PUT_PLANE_ROLL_8$QIII12VRAM_PLANE_TURUC
public CDG_PUT_PLANE_ROLL_8
; The C declaration uses the undecorated C name.  Both labels must be real
; PUBDEFs; an EQU alias is not exported by TASM's OMF writer.
CDG_PUT_PLANE_ROLL_8 label far
@cdg_put_plane_roll_8$qiii12vram_plane_turuc proc far
	; (PASCAL calling convention, parameter list needs to be reversed here)
	arg @@plane_dst:word, @@plane_src_id:word, @@slot:word, @@top:word, @@left:word
	@@stride_backwards	equ <dx>
	@@h	equ <bx>
	@@cdg_plane_size	equ <ax>

	push	bp
	mov	bp, sp
	push	si
	push	di
	mov	ax, @@plane_dst
	mov	es, ax

	mov	si, offset _cdg_slots
	mov	ax, @@slot
	mov	bx, size cdg_t
	mul	bx
	add	si, ax

	mov	di, @@left
	sar	di, 3
	mov	@@h, [si+cdg_t.pixel_h]
	mov	ax, @@top
	add	ax, @@h
	dec	ax
	shl	ax, 6
	add	di, ax
	shr	ax, 2
	add	di, ax
	mov	ax, [si+cdg_t.vram_dword_w]
	mov	word ptr cs:@@width_dwords, ax
	jmp	short $+2
	shl	ax, 2
	add	ax, ROW_SIZE
	mov	@@stride_backwards, ax
	push	ds
	mov	ax, [si+cdg_t.seg_colors]
	mov	si, [si+cdg_t.CDG_plane_size]
	mov	ds, ax
	mov	cx, @@plane_src_id
	; (-@@cdg_plane_size + (@@cdg_plane_size * (@@plane_src_id + 1)))? Sure...
	inc	cx
	mov	@@cdg_plane_size, si
	neg	si

@@plane_src_num_to_offset:
	add	si, @@cdg_plane_size
	loop	@@plane_src_num_to_offset
	cmp	di, PLANE_SIZE
	jb	short @@start_blitting
	sub	di, PLANE_SIZE

@@start_blitting:
	cld
	even

@@next_row:
@@width_dwords equ $+1
	mov	cx, 1234h

@@blit_dword:
	mov	eax, [si]
	or	es:[di], eax
	add	si, dword
	add	di, dword
	loop	@@blit_dword
	sub	di, @@stride_backwards
	jge	short @@more_rows?
	add	di, PLANE_SIZE

@@more_rows?:
	dec	@@h
	jnz	short @@next_row
	pop	ds
	pop	di
	pop	si
	pop	bp
	retf	0Ah
@cdg_put_plane_roll_8$qiii12vram_plane_turuc endp

	end
