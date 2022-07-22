#include <cassert>
#include <cstdarg>
#include <chrono>
#include <stdarg.h>

#include "gd_log_logger.h"

#pragma warning(disable: 26451)

_GD_LOG_LOGGER_BEGIN


void message::set_text(std::string_view stringText)
{
   auto uUtf8Length = gd::utf8::size(stringText);
   m_pbszText.reset( new_s( stringText.data() ));
}


/*----------------------------------------------------------------------------- append */ /**
 * append ascii text to message, adds separator if text is already set
 * \param stringAppend text to add
 * \return gd::log::message& reference to message for chaining
 */
message& message::append(const std::string_view& stringAppend)
{
   m_pbszText.reset(new_s(m_pbszText.release(), std::string_view{ "  " }, stringAppend.data()));
   return *this;
}

/*----------------------------------------------------------------------------- append */ /**
 * append ascii text to message, adds separator if text is already set
 * \param stringAppend text to add
 * \return message& reference to message for chaining
 */
message& message::append(const std::wstring_view& stringAppend)
{
   m_pbszText.reset(new_s(m_pbszText.release(), std::string_view{ "  " }, stringAppend.data()));
   return *this;
}

#if defined(__cpp_char8_t)
/*----------------------------------------------------------------------------- append */ /**
 * append ascii text to message, adds separator if text is already set
 * \param pbszUtf8Append text to add
 * \return message& reference to message for chaining
 */
message& message::append(const char8_t* pbszUtf8Append)
{
   m_pbszText.reset(new_s(m_pbszText.release(), std::string_view{ "  " }, pbszUtf8Append));
   return *this;
}
#endif

/*----------------------------------------------------------------------------- append */ /**
 * append ascii text to message, adds separator if text is already set
 * \param messageAppend text to add
 * \return message& reference to message for chaining
 */
message& message::append(const message& messageAppend)
{
   const char* pbszMessage = messageAppend.get_text();
   if( pbszMessage != nullptr ) m_pbszText.reset(new_s(m_pbszText.release(), std::string_view{ "  " }, pbszMessage, gd::utf8::utf8_tag{}));
   return *this;
}

/*----------------------------------------------------------------------------- append */ /**
 * append ascii text to message, adds separator if text is already set
 * \param streamAppend text to add
 * \return message& reference to message for chaining
 */
message& message::append(const stream& streamAppend)
{
   m_pbszText.reset(new_s(m_pbszText.release(), std::string_view{ "  " }, streamAppend.get_string()));
   return *this;
}

/*----------------------------------------------------------------------------- append */ /**
 * append ascii text to message, adds separator if text is already set
 * \param streamAppend text to add
 * \return message& reference to message for chaining
 */
message& message::append(const wstream& streamAppend)
{
   m_pbszText.reset(new_s(m_pbszText.release(), std::string_view{ "  " }, streamAppend.get_string()));
   return *this;
}


/*----------------------------------------------------------------------------- append */ /**
 * append ascii text to message, adds separator if text is already set
 * \param stringAppend text to add
 * \return gd::log::message& reference to message for chaining
 */
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


/*----------------------------------------------------------------------------- to_string */ /**
 * get message text as std string object (utf8 formated internally)
 * \return std::string
 */
std::string message::to_string() const
{
   std::string stringMessage;
   if( m_pbszTextView != nullptr ) stringMessage += m_pbszTextView;
   if( m_pbszText != nullptr ) stringMessage += m_pbszText.get();

   return stringMessage;
}


//
// ## FREE FUNCTIONS -----------------------------------------------------------------------------
//

// ================================================================================================
// ================================================================================= STATIC
// ================================================================================================


/*----------------------------------------------------------------------------- new_s */ /**
 * Allocates text on heap and inserts text in utf8 format from string sent 
 * \param stringUnicode text copied as utf8 to allocated buffer
 * \return char* heap ponter to buffer, remember to handle this otherwise it is a memory leak
 */
char* message::new_s(std::wstring_view stringUnicode)
{
   auto uLength = gd::utf8::size(stringUnicode.data(), stringUnicode.data() + stringUnicode.length());
   char* pbsz = new char[uLength + 1u];
   gd::utf8::convert_utf16_to_uft8((const wchar_t*)stringUnicode.data(), pbsz, gd::utf8::utf8_tag{});
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

   char* pbszNew = new char[uLength + 1];                                        // create new buffer on heap where text is placed

   if( pbszUtf8First != nullptr )
   {
      std::memcpy(pbszNew, pbszUtf8First, uFirstLength - stringIfFirst.length()); // copy first string to new buffer
      std::memcpy(pbszNew + (uFirstLength - stringIfFirst.length()), stringIfFirst.data(), stringIfFirst.length());// copy text if first text is set
   }

   gd::utf8::convert_unicode(stringAdd.data(), pbszNew + uFirstLength, pbszNew + uLength );

   return pbszNew;
}

#if defined(__cpp_char8_t)
/*----------------------------------------------------------------------------- new_s */ /**
 * Special method for message combining three text values into one buffer, if first text is nullptr only last Add text is inserted.
 * This method works like adding text to existing message text if set
 * \param pbszUtf8First first text (usually text set that will be appended with more text)
 * \param stringIfFirst test added as separator if first text is set
 * \param pbszUtf8Add text that is always added
 * \return char* pointer to new allocaded text with combined text
 */
char* message::new_s(const char* pbszUtf8First, const std::string_view& stringIfFirst, const char8_t* pbszUtf8Add )
{                                                                                assert( pbszUtf8Add != nullptr ); assert( std::strlen((char*)pbszUtf8Add) < 99'999 ); // ok and realistic ?
   std::size_t uLength = 0;
   if( pbszUtf8First != nullptr )                                                // if text is already set we need to make room for this text and concatenate all three
   {
      uLength = strlen(pbszUtf8First);
      uLength += stringIfFirst.length();                                         // separator chars should NOT!!! be above 0x80
   }

   std::size_t uFirstLength = uLength;                                           // store length for later
   std::size_t uUtf8Length = std::strlen( (char*)pbszUtf8Add );                  // needed space added utf8 text
   uLength += uUtf8Length;
   uLength++;                                                                    // make room for zero terminator

   char* pbszNew = new char[uLength + 1];                                        // create new buffer on heap where text is placed

   if( pbszUtf8First != nullptr )                                                // add to text ?
   {
      std::memcpy(pbszNew, pbszUtf8First, uFirstLength - stringIfFirst.length());// copy first string to new buffer
      std::memcpy(pbszNew + (uFirstLength - stringIfFirst.length()), stringIfFirst.data(), stringIfFirst.length());// copy text if first text is set
   }

   std::memcpy(pbszNew + uFirstLength, pbszUtf8Add, uUtf8Length + 1);            // append utf8 string with zero terminator

   return pbszNew;
}
#endif

/*----------------------------------------------------------------------------- new_s */ /**
 * Special method for message combining three text values into one buffer, if first text is nullptr only last Add text is inserted.
 * This method works like adding text to existing message text if set
 * \param pbszUtf8First first text (usually text set that will be appended with more text)
 * \param stringIfFirst test added as separator if first text is set
 * \param pbszUtf8Add text that is always added
 * \param utf8_tag tag dispatcher 
 * \return char* pointer to new allocaded text with combined text
 */
char* message::new_s(const char* pbszUtf8First, const std::string_view& stringIfFirst, const char* pbszUtf8Add, gd::utf8::utf8_tag)
{
   assert(pbszUtf8Add != nullptr); assert(std::strlen((char*)pbszUtf8Add) < 99'999); // ok and realistic ?
   std::size_t uLength = 0;
   if( pbszUtf8First != nullptr )                                                // if text is already set we need to make room for this text and concatenate all three
   {
      uLength = strlen(pbszUtf8First);
      uLength += stringIfFirst.length();                                         // separator chars should NOT!!! be above 0x80
   }

   std::size_t uFirstLength = uLength;                                           // store length for later
   std::size_t uUtf8Length = std::strlen((char*)pbszUtf8Add);                  // needed space added utf8 text
   uLength += uUtf8Length;
   uLength++;                                                                    // make room for zero terminator

   char* pbszNew = new char[uLength + 1];                                        // create new buffer on heap where text is placed

   if( pbszUtf8First != nullptr )                                                // add to text ?
   {
      std::memcpy(pbszNew, pbszUtf8First, uFirstLength - stringIfFirst.length());// copy first string to new buffer
      std::memcpy(pbszNew + (uFirstLength - stringIfFirst.length()), stringIfFirst.data(), stringIfFirst.length());// copy text if first text is set
   }

   std::memcpy(pbszNew + uFirstLength, pbszUtf8Add, uUtf8Length + 1);            // append utf8 string with zero terminator

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

std::wstring message::get_now_date_as_wstring_s()
{
   enum { eBufferSize = 30 };
   auto time_pointNow = std::chrono::system_clock::now();
   std::time_t timeNow = std::chrono::system_clock::to_time_t(time_pointNow);
   wchar_t pBuffer[eBufferSize];
   std::wcsftime(pBuffer, eBufferSize, L"%H:%M:%S", std::localtime(&timeNow));
   return std::wstring(pBuffer);
}

std::wstring message::get_now_time_as_wstring_s()
{
   enum { eBufferSize = 30 };
   auto time_pointNow = std::chrono::system_clock::now();
   std::time_t timeNow = std::chrono::system_clock::to_time_t(time_pointNow);
   wchar_t pBuffer[eBufferSize];
   std::wcsftime(pBuffer, eBufferSize, L"%Y-%m-%d %H:%M:%S", std::localtime(&timeNow));
   return std::wstring(pBuffer);
}

std::wstring message::wrap_s(wchar_t chBefore, const std::wstring_view& stringText, wchar_t chAfter)
{
   std::wstring stringWrapped;

   stringWrapped += chBefore;
   stringWrapped.append(stringText);
   stringWrapped += chAfter;
   return stringWrapped;
}


// ================================================================================================
// ================================================================================= GLOBAL
// ================================================================================================


/*----------------------------------------------------------------------------- severity_get_name_g */ /**
 * return pointer to name for selected severity type
 * \param uSeverity severity number for requested severity name
 * \return const char* pointer to name
 */
const char* severity_get_name_g(unsigned uSeverity)
{
   switch( uSeverity & static_cast<unsigned>(enumSeverityMask::eSeverityMaskNumber) )
   {
   case enumSeverityNumber::eSeverityNumberFatal:        return "FATAL";
   case enumSeverityNumber::eSeverityNumberError:        return "ERROR";
   case enumSeverityNumber::eSeverityNumberWarning:      return "WARNING";
   case enumSeverityNumber::eSeverityNumberInformation:  return "INFORMATION";
   case enumSeverityNumber::eSeverityNumberDebug:        return "DEBUG";
   case enumSeverityNumber::eSeverityNumberVerbose:      return "VERBOSE";
   default:                                              return "NONE";
   }
}


_GD_LOG_LOGGER_END