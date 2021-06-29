// filewizard.cpp : Defines the entry point for the application.
//

#include <iostream>

#include "gd_utf.hpp"
#include "filewizard.h"

using namespace std;

int main()
{
	cout << "Hello CMake." << gd::utf8::strlen("test") << gd::utf8::strlen((const char8_t*)"test") << endl;


	const char* p1 = "01234567890123456789";

	p1 = gd::utf8::move::next(p1);
	p1 = gd::utf8::move::next(p1);
	p1 = gd::utf8::move::next(p1);
	p1 = gd::utf8::move::next(p1);
	p1 = gd::utf8::move::next(p1);
	p1 = gd::utf8::move::previous(p1);
	p1 = gd::utf8::move::previous(p1);

	p1 = gd::utf8::move::next(p1, 5);
	p1 = gd::utf8::move::next(p1, 1);
	p1 = gd::utf8::move::previous(p1, 3);



	return 0;
}
