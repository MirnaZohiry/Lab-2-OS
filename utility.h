/*
 * MyShell Project for SOFE 3950U / CSCI 3020U: Operating Systems
 *
 * Copyright (C) 2015, <Mirna Mohamed, Mohannad Naser Abdo, Luisa Rojas-Garcia>
 * All rights reserved.
 * 
 */

#ifndef UTILITY_H_
#define UTILITY_H_

// Include your relevant functions declarations here they must start with the 
// extern keyword such as in the following example:
// extern void display_help(void);

extern void parse(char *line, char **argv);
extern void execute(char **argv);

#endif /* UTILITY_H_ */