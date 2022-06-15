#pragma once

#include <cassert>
#include <string>
#include <string_view>
#include <vector>
#include <type_traits>


#ifndef _GD_CALCULATE_BEGIN
namespace gd::calculate {
#else
_GD_CALCULATE_BEGIN
#endif

/**
 * \brief 
 *
 *
 *
 \code
 \endcode
 */
class calculate_parse 
{
// ## construction -------------------------------------------------------------
public:
   calculate_parse() {}
   // copy
   calculate_parse( const calculate_parse& o ) { common_construct( o ); }
   calculate_parse( calculate_parse&& o ) noexcept { common_construct( o ); }
   // assign
   calculate_parse& operator=( const calculate_parse& o ) { common_construct( o ); return *this; }
   calculate_parse& operator=( calculate_parse&& o ) noexcept { common_construct( o ); return *this; }
   
	~calculate_parse() {}
private:
   // common copy
   void common_construct( const calculate_parse& o ) {}
   void common_construct( calculate_parse&& o ) noexcept {}

// ## operator -----------------------------------------------------------------
public:
   

// ## methods ------------------------------------------------------------------
public:
/** \name GET/SET
*///@{
   
//@}

/** \name OPERATION
*///@{
   
//@}

protected:
/** \name INTERNAL
*///@{
   
//@}

/** \name INTERNAL FREE FUNCTIONS
*///@{
   std::pair<unsigned, const char*> get_token_s(const char* pbszPosition);
//@}


// ## attributes ----------------------------------------------------------------
public:
   


};



} // namespace _GD_CALCULATE_BEGIN