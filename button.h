#ifndef __BUTTON_H_
#define __BUTTON_H_

typedef void (* ButtonFunc)(void);

void button_init(ButtonFunc event_call);

#endif
