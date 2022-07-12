#include <cassert>
#include <cstdarg>
#include <stdarg.h>

#include "gd_log_logger.h"

#pragma warning(disable: 26451)

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

void message::set_text(std::string_view stringText)
{
   auto uUtf8Length = gd::utf8::size(stringText);
   m_pbszText.reset( new_s( stringText.data() ));
}


gd::log::message& message::printf(const char* pbszFormat, ...)
{
   va_list va_listArgument;
   va_start(va_listArgument, pbszFormat);                                        // initialize va_list (where to start)

   char* pbszOldText = nullptr;

   // if text is set, then store it and append this later
   if( m_pbszText != nullptr ) pbszOldText = m_pbszText.release();


   int iLength = _vscprintf(pbszFormat, va_listArgument);                        // calculate needed length for buffer
   if( iLength < 0 ) { assert(false); return *this; }
   iLength++;                                                                    // add zero terminator

   m_pbszText.reset( new_s( iLength ) );
   char* pbszText = m_pbszText.get();
   pbszText[0] = '\0';

   int iReturn = _vsnprintf_s(pbszText, iLength, size_t(iLength) - 1, pbszFormat, va_listArgument); // generate message text
   va_end(va_listArgument);

   // ## convert to utf8 if needed, text is stored in internally in utf8 format
   auto uUtf8Length = gd::utf8::size(pbszText) + 1;                              assert( uUtf8Length >= (uint32_t)iLength ); // if less something is seriously wrong
   if( uUtf8Length > (uint32_t)iLength )                                         // do we need to convert to utf8
   {
      char* pbsz = new_s( uUtf8Length );                                         // create new buffer
      gd::utf8::convert_ascii(pbszText, pbsz);                                   // convert to utf8
      m_pbszText.reset( pbsz );                                                  // set to new buffer
   }

   if( pbszOldText != nullptr )
   {
      auto pbszText = m_pbszText.release();
      m_pbszText.reset( join_s(&pbszOldText, std::string_view{"  "}, &pbszText) );
      clear_s(&pbszOldText);
   }

   return *this;
}

std::string message::to_string() const
{
   std::string stringMessage;
   if( m_pbszTextView != nullptr ) stringMessage += m_pbszTextView;
   if( m_pbszText != nullptr ) stringMessage += m_pbszText.get();

   return stringMessage;
}


/*----------------------------------------------------------------------------- new_s */ /**
 * Allocates text on heap and inserts text in utf8 format from string sent 
 * \param stringUnicode text copied as utf8 to allocated buffer
 * \return char* heap ponter to buffer, remember to handle this otherwise it is a memory leak
 */
char* message::new_s(std::wstring_view stringUnicode)
{
   auto uLength = gd::utf8::size(stringUnicode.data(), stringUnicode.data() + stringUnicode.length());
   char* pbsz = new char[uLength + 1u];
   gd::utf8::convert_utf16_to_uft8(stringUnicode.data(), pbsz);
   return pbsz;
}

/*----------------------------------------------------------------------------- new_s */ /**
 * Allocates text on heap and inserts text in utf8 format from string sent 
 * \param stringAscii text copied as utf8 to allocated buffer
 * \return char* heap ponter to buffer, remember to handle this otherwise it is a memory leak
 */
char* message::new_s(std::string_view stringAscii )
{
   auto uLength = gd::utf8::size(stringAscii);
   char* pbsz = new char[uLength + 1u];
   gd::utf8::convert_ascii(stringAscii.data(), pbsz);
   return pbsz;
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