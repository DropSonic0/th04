extern "C" void pascal far cdg_free_all(void);

#ifdef TH04P
#pragma codeseg OP_NATIVE_TEXT OP_NATIVE_01
#else
#pragma codeseg OP_TITLE_TEXT op_title_01
#endif
#include "src/op/title/main_cdg_free.inl"
#pragma codeseg
