#include <stdio.h>
#include <unistd.h>

#include <iostream>
#include <fstream>
#include <iomanip>

#include "../PCA9685.h"

// #define CLI_DEBUG 2

#define PORT_1  0
#define PORT_2  1
#define PORT_3  2
#define PORT_4  3
#define PORT_5  4
#define PORT_6  5

enum    Errors {OK, PORT_OPEN_ERROR, ERR_PARSE, ERR_FILE};
int     loadBootStates(std::vector<int> &bootStates);
int     parseFields(const std::string sIn, std::vector<std::string> &svFields, char cSeparator);
