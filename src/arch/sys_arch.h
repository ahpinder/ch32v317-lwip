#ifndef LWIP_ARCH_SYS_ARCH_H
#define LWIP_ARCH_SYS_ARCH_H

#include "lwip/opt.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* NO_SYS==1: minimal OS abstraction. */

void sys_init(void);
u32_t sys_now(void);

#if SYS_LIGHTWEIGHT_PROT
/* No-op protection macros for bare-metal single-threaded use. */
#define SYS_ARCH_DECL_PROTECT(lev)
#define SYS_ARCH_PROTECT(lev)
#define SYS_ARCH_UNPROTECT(lev)
#endif

#ifdef __cplusplus
}
#endif

#endif /* LWIP_ARCH_SYS_ARCH_H */
