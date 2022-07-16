#include <chrono>

#include "gd_log_logger_printer.h"

_GD_LOG_LOGGER_BEGIN

std::mutex& printer_get_mutex_g()
{
   static std::mutex mutexPrinter;
   return mutexPrinter;
}


void printer_console::print(const message& message)
{
   std::wstring stringMessage;

   if( message.is_message_type_set() == true )
   {
      if( message.is_time() == true )
      {
         auto time_pointNow = std::chrono::system_clock::now();
         std::time_t timeNow = std::chrono::system_clock::to_time_t( time_pointNow );
         std::wstring stringTime(30, '\0');
         std::wcsftime(&stringTime[0], stringTime.size(), L"%Y-%m-%d %H:%M:%S", std::localtime(&timeNow));
         stringMessage += stringTime;
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


_GD_LOG_LOGGER_END
