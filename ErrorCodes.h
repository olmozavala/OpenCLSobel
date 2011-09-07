#ifndef _ERRCODES_H
#define _ERRCODES_H

#include <iostream>
#include <CL/cl.h>
#include <string.h>

using namespace std;
class ErrorCodes{
    public:
        ErrorCodes(){};
        char* print_cl_error(cl_int err);
};

#endif
