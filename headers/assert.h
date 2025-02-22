#ifndef __ASSERT_H__
#define __ASSERT_H__

#define ASSERT(e)       if(!(e)) {\
                            __asm__ volatile (\
                                "cli\n"\
                                "hlt"\
                            );\
                        }

#endif