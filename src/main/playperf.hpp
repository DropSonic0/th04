#ifndef TH04_MAIN_PLAYPERF_HPP
#define TH04_MAIN_PLAYPERF_HPP

// Player performance ("rank") state and adjustment entry points.
extern unsigned char playperf;
extern unsigned char playperf_max;
extern char playperf_min;

void pascal playperf_raise(char delta);
void pascal playperf_lower(char delta);

#endif
