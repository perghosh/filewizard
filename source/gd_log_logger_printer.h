#pragma once

#include <mutex>
#include <iostream>
#include <corecrt_io.h>
#include <string_view>

#ifdef _WIN32
#include "windows.h"
#endif"

#include "gd_log_logger.h"

#ifndef _GD_LOG_LOGGER_BEGIN

#  define _GD_LOG_LOGGER_BEGIN namespace gd::log {
#  define _GD_LOG_LOGGER_END }

#endif


_GD_LOG_LOGGER_BEGIN

extern std::mutex& printer_get_mutex_g();

/**
 * \brief 
 *
 *
 *
 \code
 \endcode
 */
class printer_console : public i_printer
{
public:
   enum class enumOutput { eOutputStdOut, eOutputStdErr };
// ## construction -------------------------------------------------------------
public:
#ifdef _WIN32
   printer_console(): printer_console( enumOutput::eOutputStdOut ) {}
   printer_console(enumOutput eOutput)
      : m_bConsole(!!_isatty(_fileno(eOutput == enumOutput::eOutputStdOut ? stdout : stderr)))
      , m_wostreamOutput(eOutput == enumOutput::eOutputStdOut ? std::wcout : std::wcerr)
      , m_hOutput()
   {
      if( m_bConsole == true )
      {
         m_hOutput = ::GetStdHandle(eOutput == enumOutput::eOutputStdOut ? STD_OUTPUT_HANDLE : STD_ERROR_HANDLE);
      }
   }
#else
   printer_console(enumOutput eOutput)
      : m_bConsole(!!_isatty(_fileno(eOutput == enumOutput::eOutputStdOut ? stdout : stderr)))
      , m_wostreamOutput(eOutput == enumOutput::eOutputStdOut ? std::wcout : std::wcerr)
   {}
#endif
   // copy
   printer_console( const printer_console& o ): m_wostreamOutput(o.m_wostreamOutput) { common_construct( o ); }
   printer_console( printer_console&& o ) noexcept : m_wostreamOutput(o.m_wostreamOutput) { common_construct( o ); }
   
	~printer_console() {}
private:
   // common copy


   void common_construct( const printer_console& o ) {
      m_bConsole = o.m_bConsole;
#ifdef _WIN32
      m_hOutput = o.m_hOutput;
#endif
   }
   void common_construct( printer_console&& o ) noexcept {
      m_bConsole = o.m_bConsole;
#ifdef _WIN32
      m_hOutput = o.m_hOutput;
#endif
   }

// ## operator -----------------------------------------------------------------
public:
   

// ## methods ------------------------------------------------------------------
public:
/** \name GET/SET
*///@{
   
//@}

/** \name OPERATION
*///@{
   virtual void print(const message& message);
   virtual void flush();

   void print(const std::wstring_view& stringMessage);
//@}

protected:
/** \name INTERNAL
*///@{
   
//@}

public:
/** \name DEBUG
*///@{
   
//@}


// ## attributes ----------------------------------------------------------------
public:
   bool     m_bConsole;                ///< if true then write to console output
   unsigned m_uMessageCounter = 0;     ///< number of messages needed to flush (when flush is called this is reset to 0)
   std::wostream& m_wostreamOutput;
#ifdef _WIN32
   HANDLE m_hOutput;                   ///< handle to console in windows
#endif
   
   
// ## free functions ------------------------------------------------------------
public:


};

/**
 * \brief 
 *
 *
 *
 \code
 \endcode
 */
class printer_file 
{
// ## construction -------------------------------------------------------------
public:
   printer_file() {}
   // copy
   printer_file( const printer_file& o ) { common_construct( o ); }
   printer_file( printer_file&& o ) noexcept { common_construct( o ); }
   // assign
   printer_file& operator=( const printer_file& o ) { common_construct( o ); return *this; }
   printer_file& operator=( printer_file&& o ) noexcept { common_construct( o ); return *this; }
   
	~printer_file() {}
private:
   // common copy
   void common_construct( const printer_file& o ) {}
   void common_construct( printer_file&& o ) noexcept {}

// ## operator -----------------------------------------------------------------
public:
   

// ## methods ------------------------------------------------------------------
public:
/** \name GET/SET
*///@{
   
//@}

/** \name OPERATION
*///@{
   void test();
//@}

protected:
/** \name INTERNAL
*///@{
   
//@}

public:
/** \name DEBUG
*///@{
   
//@}


// ## attributes ----------------------------------------------------------------
public:
   
   
// ## free functions ------------------------------------------------------------
public:
   


};




_GD_LOG_LOGGER_END