#ifndef TH04_SHARED_CORE_GAME_INIT_HPP
#define TH04_SHARED_CORE_GAME_INIT_HPP

// OP and MAINE initialize the same TH04 PC-98 runtime with separate flows.
int game_init_op(const unsigned char *pf_fn);
int pascal game_init_main(const unsigned char *pf_fn);

#endif
