/**
 * \file gd_log_logger_printer.h
 * 
 * \brief Modify logger output by selecting one or some of the "Printer" classes found here.
 * 
 * Generate log output needs one or more printers used to transform log messages
 * to some sort of readable log information. 
 * Each logger created can have one or more printers attached. Messages sent to
 * logger is spread to these attached printers.
 * 
 */

/*

|  |  |
| - | - |
|  |  |
|  |  |






*/




#include <chrono>
#include "io.h"
#include <fcntl.h>
#include <sys/stat.h>

#include "gd_utf8.hpp"

#include "gd_log_logger_printer.h"

_GD_LOG_LOGGER_BEGIN

std::mutex& printer_get_mutex_g()
{
   static std::mutex mutexPrinter;
   return mutexPrinter;
}

// ================================================================================================
// ================================================================================= printer_console
// ===================================================================0=======2q1======================


/*----------------------------------------------------------------------------- print */ /**
 * print is overridden from i_print and is called when logger prints something and sends it
 * to attached printers. Here `printer_console` converts information to text and sends it to the console.
 * \param message printed message
 */
bool printer_console::print(const message& message)
{
   std::wstring stringMessage;

   if( message.is_message_type_set() == true )
   {
      if( message.is_severity() == true )
      {
         gd::utf8::convert_utf8_to_uft16( (const uint8_t*)severity_get_name_g( message.get_severity_number() ), stringMessage );
         stringMessage += std::wstring_view{ L"  " };
      }

      if( message.is_time() == true )
      {
         std::wstring stringTime = message::get_now_time_as_wstring_s();
         stringMessage += stringTime;
         stringMessage += std::wstring_view{ L"  " };
      }
      else if( message.is_date() == true )
      {
         std::wstring stringDate = message::get_now_date_as_wstring_s();
         stringMessage += stringDate;
         stringMessage += std::wstring_view{ L"  " };
      }
   }
   
   stringMessage += message.to_wstring();

   if( m_uMessageCounter > 0 ) print(std::wstring_view{ L"  " });                // print separator if there have been more messages before flush method is called

   print( stringMessage );

   return true;
}

bool printer_console::flush()
{
   if( m_uMessageCounter > 0 )                                                   // one or more messages printed?
   {
      print(std::wstring_view{ L"\n" });
   }

   m_uMessageCounter = 0;
   return true;
}

void printer_console::print(const std::wstring_view& stringMessage)
{
#ifdef _WIN32
   if( m_bConsole == true )
   {
      ::WriteConsoleW(m_hOutput, stringMessage.data(), static_cast<DWORD>(stringMessage.size()), NULL, NULL);

#     ifdef _DEBUG
      ::OutputDebugStringW(stringMessage.data());
#     endif // _DEBUG
   }
   else
   {
      m_wostreamOutput << stringMessage << std::flush;
   }
#else
   m_outputStream << str << std::flush;
#endif

   m_uMessageCounter++;                                                          // add message counter, number of messages before flush is called
}

// ================================================================================================
// ================================================================================= printer_file
// ================================================================================================

/*----------------------------------------------------------------------------- print */ /**
 * print is overridden from i_print and is called when logger prints something and sends it
 * to attached printers. Here `printer_console` converts information to text and sends it to the console.
 * \param message printed message
 */
bool printer_file::print(const message& message)
{
   std::wstring stringMessage;

   if( is_open() == false )                                                      // check if file has been opened, if not then open file
   {
      if( is_error(eErrorOpenFile) == true ) return true;

      auto [iFileHandle, stringError] = file_open_s(m_stringFileName);
      m_iFileHandle = iFileHandle;
       
      if( is_open() == false )                                                   // still not open? then internal error
      {
         // ## Failed to open log file, generate error message for `logger`, `logger` may fetch this using `error` method
         m_uInternalError |= eErrorOpenFile;                                     // set internal error state
         m_messageError.set_severity(eSeverityError);                            // mark message as **error**
         m_messageError << "failed to create or open log file. log file name is \"" << m_stringFileName << "\""; // error message
         return false;
      }
   }

   if( message.is_message_type_set() == true )                                   // check message "if type of message" is set, then go through message settings to add fixed information
   {
      if( message.is_severity() == true )                                        // is severity set ?
      {
         gd::utf8::convert_utf8_to_uft16((const uint8_t*)severity_get_name_g(message.get_severity_number()), stringMessage);
         cover_text( stringMessage );
         stringMessage += m_stringSplit;
      }

      if( message.is_time() == true )                                            // add time ?
      {
         std::wstring stringTime = message::get_now_time_as_wstring_s();
         stringMessage += get_cover_text(stringTime);
         stringMessage += m_stringSplit;
      }
      else if( message.is_date() == true )                                       // add date ?
      {
         std::wstring stringDate = message::get_now_date_as_wstring_s();
         stringMessage += get_cover_text(stringDate);
         stringMessage += m_stringSplit;
      }
   }

   // ## write message text to file there is any text to write

   if( stringMessage.empty() == false )
   {
      auto [bOk, stringError] = file_write_s(m_iFileHandle, stringMessage, gd::utf8::utf8_tag{});
      if( bOk == false )
      {
         // TODO: manage error, get information from string and 
         return false;
      }
   }


   const char* pbszMessage = message.get_text();

   auto [ bOk, stringError] = file_write_s(m_iFileHandle, pbszMessage);
   if( bOk == false )
   {
      // TODO: manage error, get information from string and 
      return false;
   }

   return true;
}

bool printer_file::flush()
{
   char pbsz[3];
   if( is_open() == true && m_stringNewLine.empty() == false )
   {
      if( m_stringNewLine.length() < 3 )
      {
         pbsz[0] = (char)m_stringNewLine[0];
         pbsz[1] = (char)m_stringNewLine[1];
         pbsz[2] = 0;
         file_write_s(m_iFileHandle, pbsz);
      }
   }

   return true;
}

unsigned printer_file::error(message& message)
{
   if( m_messageError.empty() == false )
   {
      message = std::move(m_messageError);
      return 1;
   }
   return 0;
}



/*----------------------------------------------------------------------------- cover_text */ /**
 * cover or wrap text within characters to make it nicer to view
 * \param stringText text to wrap within start and end character
 */
void printer_file::cover_text(std::wstring& stringText) const
{
   stringText = message::wrap_s(m_wchBeginWrap, stringText, m_wchEndWrap);
}

/*----------------------------------------------------------------------------- get_cover_text */ /**
 * cover or wrap text within characters to make it nicer to view
 * \param stringText text to wrap within start and end character
 * \return std::wstring wrapped text
 */
std::wstring printer_file::get_cover_text(const std::wstring_view& stringText) const
{
   std::wstring stringWrappedText = message::wrap_s(m_wchBeginWrap, stringText, m_wchEndWrap);
   return stringWrappedText;
}



/*----------------------------------------------------------------------------- open_s */ /**
 * open selected log file log information is written to
 * \param stringFileName name of log file to open
 * \return std::pair<int, std::string> if ok (first is valid filehandle), then no string information. otherwise return error imformation in string
 */
std::pair<int, std::string> printer_file::file_open_s(const std::wstring_view& stringFileName)
{                                                                                assert( stringFileName.length() > 3 ); // realistic filename
   // TODO: lock this (thread safety)

   int iFileHandle = 0;
#  if defined(_WIN32)
   ::_wsopen_s(&iFileHandle, stringFileName.data(), _O_CREAT | _O_WRONLY | _O_BINARY | _O_NOINHERIT, _SH_DENYWR, _S_IREAD | _S_IWRITE);
#     else
   iFileHandle = ::open(stringFileName->data(), O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
#  endif

   if( iFileHandle < 0 )
   {                                                                             // assert( false );
      std::string stringError("FILE OPEN ERROR: ");
      stringError += std::strerror(errno);
      return { iFileHandle, stringError };
   }

   return { iFileHandle, std::string() };
}

std::pair<bool, std::string> printer_file::file_write_s(int iFileHandle, const std::string_view& stringText)
{
   // TODO: lock this (thread safety)

   int iWriteCount = ::_write( iFileHandle, (const void*)stringText.data(), (unsigned int)stringText.length() );
   if( iWriteCount != (int)stringText.length() )
   {                                                                             assert( false );
      std::string stringError("FILE WRITE ERROR: ");

      stringError += std::strerror(errno);
      return { false, stringError };
   }

   return { true, std::string() };

}

/*----------------------------------------------------------------------------- file_write_s */ /**
 * write unicode text to file but before writing it will convert unicode text to utf8
 * \param iFileHandle file handle to file written to
 * \param stringText unicode text to write
 * \return std::pair<bool, std::string> true if ok, otherwise false and error information
 */
std::pair<bool, std::string> printer_file::file_write_s(int iFileHandle, const std::wstring_view& stringText, gd::utf8::utf8_tag)
{                                                                                assert( iFileHandle >= 0 );
   enum { eBufferSize = 100 };
   char pBuffer[eBufferSize];
   std::unique_ptr<char> pHeap;
   char* pbszUtf8Text = pBuffer;

   // ## convert unicode text to utf8
   auto uUtf8Size = gd::utf8::size(stringText.data());                           // how big buffer is needed to store unicode as utf8 text
   uUtf8Size++;                                                                  // make room for zero terminator
   if( uUtf8Size > static_cast<decltype(uUtf8Size)>(eBufferSize) )
   {  // cant fit in local buffer, allocate on heap
      pHeap.reset(new char[uUtf8Size]);
      pbszUtf8Text = pHeap.get();
   }

   gd::utf8::convert_unicode(stringText.data(), pbszUtf8Text, pbszUtf8Text + uUtf8Size );

   return file_write_s( iFileHandle, pbszUtf8Text );
}

void printer_file::file_close_s(int iFileHandle)
{
   ::_close( iFileHandle );
}

std::pair<bool, std::string> printer_file::file_backup_s(const std::string_view& stringFileName, const std::string_view& stringFormat, int iRotateCount)
{


   return { true, "" };
}

_GD_LOG_LOGGER_END
