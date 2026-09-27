#ifndef TH04_MAINE_SCORE_PLAYCHAR_HPP
#define TH04_MAINE_SCORE_PLAYCHAR_HPP

typedef enum {
    PLAYCHAR_REIMU = 0,
    PLAYCHAR_MARISA = 1,
    PLAYCHAR_COUNT = 2
} playchar_t;

typedef char th04_playchar_size_check[(sizeof(playchar_t) == 1) ? 1 : -1];

inline playchar_t playchar_other(playchar_t value)
{
    return static_cast<playchar_t>(PLAYCHAR_MARISA - value);
}

#endif
