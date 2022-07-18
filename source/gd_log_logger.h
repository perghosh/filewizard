/**
 * \file gd_log_logger.h
 * 
 * \brief Core file for logger functionality
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
---------------------
**Geting started** *Sample getting default logger (id=0), adds printer and sets severity filter and prints a message*
```cpp
using namespace gd::log;

gd::log::logger<0>* plogger = gd::log::get_s();                             // get pointer to logger 0
plogger->append( std::make_unique<gd::log::printer_console>() );            // append printer to logger
plogger->set_severity(eSeverityNumberWarning | eSeverityGroupDebug);        // set severity filter, messages within this filter is printed
plogger->print(message(eSeverityGroupDebug, eMessageTypeTime).printf("%s", "## MESSAGE ##")); plogger->flush();// write to logger
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


#include "gd_utf8.hpp"

#pragma warning( disable: 4996 )

#ifndef GD_LOG_DEFAULT_INSTANCE_ID
#   define GD_LOG_DEFAULT_INSTANCE_ID 0
#endif



#ifndef _GD_LOG_LOGGER_BEGIN

#  define _GD_LOG_LOGGER_BEGIN namespace gd::log {
#  define _GD_LOG_LOGGER_END }

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
   eSeverityNumberNone = 0,         ///< allways write this, no severity set
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


enum enumSeverity
{
   eSeverityNone =         eSeverityNumberNone,
   eSeverityFatal =        eSeverityGroupFatal           | eSeverityNumberFatal,
   eSeverityError =        eSeverityGroupError           | eSeverityNumberError,
   eSeverityWarning =      eSeverityGroupWarning         | eSeverityNumberNone,
   eSeverityInformation =  eSeverityGroupInformation     | eSeverityNumberWarning,
   eSeverityDebug =        eSeverityGroupDebug           | eSeverityNumberDebug,
   eSeverityVerbose =      eSeverityGroupVerbose         | eSeverityNumberVerbose,
};

enum enumSeverityMask
{
   eSeverityMaskNumber = 0x0000'00FF,
   eSeverityMaskGroup  = 0x0000'FF00,
};


enum enumMessageType
{
   eMessageTypeText = 0,
   eMessageTypeMethodName = (1 << 1),
   eMessageTypeFileName = (1 << 2),
   eMessageTypeSeverity = (1 << 3),
   eMessageTypeTime = (1 << 4),
   eMessageTypeDate = (1 << 5),

   eMessageTypeAll = eMessageTypeMethodName | eMessageTypeFileName | eMessageTypeSeverity | eMessageTypeTime | eMessageTypeDate,
};



const char* severity_get_name_g(unsigned uSeverity);
//constexpr static enumSeverityNumber severity_get_number_g(const std::string_view& stringSeverity);
//constexpr static enumSeverityGroup severity_get_group_g(const std::string_view& stringSeverity);


/*-----------------------------------------*/ /**
 * \brief compatible stringstream logic for logger
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

/*-----------------------------------------*/ /**
 * \brief compatible stringstream logic for logger
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

/*-----------------------------------------*/ /**
 * \brief 
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
      m_pbszText.reset(message::new_s(stringResult));
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
      m_pbszText.reset(message::new_s(stringResult));
   }

#  if defined(__cpp_char8_t)
   operator const char8_t*() const { return (const char8_t*)m_pbszText.get();  }
#  endif
   operator const char*() const { return (const char*)m_pbszText.get();  }
   // attributes
   public:
      std::unique_ptr<char> m_pbszText;
};


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
   explicit message( unsigned uSeverity, enumMessageType eMessageType ): m_uSeverity( uSeverity ), m_eMessageType(eMessageType) {}
   explicit message( unsigned uSeverity, const char* pbszText ): m_uSeverity( uSeverity ), m_pbszTextView(pbszText) {}
   message( unsigned uSeverity, enumMessageType eMessageType, const char* pbszText ): m_uSeverity( uSeverity ), m_eMessageType(eMessageType), m_pbszTextView(pbszText) {}

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
      m_eMessageType = o.m_eMessageType;
      m_pbszTextView = o.m_pbszTextView;
      m_pbszText.reset( clone_s(o.m_pbszText.get()) );
   }
   void common_construct( message&& o ) noexcept {
      m_uSeverity = o.m_uSeverity;
      m_eMessageType = o.m_eMessageType;
      m_pbszText = std::move(o.m_pbszText);
      m_pbszTextView = o.m_pbszTextView;
   }

// ## operator -----------------------------------------------------------------
public:
   message& operator<<(const std::string_view& stringAppend) { return append(stringAppend); }
   message& operator<<(const std::wstring_view& stringAppend) { return append(stringAppend); }
#  if defined(__cpp_char8_t)
   message& operator<<(const char8_t* pbszUtf8Append) { return append(pbszUtf8Append); }
#  endif
   message& operator<<(const stream& streamAppend) { return append(streamAppend); }
   message& operator<<(const wstream& streamAppend) { return append(streamAppend); }
   message& operator<<(const format& formatAppend) { return append(formatAppend); }
   //message& operator<<(std::wostream& ) { return append(formatAppend); }
   template<typename APPEND>
   message& operator<<(APPEND appendValue) {
      std::wstringstream wstringstreamAppend;
      wstringstreamAppend << appendValue;
      m_pbszText.reset(new_s(m_pbszText.release(), std::string_view{ "  " }, wstringstreamAppend.str().c_str()));
      return *this;
   }


// ## methods ------------------------------------------------------------------
public:
/** \name GET/SET
*///@{
   // ## getters and setters for severity information
   unsigned get_severity() const { return m_uSeverity; }
   enumSeverityNumber get_severity_number() const { return (enumSeverityNumber)(m_uSeverity & eSeverityMaskNumber); }
   enumSeverityGroup get_severity_group() const { return (enumSeverityGroup)(m_uSeverity & eSeverityMaskGroup); }
   void set_severity(unsigned uSeverity) { m_uSeverity = uSeverity; }
//@}

/** \name OPERATION
*///@{
   /// check if message has any special type set, these type are used to produce 
   bool is_message_type_set() const { return m_eMessageType != 0; }
   bool is_severity() const { return (m_eMessageType & eMessageTypeSeverity); }
   bool is_time() const { return m_eMessageType & eMessageTypeTime; }
   bool is_date() const { return m_eMessageType & eMessageTypeDate; }

   /// check if message has severity level below or equal to (max) severity sent
   /// ``` cpp
   /// messageItem.is_active( eSeverityError ); // returns true if messageItem is eSeverityError, eSeverityFatal or eSeverityNone
   /// ```
   bool check_severity(unsigned uSeverity) const { 
      if( (uSeverity & eSeverityMaskNumber) >= (m_uSeverity & eSeverityMaskNumber) ) return true;
      else if( (uSeverity & m_uSeverity & eSeverityMaskGroup) != 0 ) return true;
      return false;
   }
   template <typename INTEGER>
   bool check_severity(INTEGER uSeverity) const {
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

   message& printf( const char* pbszFormat, ... );
   message& printf( const wchar_t* pwszFormat, ...);

   std::string to_string() const;
   std::wstring to_wstring() const { return to_wstring_s(get_text_all()); };

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
   enumMessageType m_eMessageType = enumMessageType::eMessageTypeText;
   std::unique_ptr<char> m_pbszText;   ///< when message need to control text it is placed here
   const char* m_pbszTextView = nullptr; ///< pointer to text 

   
// ## free functions ------------------------------------------------------------
public:
   [[nodiscard]] static char* new_s( std::size_t uSize ) { return new char[uSize]; }
   [[nodiscard]] static char* new_s( std::string_view stringAscii );
   [[nodiscard]] static char* new_s( std::wstring_view stringUnicode );
   /// creates new string from strings sent to method, if first is null then just create new string from last `stringAdd`
   [[nodiscard]] static char* new_s( const char* pbszUtf8First, const std::string_view& stringIfFirst, const std::string_view& stringAdd );  // ascii
   /// creates new string from strings sent to method, if first is null then just create new string from last `stringAdd`
   [[nodiscard]] static char* new_s( const char* pbszUtf8First, const std::string_view& stringIfFirst, const std::wstring_view& stringAdd ); // unicode
   /// create new text and add utf8 text sent
#  if defined(__cpp_char8_t)
   [[nodiscard]] static char* new_s( const char* pbszUtf8First, const std::string_view& stringIfFirst, const char8_t* pbszUtf8Add ); // utf8 (no conversion)
#  endif
   [[nodiscard]] static char* new_s(const char* pbszUtf8First, const std::string_view& stringIfFirst, const char* pbszUtf8Add, gd::utf8::utf8_tag); // utf8 (no conversion)
   
   /// clear text if not null
   static void clear_s(char** ppbsz) {
      if( *ppbsz != nullptr ) { delete [] *ppbsz; *ppbsz = nullptr; }
   }

   /// convert char string to std::wstring
   [[nodiscard]] static std::wstring to_wstring_s( const char* pbsz ) { 
      std::wstring stringReturn;
      gd::utf8::convert_utf8_to_uft16((const uint8_t*)pbsz, stringReturn);
      return stringReturn;
   }

   /// clone text into allocated buffer in heap
   [[nodiscard]] static char* clone_s(char* pbsz) {
      if( pbsz == nullptr ) return pbsz;
      auto uLength = strlen(pbsz);
      char* pbszTemp = new char[uLength + 1];
      strcpy(pbszTemp, pbsz);
      return pbszTemp;
   }

   /// joins two texts and deletes the first text sent, pointer returned is allocated on heap (need to be deleted)
   [[nodiscard]] char* append_s(char** ppbszText, const std::string_view& stringAdd);
   /// joins two texts and deletes both, returned text pointer is allocated on heap (need to be deleted)
   [[nodiscard]] static char* join_s( char** ppbszText, char** ppbszAdd );
   /// joins three texts where the second one is just a pointer, good for combining text with separator
   [[nodiscard]] static char* join_s(char** ppbszText, const std::string_view& stringAdd, char** ppbszAdd);

   static std::wstring get_now_date_as_wstring_s();
   static std::wstring get_now_time_as_wstring_s();

   
};

inline message& message::append(const format& formatAppend) {
   m_pbszText.reset(new_s(m_pbszText.get(), std::string_view{ "  " }, (const char*)formatAppend, gd::utf8::utf8_tag{}));
   return *this;
}



/*-----------------------------------------*/ /**
 * \brief interface for connected printers in logger. 
 *
 * Add any number of printers to logger, each printer can have it's specific logic on how
 * to generate log information.
 */
class i_printer
{
public:
   virtual ~i_printer() {}

   virtual void print(const message& message) {};
   virtual void flush() {};
};




/**
 * \brief 
 *
 *
 *
 \code
 \endcode
 */
template<int iLoggerKey>
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

   virtual void print( const message& message );
   virtual void print( std::initializer_list<message> listMessage );
   virtual void flush();
//@}

protected:
/** \name INTERNAL
*///@{
   bool check_severity(unsigned uSeverity) const {
      if( (m_uSeverity & eSeverityMaskNumber) >= (uSeverity & eSeverityMaskNumber) ) return true;
      else if( (m_uSeverity & uSeverity & eSeverityMaskGroup) != 0 ) return true;
      return false;
   }
   
//@}

public:
/** \name DEBUG
*///@{
   
//@}


// ## attributes ----------------------------------------------------------------
public:
   unsigned m_uSeverity;
   std::vector<std::unique_ptr<i_printer>> m_vectorPrinter;
   
   
// ## free functions ------------------------------------------------------------
public:
   /// return pointer to logger with selected instance number
   static logger<iLoggerKey>* get_s();
   /// get reference to logger for selected instance number
   static logger<iLoggerKey>& get_instance_s();
};

#ifndef GD_LOG_DISABLE_ALL

/// ----------------------------------------------------------------------------
/// Sends message to all attached printers, 
template<int iLoggerKey>
void logger<iLoggerKey>::print(const message& message)
{
//   if( message.check_severity(m_uSeverity) )                                     // check if message has severity within bounds for output
   if( check_severity(message.get_severity()) )                                                   // check if message has severity within bounds for output
   {
      // ## print message to all attached printers
      for( auto it = m_vectorPrinter.begin(); it != m_vectorPrinter.end(); ++it )
      {
         (*it)->print(message);
      }
   }
}
#else
template<int iLoggerKey>
void logger<iLoggerKey>::print(const message& message)
{
}
#endif

/// ----------------------------------------------------------------------------
/// Sends message list to all attached printers, 
template<int iLoggerKey>
void logger<iLoggerKey>::print(std::initializer_list<message> listMessage)
{
   auto itBegin = listMessage.begin();
   //message* pmessage = &(*listMessage.begin());
   if( itBegin->check_severity(m_uSeverity) )                                   // check first message has severity within bounds for output
   {
      // ## print message to all attached printers
      for( auto it = m_vectorPrinter.begin(); it != m_vectorPrinter.end(); ++it )
      {
         for( auto itMessage: listMessage) (*it)->print(itMessage);
      }
   }

   flush();
}


/// ----------------------------------------------------------------------------
/// Flush all connected printers. 
template<int iLoggerKey>
void logger<iLoggerKey>::flush()
{
   // ## print message to all attached printers
   for( auto it = m_vectorPrinter.begin(); it != m_vectorPrinter.end(); ++it )
   {
      (*it)->flush();
   }
}


/// ----------------------------------------------------------------------------
/// static member returning pointer to logger for selected instance id
template<int iLoggerKey>
inline logger<iLoggerKey>* logger<iLoggerKey>::get_s() {
   return &logger<iLoggerKey>::get_instance_s();
}

/// ----------------------------------------------------------------------------
/// global method returning pointer to logger for selected instance id
template<int iLoggerKey>
inline logger<iLoggerKey>* get_g() {
   return &logger<iLoggerKey>::get_instance_s();
}

/// ----------------------------------------------------------------------------
/// return pointer to logger for default instance id ( id = zero )
inline logger<GD_LOG_DEFAULT_INSTANCE_ID>* get_s() {
   return &logger<GD_LOG_DEFAULT_INSTANCE_ID>::get_instance_s();
}



/// ----------------------------------------------------------------------------
/// Return reference to logger for instance id. Here is where each logger is created.
/// *sample*
/// `auto plogger = gd::log::get_s();`  
/// `auto plogger1 = gd::log::get_s<1>();`  
/// `auto plogger2 = gd::log::logger::get_s<2>();`  
/// `gd::log::logger<3>* plogger3 = gd::log::get_s<3>();`  
template<int iLoggerKey>
logger<iLoggerKey>& logger<iLoggerKey>::get_instance_s() {
   static logger<iLoggerKey> logger_s;
   return logger_s;
}


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


_GD_LOG_LOGGER_END