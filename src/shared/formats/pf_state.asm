; TH04 PAR service globals shared by the game and DOS file hook.
; This unit owns storage and C/Pascal aliases; it does not install the hook.

    .8086
    .model use16 large SHARED
    .data

public bbufsiz, _bbufsiz, pferrno, _pferrno, pfkey, _pfkey

bbufsiz label word
_bbufsiz dw 512

pferrno label word
_pferrno dw 0

pfkey label byte
_pfkey db 0
        db 0

end
