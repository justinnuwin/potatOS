void PROC_run();

typedef void (*kproc_t)(void *);
void PROC_create_kthread(kproc_t entry_point, void *arg);

void PROC_reschedule;

void yield();

void kexit();

