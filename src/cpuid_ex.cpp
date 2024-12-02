#include <stdint.h>

#include "cpuid_ex.h"

#if defined(_MSC_VER)
#include <intrin.h>
#endif

extern "C" void cpuid(uint32_t code, cpuid_regs* regs) {
#if defined(_MSC_VER)
	int cpuInfo[4];
	__cpuid(cpuInfo, code);
	regs->eax = cpuInfo[0];
	regs->ebx = cpuInfo[1];
	regs->ecx = cpuInfo[2];
	regs->edx = cpuInfo[3];
#elif defined(__GNUC__) || defined(__clang__)
	#if defined(__x86_64__)
		__asm__ volatile(
			"cpuid"
			: "=a" (regs->eax), "=b" (regs->ebx), "=c" (regs->ecx), "=d" (regs->edx)
			: "a" (code), "c" (0)
		);
	#elif defined(__i386__)
		__asm__ volatile(
			"pushl %%ebx\n\t"
			"cpuid\n\t"
			"movl %%ebx, %1\n\t"
			"popl %%ebx\n\t"
			: "=a" (regs->eax), "=r" (regs->ebx), "=c" (regs->ecx), "=d" (regs->edx)
			: "a" (code), "c" (0)
		);
	#else
		#error "Only i386, x86-64 are supported!"
	#endif
#else
	#error "Only MSVC, GCC, Clang are supported!"
#endif
}

extern "C" void cpuidex(uint32_t code, uint32_t subcode, cpuid_regs* regs) {
#if defined(_MSC_VER)
	int cpuInfo[4];
	__cpuidex(cpuInfo, code, subcode);
	regs->eax = cpuInfo[0];
	regs->ebx = cpuInfo[1];
	regs->ecx = cpuInfo[2];
	regs->edx = cpuInfo[3];
#elif defined(__GNUC__) || defined(__clang__)
	#if defined(__x86_64__)
		__asm__ volatile(
			"cpuid"
			: "=a" (regs->eax), "=b" (regs->ebx), "=c" (regs->ecx), "=d" (regs->edx)
			: "a" (code), "c" (subcode)
		);
	#elif defined(__i386__)
		__asm__ volatile(
			"pushl %%ebx\n\t"
			"cpuid\n\t"
			"movl %%ebx, %1\n\t"
			"popl %%ebx\n\t"
			: "=a" (regs->eax), "=r" (regs->ebx), "=c" (regs->ecx), "=d" (regs->edx)
			: "a" (code), "c" (subcode)
		);
	#else
		#error "Only i386, x86-64 are supported!"
	#endif
#else
	#error "Only MSVC, GCC, Clang are supported!"
#endif
}