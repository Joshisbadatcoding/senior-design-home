#include "password.h"

bool checkPass(char buffer[], const char password[])
{
    if(std::strcmp(buffer, password) == 0)
        return true;
    else
        return false;

}