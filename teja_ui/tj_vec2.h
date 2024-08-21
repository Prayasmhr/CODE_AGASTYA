#ifndef _TJ_VEC2_H
#define _TJ_VEC2_H

#include"../heart_lib/lib.h"

#define _TJ_VEC2_TEMPLATE(type,prefix) \
	typedef union __tj_vec2##prefix{\
		struct {\
			type x;\
			type y;\
		};\
		type handle[2];\
	} tj_vec2##prefix;\
	\

_TJ_VEC2_TEMPLATE(i32, i)
_TJ_VEC2_TEMPLATE(u32, u)

#endif
