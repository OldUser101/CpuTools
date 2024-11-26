#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "cpuid_ex.h"

long myAtoi(char* str) {
	char* endptr;
	int base = 10;
	
	if (str[0] == '0' && str[1] == 'X')
		base = 16;
	
	unsigned long result = strtoul(str, &endptr, base);
	if (endptr == str)
		return -1;
	
	return result;
}

int isNumeric(char* s, int hex) {
	while (*s) {
		if (!(((*s) >= '0' && (*s) <= '9') || ((((*s) >= 'A' && (*s) <= 'F') || (*s) == 'X')) && hex))
			return 0;
		s++;
	}
	return 1;
}

void showHelp() {
	printf("CPUID - A command line wrapper for the CPUID instruction\n");
	printf("      - Part of CPUTOOLS. Copyright (c) Nathan Gill, under the Mozilla Public License v2.0.\n");
	printf("      - Type \"CPUTOOLS --HELP\" for more information\n");
	printf("USAGE\n");
	printf("	CPUID [OPTIONS]... <FUNCTION CODE> [SUBFUNCTION CODE]\n");
	printf("DESCRIPTION\n");
	printf("	FUNCTION CODE\n");
	printf("		Specifies the CPUID function (EAX) to call.\n");
	printf("		This must be less than the maximum valid function for this CPU\n");
	printf("		unless the ignore option is specified.\n");
	printf("	SUBFUNCTION CODE\n");
	printf("		Specifies the CPUID subfunction (ECX) to call. Defaults to 0 if not specified.\n");
	printf("	Either code can be specified in hexadecimal, rather than decimal, using the \'0x\' prefix.\n");
	printf("	OPTIONS\n");
	printf("		One or more of the following options:\n");
	printf("			-(a)scii	: Show ASCII string conversions. Little-endian byte order.\n");
	printf("			-(c)lean 	: Clean output, values only. Incompatible with other options.\n");
	printf("			-(h)elp		: Displays this message.\n");
	printf("			-(i)gnore	: Ignores invalid or out of range function codes.\n");
	printf("			-(v)erbose	: Enables verbose output.\n");
	printf("			-?			: Displays this message.\n");
	printf("EXAMPLES\n");
	printf("	CPUID 0\n");
	printf("		Returns the maximum supported CPUID function number in the EAX. Function: 0\n");
	printf("	CPUID 7 2\n");
	printf("		Returns extended feature flags in the EDX. Function: 7, Subfunction: 2\n");
}

void showReg(int reg, char* name, int verbose, int clean, int ascii, int noNewLine) {
	if (verbose)
		printf("	");
	if (!clean)
		printf("%s: 0x", name);
	printf("%08x", reg);
	if (ascii) {
		char regStr[5];
		regStr[3] = (reg >> 24) & 0xFF;
		regStr[2] = (reg >> 16) & 0xFF;
		regStr[1] = (reg >> 8) & 0xFF;
		regStr[0] = reg & 0xFF;
		regStr[4] = '\0';
		printf(" : \"%s\"", regStr);
	}
	
	if (!noNewLine)
		printf("\n");
}

void printRegs(cpuid_regs* regs, int clean, int ascii, int verbose) {
	if (verbose) {
		printf("CPUID call successful.\n");
		printf("Registers:\n");
	}
	showReg(regs->eax, "EAX", verbose, clean, ascii, 0);
	showReg(regs->ebx, "EBX", verbose, clean, ascii, 0);
	showReg(regs->ecx, "ECX", verbose, clean, ascii, 0);
	showReg(regs->edx, "EDX", verbose, clean, ascii, (clean) ? 1 : 0);
}

void toUpperCase(char* str) {
    while (*str) {
        *str = (unsigned char)toupper((unsigned int)*str);
        str++;
    }
}

int main(int argc, char* argv[]) {	
	// Too few arguments, show help
	if (argc < 2) {
		showHelp();
		return 1;
	}
	
	// Values that are set by the argument parser
	int clean = 0;
	int ascii = 0;
	int verbose = 0;
	int ignore = 0;
	
	// Indicies of codes in the argument array
	int functionCode = -1;
	int subfunctionCode = -1;
	
	if (argc > 1) {
		for (int i = 1; i < argc; i++) {
			// Load the argument, convert to uppercase
			char* s = argv[i];
			toUpperCase(s);
			
			// Is this value in hexadecimal?
			int hex = 0;
			if (s[0] == '0' && s[1] == 'X')
				hex = 1;
			
			// Check if it is numeric
			if (isNumeric(s, hex)) {
				// Is it a function code?
				if (functionCode == -1) {
					functionCode = i; 
					continue;
				}
				// Is it a subfunction code?
				else if (subfunctionCode == -1) {
					subfunctionCode = i; 
					continue;
				}
			}
			while (((*s) == '-' || (*s) == '/' || (*s) == ' ') && ((*s) != '\0')) { s++; }
			if (!strcmp(s, "C") || !strcmp(s, "CLEAN")) {
				if (ascii || verbose) {
					printf("Clean output is not compatible with the ASCII or verbose options set!\n");
					return 1;
				}
				clean = 1;
			}
			else if (!strcmp(s, "A") || !strcmp(s, "ASCII")) {
				if (clean) {
					printf("Clean output is not compatible with the ASCII or verbose options set!\n");
					return 1;
				}
				ascii = 1;
			}
			else if (!strcmp(s, "V") || !strcmp(s, "VERBOSE")) {
				if (clean) {
					printf("Clean output is not compatible with the ASCII or verbose options set!\n");
					return 1;
				}
				verbose = 1;
			}
			else if (!strcmp(s, "I") || !strcmp(s, "IGNORE")) {
				ignore = 1;
			}
			else if (!strcmp(s, "?") || !strcmp(s, "H") || !strcmp(s, "HELP")) {
				showHelp();
				return 1;
			}
			else if (verbose) {
				printf("Ignoring unknown argument \"%s\".\n", argv[i]);
			}
		}
	}
	
	if (functionCode == -1) {
		printf("Invalid function code!\n");
		showHelp();
		return 1;
	}
	
	uint32_t code = (uint32_t)myAtoi(argv[functionCode]);
	if (code == -1) {
		printf("Invalid function code code \"%s\"!\n", argv[1]);
		return 1;
	}
	
	if (verbose)
		printf("Setting function code to 0x%x...\n", code);
	
	uint32_t subcode = ((subfunctionCode == -1) ? 0 : (uint32_t)myAtoi(argv[subfunctionCode]));
	if (subcode == -1) {
		printf("Invalid subfunction code code \"%s\"!\n", argv[1]);
		return 1;
	}
	
	if (verbose) {
		printf("Setting subfunction code to 0x%x...\n", subcode);
		printf("Retrieving maximum function code for this CPU...\n");
	}
	
	cpuid_regs regs = {};
	cpuid(0, &regs);
	
	if (verbose)
		printf("Maximum function code is 0x%x.\n", regs.eax);
	
	if (code >= regs.eax && !ignore) {
		printf("Function code must be less than 0x%x!\n", regs.eax);
		return 1;
	}
	
	if (verbose)
		printf("Calling CPUID with EAX = 0x%x and ECX = 0x%x...\n", code, subcode);
	
	regs.eax = 0;
	regs.ebx = 0;
	regs.ecx = 0;
	regs.edx = 0;
	
	cpuidex(code, subcode, &regs);
	
	printRegs(&regs, clean, ascii, verbose);
	
	if (verbose)
		printf("Done.");
	
	return 0;
}