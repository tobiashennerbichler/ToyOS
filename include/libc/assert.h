#ifndef __ASSERT_H__
#define __ASSERT_H__

#include <stdbool.h>

void assert(bool cond);
void assert_msg(bool cond, const char *err_msg);

#endif
