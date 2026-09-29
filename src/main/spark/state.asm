; MAIN spark storage reconstructed from spark.hpp and the target spark_t
; declaration. spark_t is flag/age + three SPPoints + angle = 16 bytes in
; the 16-bit ABI; the extra four entries preserve SPARK_COUNT_BUG behavior.

.8086

_BSS segment word public 'BSS' use16
public _sparks, _sparks_unused, _spark_ring_offset
_sparks        db (96 * 16) dup(?)
_sparks_unused db (4 * 16) dup(?)
_spark_ring_offset dw ?
_BSS ends

DGROUP group _BSS
end
