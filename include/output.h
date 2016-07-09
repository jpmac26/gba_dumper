#include <stdio.h>

#ifndef COMMON_H
#define COMMON_H
	#include "common.h"
#endif

#ifndef SEARCHING_H
#define SEARCHING_H
	#include "searching.h"
#endif

void print_buffer_contents_f( rom_file*, unsigned long );
void print_match_list( rom_file*, match_info*, int, int );
void print_buffer_as_bytes( FILE*, unsigned char*, long );