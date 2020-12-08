#include <iostream>

#include "Differentiator.hpp"

int main()
{
    Differentiator differentiator;
    differentiator();
    differentiator.dumpToText();
    differentiator.dumpToDot();

    differentiator.dtor();
    return EXIT_SUCCESS;
}
