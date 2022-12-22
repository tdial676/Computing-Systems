typedef void (*fsecs_test_funct)(void *);

void init_fsecs(void);
double fsecs(fsecs_test_funct f, void *argp);
void deinit_fsecs(void);
