; MAIN-owned custom-entity arena.
; TH04 custom_t is 26 bytes and the target reserves 32 live records plus one
; additional unused record.

.8086

_TEXT segment word public 'CODE' use16
_TEXT ends

_BSS segment word public 'BSS' use16
public _custom_entities
_custom_entities db (33 * 26) dup(?)
_BSS ends

DGROUP group _BSS
end
