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

// ================================================================================================
// ================================================================================= printer_console
// ================================================================================================


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
   virtual bool print(const message& message);
   virtual bool flush();

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

// ================================================================================================
// ================================================================================= printer_file
// ================================================================================================

/**
 * \brief 
 *
 * note _wsopen_s , ::_write
 *
 \code
 \endcode
 */
class printer_file : public i_printer
{
   // ## construction -------------------------------------------------------------
public:
   printer_file() : m_stringSplit{ L"  " }, m_stringNewLine{ L"\n" } {}
   printer_file(const std::wstring_view& stringFileName) : m_stringFileName(stringFileName), m_stringSplit{ L"  " }, m_stringNewLine{ L"\n" } {}
   // copy
   printer_file( const printer_file& o ) { common_construct( o ); }
   printer_file( printer_file&& o ) noexcept { common_construct( o ); }
   // assign
   printer_file& operator=( const printer_file& o ) { common_construct( o ); return *this; }
   printer_file& operator=( printer_file&& o ) noexcept { common_construct( o ); return *this; }
   
   ~printer_file() { if( m_iFileHandle >= 0 ) file_close_s(m_iFileHandle); }
private:
   // common copy
   void common_construct( const printer_file& o ) {}
   void common_construct( printer_file&& o ) noexcept {}

// ## operator -----------------------------------------------------------------
public:


// ## override -----------------------------------------------------------------
public:
   virtual bool print(const message& message);
   virtual bool flush();


// ## methods ------------------------------------------------------------------
public:
/** \name GET/SET
*///@{
   void set_split_text(const std::wstring_view& stringSplitText) { m_stringSplit = stringSplitText;  }
//@}

/** \name OPERATION
*///@{
   /// checks if valid file handle, if handle is valid (above 0) then the file is open
   bool is_open() const { return m_iFileHandle >= 0; }

   // ## operations used to cover (wrap) text like "text" -> "[text]"
   void cover_text( std::wstring& stringText ) const;
   std::wstring get_cover_text( const std::wstring_view& stringText ) const;


   
//@}

protected:
/** \name INTERNAL
*///@{
   /*
   static std::pair<int, int> open_s(const std::wstring_view& stringFileName);
   static int write_s(int iFileHandle, const std::string_view& stringText);
   static int write_s(int iFileHandle, const std::wstring_view& stringText);
   */
   
//@}

public:
/** \name DEBUG
*///@{
   
//@}


// ## attributes ----------------------------------------------------------------
public:
   std::wstring m_stringFileName;   ///< file log information is written to
   std::wstring m_stringSplit;      ///< text put between messages.
   std::wstring m_stringNewLine;    ///< Text inserted at end of message (newline maybe ?)
   wchar_t m_wchBeginWrap = L'[';   ///< if text is wrapped then add this before text
   wchar_t m_wchEndWrap  = L']';    ///< If text is wrapped then add this after text
   int m_iFileHandle = -1;          ///< used as file handle to log file that is written to
   
   
// ## free functions ------------------------------------------------------------
public:
   static std::pair<int, std::string> file_open_s(const std::wstring_view& stringFileName);
   static std::pair<bool, std::string> file_write_s(int iFileHandle, const std::string_view& stringText);
   static std::pair<bool, std::string> file_write_s(int iFileHandle, const std::wstring_view& stringText, gd::utf8::utf8_tag );
   static void file_close_s(int iFileHandle);
   


};




_GD_LOG_LOGGER_END