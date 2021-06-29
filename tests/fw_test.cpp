// filewizard.cpp : Defines the entry point for the application.
//



#include <iostream>

#define CATCH_CONFIG_RUNNER
#include "catch2/catch.hpp"


#include "gd_utf.hpp"

#include "fw_test.h"

using namespace std;

int main(int iArgumentCount, char* ppbszArgumentValue[] )
{
	//std::cout << "Hello CMake." << gd::utf8::strlen("test") << gd::utf8::strlen((const char8_t*)"test") << endl;

	const char* p = "12345";
	const char** pp = &p;


	int result = Catch::Session().run(iArgumentCount, ppbszArgumentValue);

	/*

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

	*/

	//std::ostream& Catch::operator <<(std::ostream&, const Catch::StringRef&)
	//std::ostream& Catch::operator <<(std::ostream&, const Catch::StringRef&)


	//..\..\..\libraries\catch2\catch_amalgamated.cpp(5118) : note: could be 'std::ostream &Catch::operator <<(std::ostream &,const Catch::StringRef &)'
	//	C : \_dev\c\cmake\filewizard\libraries\catch2\catch_amalgamated.hpp(730) : note: or 'std::ostream &Catch::operator <<(std::ostream &,const Catch::StringRef &)'[found using argument - dependent lookup]

	return 0;
}
