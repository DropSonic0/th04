#include <stdio.h>

#include "src/shared/platform/types.hpp"
#include "src/shared/platform/pc98.hpp"
#include "src/shared/config/resident.hpp"
#include "src/maine/score/scoredat.hpp"
#include "src/shared/hardware/graphics.hpp"
#include "src/shared/hardware/v_colors.hpp"
#include "src/shared/hardware/gaiji.hpp"
#include "src/shared/hardware/putsa.hpp"
#include "src/shared/hardware/input.hpp"
#include "src/shared/hardware/frame_delay.hpp"
#include "src/shared/formats/pi.hpp"
#include "src/shared/runtime/api.hpp"

enum {
    STAGE_EXTRA = 6,
    RANK_EASY = 0,
    RANK_NORMAL = 1,
    RANK_HARD = 2,
    RANK_LUNATIC = 3,
    RANK_EXTRA = 4,
    ES_BAD = 0xFE,
    ES_GOOD = 0xFF,
};

extern bool skill_subtract;
extern bool graph_3_digit_put_as_fixed_2_digit;
extern uint32_t skill;
extern unsigned char verdict_rank;
extern unsigned char byte_124CC;
extern unsigned char unk_124D3[30];

// Target 0E53:3FBF is byte 28 of the text buffer at 0E53:3FA3.
#define byte_124EF (unk_124D3[28])

extern "C" {
extern char grEASY[5][8];
extern char aU_[], aBd[], aBu[], aBd_0[], aBu_0[];
extern char aB_b_b_b_b_b_b[], aUqiUx[], aNPiuU_[], aGGxi[];
extern char aGGaogcpi[], aGqbGatbrmcj[], aIlcSObcj[];
extern char aGagcgegai[], aUU_gagcgeganNv[], aLcnzvv[];
extern char aPicacovCj[], aVavVVSrso[];
extern char aTimes[], aTimes_0[], aPoint[], a_ude_txt[];
extern char aBhbhbhbhbhbhu_[], aPicacovVVcvsfT[], aUde_pi[];
}

#pragma codeseg MAINE_01_TEXT maine_01
#include "src/maine/end/graph_3_digit_put.inl"
#include "src/maine/end/graph_fraction_of_million_put.inl"
#include "src/maine/end/skill_apply_and_graph_percentage_put.inl"
#include "src/maine/end/sub_B81D.inl"
#include "src/maine/end/sub_B9F2.inl"
#include "src/maine/end/sub_BB81.inl"
#include "src/maine/end/verdict_animate.inl"
#pragma codeseg

#undef byte_124EF
