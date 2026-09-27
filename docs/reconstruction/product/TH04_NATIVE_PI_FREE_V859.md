# TH04 native PI cleanup owner

2026-09-27 product-build investigation. `src/shared/hardware/graph_pi_free.cpp`
owns the far Pascal `GRAPH_PI_FREE(PiHeader*, image)` entry. The bounded
historical master-library reference frees the comment, machine-extension,
and image heap segments in that order. It clears the first two pointers and
lengths in the header after freeing them. The TH04-local C++ owner implements
those operations; it still depends on the unresolved TH04 `HMEM_FREE` owner.
This is a semantic build candidate, not a raw target-byte or runtime claim.

TC4J 4.02J rejected a direct `reinterpret_cast<void __seg *>(image)` when
`image` was `const void far *` (`compile-054.log` SHA-256
`11299ba76fa11c718f27b1f68be90584bf681c299076002fadcac20907ecc470`).
Removing the `const` qualifier from the far pointer before extracting its
segment compiled. The segment passed to `hmem_free` is the allocation owner;
the pointer offset is not a separate allocation.

Two cold no-archive MAINE builds compile 115 TH04 source units (73 C++,
42 ASM), leave 27 unresolved names and zero warnings. Their link-relevant
OMF records and timestamp-normalized objects agree; only the known BGIMAGE
raw timestamp differs. The first and second receipt SHA-256 values are
`dbdd2f44b111f798789ca3181f54dc4a741008ff6da9002f0441d4c7194e7de4`
and `4e25cce41be6ccc47985a25ef0f2893752928bccc7107c8153cd9554003d8195`.
The explicit manifest SHA-256 is
`4460ef12b60c66d9fc6bd06d7d7abee6da077894915197e4e405a02e87e04ee4`.

The historical-library calibration link exits 0 (receipt
`f6223a52b2db552a582281639d3b95192fff50f417cdf0e61d270fc443db390c`).
The MZ audit validates 597 relocation sites, entry/stack boundaries, and
two DOS load segments (receipt
`14903587fb3711a38b11c1e1e6458689f2cf5ab621e24ed06e8463026ecd788b`).
The call ABI audit checks 15 local far returns, including `GRAPH_PI_FREE`
with `RETF 8`, 60 relocated direct far calls and one same-CS
`push cs; call rel16` (receipt
`720297612a4741782acab3c54ad7b9d2d41c1468dc81a3a149fbed8966603a51`).
The calibration MZ still contains historical-library support and has not
entered MAINE in a runtime scenario. For TH05, validate far-pointer qualifier
casts with the pinned compiler and treat link success and call-distance checks
as separate gates.
