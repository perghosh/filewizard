#include <cassert>
#include <cstdarg>
#include <stdarg.h>

#include "gd_log_logger.h"


_GD_LOG_LOGGER_BEGIN

const char* severity_get_name_g(enumSeverity eSeverity)
{
   switch( eSeverity )
   {
   case enumSeverity::eSeverityFatal:    return "FATAL";
   case enumSeverity::eSeverityError:    return "ERROR";
   case enumSeverity::eSeverityWarning:  return "WARNING";
   case enumSeverity::eSeverityInfo:     return "INFO";
   case enumSeverity::eSeverityDebug:    return "DEBUG";
   case enumSeverity::eSeverityVerbose:  return "VERBOSE";
   default:                              return "NONE";
   }
}

enumSeverity severity_get_number_g(const std::string_view& stringSeverity)
{                                                                                assert( stringSeverity.empty() == false );
   switch( std::toupper( stringSeverity[0] ) )
   {
   case 'F': return enumSeverity::eSeverityFatal;
   case 'E': return enumSeverity::eSeverityError;
   case 'W': return enumSeverity::eSeverityWarning;
   case 'I': return enumSeverity::eSeverityInfo;
   case 'D': return enumSeverity::eSeverityDebug;
   case 'V': return enumSeverity::eSeverityVerbose;
   default:  return enumSeverity::eSeverityNone;
   }
}

gd::log::message& message::printf(const char* pbszFormat, ...)
{
   va_list va_listArgument;
   va_start(va_listArgument, pbszFormat);                                        // initialize va_list (where to start)

   char* pbszOldText = nullptr;

   // if text is set, then store it and append this later
   if( m_pbszText != nullptr ) pbszOldText = m_pbszText;


   int iLength = _vscprintf(pbszFormat, va_listArgument);                        // calculate needed length for buffer
   if( iLength < 0 ) { assert(false); return *this; }
   iLength++;                                                                    // add zero terminator

   m_pbszText = new_s( iLength );
   m_pbszText[0] = '\0';

   int iReturn = _vsnprintf_s(m_pbszText, iLength, size_t(iLength) - 1, pbszFormat, va_listArgument); // generate message text
   va_end(va_listArgument);

   // ## convert to utf8 if needed, text is stored in internally in utf8 format
   auto uUtf8Length = gd::utf8::size(m_pbszText) + 1;                            assert( uUtf8Length >= (uint32_t)iLength ); // if less something is seriously wrong
   if( uUtf8Length > (uint32_t)iLength )                                         // do we need to convert to utf8
   {
      char* pbsz = new_s( uUtf8Length );                                         // create new buffer
      gd::utf8::convert_ascii(m_pbszText, pbsz);                                 // convert to utf8
      clear_s( &m_pbszText );                                                    // delete old buffer
      m_pbszText = pbsz;                                                         // set to new buffer
   }

   if( pbszOldText != nullptr )
   {
      m_pbszText = join_s(&pbszOldText, std::string_view{"  "}, &m_pbszText);
      clear_s(&pbszOldText);
   }

   return *this;
}

std::string message::to_string() const
{
   std::string stringMessage;
   if( m_pbszTextView != nullptr ) stringMessage += m_pbszTextView;
   if( m_pbszText != nullptr ) stringMessage += m_pbszText;

   return stringMessage;
}

std::wstring message::to_wstring() const
{
   std::wstring stringMessage;
   const char* pbszText = m_pbszTextView != nullptr ? m_pbszTextView : m_pbszText;

   unsigned uLength = 0;

   if( pbszText == nullptr ) { pbszText = severity_get_name_g( m_eSeverity ); }

   gd::utf8::convert_utf8_to_uft16((const uint8_t*)pbszText, stringMessage);

   //uLength += gd::utf8::strlen(pbszText);
   

   /*
   if( m_pbszTextView != nullptr )
   {
      uLength += gd::utf8::strlen(m_pbszTextView);
      //stringMessage += m_pbszTextView;
   }
   
   if( m_pbszText != nullptr )
   {
      uLength += gd::utf8::strlen(m_pbszText);
      //stringMessage += m_pbszText;
   }
   */

   return stringMessage;
}

char* message::append_s(char** ppbszText, const std::string_view& stringAdd )
{
   // ## count lengths to speed up join for the two texts sent to method
   auto uLengthText = std::strlen(*ppbszText);

   char* pbszNew = new_s(uLengthText + stringAdd.length() + 1);

   // ## insert texts to allocated buffer
   std::memcpy(pbszNew, *ppbszText, uLengthText);
   std::memcpy(pbszNew + uLengthText, stringAdd.data(), stringAdd.length() + 1);

   // ## delete memory
   clear_s(ppbszText);

   return pbszNew;
}


char* message::join_s(char** ppbszText, char** ppbszAdd)
{
   // ## count lengths to speed up join for the two texts sent to method
   auto uLengthText = std::strlen(*ppbszText);
   auto uLengthAdd = std::strlen(*ppbszAdd);

   char* pbszNew = new_s(uLengthText + uLengthAdd + 1);

   // ## insert texts to allocated buffer
   std::memcpy(pbszNew, *ppbszText, uLengthText);
   std::memcpy(pbszNew + uLengthText, *ppbszAdd, uLengthAdd + 1);

   // ## delete memory
   clear_s(ppbszText);
   clear_s(ppbszAdd);

   return pbszNew;
}

char* message::join_s(char** ppbszText, const std::string_view& stringAdd, char** ppbszAdd)
{
   // ## count lengths to speed up join for the two texts sent to method
   auto uLengthText = std::strlen(*ppbszText);
   auto uLengthAdd = std::strlen(*ppbszAdd);

   char* pbszNew = new_s(uLengthText + stringAdd.length() + uLengthAdd + 1);

   // ## insert texts to allocated buffer
   std::memcpy(pbszNew, *ppbszText, uLengthText);
   std::memcpy(pbszNew + uLengthText, stringAdd.data(), stringAdd.length());
   std::memcpy(pbszNew + uLengthText + stringAdd.length(), *ppbszAdd, uLengthAdd + 1);

   // ## delete memory
   clear_s(ppbszText);
   clear_s(ppbszAdd);

   return pbszNew;
}

_GD_LOG_LOGGER_END