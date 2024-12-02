#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

void toUpperCase(char* str) {
    while (*str) {
        *str = (unsigned char)toupper((unsigned int)*str);
        str++;
    }
}

void showHelp() {
	printf("CPUTOOLS - A CPU information gathering suite\n");
	printf("         - Copyright (c) Nathan Gill, under the Mozilla Public License v2.0.\n");
	printf("USAGE\n");
	printf("	CPUTOOLS [OPTIONS]\n");
	printf("DESCRIPTION\n");
	printf("	OPTIONS\n");
	printf("		One of the options below:\n");
	printf("			-(h)elp    - Displays this message.\n");
	printf("			-(l)icense - Displays license information.\n");
	printf("			-?         - Displays this message.\n");
	printf("TOOLS\n");
	printf("	CPUID - A command line wrapper for the CPUID instruction.\n");
	printf("		Type \"CPUID --HELP\" for more information.\n");
}

void showLicense() {
	FILE* f = fopen("LICENSE", "r");
	if (!f) {
		printf("License file corrupt or missing. Please reinstall CPUTOOLS.\n");
		return;
	}
	
	int c;
	while ((c = fgetc(f)) != EOF)
		putchar((char)c);
		
	fclose(f);
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		showHelp();
		return 1;
	}
	
	for (int i = 1; i < argc; i++) {
		char* s = argv[i];
		toUpperCase(s);
		
		while (((*s) == '-' || (*s) == '/' || (*s) == ' ') && ((*s) != '\0')) { s++; }
		
		if (!strcmp(s, "H") || !strcmp(s, "HELP") || !strcmp(s, "?")) {
			showHelp();
			return 0;
		}
		else if (!strcmp(s, "L") || !strcmp(s, "LICENSE")) {
			showLicense();
			return 0;
		}
		else {
			showHelp();
			return 1;
		}
	}
	
	return 0;	
}