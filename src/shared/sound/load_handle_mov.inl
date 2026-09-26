	// TC4J's ordinary pseudoregister assignment selects 8B D8 here. The
	// target uses the equivalent 89 C3 direction. TH04/TH05 release targets
	// independently preserve 89 C3 in dialog_face_unput_8(), and v394 already
	// accepts that register-direction primitive inside a bounded hybrid source.
	asm { mov bx, ax; }
