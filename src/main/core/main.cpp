#ifndef TH04_DEMO_PREFIX_COMBINED
#pragma option -zCDEMO_TEXT -zPmain_01
#endif

#include "platform.h"
#include "src/shared/config/resident.hpp"
#include "src/shared/hardware/graphics.hpp"
#include "th04/snd/snd.h"
#include "th04/main/quit.hpp"
#include <stdio.h>

extern unsigned int mem_assign_paras;
extern long random_seed;

resident_t __seg* near cfg_load_resident_ptr(void);
int pascal game_init_main(const unsigned char *pf_fn);
void near ems_allocate_and_preload_eyecatch(void);
void near stage_session_init(void);
void near gameplay_loop(void);
extern "C" void near stage_session_free(void);
int pascal GameExecl(const char *binary_fn);
#pragma samecodeseg GameExecl

extern const unsigned char main_pf_fn[];
extern const char gaiji_fn[];
extern const char se_fn[];
extern const char op_fn[];

void main(void)
{
    printf("[TH04 PS3 Main] main() entered\n");
    if(!cfg_load_resident_ptr()) {
        printf("[TH04 PS3 Main] cfg_load_resident_ptr failed\n");
        return;
    }
    printf("[TH04 PS3 Main] cfg_load_resident_ptr SUCCESS\n");

    mem_assign_paras = (320000 >> 4);
    printf("[TH04 PS3 Main] Calling game_init_main...\n");
    game_init_main(main_pf_fn);
    printf("[TH04 PS3 Main] game_init_main returned SUCCESS\n");

    random_seed = resident->rand;
    printf("[TH04 PS3 Main] Calling ems_allocate_and_preload_eyecatch...\n");
    ems_allocate_and_preload_eyecatch();
    printf("[TH04 PS3 Main] ems_allocate_and_preload_eyecatch completed\n");

    text_clear();
    printf("[TH04 PS3 Main] Calling gaiji_backup & gaiji_entry_bfnt...\n");
    gaiji_backup();
    gaiji_entry_bfnt(gaiji_fn);

    printf("[TH04 PS3 Main] Calling snd_determine_modes & snd_load...\n");
    snd_determine_modes(resident->bgm_mode, resident->se_mode);
    snd_load(se_fn, SND_LOAD_SE);

    printf("[TH04 PS3 Main] Entering main gameplay loop...\n");
    for(;;) {
        printf("[TH04 PS3 Main] stage_session_init()...\n");
        stage_session_init();
        printf("[TH04 PS3 Main] gameplay_loop()...\n");
        gameplay_loop();
        if(quit != Q_NEXT_STAGE) {
            printf("[TH04 PS3 Main] Loop exit with quit code %d\n", quit);
            break;
        }
        printf("[TH04 PS3 Main] Next stage reached, stage_session_free()...\n");
        stage_session_free();
    }

    printf("[TH04 PS3 Main] GameExecl(%s)\n", op_fn);
    GameExecl(op_fn);
}
