// TH04 MAIN's historical snd_se.cpp object is one physical SHARED producer.
// Keep the accepted play/update implementations together for native link
// routing; the separate source files remain the owners used by OP/MAINE
// focused replays.
#include "src/shared/sound/se_play.cpp"
#include "src/shared/sound/se_update.cpp"
