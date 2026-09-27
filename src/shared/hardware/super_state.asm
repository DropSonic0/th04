; TH04 sprite ownership. Keep the historical Pascal and C public spellings
; on the same DGROUP storage without importing a master.lib data object.

    .8086
    .model use16 large SHARED
    .data

public super_buffer, _super_buffer, super_patnum, _super_patnum
public super_patdata, _super_patdata, super_patsize, _super_patsize

super_buffer label word
_super_buffer dw 0
super_patnum label word
_super_patnum dw 0

super_patdata label word
_super_patdata dw 512 dup (0)
super_patsize label word
_super_patsize dw 512 dup (0)

end
