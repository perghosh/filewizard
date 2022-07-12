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
 */

#pragma once
#include <cassert>
#include <string>
#include <string_view>
#include <format>
#include <vector>
#include <type_traits>
#include <memory>

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

struct tag_text {};


/**
 * \brief message level to print, how severe information sent to printers
 *
 *
 *
 */
enum class enumSeverity
{
   eSeverityNone = 0,
   eSeverityFatal = 1,
   eSeverityError = 2,
   eSeverityWarning = 3,
   eSeverityInfo = 4,
   eSeverityDebug = 5,
   eSeverityVerbose = 6,
};

enum class enumMessageType
{
   eMessageTypeText           = 0,
   eMessageTypeMethodName     = (1<<1),
   eMessageTypeFileName       = (1<<2),
   eMessageTypeTime           = (1<<3),
};



const char* severity_get_name_g(enumSeverity eSeverity);
enumSeverity severity_get_number_g(const std::string_view& stringSeverity);



/*-----------------------------------------*/ /**
 * \brief 
 *
 *
 */
struct format 
{
   format() {}
   format( const format& o ) { common_construct( o ); }
   format( format&& o ) noexcept { common_construct( o ); }
   format& operator=( const format& o ) { common_construct( o ); return *this; }
   format& operator=( format&& o ) noexcept { common_construct( o ); return *this; }

   void common_construct( const format& o ) {}
   void common_construct( format&& o ) noexcept {}

   // attributes
   public:
};

/*-----------------------------------------*/ /**
 * \brief 
 *
 *
 */
struct print 
{
   print() {}
   print( const print& o ) { common_construct( o ); }
   print( print&& o ) noexcept { common_construct( o ); }
   print& operator=( const print& o ) { common_construct( o ); return *this; }
   print& operator=( print&& o ) noexcept { common_construct( o ); return *this; }

   void common_construct( const print& o ) {}
   void common_construct( print&& o ) noexcept {}

   // attributes
   public:
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
   message(): m_eSeverity( enumSeverity::eSeverityNone ) {}
   explicit message( enumSeverity eSeverity ): m_eSeverity( eSeverity ) {}
   explicit message( enumSeverity eSeverity, const char* pbszText ): m_eSeverity( eSeverity ), m_pbszTextView(pbszText) {}
   message( enumSeverity eSeverity, enumMessageType eMessageType, const char* pbszText ): m_eSeverity( eSeverity ), m_eMessageType(eMessageType), m_pbszTextView(pbszText) {}

   message( const char* pbszMessage ): m_eSeverity( enumSeverity::eSeverityNone ), m_pbszTextView( pbszMessage ) {}
   message( const char8_t* pbszMessage ): m_eSeverity( enumSeverity::eSeverityNone ), m_pbszTextView( (const char*)pbszMessage ) {}
   message(const wchar_t* pwszMessage) : m_eSeverity(enumSeverity::eSeverityNone), m_pbszText( new_s( pwszMessage ) ) {}
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
      m_eSeverity = o.m_eSeverity;
      m_eMessageType = o.m_eMessageType;
      m_pbszTextView = o.m_pbszTextView;
      m_pbszText.reset( clone_s(o.m_pbszText.get()) );
   }
   void common_construct( message&& o ) noexcept {
      m_eSeverity = o.m_eSeverity;
      m_eMessageType = o.m_eMessageType;
      m_pbszText = std::move(o.m_pbszText);
      m_pbszTextView = o.m_pbszTextView;
   }

// ## operator -----------------------------------------------------------------
public:
   

// ## methods ------------------------------------------------------------------
public:
/** \name GET/SET
*///@{
   enumSeverity get_severity() const { return m_eSeverity; }
   void set_severity(enumSeverity eSeverity) { m_eSeverity = eSeverity; }
//@}

/** \name OPERATION
*///@{
   /// check if message has severity level below or equal to (max) severity sent
   /// ``` cpp
   /// messageItem.is_active( eSeverityError ); // returns true if messageItem is eSeverityError, eSeverityFatal or eSeverityNone
   /// ```
   bool check_severity(enumSeverity eSeverity) const { return eSeverity >= m_eSeverity;  }

   /// return 
   const char* get_text() const { return m_pbszTextView != nullptr ? m_pbszTextView : m_pbszText.get(); }
   /// returns message text and the priority is 1) TextView, 2) Text, 3) severity text
   const char* get_text_all() const { return m_pbszTextView != nullptr ? m_pbszTextView : (m_pbszText != nullptr ? m_pbszText.get() : severity_get_name_g( m_eSeverity )); }
   /// set ascii texts
   void set_text(std::string_view stringText);

   message& printf( const char* pbszFormat, ... );
   message& printf( const wchar_t* pwszFormat, ...);
   template <typename... ARGUMENTS>
   message& format(std::string_view stringFormat, ARGUMENTS&&... arguments);
   template <typename... ARGUMENTS>
   message& format(std::wstring_view stringFormat, ARGUMENTS&&... arguments);

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
   enumSeverity m_eSeverity;   ///< type of message severity, used to filter output
   enumMessageType m_eMessageType = enumMessageType::eMessageTypeText;
   //char* m_pbszText = nullptr;
   std::unique_ptr<char> m_pbszText;
   const char* m_pbszTextView = nullptr;

   
// ## free functions ------------------------------------------------------------
public:
   [[nodiscard]] static char* new_s(std::size_t uSize) { return new char[uSize]; }
   [[nodiscard]] static char* new_s( std::string_view stringUnicode );
   [[nodiscard]] static char* new_s( std::wstring_view stringUnicode );
   [[nodiscard]] static char* new_s( const char* pbszUtf8First, const std::string_view& stringIfFirst, const std::string_view& stringAdd );
   [[nodiscard]] static char* new_s(const char* pbszUtf8First, const std::string_view& stringIfFirst, const std::wstring_view& stringAdd);

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

   
};

/*----------------------------------------------------------------------------- format */ /**
 * See std::format on how to generate text, this member method forwards logic to format
 * \param stringFormat format string 
 * \param arguments arguments forwarded to vformat (vformat is used by std::format)
 * \return gd::log::message& return reference for chaining
 */
template <typename... ARGUMENTS>
message& message::format(std::string_view stringFormat, ARGUMENTS&&... arguments) {
   // same logic as std::format
   // takes a number of arguments and wraps them into std::format_args that has logic to convert arguments into formated text
   std::string stringResult = std::vformat(stringFormat, std::make_format_args(std::forward<ARGUMENTS>(arguments)...));
   // m_pbszText.reset(new_s(stringResult));

   m_pbszText.reset(new_s(m_pbszText.get(), std::string_view{ "  " }, stringResult));

   return *this;
}

/*----------------------------------------------------------------------------- format */ /**
 * See std::format on how to generate text, this member method forwards logic to format
 * \param stringFormat format string 
 * \param arguments arguments forwarded to vformat (vformat is used by std::format)
 * \return gd::log::message& return reference for chaining
 */
template <typename... ARGUMENTS>
message& message::format(std::wstring_view stringFormat, ARGUMENTS&&... arguments) {
   // same logic as std::format
   // takes a number of arguments and wraps them into std::format_args that has logic to convert arguments into formated text
   std::wstring stringResult = std::vformat(stringFormat, std::make_wformat_args(std::forward<ARGUMENTS>(arguments)...));
   m_pbszText.reset(new_s(m_pbszText.get(), std::string_view{ "  " }, stringResult));
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
   logger(): m_eSeverity( enumSeverity::eSeverityNone ) {}
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
   enumSeverity get_severity() const { return m_eSeverity;  }
   void set_severity( enumSeverity eSeverity ) { m_eSeverity = eSeverity;  }
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
   
//@}

public:
/** \name DEBUG
*///@{
   
//@}


// ## attributes ----------------------------------------------------------------
public:
   enumSeverity m_eSeverity;
   std::vector<std::unique_ptr<i_printer>> m_vectorPrinter;
   
   
// ## free functions ------------------------------------------------------------
public:
   static logger<iLoggerKey>& initialize_s(enumSeverity eSeverity, std::string_view stringFileName);

   /// return pointer to logger with selected instance number
   static logger<iLoggerKey>* get_s();
   /// get reference to logger for selected instance number
   static logger<iLoggerKey>& get_instance_s();
};

/// ----------------------------------------------------------------------------
/// Sends message to all attached printers, 
template<int iLoggerKey>
void logger<iLoggerKey>::print(const message& message)
{
   if( message.check_severity(m_eSeverity) )                                     // check if message has severity within bounds for output
   {
      // ## print message to all attached printers
      for( auto it = m_vectorPrinter.begin(); it != m_vectorPrinter.end(); ++it )
      {
         (*it)->print(message);
      }
   }
}

/// ----------------------------------------------------------------------------
/// Sends message list to all attached printers, 
template<int iLoggerKey>
void logger<iLoggerKey>::print(std::initializer_list<message> listMessage)
{
   auto itBegin = listMessage.begin();
   //message* pmessage = &(*listMessage.begin());
   if( itBegin->check_severity(m_eSeverity) )                                   // check first message has severity within bounds for output
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


template<int iLoggerKey>
inline logger<iLoggerKey>& logger<iLoggerKey>::initialize_s(enumSeverity eSeverity, std::string_view stringFileName)
{

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

_GD_LOG_LOGGER_END