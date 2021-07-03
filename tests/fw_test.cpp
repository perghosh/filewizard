// filewizard.cpp : Defines the entry point for the application.
//



#include <iostream>

#define CATCH_CONFIG_RUNNER
#include "catch2/catch.hpp"


#include "gd_utf8.hpp"
#include "gd_utf8_string.hpp"

#include "fw_test.h"

using namespace std;

int main(int iArgumentCount, char* ppbszArgumentValue[] )
{
	//std::cout << typeid( gd::utf8::string::iterator::iterator_category).name() << std::endl;

	int iResult = Catch::Session().run(iArgumentCount, ppbszArgumentValue);

	return iResult;
}
