#include <fstream>
#include <string>

#include <FVMCode/input.h>

#include "test_helpers.h"

using namespace FVMCode;

int comment_skipping_01 (int, char **)
{
    Input::comment_istream infile ("comment_skipping_01/input.txt");

    std::cout << "Init" << std::endl;

    unsigned int input;
    infile >> input;
    std::cout << "Read: " << input << std::endl;
    AssertTest (input == 256);

    infile >> input;
    std::cout << "Read: " << input << std::endl;
    AssertTest (input == 512);

    int int_input;
    infile >> int_input;
    std::cout << "Read: " << int_input << std::endl;
    AssertTest (int_input == -1024);

    std::string word;
    infile >> word;
    std::cout << "Read: " << word << std::endl;
    AssertTest (word == "word");

    double number;
    infile >> number;
    std::cout << "Read: " << number << std::endl;
    AssertTest (number == 43.2);

    infile >> word;
    std::cout << "Read: " << word << std::endl;
    AssertTest (word == "hello");

    infile >> number;
    std::cout << "Read: " << number << std::endl;
    AssertTest (number == -0.43);
    
    return EXIT_SUCCESS;
}