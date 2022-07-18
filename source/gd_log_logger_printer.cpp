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
void printer_console::print(const message& message)
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
}

void printer_console::flush()
{
   if( m_uMessageCounter > 0 )                                                   // one or more messages printed?
   {
      print(std::wstring_view{ L"\n" });
   }

   m_uMessageCounter = 0;
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
void printer_file::print(const message& message)
{
   std::wstring stringMessage;

   if( is_open() == false )
   {
      auto [iFileHandle, stringError] = file_open_s(m_stringFileName);
      m_iFileHandle = iFileHandle;
       
      if( is_open() == false )                                                   // still not open? then internal error
      {
         // TODO: manage error, get information from string and 
         return;
      }
   }

   if( message.is_message_type_set() == true )
   {
      if( message.is_severity() == true )
      {
         gd::utf8::convert_utf8_to_uft16((const uint8_t*)severity_get_name_g(message.get_severity_number()), stringMessage);
         stringMessage += m_stringSplit;
      }

      if( message.is_time() == true )
      {
         std::wstring stringTime = message::get_now_time_as_wstring_s();
         stringMessage += stringTime;
      }
      else if( message.is_date() == true )
      {
         std::wstring stringDate = message::get_now_date_as_wstring_s();
         stringMessage += stringDate;
      }

      if( stringMessage.empty() == false ) { stringMessage += m_stringSplit; }   // if text then add split section to divide for next text in log message
   }

   if( stringMessage.empty() == false )
   {
      auto [bOk, stringError] = file_write_s(m_iFileHandle, stringMessage, gd::utf8::utf8_tag{});
      if( bOk == false )
      {
         // TODO: manage error, get information from string and 
         return;
      }
   }


   const char* pbszMessage = message.get_text();

   auto [ bOk, stringError] = file_write_s(m_iFileHandle, pbszMessage);
   if( bOk == false )
   {
      // TODO: manage error, get information from string and 
      return;
   }


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
   {                                                                             assert( false );
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

_GD_LOG_LOGGER_END
