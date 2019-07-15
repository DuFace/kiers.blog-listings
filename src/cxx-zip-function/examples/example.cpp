//==============================================================================
// Demonstration of the zip function usage.
//------------------------------------------------------------------------------
// Copyright (c) 2019 Kier Dugan.
// MIT Licence, see LICENCE file for details.
//==============================================================================

#include "zip.hpp"
using namespace kjd;

#include <iostream>
#include <string>
#include <vector>
using namespace std;


//==============================================================================
int main(int argc, char **argv)
{
    cout << "Hello, World!" << endl;

    vector<string> labels = { "first", "second", "third", "fourth", "fifth" };
    vector<int> odds  = { 1, 3, 5, 7, 9  };
    vector<int> evens = { 2, 4, 6, 8, 10 };

    for (const auto &[label, odd, even] : zip(labels, odds, evens))
        cout << label << ": " << odd << ", " << even << endl;

    return 0;
}
