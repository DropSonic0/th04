; MAIN-owned visible/back page selectors.

.8086

_TEXT segment word public 'CODE' use16
_TEXT ends

_BSS segment word public 'BSS' use16
public _page_back, _page_front
_page_back db ?
_page_front db ?
_BSS ends

DGROUP group _BSS
end
