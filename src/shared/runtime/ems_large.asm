; TH04 large-model EMS support.
;
; The pinned ReC98 masters.lib archive contains the small/near-return EMS
; members.  TH04's large-model declarations call these entry points as far
; Pascal functions, and the attested MAIN target's support members end in
; RETF (including the far-string cleanup for EMS_SETNAME/EMS_READ/EMS_WRITE).
; Keep the algorithmic bodies local and select this unit before masters.lib;
; this is an ABI-correct runtime owner, not an authored MAIN exactness claim.

    .8086
    .model use16 large SHARED
    .code SHARED

public EMS_ALLOCATE
public EMS_EXIST
public EMS_FREE
public EMS_READ
public EMS_SETNAME
public EMS_SPACE
public EMS_WRITE

EMS_EXIST proc far
    push si
    push di
    push ds

    xor ax, ax
    mov es, ax
    mov es, word ptr es:[67h*4+2]
    mov di, 000Ah
    mov bx, cs
    mov ds, bx
    mov si, offset th04_ems_device_name
    mov cx, 4
    repe cmpsw
    jnz short ems_exist_done
    inc ax
ems_exist_done:
    pop ds
    pop di
    pop si
    retf
EMS_EXIST endp

th04_ems_device_name db 'EMMXXXX0'

EMS_SPACE proc far
    mov ah, 42h
    int 67h
    xor dx, dx
    cmp ah, 0
    mov ax, dx
    jnz short ems_space_done
    mov dx, bx
    shr dx, 1
    rcr ax, 1
    shr dx, 1
    rcr ax, 1
ems_space_done:
    retf
EMS_SPACE endp

EMS_ALLOCATE proc far
    mov bx, sp
    mov cx, ss:[bx+4]
    mov bx, ss:[bx+6]
    shl cx, 1
    rcl bx, 1
    shl cx, 1
    rcl bx, 1
    cmp cx, 1
    sbb bx, -1
    mov ah, 43h
    int 67h
    sub ah, 1
    sbb ax, ax
    and ax, dx
    retf 4
EMS_ALLOCATE endp

EMS_FREE proc far
    mov bx, sp
    mov dx, ss:[bx+4]
    mov ah, 45h
    int 67h
    mov al, ah
    xor ah, ah
    retf 2
EMS_FREE endp

EMS_SETNAME proc far
    push si
    mov si, sp
    push ds

    mov dx, ss:[si+10]
    lds si, dword ptr ss:[si+6]
    mov ax, 5301h
    int 67h
    mov al, ah
    xor ah, ah

    pop ds
    pop si
    retf 6
EMS_SETNAME endp

; These private near helpers mirror the two internal master-library calls used
; by EMS_READ/EMS_WRITE.  Keeping them in SHARED avoids a near call into the
; archive's _TEXT segment after the public ABI is corrected to far.
TH04_EMS_MOVEMEMORYREGION proc near
    push si
    mov si, sp
    push ds
    lds si, dword ptr ss:[si+4]
    mov ax, 5700h
    int 67h
    mov al, ah
    xor ah, ah
    pop ds
    pop si
    ret 4
TH04_EMS_MOVEMEMORYREGION endp

TH04_EMS_ENABLEPAGEFRAME proc near
    mov bx, sp
    mov bx, ss:[bx+2]
    mov ax, 7001h
    int 67h
    ret 2
TH04_EMS_ENABLEPAGEFRAME endp

EMS_READ proc far
    push bp
    mov bp, sp
    sub sp, 18

    mov ax, [bp+6]
    mov dx, [bp+8]
    mov [bp-18], ax
    mov [bp-16], dx
    mov byte ptr [bp-14], 1

    mov ax, [bp+18]
    mov [bp-13], ax

    mov ax, [bp+14]
    mov dx, ax
    and dh, 3Fh
    mov [bp-11], dx
    mov dx, [bp+16]
    shl ax, 1
    rcl dx, 1
    shl ax, 1
    rcl dx, 1
    mov [bp-9], dx

    mov byte ptr [bp-7], 0
    mov word ptr [bp-6], 0
    mov ax, [bp+10]
    mov [bp-4], ax
    mov ax, [bp+12]
    mov [bp-2], ax

    push ss
    lea ax, [bp-18]
    push ax
    call near ptr TH04_EMS_MOVEMEMORYREGION
    push ax
    xor ax, ax
    push ax
    call near ptr TH04_EMS_ENABLEPAGEFRAME
    pop ax
    mov sp, bp
    pop bp
    retf 14
EMS_READ endp

EMS_WRITE proc far
    push bp
    mov bp, sp
    sub sp, 18

    mov ax, [bp+6]
    mov dx, [bp+8]
    mov [bp-18], ax
    mov [bp-16], dx
    mov byte ptr [bp-14], 0
    mov word ptr [bp-13], 0

    mov ax, [bp+10]
    mov [bp-11], ax
    mov ax, [bp+12]
    mov [bp-9], ax
    mov byte ptr [bp-7], 1
    mov ax, [bp+18]
    mov [bp-6], ax

    mov ax, [bp+14]
    mov dx, ax
    and dh, 3Fh
    mov [bp-4], dx
    mov dx, [bp+16]
    shl ax, 1
    rcl dx, 1
    shl ax, 1
    rcl dx, 1
    mov [bp-2], dx

    push ss
    lea ax, [bp-18]
    push ax
    call near ptr TH04_EMS_MOVEMEMORYREGION
    push ax
    xor ax, ax
    push ax
    call near ptr TH04_EMS_ENABLEPAGEFRAME
    pop ax
    mov sp, bp
    pop bp
    retf 14
EMS_WRITE endp

end
