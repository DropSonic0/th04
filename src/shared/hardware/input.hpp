#ifndef TH04_SHARED_HARDWARE_INPUT_HPP
#define TH04_SHARED_HARDWARE_INPUT_HPP

typedef unsigned short input_t;

extern input_t key_det;

void input_reset_sense(void);
void input_sense(void);
void pascal input_wait_for_change(int frames);

enum {
    INPUT_NONE = 0,
    INPUT_UP = 0x0001,
    INPUT_DOWN = 0x0002,
    INPUT_LEFT = 0x0004,
    INPUT_RIGHT = 0x0008,
    INPUT_BOMB = 0x0010,
    INPUT_SHOT = 0x0020,
    INPUT_CANCEL = 0x1000,
    INPUT_OK = 0x2000,
};

#endif
