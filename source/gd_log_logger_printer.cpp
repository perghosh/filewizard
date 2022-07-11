#include "gd_log_logger_printer.h"

_GD_LOG_LOGGER_BEGIN

std::mutex& printer_get_mutex_g()
{
   static std::mutex mutexPrinter;
   return mutexPrinter;
}


void printer_console::print(const message& message)
{
   std::wstring stringMessage = message.to_wstring();

#ifdef _WIN32
   if( m_bConsole == true )
   {
      ::WriteConsoleW(m_hOutput, stringMessage.c_str(), static_cast<DWORD>(stringMessage.size()), NULL, NULL);
   }
   else
   {
      m_wostreamOutput << stringMessage << std::flush;
   }
#else
   m_outputStream << str << std::flush;
#endif
}

void printer_console::flush()
{

}


_GD_LOG_LOGGER_END
