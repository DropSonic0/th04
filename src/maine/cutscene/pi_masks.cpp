// MAINE decoded load 0xEB3C, candidate MAP 0E53:060C. Four 16-bit EGC
// masks per dissolve phase. The first mask is applied to scan line zero.
extern const unsigned short PI_MASKS[4][4] = {
    { 0x0000, 0x1111, 0x0000, 0x4444 },
    { 0x8888, 0x1111, 0x2222, 0x4444 },
    { 0xAAAA, 0x5555, 0xAAAA, 0x5555 },
    { 0xEEEE, 0x7777, 0xBBBB, 0xDDDD },
};
