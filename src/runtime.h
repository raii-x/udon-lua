#ifndef runtime_h
#define runtime_h

int rb_wasm_rt_start(int (main)(int argc, char **argv), int argc, char **argv);
void init_fctx_main(void);
void yield_if_time_exceeded(void);

#endif
