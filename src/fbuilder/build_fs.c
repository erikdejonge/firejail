/*
 * Copyright (C) 2014-2017 Firejail Authors
 *
 * This file is part of firejail project
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "fbuilder.h"

// common file processing function, using the callback for each line in the file
static void process_file(const char *fname, const char *dir, void (*callback)(char *)) {
	assert(fname);
	assert(dir);
	assert(callback);
	
	int dir_len = strlen(dir);
	
	// process trace file
	FILE *fp = fopen(fname, "r");
	if (!fp) {
		fprintf(stderr, "Error: cannot open %s\n", fname);
		exit(1);
	}
	
	char buf[MAX_BUF];
	while (fgets(buf, MAX_BUF, fp)) {
		// remove \n
		char *ptr = strchr(buf, '\n');
		if (ptr)
			*ptr = '\0';
	
		// parse line: 4:galculator:access /etc/fonts/conf.d:0
		// number followed by :
		ptr = buf;
		if (!isdigit(*ptr))
			continue;
		while (isdigit(*ptr))
			ptr++;
		if (*ptr != ':')
			continue;
		ptr++;

		// next :
		ptr = strchr(ptr, ':');
		if (!ptr)
			continue;
		ptr++;
		if (strncmp(ptr, "access ", 7) == 0)
			ptr +=  7;
		else if (strncmp(ptr, "fopen ", 6) == 0)
			ptr += 6;
		else if (strncmp(ptr, "fopen64 ", 8) == 0)
			ptr += 8;
		else if (strncmp(ptr, "open64 ", 7) == 0)
			ptr += 7;
		else if (strncmp(ptr, "open ", 5) == 0)
			ptr += 5;
		else
			continue;
		if (strncmp(ptr, dir, dir_len) != 0)
			continue;

		// end of filename
		char *ptr2 = strchr(ptr, ':');
		if (!ptr2)
			continue;
		*ptr2 = '\0';
		
		callback(ptr);
	}
	
	fclose(fp);
}

// process fname, fname.1, fname.2, fname.3, fname.4, fname.5
static void process_files(const char *fname, const char *dir, void (*callback)(char *)) {
	assert(fname);
	assert(dir);
	assert(callback);
	
	// run fname
	process_file(fname, dir, callback);
	
	// run all the rest
	struct stat s;
	int i;
	for (i = 1; i <= 5; i++) {
		char *newname;
		if (asprintf(&newname, "%s.%d", fname, i) == -1)
			errExit("asprintf");
		if (stat(newname, &s) == 0)
			process_file(newname, dir, callback);
		free(newname);
	}
}

//*******************************************
// etc directory
//*******************************************
static FileDB *etc_out = NULL;

static void etc_callback(char *ptr) {
	// skip firejail directory
	if (strncmp(ptr, "/etc/firejail", 13) == 0)
		return;

	// add only top files and directories
	ptr += 5;	// skip "/etc/"
	char *end = strchr(ptr, '/');
	if (end)
		*end = '\0';
	etc_out = filedb_add(etc_out, ptr);
}

void build_etc(const char *fname) {
	assert(fname);
	
	process_files(fname, "/etc", etc_callback);
	
	printf("private-etc ");
	if (etc_out == NULL)
		printf("none\n");
	else {
		FileDB *ptr = etc_out;
		while (ptr) {
			printf("%s,", ptr->fname);
			ptr = ptr->next;
		}
		printf("\n");
	}	
}

//*******************************************
// var directory
//*******************************************
static FileDB *var_out = NULL;
static void var_callback(char *ptr) {
	if (strcmp(ptr, "/var/lib") == 0)
		;
	else if (strcmp(ptr, "/var/cache") == 0)
		;
	else if (strncmp(ptr, "/var/lib/menu-xdg", 17) == 0)
		var_out = filedb_add(var_out, "/var/lib/menu-xdg");
	else if (strncmp(ptr, "/var/cache/fontconfig", 21) == 0)
		var_out = filedb_add(var_out, "/var/cache/fontconfig");
	else
		var_out = filedb_add(var_out, ptr);
}

void build_var(const char *fname) {
	assert(fname);

	process_files(fname, "/var", var_callback);
	
	if (var_out == NULL)
		printf("blacklist /var\n");
	else
		filedb_print(var_out, "whitelist ");
}

//*******************************************
// tmp directory
//*******************************************
static FileDB *tmp_out = NULL;
static void tmp_callback(char *ptr) {
	filedb_add(tmp_out, ptr);
}

void build_tmp(const char *fname) {
	assert(fname);
	
	process_files(fname, "/tmp", tmp_callback);
	
	if (tmp_out == NULL)
		printf("private-tmp\n");
	else {
		printf("\n");
		printf("# private-tmp\n");
		printf("# File accessed in /tmp directory:\n");
		printf("# ");
		FileDB *ptr = tmp_out;
		while (ptr) {
			printf("%s,", ptr->fname);
			ptr = ptr->next;
		}
		printf("\n");
	}
}

//*******************************************
// dev directory
//*******************************************
static char *dev_skip[] = {
	"/dev/zero",
	"/dev/null",
	"/dev/full",
	"/dev/random",
	"/dev/urandom",
	"/dev/tty",
	"/dev/snd", 
	"/dev/dri",
	"/dev/pts",
	"/dev/nvidia0",
	"/dev/nvidia1",
	"/dev/nvidia2",
	"/dev/nvidia3",
	"/dev/nvidia4",
	"/dev/nvidia5",
	"/dev/nvidia6",
	"/dev/nvidia7",
	"/dev/nvidia8",
	"/dev/nvidia9",
	"/dev/nvidiactl",
	"/dev/nvidia-modeset",
	"/dev/nvidia-uvm",
	"/dev/video0",
	"/dev/video1",
	"/dev/video2",
	"/dev/video3",
	"/dev/video4",
	"/dev/video5",
	"/dev/video6",
	"/dev/video7",
	"/dev/video8",
	"/dev/video9",
	"/dev/dvb",
	"/dev/sr0",
	NULL
};

static FileDB *dev_out = NULL;
static void dev_callback(char *ptr) {
	// skip private-dev devices
	int i = 0;
	int found = 0;
	while (dev_skip[i]) {
		if (strcmp(ptr, dev_skip[i]) == 0) {
			found = 1;
			break;
		}
		i++;
	}
	if (!found)
		filedb_add(dev_out, ptr);
}

void build_dev(const char *fname) {
	assert(fname);
	
	process_files(fname, "/dev", dev_callback);
	
	if (dev_out == NULL)
		printf("private-dev\n");
	else {
		printf("\n");
		printf("# private-dev\n");
		printf("# This is the list of devices accessed (on top of regular private-dev devices:\n");
		printf("# ");
		FileDB *ptr = dev_out;
		while (ptr) {
			printf("%s,", ptr->fname);
			ptr = ptr->next;
		}
		printf("\n");
	}
}

