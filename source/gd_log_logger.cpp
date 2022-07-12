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

gd::log::message& message::printf(const wchar_t* pwszFormat, ...)
{
   va_list va_listArgument;
   va_start(va_listArgument, pwszFormat);                                        // initialize va_list (where to start)

   char* pbszOldText = nullptr;

   // if text is set, then store it and append this later
   if( m_pbszText != nullptr ) pbszOldText = m_pbszText.release();


   int iLength = _vscwprintf(pwszFormat, va_listArgument);                       // calculate needed length for buffer
   if( iLength < 0 ) { assert(false); return *this; }
   iLength++;                                                                    // add zero terminator

   //m_pbszText.reset(new_s(iLength));
   //char* pbszText = m_pbszText.get();
   //pbszText[0] = '\0';
   wchar_t* pwszText = (wchar_t*)_alloca(iLength * sizeof(wchar_t));

   int iReturn = _vsnwprintf_s(pwszText, iLength, size_t(iLength) - 1, pwszFormat, va_listArgument); // generate message text
   va_end(va_listArgument);

   // ## convert to utf8 if needed, text is stored in internally in utf8 format
   auto uUtf8Length = gd::utf8::size(pwszText) + 1;                              assert(uUtf8Length >= (uint32_t)iLength); // if less something is seriously wrong
   m_pbszText.reset( new_s(uUtf8Length) );
   char* pbsz = m_pbszText.get();
   gd::utf8::convert_unicode(pwszText, pbsz, pbsz + uUtf8Length);

   if( pbszOldText != nullptr )
   {
      auto pbszText = m_pbszText.release();
      m_pbszText.reset(join_s(&pbszOldText, std::string_view{ "  " }, &pbszText));
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

/*----------------------------------------------------------------------------- new_s */ /**
 * Special method for message combining three text values into one buffer, if first text is nullptr only last Add text is inserted.
 * This method works like adding text to existing message text if set
 * \param pbszUtf8First first text (usually text set that will be appended with more text)
 * \param stringIfFirst test added as separator if first text is set
 * \param stringAdd text that is always added
 * \return char* pointer to new allocaded text with combined text
 */
char* message::new_s(const char* pbszUtf8First, const std::string_view& stringIfFirst, const std::string_view& stringAdd)
{
   std::size_t uLength = 0;
   if( pbszUtf8First != nullptr )
   {
      uLength = strlen( pbszUtf8First );
      uLength += stringIfFirst.length();
   }

   std::size_t uFirstLength = uLength;
   uLength += gd::utf8::size( stringAdd.data() );

   char* pbszNew = new char[uLength + 1];

   if( pbszUtf8First != nullptr )
   {
      std::memcpy(pbszNew, pbszUtf8First, uFirstLength - stringIfFirst.length()); // copy first string to new buffer
      std::memcpy(pbszNew + (uFirstLength - stringIfFirst.length()), stringIfFirst.data(), stringIfFirst.length());// copy text if first text is set
   }

   gd::utf8::convert_ascii(stringAdd.data(), pbszNew + uFirstLength);

   return pbszNew;
}

/*----------------------------------------------------------------------------- new_s */ /**
 * Special method for message combining three text values into one buffer, if first text is nullptr only last Add text is inserted.
 * This method works like adding text to existing message text if set
 * \param pbszUtf8First first text (usually text set that will be appended with more text)
 * \param stringIfFirst test added as separator if first text is set
 * \param stringAdd text that is always added
 * \return char* pointer to new allocaded text with combined text
 */
char* message::new_s(const char* pbszUtf8First, const std::string_view& stringIfFirst, const std::wstring_view& stringAdd)
{
   std::size_t uLength = 0;
   if( pbszUtf8First != nullptr )                                                // if text is already set we need to make room for this text and concatenate all three
   {
      uLength = strlen( pbszUtf8First );
      uLength += stringIfFirst.length();                                         // separator chars should NOT!!! be above 0x80
   }

   std::size_t uFirstLength = uLength;                                           // store length for later
   uLength += gd::utf8::size( stringAdd.data() );                                // needed space for added unicode text
   uLength++;                                                                    // make room for zero terminator

   char* pbszNew = new char[uLength + 1];

   if( pbszUtf8First != nullptr )
   {
      std::memcpy(pbszNew, pbszUtf8First, uFirstLength - stringIfFirst.length()); // copy first string to new buffer
      std::memcpy(pbszNew + (uFirstLength - stringIfFirst.length()), stringIfFirst.data(), stringIfFirst.length());// copy text if first text is set
   }

   gd::utf8::convert_unicode(stringAdd.data(), pbszNew + uFirstLength, pbszNew + uLength );

   return pbszNew;
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