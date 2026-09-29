; MAIN-owned EMS/eyecatch resource names.

.8086

_TEXT segment word public 'CODE' use16
_TEXT ends

_DATA segment word public 'DATA' use16
public _eyename, _bbname, _EMS_NAME
public _FACESET_REIMU_FN_0, _FACESET_MARISA_FN_0
eyecatch_fn_format db 'eye0.cdg', 0
bb0_cdg_name       db 'BB0.CDG', 0
_eyename dd eyecatch_fn_format
_bbname  dd bb0_cdg_name
_EMS_NAME db 'GENSOEMS', 0
_FACESET_REIMU_FN_0  db 'KAO0.cd2', 0
_FACESET_MARISA_FN_0 db 'KAO1.cd2', 0
_DATA ends

_BSS segment word public 'BSS' use16
public _Ems
_Ems dw ?
_BSS ends

DGROUP group _DATA, _BSS
end
