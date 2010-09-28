#ifndef __ROTARY_H_
#define __ROTARY_H_

typedef void (* RotaryFunc)(char);

void rotary_init(RotaryFunc change_call, RotaryFunc save_call);
void rotary_sleep(void);
void rotary_read(void);

#endif
