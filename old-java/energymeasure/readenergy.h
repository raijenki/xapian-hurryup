/*    Copyright 2014-2015 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/


/*
 * readenergy.c 
 *
 * Reads APB energy registers in Juno and outputs the measurements (converted to appropriate units).
 *
*/
#pragma once
#include <stdio.h>

struct emeter
{
	int fd;
	FILE *out;
	void *mmap_base;
};

struct reading
{
	double sys_enm_ch0_sys;
	double sys_enm_ch1_a57;
	double sys_enm_ch0_a53;
};

extern void emeter_init(struct emeter *, char *outfile);
extern void emeter_finalize(struct emeter *);
extern void emeter_read_measurements(struct emeter *,
                                     struct reading *);
extern void emeter_take_reading(struct emeter *);

