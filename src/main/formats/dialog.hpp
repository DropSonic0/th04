#ifndef TH04_MAIN_FORMATS_DIALOG_HPP
#define TH04_MAIN_FORMATS_DIALOG_HPP

// Script buffer pointer. The segment part also doubles as the base pointer to
// the allocated dialog buffer for later dialog_free() calls, and therefore
// must not be changed while running the script.
extern unsigned char far *dialog_p;

// Loads a dialog script from the file with the given name.
void pascal near dialog_load(const char *fn);

// Loads the dialog script after defeating Stage 5 Yuuka with continues or on
// Easy mode, shown before the Bad Ending.
void dialog_load_yuuka5_defeat_bad(void);

// Loads the dialog script for the current player character and stage.
void near dialog_load(void);

// Frees any previously loaded dialog script.
void near dialog_free(void);

#endif
