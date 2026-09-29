#ifndef TH04_MAIN_END_END_HPP
#define TH04_MAIN_END_END_HPP

// End-chain state shared by MAIN and the ending executable.
typedef enum {
	ES_GOOD = 0xFF,
	ES_BAD = 0xFE,
	ES_EXTRA = 0xFD,
	ES_INGAME = 0x37,
	ES_SCORE = 0,
} end_sequence_t;

#endif
