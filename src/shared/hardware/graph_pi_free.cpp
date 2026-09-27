#pragma option -zCSHARED -3

#include "src/shared/hardware/graphics.hpp"
#include "src/shared/runtime/api.hpp"

// Release the three heap-owned allocations recorded by a loaded PI image.
// A caller owns the image slot pointer and clears it separately if needed.
extern "C" void TH04_PASCAL graph_pi_free(
	PiHeader far *header, const void far *image
)
{
	if(header->comment) {
		hmem_free(reinterpret_cast<void __seg *>(header->comment));
		header->commentlen = 0;
		header->comment = 0;
	}
	if(header->maex) {
		hmem_free(reinterpret_cast<void __seg *>(header->maex));
		header->maexlen = 0;
		header->maex = 0;
	}
	if(image) {
		hmem_free(reinterpret_cast<void __seg *>((void far *)image));
	}
}
