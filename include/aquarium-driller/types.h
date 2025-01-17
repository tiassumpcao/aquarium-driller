#ifndef	_TYPES_H
#define _TYPES_H

#pragma once

#ifndef __KERNEL__
#include <stdint.h>
#include <sys/types.h>
#include <stdbool.h>
#endif

#if __GNUC__
#if __x86_64__ || __ppc64__
#define __WORDSIZE 64
#else
#define __WORDSIZE 32
#endif
#endif

#endif /* _TYPES_H */

