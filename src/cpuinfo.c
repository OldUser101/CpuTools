#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "cpuid_ex.h"

#define CPU_UNDEFINED -1
#define CPU_INTEL 0
#define CPU_AMD 1
#define CPU_OTHER 2

int cpuModel = CPU_UNDEFINED;

void loadRegString(char* str, uint32_t reg, int first) {
	str[first + 3] = (reg >> 24) & 0xFF;
	str[first + 2] = (reg >> 16) & 0xFF;
	str[first + 1] = (reg >> 8) & 0xFF;
	str[first] = reg & 0xFF;
	str[first + 4] = '\0';
}

void clearRegs(cpuid_regs* regs) {
	regs->eax = 0;
	regs->ebx = 0;
	regs->ecx = 0;
	regs->edx = 0;
}

void printCpuInfoString(char* name, int value) {
	printf("	%s: 0x%x\n", name, value);
}

void printCpuInfoSupportedState(char* name, int value) {
	printf("	%s: ", name);
	if (value > 0)
		printf("Supported\n");
	else
		printf("Not supported\n");
}

uint32_t extractBits(uint32_t value, int high, int low) {
    return (value >> low) & ((1U << (high - low + 1)) - 1);
}

void dispCPUIdentification() {
	cpuid_regs regs = {};
	
	// EAX = 0 ECX = 0
	cpuid(0, &regs);
	uint32_t maxFunctionCode = regs.eax;
	char vendorString[13];
	loadRegString(vendorString, regs.ebx, 0);
	loadRegString(vendorString, regs.edx, 4);
	loadRegString(vendorString, regs.ecx, 8);
	
	clearRegs(&regs);
	
	// EAX = 1 ECX = 0
	cpuid(1, &regs);
	uint32_t stepping = extractBits(regs.eax, 3, 0);
	uint32_t baseModel = extractBits(regs.eax, 7, 4);
	uint32_t family = extractBits(regs.eax, 11, 8);
	uint32_t processorType = extractBits(regs.eax, 13, 12);
	uint32_t extendedModel = extractBits(regs.eax, 19, 16);
	uint32_t extendedFamily = extractBits(regs.eax, 27, 20);
	
	clearRegs(&regs);
	
	// Processor Brand String
	char brandString[49];
	cpuid(0x80000002, &regs);
	loadRegString(brandString, regs.eax, 0);
	loadRegString(brandString, regs.ebx, 4);
	loadRegString(brandString, regs.ecx, 8);
	loadRegString(brandString, regs.edx, 12);
	clearRegs(&regs);
	cpuid(0x80000003, &regs);
	loadRegString(brandString, regs.eax, 16);
	loadRegString(brandString, regs.ebx, 20);
	loadRegString(brandString, regs.ecx, 24);
	loadRegString(brandString, regs.edx, 28);
	clearRegs(&regs);
	cpuid(0x80000004, &regs);
	loadRegString(brandString, regs.eax, 32);
	loadRegString(brandString, regs.ebx, 36);
	loadRegString(brandString, regs.ecx, 40);
	loadRegString(brandString, regs.edx, 44);
	
	uint32_t effectiveModel = ((family == 6 || family == 15) ? ((extendedModel << 4) + baseModel) : baseModel);
	uint32_t effectiveFamily = ((family == 15) ? (family + extendedFamily) : family);
	
	if (!strcmp(vendorString, "GenuineIntel"))
		cpuModel = CPU_INTEL;
	else if (!strcmp(vendorString, "AuthenticAMD"))
		cpuModel = CPU_AMD;
	else
		cpuModel = CPU_OTHER;	
	
	printf("CPU IDENTIFICATION\n");
	printf("	Brand string: \"%s\"\n", brandString);
	printf("	Vendor string: \"%s\"\n", vendorString);
	printCpuInfoString("Max EAX basic function code", maxFunctionCode);
	printCpuInfoString("Stepping ID", stepping);
	printf("	Processor type: 0x%x : ", processorType);
	
	switch (processorType) {
		case 0x0:
			printf("OEM\n"); break;
		case 0x1:
			printf("Intel Overdrive\n"); break;
		case 0x2:
			printf("Dual Processor\n"); break;
		default:
			printf("Unknown\n"); break;
	}
	
	printCpuInfoString("Base model", baseModel);
	printCpuInfoString("Extended model", extendedModel);
	printCpuInfoString("Base family", family);
	printCpuInfoString("Extended family", extendedFamily);
	printCpuInfoString("Effective model", effectiveModel);
	printCpuInfoString("Effective family", effectiveFamily);
	printf("\n");
}

void dispCPUFeaturesBasic() {
	cpuid_regs regs = {};
	
	// EAX = 1 ECX = 0
	cpuid(1, &regs);
	uint32_t fpu = (regs.edx & 0x1);
	uint32_t vme = (regs.edx & 0x2);
	uint32_t de = (regs.edx & 0x4);
	uint32_t pse = (regs.edx & 0x8);
	uint32_t tsc = (regs.edx & 0x10);
	uint32_t msr = (regs.edx & 0x20);
	uint32_t pae = (regs.edx & 0x40);
	uint32_t apic = (regs.edx & 0x200);
	uint32_t mmx = (regs.edx & 0x800000);
	uint32_t sse = (regs.edx & 0x2000000);
	uint32_t sse2 = (regs.edx & 0x4000000);
	uint32_t htt = (regs.edx & 0x10000000);
	uint32_t sse3 = (regs.ecx & 0x1);
	uint32_t ssse3 = (regs.ecx & 0x200);
	uint32_t fma = (regs.ecx & 0x1000);
	uint32_t sse41 = (regs.ecx & 0x80000);
	uint32_t sse42 = (regs.ecx & 0x100000);
	uint32_t aes = (regs.ecx & 0x200000);
	uint32_t avx = (regs.ecx & 0x1000000);
	
	clearRegs(&regs);
	
	// EAX = 7 ECX = 0
	cpuid(7, &regs);
	uint32_t avx2 = (regs.ebx & 0x20);
	uint32_t sha = (regs.ebx & 0x20000000);
	
	printf("CPU BASIC FEATURES\n");
	printCpuInfoSupportedState("FPU", fpu);
	printCpuInfoSupportedState("VME", vme);
	printCpuInfoSupportedState("DE", de);
	printCpuInfoSupportedState("PSE", pse);
	printCpuInfoSupportedState("TSC", tsc);
	printCpuInfoSupportedState("MSR", msr);
	printCpuInfoSupportedState("PAE", pae);
	printCpuInfoSupportedState("APIC", apic);
	printCpuInfoSupportedState("MMX", mmx);
	printCpuInfoSupportedState("SSE", sse);
	printCpuInfoSupportedState("SSE2", sse2);
	printCpuInfoSupportedState("HTT", htt);
	printCpuInfoSupportedState("SSE3", sse3);
	printCpuInfoSupportedState("SSSE3", ssse3);
	printCpuInfoSupportedState("FMA", fma);
	printCpuInfoSupportedState("SSE4.1", sse41);
	printCpuInfoSupportedState("SSE4.2", sse42);
	printCpuInfoSupportedState("AES", aes);
	printCpuInfoSupportedState("AVX", avx);
	printCpuInfoSupportedState("AVX2", avx2);
	printCpuInfoSupportedState("SHA", sha);
	printf("\n");
}

void dispAVX512Features() {
	cpuid_regs regs = {};
	
	// EAX = 7 ECX = 0
	cpuid(7, &regs);
	
	uint32_t maxEax7 = regs.eax;
	
	// AVX-512
	uint32_t avx512_f = (regs.ebx & 0x10000);
	uint32_t avx512_dq = (regs.ebx & 0x20000);
	uint32_t avx512_ifma = (regs.ebx & 0x200000);
	uint32_t avx512_pf = (regs.ebx & 0x4000000);
	uint32_t avx512_er = (regs.ebx & 0x8000000);
	uint32_t avx512_cd = (regs.ebx & 0x10000000);
	uint32_t avx512_bw = (regs.ebx & 0x40000000);
	uint32_t avx512_vl = (regs.ebx & 0x80000000);
	uint32_t avx512_vbmi = (regs.ecx & 0x2);
	uint32_t avx512_vbmi2 = (regs.ecx & 0x40);
	uint32_t avx512_vnni = (regs.ecx & 0x800);
	uint32_t avx512_bitalg = (regs.ecx & 0x1000);
	uint32_t avx512_vpopcntdq = (regs.ecx & 0x4000);
	uint32_t avx512_4vnniw = (regs.edx & 0x4);
	uint32_t avx512_4fmaps = (regs.edx & 0x8);
	uint32_t avx512_vp2intersect = (regs.edx & 0x100);
	uint32_t avx512_fp16 = (regs.edx & 0x800000);
	
	clearRegs(&regs);
	
	// EAX = 7 ECX = 1
	uint32_t avx512_bf16 = (regs.eax & 0x20);
	
	printf("AVX-512 COMPATIBILITY\n");
	printCpuInfoSupportedState("AVX-512 Foundation", avx512_f);
	printCpuInfoSupportedState("AVX-512 DWORD and QWORD Instructions", avx512_dq);
	printCpuInfoSupportedState("AVX-512 Integer Fused Multiply-Add Instructions", avx512_ifma);
	printCpuInfoSupportedState("AVX-512 Prefetch Instructions", avx512_pf);
	printCpuInfoSupportedState("AVX-512 Exponential and Reciprocal Instructions", avx512_er);
	printCpuInfoSupportedState("AVX-512 Conflict Detection Instructions", avx512_cd);
	printCpuInfoSupportedState("AVX-512 Byte and Word Instructions", avx512_bw);
	printCpuInfoSupportedState("AVX-512 Vector Length Extensions", avx512_vl);
	printCpuInfoSupportedState("AVX-512 Vector Bit Manipulation Instructions", avx512_vbmi);
	printCpuInfoSupportedState("AVX-512 Vector Bit Manipulation Instructions 2", avx512_vbmi2);
	printCpuInfoSupportedState("AVX-512 Vector Neural Network Instructions", avx512_vnni);
	printCpuInfoSupportedState("AVX-512 BITALG Instructions", avx512_bitalg);
	printCpuInfoSupportedState("AVX-512 Vector Population Count DWORD and QWORD", avx512_vpopcntdq);
	printCpuInfoSupportedState("AVX-512 4-Register Neural Network Instructions", avx512_4vnniw);
	printCpuInfoSupportedState("AVX-512 4-Register Multiple Accumulation Single Precision", avx512_4fmaps);
	printCpuInfoSupportedState("AVX-512 Vector Intersection Instructions On 32/64-bit Integers", avx512_vp2intersect);
	printCpuInfoSupportedState("AVX-512 Half-Precision Floating-Point Arithmetic Instructions", avx512_fp16);
	printCpuInfoSupportedState("AVX-512 Instructions For bfloat16 Numbers", avx512_bf16);
	printf("\n");
}

void dispCPUFeaturesExtended() {
	cpuid_regs regs = {};
	
	cpuid(1, &regs);
	uint32_t mce = (regs.edx & 0x80); // ADDED
	uint32_t cx8 = (regs.edx & 0x100); // ADDED
	uint32_t sep = (regs.edx & 0x400); // ADDED
	
	clearRegs(&regs);
	
	// EAX = 7 ECX = 0
	cpuid(7, &regs);
	uint32_t bmi1 = (regs.ebx & 0x8);
	uint32_t smap = (regs.ebx & 0x80);
	uint32_t bmi2 = (regs.ebx & 0x100);
	uint32_t rdseed = (regs.ebx & 0x40000);
	uint32_t adx = (regs.ebx & 0x80000);
	uint32_t sha = (regs.ebx & 0x20000000);
	
	clearRegs(&regs);
	
	// HANDLE OTHER EAX = 7 SUBCODES
	
	// EAX = 0x80000001 ECX = 0
	cpuid(0x80000001, &regs);
	uint32_t syscall = (regs.edx & 0x800);
	uint32_t nx = (regs.edx & 0x100000);
	uint32_t _3dnow = (regs.edx & 0x80000000);
	uint32_t lzcnt = (regs.ecx & 0x20);
	uint32_t sse4a = (regs.ecx & 0x40);
	uint32_t fma4 = (regs.ecx & 0x10000);
	uint32_t prefetchw = (regs.ecx & 0x20000000);
	
	clearRegs(&regs);
	
	// EAX = 0x80000008 ECX = 0
	cpuid(0x80000008, &regs);
	uint32_t clzero = (regs.ebx & 0x1);
	uint32_t wbnoinvd = (regs.ebx & 0x4);
	
	clearRegs(&regs);
	
	// EAX = 6 ECX = 0
	cpuid(0x6, &regs);
	uint32_t dts = (regs.eax & 0x1);
	uint32_t turboBoost = (regs.eax & 0x40);
	uint32_t arat = (regs.eax & 0x80);
}

void dispCacheInfo() {
	
}

void dispCPUTopology() {
	
}

void dispPwrManPerf() {
	
}

void dispMultithreading() {
	
}

void dispSecurity() {
	
}

void dispBrandInfo() {
	
}

void dispPSN() {
	
}

void dispExtendedFeatures() {
	
}

void dispTechSupport() {
	
}

void getCpuIdentificationInfo() {
		
}

int main(int argc, char* argv[]) {
	dispCPUIdentification();
	dispCPUFeaturesBasic();
	dispAVX512Features();
	printf("Done.");
	return 0;
}