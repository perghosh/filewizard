/**
 * \file gd_log_logger.h
 * 
 * \brief Core file for logger functionality
 * 
 * 
   |  |  |
   | - | - |
   | `stream` | stream ascii text into message |
   | `wstream` | stream unicode text into message |
   | `format` | use std::format syntax to stream text into message |
   | `printf` | use printf format to format text and stream it to into message |
   | `message` | main text object when building log text, here all text is collected before print is called in logger |
   | `logger` | core log object holding printers that are used to process message items holding text to log |
   | `` |  |
   | `` |  |
   | `` |  |
 * 
 * 
 * ### Important classes
 * - `logger`: logger is the instance (singleton) used when log messages are generated. 
 *    Generated log text is sent to logger instance where logger spread the message 
 *    to connected `printer`s.
 * - `i_printer`: i_printer is the base class for all printer classes used by logger. 
 *    You need at least one printer to produce some sort of output, how depends on 
 *    the printer, how that printer works. You can create your own `printer` with 
 *    desired functionality and attach it to logger producing messages.
 * - `message`: message is used to store messages sent to logger instances. How 
 *    messages are handled depends on the printer that receives message from logger instance.
 * 
 * ### Preface
 * `logger` is a singleton item. You can create many `logger` object where each logger 
 * item is created based on it's instance id. It is a template class where each new
 * instance id will create a new class for logger.
 * Using different instance id values creating `logger` object it is possible to have
 * multiple logger items in your code if that is desired.
 * 
 * ### Severity
 * Severity is used to level log information in some sort of importance. Levels are
 * from Verbose to Fatal.
 * All levels: Verbose, Debug, Information, Warning, Error and Fatal.
 * It is possible to turn on a lesser important level setting it in logger using
 * `enumSeverityGroup.
 * 
 * 
 * ### Tutorial (code samples)
 * 
-------------------------------------------------
*Geting started** *Sample getting default logger (id=0), adds printer and sets severity filter and prints a message*
```cpp
using namespace gd::log;

gd::log::logger<0>* plogger = gd::log::get_s();                             // get pointer to logger 0
plogger->append( std::make_unique<gd::log::printer_console>() );            // append printer to logger
plogger->set_severity(eSeverityNumberWarning | eSeverityGroupDebug);        // set severity filter, messages within this filter is printed
plogger->print(message(eSeverityGroupDebug, eMessageTypeTime).printf("%s", "## MESSAGE ##")); plogger->flush();// write to logger
```
-------------------------------------------------
*Macro sample, two macros named to LOG_ and LOG. *
```cpp
#define LOG_( uLogger, uSeverity, expression ) gd::log::get_g<uLogger>()->print( gd::log::message( gd::log::severity_get_g( uSeverity ), gd::log::eMessageTypeAll ) << __FILE__ << __func__ << expression )
#define LOG( uSeverity, expression ) LOG_( 0, gd::log::severity_get_g( uSeverity ), expression )

// Use macros

auto plogger = gd::log::get_s();                               // create default logger
plogger->append(std::make_unique<gd::log::printer_console>()); // append console printer 
plogger->append(std::make_unique<gd::log::printer_file>(L"testlog.txt"));// append file printer and set file

plogger->set_severity(gd::log::eSeverityVerbose);              // set severity

LOG( gd::log::eSeverityWarning, L"testing this message" );     // print log text (set severity with constant)
LOG("WARNING", L"WARNING");                                    // print log text (set severity with text)

plogger->set_severity(gd::log::eSeverityError);                // change severity filter

LOG("WARNING", L"warning message");                            // not printed, warning is higher compared to error () not printed
LOG("FATAL", L"fatal message");                                // this is printed
```
-------------------------------------------------
*ID macros, uses LOG_ in previous sample*
```cpp
#define LOG_IF_(uLogger, uSeverity, condition, expression)  if(!(condition)) {;} else LOG_(uLogger, uSeverity, expression)
#define LOG_IF(uSeverity, condition, expression)  LOG_IF_(0, uSeverity, condition, expression)

#define LOG_ERROR_IF(condition, expression) LOG_IF("ERROR", condition, expression)
#define LOG_FATAL_IF(condition, expression) LOG_IF("FATAL", condition, expression)

// in code
LOG_IF_(0, "FATAL", 1 == 1, "1 == 1");  // printed, condition is true
LOG_IF( "FATAL", 10 > 5, "10 > 5");     // printed, condition is true
LOG_FATAL_IF( false, "fatal message");  // not printed, condition is false
```
-------------------------------------------------
*retrieve internal error from connected printer*
```cpp
using namespace gd::log;
auto plogger = get_s();

plogger->append(std::make_unique<printer_file>(L"C:\\invalid\\<>:\"/\\|?*.txt"));// filename with invalid file name
plogger->set_severity(eSeverityVerbose);  // set filter
plogger->print(message(severity_get_g("VERBOSE"), gd::log::eMessageTypeAll)
   << __FILE__
   << __func__
   << "This will generate error, filename is invalid");

while( plogger->error_size() != 0 )
{
   auto stringError = plogger->error_pop(); 
   std::cout << stringError << std::endl;
}
```

-------------------------------------------------
*generate multiple loggers*
```cpp
// logger is a singelton class but it is also a template, it is possible to create
// multiple loggers using integer values. for each new number a new logger is created.
// with numbers it is possible to create any number of loggers if needed.
using namespace gd::log;
auto plogger = get_s();            // default logger

```


 * 
 */


// links
// https://www.reddit.com/r/cpp/comments/p9annk/conditional_compilation_with_constexpr_instead_of/
// https://stackoverflow.com/questions/57540155/issue-converting-a-macro-with-variable-args-into-constexpr-variadic-templa
// https://stackoverflow.com/questions/52433953/using-constexpr-to-replace-define-and-ifdef-for-conditional-compilation
// https://www.sobyte.net/post/2022-03/if-constexpr/
// https://stackoverflow.com/questions/28596298/is-it-possible-to-compile-out-stream-expressions-in-c

#pragma once
#include <cassert>
#include <string>
#include <string_view>
#include <format>
#include <vector>
#include <type_traits>
#include <memory>
#include <sstream>
#include <iosfwd>
#include <mutex>


#include "gd_utf8.hpp"

#pragma warning( disable: 4996 26812 )

#ifndef GD_LOG_DEFAULT_INSTANCE_ID
#   define GD_LOG_DEFAULT_INSTANCE_ID 0
#endif



#ifndef _GD_LOG_LOGGER_BEGIN

#  define _GD_LOG_LOGGER_BEGIN namespace gd { namespace  log {
#  define _GD_LOG_LOGGER_END } }

#endif

_GD_LOG_LOGGER_BEGIN

class message;
class i_printer;


/**
 * \brief message level to print, how severe information sent to printers
 * 
 * Using level messages you can set the priority and filter whats written to log
 */
enum enumSeverityNumber
{
   eSeverityNumberNone = 0,         ///< always write this, no severity set
   eSeverityNumberFatal = 1,        ///< most important severity, 
   eSeverityNumberError = 2,        ///< almost as important as fatal and rest is less important in list
   eSeverityNumberWarning = 3,
   eSeverityNumberInformation = 4,
   eSeverityNumberDebug = 5,
   eSeverityNumberVerbose = 6,
};


/**
 * \brief message level to print, how severe information sent to printers
 * 
 */
enum enumSeverityGroup
{
   eSeverityGroupNone = 0,
   //                              3       2 2       1 1
   //                              1       4 3       6 5       8 7       0       
   eSeverityGroupFatal =         0b0000'0000'0000'0000'0000'0001'0000'0000,
   eSeverityGroupError =         0b0000'0000'0000'0000'0000'0010'0000'0000,
   eSeverityGroupWarning =       0b0000'0000'0000'0000'0000'0100'0000'0000,
   eSeverityGroupInformation =   0b0000'0000'0000'0000'0000'1000'0000'0000,
   eSeverityGroupDebug =         0b0000'0000'0000'0000'0001'0000'0000'0000,
   eSeverityGroupVerbose =       0b0000'0000'0000'0000'0010'0000'0000'0000,
};


/**
 * \brief final severity types, combines severity number and severity bit
 * 
 */
enum enumSeverity
{
   eSeverityNone =         eSeverityNumberNone,
   eSeverityFatal =        eSeverityGroupFatal           | eSeverityNumberFatal,
   eSeverityError =        eSeverityGroupError           | eSeverityNumberError,
   eSeverityWarning =      eSeverityGroupWarning         | eSeverityNumberWarning,
   eSeverityInformation =  eSeverityGroupInformation     | eSeverityNumberInformation,
   eSeverityDebug =        eSeverityGroupDebug           | eSeverityNumberDebug,
   eSeverityVerbose =      eSeverityGroupVerbose         | eSeverityNumberVerbose,
};

/**
 * \brief severity bits, 16 bits available for custom severity group settings to filter messages
 * 
 */
enum enumSeverityBit
{
   eSeverityBit01 = (1 << (15 +  1)),
   eSeverityBit02 = (1 << (15 +  2)),
   eSeverityBit03 = (1 << (15 +  3)),
   eSeverityBit04 = (1 << (15 +  4)),
   eSeverityBit05 = (1 << (15 +  5)),
   eSeverityBit06 = (1 << (15 +  6)),
   eSeverityBit07 = (1 << (15 +  7)),
   eSeverityBit08 = (1 << (15 +  8)),
   eSeverityBit09 = (1 << (15 +  9)),
   eSeverityBit10 = (1 << (15 + 10)),
   eSeverityBit11 = (1 << (15 + 11)),
   eSeverityBit12 = (1 << (15 + 12)),
   eSeverityBit13 = (1 << (15 + 13)),
   eSeverityBit14 = (1 << (15 + 14)),
   eSeverityBit15 = (1 << (15 + 15)),
   eSeverityBit16 = (1 << (15 + 16)),
};


/**
 * \brief mask numbers are used to extract different parts from severity, use mask to zero out and keep whats requested 
 */
enum class enumSeverityMask : uint32_t
{
   eSeverityMaskNumber = 0x0000'00FF,
   eSeverityMaskGroup  = 0x0000'FF00,
   eSeverityMaskFlag   = 0xFFFF'0000,
   eSeverityMaskFlagAndGroup = eSeverityMaskGroup | eSeverityMaskFlag,
};


/**
 * \brief type flags to generate typed information parts
 * 
 * Set message type flags will automatically generate information flag represent without
 * stream text to message. This types are common information part that may be usable 
 * producing log information.
 */
enum enumMessageType
{
   eMessageTypeText = 0,
   eMessageTypeMethodName =   (1 << 1),
   eMessageTypeFileName =     (1 << 2),
   eMessageTypeSeverity =     (1 << 3),
   eMessageTypeTime =         (1 << 4),
   eMessageTypeDate =         (1 << 5),

   eMessageTypeAll = eMessageTypeMethodName | eMessageTypeFileName | eMessageTypeSeverity | eMessageTypeTime | eMessageTypeDate,
};

///
const size_t MESSAGE_BUFFER_START_SIZE = 128;


const char* severity_get_name_g(unsigned uSeverity);

// ================================================================================================
// ================================================================================= stream
// ================================================================================================

/*-----------------------------------------*/ /**
 * \brief holds pointer to text, may be used for __FILE__ and similar
 *
 *
 */
struct view 
{
   view(const std::string_view&& stringView) : m_stringView(stringView) {}

   // attributes
   public:
      std::string_view m_stringView;
};


/*-----------------------------------------*/ /**
 * \brief helper to enable compatibility between stringstream and logger
 *
 * *sample*
 * ```cpp
 * auto plogger = gd::log::get_s();
 * plogger->print(message() << "one" << gd::log::stream(std::stringstream() << std::setw(5) << std::setfill('0') << 1 << 2) );
 * // output = "one  000012"
 * ```
 */
struct stream 
{
   stream() {}
   stream( std::stringstream&& stringstream ): m_stringstream( std::forward<std::stringstream>(stringstream) ) {}

   std::string get_string() const { return m_stringstream.str(); }

   // attributes
   public:
      std::stringstream m_stringstream;
};

// ================================================================================================
// ================================================================================= wstream
// ================================================================================================


/*-----------------------------------------*/ /**
 * \brief helper to enable compatibility between wstringstream and logger
 *
 * *sample*
 * ```cpp
 * auto plogger = gd::log::get_s();
 * plogger->print(message() << L"TWO" << gd::log::wstream(std::wstringstream() << std::setw(5) << std::setfill(L'0') << 111) );
 * // output = "TWO  00111"
 * ```
 */
struct wstream 
{
   wstream() {}
   wstream( std::wstringstream&& stringstream ): m_stringstream( std::forward<std::wstringstream>(stringstream) ) {}

   std::wstring get_string() const { return m_stringstream.str(); }

   // attributes
   public:
      std::wstringstream m_stringstream;
};

// ================================================================================================
// ================================================================================= format
// ================================================================================================


/*-----------------------------------------*/ /**
 * \brief implements std::format functionality to generate log text
 *
 *
 */
struct format 
{
   format() {}

   /*----------------------------------------------------------------------------- format */ /**
    * See std::format on how to generate text, this member method forwards logic to format
    * \param stringFormat format string 
    * \param arguments arguments forwarded to vformat (vformat is used by std::format)
    * \return gd::log::message& return reference for chaining
    */
   template <typename... ARGUMENTS>
   format(std::string_view stringFormat, ARGUMENTS&&... arguments) {
      // same logic as std::format
      // takes a number of arguments and wraps them into std::format_args that has logic to convert arguments into formated text
      std::string stringResult = std::vformat(stringFormat, std::make_format_args(std::forward<ARGUMENTS>(arguments)...));
      m_pbszText.reset(message::new_s(stringResult, m_pbszText.release()));
   }

   /*----------------------------------------------------------------------------- format */ /**
    * See std::format on how to generate text, this member method forwards logic to format
    * \param stringFormat format string 
    * \param arguments arguments forwarded to vformat (vformat is used by std::format)
    * \return gd::log::message& return reference for chaining
    */
   template <typename... ARGUMENTS>
   format(std::wstring_view stringFormat, ARGUMENTS&&... arguments) {
      // same logic as std::format
      // takes a number of arguments and wraps them into std::format_args that has logic to convert arguments into formated text
      std::wstring stringResult = std::vformat(stringFormat, std::make_wformat_args(std::forward<ARGUMENTS>(arguments)...));
      m_pbszText.reset(message::new_s(stringResult, m_pbszText.release()));
   }

#  if defined(__cpp_char8_t)
   operator const char8_t*() const { return (const char8_t*)m_pbszText.get();  }
#  endif
   operator const char*() const { return (const char*)m_pbszText.get();  }
   // attributes
   public:
      std::unique_ptr<char> m_pbszText;
};

/*-----------------------------------------*/ /**
 * \brief 
 *
 *
 */
struct printf 
{
   printf() {}

   printf(const char* pbszFormat, ...);
   printf(const wchar_t* pwszFormat, ...);


#  if defined(__cpp_char8_t)
   operator const char8_t* () const { return (const char8_t*)(m_pbszText.get() ? m_pbszText.get() : ""); }
#  endif
   operator const char* () const { return (const char*)(m_pbszText.get() ? m_pbszText.get() : ""); }


   // attributes
public:
   std::unique_ptr<char> m_pbszText;
};

// ================================================================================================
// ================================================================================= message
// ================================================================================================

/**
 * \brief 
 *
 *
 *
 \code
 \endcode
 */
class message 
{
// ## construction -------------------------------------------------------------
public:
   message(): m_uSeverity( enumSeverity::eSeverityNone ) {}
   explicit message( unsigned uSeverity ): m_uSeverity( uSeverity ) {}
   explicit message( unsigned uSeverity, unsigned uMessageType ): m_uSeverity( uSeverity ), m_uMessageType(uMessageType) {}
   explicit message( unsigned uSeverity, const char* pbszText ): m_uSeverity( uSeverity ), m_pbszTextView(pbszText) {}
   message( unsigned uSeverity, enumMessageType eMessageType, const char* pbszText ): m_uSeverity( uSeverity ), m_uMessageType(eMessageType), m_pbszTextView(pbszText) {}

   message( const char* pbszMessage ): m_uSeverity( enumSeverity::eSeverityNone ), m_pbszTextView( pbszMessage ) {}
#  if defined(__cpp_char8_t)
   message( const char8_t* pbszMessage ): m_uSeverity( enumSeverity::eSeverityNone ), m_pbszTextView( (const char*)pbszMessage ) {}
#  endif
   message(const wchar_t* pwszMessage) : m_uSeverity(enumSeverity::eSeverityNone), m_pbszText( new_s( pwszMessage ) ) {}
   // copy
   message( const message& o ) { common_construct( o ); }
   message( message&& o ) noexcept { common_construct( o ); }
   // assign
   message& operator=( const message& o ) { common_construct( o ); return *this; }
   message& operator=( message&& o ) noexcept { common_construct( o ); return *this; }
   
   virtual ~message() {}
private:
   // common copy
   void common_construct( const message& o ) {
      m_uSeverity = o.m_uSeverity;
      m_uMessageType = o.m_uMessageType;
      m_pbszTextView = o.m_pbszTextView;
      m_pbszText.reset( clone_s(o.m_pbszText.get()) );
   }
   void common_construct( message&& o ) noexcept {
      m_uSeverity = o.m_uSeverity;
      o.m_uSeverity = enumSeverity::eSeverityNone;                               // reset severity type
      m_uMessageType = o.m_uMessageType;
      o.m_uMessageType = enumMessageType::eMessageTypeText;                      // reset to text
      m_pbszText = std::move(o.m_pbszText);
      m_pbszTextView = o.m_pbszTextView;
      o.m_pbszTextView = nullptr;
   }

// ## operator -----------------------------------------------------------------
public:
   /// if empty empty string is returned, if text then return text as stl string. internally text should be stored as utf8
   operator std::string() const { return empty() == false ? std::string( get_text() ) : std::string(); }

   message& operator<<(const std::string_view& stringAppend) { return append(stringAppend); }
   message& operator<<(const std::wstring_view& stringAppend) { return append(stringAppend); }
#  if defined(__cpp_char8_t)
   message& operator<<(const char8_t* pbszUtf8Append) { return append(pbszUtf8Append); }
#  endif
   message& operator<<(const stream& streamAppend) { return append(streamAppend); }
   message& operator<<(const wstream& streamAppend) { return append(streamAppend); }
   message& operator<<(const format& formatAppend) { return append(formatAppend); }
   message& operator<<(const printf& printfAppend) { return append(printfAppend); }
   //message& operator<<(std::wostream& ) { return append(formatAppend); }
   template<typename APPEND>
   message& operator<<(APPEND appendValue) {
      std::wstringstream wstringstreamAppend;
      wstringstreamAppend << appendValue;
      auto pbszCurrent = m_pbszText.get();
      auto pbszNew = new_s(pbszCurrent, std::string_view{ "  " }, wstringstreamAppend.str().c_str(), pbszCurrent);
      if( pbszNew != pbszCurrent ) 
      { 
         m_pbszText.reset( pbszNew ); 
      }

      //m_pbszText.reset(new_s(pbszCurrent, std::string_view{ "  " }, wstringstreamAppend.str().c_str(), pbszCurrent));
      //clear_s(&pbszCurrent);
      return *this;
   }


// ## methods ------------------------------------------------------------------
public:
/** \name GET/SET
*///@{
   // ## getters and setters for severity information
   [[nodiscard]] unsigned get_severity() const { return m_uSeverity; }
   [[nodiscard]] enumSeverityNumber get_severity_number() const { return (enumSeverityNumber)(m_uSeverity & static_cast<unsigned>(enumSeverityMask::eSeverityMaskNumber)); }
   [[nodiscard]] enumSeverityGroup get_severity_group() const { return (enumSeverityGroup)(m_uSeverity & static_cast<unsigned>(enumSeverityMask::eSeverityMaskFlagAndGroup)); }
   void set_severity(unsigned uSeverity) { m_uSeverity = uSeverity; }
//@}

/** \name OPERATION
*///@{
   // ## check if message has any special type set, these type are used to produce 
   //    message can be marked with different flags to produce info like time etc in log

   /// check if message has any specific type set, if set then use other `is_` to test what to do
   [[nodiscard]] bool is_message_type_set() const { return m_uMessageType != 0; }
   [[nodiscard]] bool is_severity() const { return (m_uMessageType & eMessageTypeSeverity); } // print severity ?
   [[nodiscard]] bool is_time() const { return m_uMessageType & eMessageTypeTime; } // print time ?
   [[nodiscard]] bool is_date() const { return m_uMessageType & eMessageTypeDate; } // print date ?

   /// check if message has severity level below or equal to (max) severity sent
   /// ``` cpp
   /// messageItem.is_active( eSeverityError ); // returns true if messageItem is eSeverityError, eSeverityFatal or eSeverityNone
   /// ```
   [[nodiscard]] bool check_severity(unsigned uSeverity) const { 
      if( (m_uSeverity & static_cast<unsigned>(enumSeverityMask::eSeverityMaskNumber)) >= (uSeverity & static_cast<unsigned>(enumSeverityMask::eSeverityMaskNumber)) ) return true;
      else if( (m_uSeverity & uSeverity & static_cast<unsigned>(enumSeverityMask::eSeverityMaskFlagAndGroup)) != 0 ) return true;
      return false;
   }
   template <typename INTEGER>
   [[nodiscard]] bool check_severity(INTEGER uSeverity) const {
      static_assert( std::numeric_limits<INTEGER>::is_integer, "Unable to cast to this type to unsigned in a save way");
      return check_severity( static_cast<unsigned>( uSeverity ) );
   }

   /// return 
   const char* get_text() const { return m_pbszTextView != nullptr ? m_pbszTextView : m_pbszText.get(); }
   /// returns message text and the priority is 1) TextView, 2) Text, 3) severity text
   const char* get_text_all() const { return m_pbszTextView != nullptr ? m_pbszTextView : (m_pbszText != nullptr ? m_pbszText.get() : severity_get_name_g( m_uSeverity )); }
   /// set ascii texts
   void set_text(std::string_view stringText);

   // ## append methods are the main methods to append text.
   //    There are a number of different techniques adding text.
   //    - `string_view` adds ascii text
   //    - `wstring_view` adds unicode text
   //    - `char8_t` adds utf8 text
   //    - `message` add text from another message object
   //    - `stream` works like std::stringstream
   //    - `wstream` works like std::wstringstream
   //    - `format` works like std::format

   message& append(const std::string_view& stringAppend);
   message& append(const std::wstring_view& stringAppend);
#  if defined(__cpp_char8_t)
   message& append(const char8_t* pbszUtf8Append);                               // C++20
#  endif
   message& append(const message& messageAppend);
   message& append(const stream& streamAppend);
   message& append(const wstream& streamAppend);
   message& append(const format& formatAppend);
   message& append(const printf& printfAppend);

   message& printf( const char* pbszFormat, ... );
   message& printf( const wchar_t* pwszFormat, ...);

   std::string to_string() const;
   std::wstring to_wstring() const { return to_wstring_s(get_text_all()); };

   bool empty() const;

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
   unsigned m_uSeverity;               ///< type of message severity, used to filter output
   unsigned m_uMessageType = enumMessageType::eMessageTypeText;///< Type of message, setting type affects type of information generated in printer
   std::unique_ptr<char> m_pbszText;   ///< when message need to control text it is placed here
   const char* m_pbszTextView = nullptr; ///< pointer to text 

   
// ## free functions ------------------------------------------------------------
private:
   // ## main allocator
   [[nodiscard]] static char* allocate_s(std::size_t uSize);
   [[nodiscard]] static char* allocate_s(std::size_t uSize, char* pbszCurrent );
   
public:

   // ## allocators 
   [[nodiscard]] static char* new_s( std::size_t uSize ) { return allocate_s(uSize, nullptr ); }
   [[nodiscard]] static char* new_s( std::size_t uSize, char* pbszCurrent ) { return allocate_s(uSize, pbszCurrent); }
   [[nodiscard]] static char* new_s( const std::string_view& stringAscii) { return new_s(stringAscii, nullptr); }
   [[nodiscard]] static char* new_s( const std::string_view& stringAscii, char* pbszCurrent );
   [[nodiscard]] static char* new_s( const std::wstring_view& stringUnicode) { return new_s(stringUnicode, nullptr); }
   [[nodiscard]] static char* new_s( const std::wstring_view& stringUnicode, char* pbszCurrent );
   /// creates new string from strings sent to method, if first is null then just create new string from last `stringAdd`, ascii version
   [[nodiscard]] static char* new_s(const char* pbszUtf8First, const std::string_view& stringIfFirst, const std::string_view& stringAdd) { return new_s( pbszUtf8First, stringIfFirst, stringAdd, nullptr ); }
   [[nodiscard]] static char* new_s(const char* pbszUtf8First, const std::string_view& stringIfFirst, const std::string_view& stringAdd, char* pbszCurrent);  // ascii
   /// creates new string from strings sent to method, if first is null then just create new string from last `stringAdd` (unicode)
   [[nodiscard]] static char* new_s( const char* pbszUtf8First, const std::string_view& stringIfFirst, const std::wstring_view& stringAdd ) { return new_s( pbszUtf8First, stringIfFirst, stringAdd, nullptr ); }
   [[nodiscard]] static char* new_s(const char* pbszUtf8First, const std::string_view& stringIfFirst, const std::wstring_view& stringAdd, char* pbszCurrent); // unicode
   /// create new text and add utf8 text sent
#  if defined(__cpp_char8_t)
   [[nodiscard]] static char* new_s(const char* pbszUtf8First, const std::string_view& stringIfFirst, const char8_t* pbszUtf8Add) { return new_s( pbszUtf8First, stringIfFirst, pbszUtf8Add, nullptr); }
   [[nodiscard]] static char* new_s( const char* pbszUtf8First, const std::string_view& stringIfFirst, const char8_t* pbszUtf8Add, char* pbszCurrent ); // utf8 (no conversion)
#  endif
   [[nodiscard]] static char* new_s(const char* pbszUtf8First, const std::string_view& stringIfFirst, const char* pbszUtf8Add, gd::utf8::utf8_tag) { return new_s(pbszUtf8First, stringIfFirst, pbszUtf8Add, nullptr, gd::utf8::utf8_tag{}); }
   [[nodiscard]] static char* new_s(const char* pbszUtf8First, const std::string_view& stringIfFirst, const char* pbszUtf8Add, char* pbszCurrent, gd::utf8::utf8_tag); // utf8 (no conversion)
   
   /// clear text if not null
   static void clear_s(char** ppbsz) {
      if( *ppbsz != nullptr ) { delete [] *ppbsz; *ppbsz = nullptr; }
   }

   // ## string converters
   /// convert char string to std::wstring
   [[nodiscard]] static std::wstring to_wstring_s( const char* pbsz ) { 
      std::wstring stringReturn;
      gd::utf8::convert_utf8_to_uft16((const uint8_t*)pbsz, stringReturn);
      return stringReturn;
   }

   // ## copy
   /// clone text into allocated buffer in heap
   [[nodiscard]] static char* clone_s(char* pbsz) {
      if( pbsz == nullptr ) return pbsz;
      auto uLength = strlen(pbsz);
      char* pbszTemp = allocate_s( uLength );
      strcpy(pbszTemp, pbsz);
      return pbszTemp;
   }

   // ## appenders
   /// joins two texts and deletes the first text sent, pointer returned is allocated on heap (need to be deleted)
   [[nodiscard]] char* append_s(char** ppbszText, const std::string_view& stringAdd) { return append_s(ppbszText, stringAdd, nullptr); }
   [[nodiscard]] char* append_s(char** ppbszText, const std::string_view& stringAdd, char* pbszCurrent);
   /// joins two texts and deletes both, returned text pointer is allocated on heap (need to be deleted)
   [[nodiscard]] static char* join_s( char** ppbszText, char** ppbszAdd );
   /// joins three texts where the second one is just a pointer, good for combining text with separator
   [[nodiscard]] static char* join_s(char** ppbszText, const std::string_view& stringAdd, char** ppbszAdd);

   // ## wrappers
   [[nodiscard]] static std::wstring wrap_s(wchar_t chBefore, const std::wstring_view& stringText, wchar_t chAfter);

   // ## format text
   [[nodiscard]] static std::unique_ptr<char> printf_s(const char* pbszFormat, va_list va_listArgument);
   [[nodiscard]] static std::unique_ptr<char> printf_s(const wchar_t* pwszFormat, va_list va_listArgument);

   // ## value generators
   static std::wstring get_now_date_as_wstring_s();
   static std::wstring get_now_time_as_wstring_s();

   
};

/// check if message is "empty", it is empty if there isn't any text attached
inline bool message::empty() const { 
   if( m_pbszTextView == nullptr && m_pbszText == nullptr ) return true; 
   return false;
}

/// append text from format object (format has logic for same as C++20 format in standard template library)
inline message& message::append(const format& formatAppend) {
   m_pbszText.reset(new_s(m_pbszText.get(), std::string_view{ "  " }, (const char*)formatAppend, m_pbszText.release(), gd::utf8::utf8_tag{}));
   return *this;
}

/// standard allocator for text messages, this will always allocate
inline char* message::allocate_s(std::size_t uSize) {
   uSize++;                                                                      // add one for zero terminator
   if( uSize < MESSAGE_BUFFER_START_SIZE) uSize = MESSAGE_BUFFER_START_SIZE;
   return new char[uSize];
}

/// standard allocator for text messages, this will only allocate if pointer is null or size is over defined start size
inline char* message::allocate_s(std::size_t uSize, char* pbszCurrent) {
   uSize++;                                                                      // add one for zero terminator
   if( pbszCurrent != nullptr && uSize < MESSAGE_BUFFER_START_SIZE ) return pbszCurrent;
   if( uSize < MESSAGE_BUFFER_START_SIZE) uSize = MESSAGE_BUFFER_START_SIZE;
   char* pbszNew = new char[uSize];
   //delete[] pbszCurrent;
   return pbszNew;
}

// ================================================================================================
// ================================================================================= printer_file
// ================================================================================================


/*-----------------------------------------*/ /**
 * \brief interface for connected printers in logger. 
 *
 * Add any number of printers to logger, each printer can have it's specific logic on how
 * to generate log information.
 */
class i_printer
{
public:
   i_printer() {}
   i_printer( unsigned uSeverity ) : m_uSeverity( uSeverity ) {}

   i_printer( const i_printer& o ): m_uSeverity( o.m_uSeverity ) {}

   i_printer& operator=(const i_printer& o) { m_uSeverity = o.m_uSeverity; return *this; }

   virtual ~i_printer() {}

protected:
   void common_construct(const i_printer& o) { m_uSeverity = o.m_uSeverity; }

public:
   /** \name GET/SET
   *///@{
   unsigned get_severity() const { return m_uSeverity; }
   void set_severity(unsigned uSeverity) { m_uSeverity = uSeverity; }
   //@}

   /// This is called when logger send (prints) message to attached printers.
   /// Each printer needs to implement this in order to print something.
   /// \return true if ok, false if error (get error information from error method)
   virtual bool print(const message& message) { return true; };

   /// may be called occasionally and printer should here finish jobs that are
   /// pending, may be some sort of heavy write to media.
   /// \return true if ok, false if error (get error information from error method)
   virtual bool flush() { return true; };

   /// Collect error information, if printer has some internal error then call
   /// error to get information about internal error if there is any
   /// \param message gets error information
   /// \return number of errors left to get
   virtual unsigned error( message& message ) { return 0; };
public:
   unsigned m_uSeverity = 0;     ///< setting severity filter for printer
};


// ================================================================================================
// ================================================================================= logger
// ================================================================================================


/**
 * \brief logger is the meat in log functionality, use this to send log messages to printers
 *
 * logger is a tiny class, it only has two members, one array holding pointer to printers
 * and selected filter. Filter is used to filter what type of messages to print.
 * 
 * logger is a template and by setting a new logger id compiler will generate a new
 * logger class. logger are singletons so this is necessary in order to have different
 * loggers.
 *
 \code
 \endcode
 */
template<int iLoggerKey, bool bThread = false>
class logger 
{
// ## construction -------------------------------------------------------------
public:
   logger(): m_uSeverity( enumSeverity::eSeverityNone ) {}
   ~logger() {}

private:
   logger( const logger& ) {}
   
private:

// ## operator -----------------------------------------------------------------
public:
   

// ## methods ------------------------------------------------------------------
public:
/** \name GET/SET
*///@{
   unsigned get_severity() const { return m_uSeverity;  }
   void set_severity( unsigned uSeverity ) { m_uSeverity = uSeverity;  }
//@}

/** \name OPERATION
*///@{
   void append(std::unique_ptr<i_printer> pprinter) { m_vectorPrinter.push_back( std::move(pprinter) ); }

   void print(const message& message) { print(message, true); }

   virtual void print( const message& message, bool bFlush );
   virtual void print( std::initializer_list<message> listMessage );
   virtual void flush();

   /// remove all printers
   void clear() { m_vectorPrinter.clear(); }

   // ## error methods used to manage printer errors

   /// add error from message (is converted to string)
   void error_push(const message& messageError) { m_vectorError.push_back( messageError ); }
   /// pop error from list of internal errors
   std::string error_pop();
   /// count number of internal printer errors
   size_t error_size() const { return m_vectorError.size(); }

//@}

protected:
   // internal printing
   void print_(const message& message);
/** \name INTERNAL
*///@{
   /// check severity against internal severity filter
   /// two checks:
   ///   first is to check param severity level against member severity level.
   ///   if param severity level is lower compared to member severity level return true (logger is above and should take action)
   ///   second test check bit, if bit is matched then take action (return true)
   bool check_severity(unsigned uSeverity) const {
      if( (m_uSeverity & static_cast<unsigned>(enumSeverityMask::eSeverityMaskNumber)) >= (uSeverity & static_cast<unsigned>(enumSeverityMask::eSeverityMaskNumber)) ) return true;
      else if( (m_uSeverity & uSeverity & static_cast<unsigned>(enumSeverityMask::eSeverityMaskFlagAndGroup) ) != 0 ) return true;
      return false;
   }
   
//@}

public:
/** \name DEBUG
*///@{
   
//@}


// ## attributes ----------------------------------------------------------------
public:
   unsigned m_uSeverity;                     ///< severity filter, used to filter log messages
   std::vector<std::unique_ptr<i_printer>> m_vectorPrinter;///< list of connected printers
   std::vector<std::string> m_vectorError;   ///< list of internal errors stored as text

   static std::mutex m_mutex_s;              ///< mutex to enable thread safety printing messages
   
   
// ## free functions ------------------------------------------------------------
public:
   /// return pointer to logger with selected instance number
   static logger<iLoggerKey,bThread>* get_s();
   /// get reference to logger for selected instance number
   static logger<iLoggerKey,bThread>& get_instance_s();
   /// get reference to mutex if logger is set to thread safe mode
   static std::mutex& get_mutex_s();
};

/// generate static mutex object
template<int iLoggerKey, bool bThread>
std::mutex logger<iLoggerKey, bThread>::m_mutex_s;

#ifndef GD_LOG_DISABLE_ALL                                                       // GD_LOG_DISABLE_ALL {

/// ----------------------------------------------------------------------------
/// Sends message to all attached printers, 
template<int iLoggerKey, bool bThread>
void logger<iLoggerKey,bThread>::print(const message& message, bool bFlush)
{
   if( check_severity(message.get_severity()) )                                  // check if message has severity within bounds for output
   {
      // if template argument bThread is set to true this print method will be thread safe
      if constexpr( bThread == true )
      {
         std::lock_guard<std::mutex> lock(get_mutex_s());
         print_( message );
         if( bFlush == true ) flush();
      }
      else
      {
         print_( message );
         if( bFlush == true ) flush();
      }
   }
}

/// ----------------------------------------------------------------------------
/// Sends message list to all attached printers, 
template<int iLoggerKey, bool bThread>
void logger<iLoggerKey, bThread>::print(std::initializer_list<message> listMessage)
{
   auto itBegin = listMessage.begin();
   //message* pmessage = &(*listMessage.begin());
   if( itBegin->check_severity(m_uSeverity) )                                   // check first message has severity within bounds for output
   {
      // if template argument bThread is set to true this print method will be thread safe
      if constexpr( bThread == true )
      {
         std::lock_guard<std::mutex> lock(get_mutex_s());

         // ## print message to all attached printers
         for( auto it = m_vectorPrinter.begin(); it != m_vectorPrinter.end(); ++it )
         {
            for( auto itMessage : listMessage )
            {
               if( (*it)->print(itMessage) == false )                               // sen message to printer
               {
                  // ## collect error information and store it in logger
                  gd::log::message messageError;
                  (*it)->error(messageError);
                  if( messageError.empty() == false ) error_push(messageError);     // push error text to logger error stack
               }
            }
         }
         flush();
      }
      else
      {
         // ## print message to all attached printers
         for( auto it = m_vectorPrinter.begin(); it != m_vectorPrinter.end(); ++it )
         {
            for( auto itMessage : listMessage )
            {
               if( (*it)->print(itMessage) == false )                               // sen message to printer
               {
                  // ## collect error information and store it in logger
                  gd::log::message messageError;
                  (*it)->error(messageError);
                  if( messageError.empty() == false ) error_push(messageError);     // push error text to logger error stack
               }
            }
         }
         flush();
      }
   }
}


template<int iLoggerKey, bool bThread>
void logger<iLoggerKey, bThread>::print_(const message& message)
{
   // ## print message to all attached printers
   for( auto it = m_vectorPrinter.begin(); it != m_vectorPrinter.end(); it++ )
   {
      // check printer severity filter compared to message severity
      // if printer has 0 as severity then it will always print
      // if severity is set to printer then check against message severity
      //    it will print if severity in printer is lower or equal to message.
      //    if printer severity is set to FATAL, then it will only print FATAL messages.
      if( (*it).get()->get_severity() == 0 || message.check_severity((*it).get()->get_severity()) == true )
      {
         if( (*it)->print(message) == true ) continue;
      }

      // ## if print returned false there we have one internal error in printer
      {
         gd::log::message messageError;
         (*it)->error(messageError);
         if( messageError.empty() == false ) error_push(messageError);
      }
   }
}




#else
template<int iLoggerKey>
void logger<iLoggerKey,bThread>::print(const message& message)
{
}

template<int iLoggerKey, bool bThread>
void logger<iLoggerKey, bThread>::print(std::initializer_list<message> listMessage)
{
}

#endif                                                                           // } GD_LOG_DISABLE_ALL 



/// ----------------------------------------------------------------------------
/// Flush all connected printers. 
template<int iLoggerKey, bool bThread>
void logger<iLoggerKey,bThread>::flush()
{
   // ## print message to all attached printers
   for( auto it = m_vectorPrinter.begin(); it != m_vectorPrinter.end(); ++it )
   {
      (*it)->flush();
   }
}

template<int iLoggerKey, bool bThread>
std::string logger<iLoggerKey,bThread>::error_pop()
{
   if( m_vectorError.empty() == false )
   {
      std::string stringReturn(std::move(m_vectorError.back()));
      m_vectorError.pop_back();
      return stringReturn;
   }

   return std::string();
}


/// ----------------------------------------------------------------------------
/// static member returning pointer to logger for selected instance id
template<int iLoggerKey, bool bThread>
inline logger<iLoggerKey,bThread>* logger<iLoggerKey,bThread>::get_s() {
   return &logger<iLoggerKey,bThread>::get_instance_s();
}

/// ----------------------------------------------------------------------------
/// global method returning pointer to logger for selected instance id
template<int iLoggerKey, bool bThread=false>
inline logger<iLoggerKey,bThread>* get_g() {
   return &logger<iLoggerKey,bThread>::get_instance_s();
}

/// ----------------------------------------------------------------------------
/// return pointer to logger for default instance id ( id = zero )
inline logger<GD_LOG_DEFAULT_INSTANCE_ID>* get_s() {
   return &logger<GD_LOG_DEFAULT_INSTANCE_ID>::get_instance_s();
}






/*----------------------------------------------------------------------------- get_instance_s */ /**
 * Get reference to logger instance. 
 * logger is a singleton where each logger instance are created from template parameter (integer value)
Sample showing four different loggers, compiler will generate these based on the integer 
```cpp
auto plogger = gd::log::get_s();
auto plogger1 = gd::log::get_s<1>();
auto plogger2 = gd::log::logger::get_s<2>();
gd::log::logger<3>* plogger3 = gd::log::get_s<3>();
```

Generate thread safe logger
```cpp
// setting second template argument to true will generate a thread safe logger
auto ploggerThreadSafe = gd::log::get_s<1, true>();
```

 * \return logger<iLoggerKey,bThread>& reference to logger instance
 */
template<int iLoggerKey, bool bThread>
logger<iLoggerKey,bThread>& logger<iLoggerKey,bThread>::get_instance_s() {
   static logger<iLoggerKey,bThread> logger_s;
   return logger_s;
}

template<int iLoggerKey, bool bThread>
std::mutex& logger<iLoggerKey, bThread>::get_mutex_s() {
   return logger<iLoggerKey, bThread>::m_mutex_s;
}



/*----------------------------------------------------------------------------- severity_get_number_g */ /**
 * Get severity number from severity name
Get severity number
```cpp
   auto eSeverityNumber = severity_get_number_g("FATAL");
   if( eSeverityNumber == severity_get_number_g("F") ) std::count << "FATAL and F returns same severity number" << std::endl;
```
 * \param stringSeverity severity name that severity number is returned for (matches on first character)
 * \return constexpr enumSeverity severity number for severity name
 */
constexpr enumSeverity severity_get_number_g(const std::string_view& stringSeverity)
{                                                                                assert(stringSeverity.empty() == false);
   // ## convert character to uppercase if lowercase is found
   constexpr uint8_t LOWER_A = 'a';
   uint8_t uFirst = (uint8_t)stringSeverity[0];
   if( uFirst >= LOWER_A ) uFirst -= 32;

   switch( uFirst )
   {
   case 'F': return enumSeverity::eSeverityFatal;
   case 'E': return enumSeverity::eSeverityError;
   case 'W': return enumSeverity::eSeverityWarning;
   case 'I': return enumSeverity::eSeverityInformation;
   case 'D': return enumSeverity::eSeverityDebug;
   case 'V': return enumSeverity::eSeverityVerbose;
   default:  return enumSeverity::eSeverityNone;
   }
}


/*----------------------------------------------------------------------------- severity_get_group_g */ /**
 * Get severity group number from severity name
 * \param stringSeverity severity name (matches on first character in name)
 * \return enumSeverityGroup severity group number
 */
constexpr enumSeverityGroup severity_get_group_g(const std::string_view& stringSeverity)
{                                                                                assert(stringSeverity.empty() == false);
   // ## convert character to uppercase if lowercase is found
   constexpr uint8_t LOWER_A = 'a';
   uint8_t uFirst = (uint8_t)stringSeverity[0];
   if( uFirst >= LOWER_A ) uFirst -= 32;

   switch( uFirst )
   {
   case 'F': return enumSeverityGroup::eSeverityGroupFatal;
   case 'E': return enumSeverityGroup::eSeverityGroupError;
   case 'W': return enumSeverityGroup::eSeverityGroupWarning;
   case 'I': return enumSeverityGroup::eSeverityGroupInformation;
   case 'D': return enumSeverityGroup::eSeverityGroupDebug;
   case 'V': return enumSeverityGroup::eSeverityGroupVerbose;
   default:  return enumSeverityGroup::eSeverityGroupNone;
   }
}


template<typename TYPE>
constexpr enumSeverity severity_get_g(TYPE typeSeverity)
{
   if constexpr( std::is_pointer<TYPE>::value )
      return severity_get_number_g(typeSeverity);
   else
      return typeSeverity;
}



_GD_LOG_LOGGER_END