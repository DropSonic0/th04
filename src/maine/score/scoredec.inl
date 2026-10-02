{
	unsigned char feedback;
	register unsigned int sum;
	int i;

	for (i = offsetof(scoredat_section_t, score);
		i < sizeof(scoredat_section_t)-1; i++) {
		feedback = ((unsigned char *)&hi)[i + 1];

#ifdef __TURBOC__
		_AL = hi.key2;
		// TC4J has no 8-bit rotate intrinsic; TH03 OP/MAINL independently
		// corroborate this single-byte primitive.
		asm{ ror feedback, 3; }
		feedback ^= _AL;
#else
		// Rotación a la derecha de 8 bits por 3 posiciones equivalente en C++
		feedback = static_cast<unsigned char>((feedback >> 3) | (feedback << 5));
		feedback ^= hi.key2;
#endif

		((unsigned char *)&hi)[i] =
			hi.key1 + feedback + ((unsigned char *)&hi)[i];
	}
	((unsigned char near *)&hi)[i] += (unsigned char)hi.key1;

	sum = 0;
	for (i = offsetof(scoredat_section_t, score);
		i < sizeof(scoredat_section_t); i++) {
		sum += ((unsigned char near *)&hi)[i];
	}
	return (unsigned char)(hi.score_sum - sum);
}