#include "ctype.h"

bool islower(char chr)
{
    // check if a character is a lowercase character
    return chr >= 'a' && chr <= 'z';
}

char toupper(char chr)
{
    //convert a lowercase to a uppercase letter
    return islower(chr) ? (chr - 'a' + 'A') : chr;
}
