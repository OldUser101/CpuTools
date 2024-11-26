#ifndef CPUID_EX_H

#define CPUID_EX_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	uint32_t eax;
	uint32_t ebx;
	uint32_t ecx;
	uint32_t edx;
} cpuid_regs;

void cpuid(uint32_t code, cpuid_regs* regs);
void cpuidex(uint32_t code, uint32_t subcode, cpuid_regs* regs);

#ifdef __cplusplus
}
#endif

#endif