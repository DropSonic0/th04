// MAINE registration resources at decoded load offset 0xEDBB. These arrays
// remain separately addressable to match the callers' file and text pointers.
extern "C" {
char aHi01_pi[] = "hi01.pi";
char aScnum2_bft[] = "scnum2.bft";

// スローモードでのプレイでは、スコアは記録されません
#define SLOW_MODE_SCORE_NOTICE \
    "\x83\x58\x83\x8D\x81\x5B\x83\x82\x81\x5B\x83\x68\x82\xC5\x82\xCC" \
    "\x83\x76\x83\x8C\x83\x43\x82\xC5\x82\xCD\x81\x41\x83\x58\x83\x52" \
    "\x83\x41\x82\xCD\x8B\x4C\x98\x5E\x82\xB3\x82\xEA\x82\xDC\x82\xB9\x82\xF1"

char aGxgnbGvbGhvVGv[] = SLOW_MODE_SCORE_NOTICE;
char aGxgnbGvbGhvV_1[] = SLOW_MODE_SCORE_NOTICE;
#undef SLOW_MODE_SCORE_NOTICE

char aName[] = "name";
}
