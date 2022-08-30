/**
 * \file gd_log_logger_define.h
 * 
 * \brief macros used to simplify information sent to logger 
 * 
 */

#pragma once


#ifdef GD_LOG_SIMPLE

   // `LOG_` does it all, in the end all other log macros will call `LOG_`
   #define LOG_( uLogger, uSeverity, expression ) gd::log::get_g<uLogger,false>()->print( gd::log::message( gd::log::severity_get_g( uSeverity ), gd::log::eMessageTypeAll ) << __FILE__ << __func__ << expression )

   #define LOG( uSeverity, expression ) LOG_( 0, gd::log::severity_get_g( uSeverity ), expression )
   #define LOG_NR( uNumber, uSeverity, expression ) LOG_( uNumber, gd::log::severity_get_g( uSeverity ), expression )

   #define LOG_IF_(uLogger, uSeverity, condition, expression)  if(!(condition)) {;} else LOG_(uLogger, uSeverity, expression)
   #define LOG_IF(uSeverity, condition, expression)  LOG_IF_(0, uSeverity, condition, expression)
   #define LOG_IF_NR( uNumber, uSeverity, condition, expression)  LOG_IF_(uNumber, uSeverity, condition, expression)
   
   #define LOG_FATAL(expression)       LOG("FATAL", expression)
   #define LOG_ERROR(expression)       LOG("ERROR", expression)
   #define LOG_WARNING(expression)     LOG("WARNING", expression)
   #define LOG_INFORMATION(expression) LOG("INFORMATION", expression)
   #define LOG_DEBUG(expression)       LOG("DEBUG", expression)
   #define LOG_VERBOSE(expression)     LOG("VERBOSE", expression)
   #define LOG_NONE(expression)        LOG("NONE", expression)


   #define LOG_FATAL_IF(condition, expression)        LOG_IF("FATAL", condition, expression)
   #define LOG_ERROR_IF(condition, expression)        LOG_IF("ERROR", condition, expression)
   #define LOG_WARNING_IF(condition, expression)      LOG_IF("WARNING", condition, expression)
   #define LOG_INFORMATION_IF(condition, expression)  LOG_IF("INFORMATION", condition, expression)
   #define LOG_DEBUG_IF(condition, expression)        LOG_IF("DEBUG", condition, expression)
   #define LOG_VERBOSE_IF(condition, expression)      LOG_IF("VERBOSE", condition, expression)
   #define LOG_NONE_IF(condition, expression)         LOG_IF("NONE", condition, expression)


   #define LOG_FATAL_NR(uNumber, expression)       LOG_NR(uNumber, "FATAL", expression)
   #define LOG_ERROR_NR(uNumber, expression)       LOG_NR(uNumber, "ERROR", expression)
   #define LOG_WARNING_NR(uNumber, expression)     LOG_NR(uNumber, "WARNING", expression)
   #define LOG_INFORMATION_NR(uNumber, expression) LOG_NR(uNumber, "INFORMATION", expression)
   #define LOG_DEBUG_NR( uNumber, expression)      LOG_NR(uNumber, "DEBUG", expression)
   #define LOG_VERBOSE_NR(uNumber, expression)     LOG_NR(uNumber, "VERBOSE", expression)
   #define LOG_NONE_NR(uNumber, expression)        LOG_NR(uNumber, "NONE", expression)

   #define LOG_FATAL_IF_NR( uNumber, condition, expression)        LOG_IF_NR(uNumber, "FATAL", condition, expression)
   #define LOG_ERROR_IF_NR( uNumber, condition, expression)        LOG_IF_NR(uNumber, "ERROR", condition, expression)
   #define LOG_WARNING_IF_NR( uNumber, condition, expression)      LOG_IF_NR(uNumber, "WARNING", condition, expression)
   #define LOG_INFORMATION_IF_NR( uNumber, condition, expression)  LOG_IF_NR(uNumber, "INFORMATION", condition, expression)
   #define LOG_DEBUG_IF_NR( uNumber, condition, expression)        LOG_IF_NR(uNumber, "DEBUG", condition, expression)
   #define LOG_VERBOSE_IF_NR( uNumber, condition, expression)      LOG_IF_NR(uNumber, "VERBOSE", condition, expression)
   #define LOG_NONE_IF_NR( uNumber, condition, expression)         LOG_IF_NR(uNumber, "NONE", condition, expression)

#else

   #define LOG_( uLogger, uSeverity, expression ) ((void)0)

   #define LOG( uSeverity, expression ) LOG_( 0, gd::log::severity_get_g( uSeverity ), expression )
   #define LOG_NR( uNumber, uSeverity, expression ) LOG_( uNumber, gd::log::severity_get_g( uSeverity ), expression )


   #define LOG_IF_(uLogger, uSeverity, condition, expression)  if(!(condition)) {;} else LOG_(uLogger, uSeverity, expression)
   #define LOG_IF(uSeverity, condition, expression)  LOG_IF_(0, uSeverity, condition, expression)
   #define LOG_IF_NR( uNumber, uSeverity, condition, expression)  LOG_IF_(uNumber, uSeverity, condition, expression)


   #define LOG_FATAL(expression)       LOG("FATAL", expression)
   #define LOG_ERROR(expression)       LOG("ERROR", expression)
   #define LOG_WARNING(expression)     LOG("WARNING", expression)
   #define LOG_INFORMATION(expression) LOG("INFORMATION", expression)
   #define LOG_DEBUG(expression)       LOG("DEBUG", expression)
   #define LOG_VERBOSE(expression)     LOG("VERBOSE", expression)
   #define LOG_NONE(expression)        LOG("NONE", expression)


   #define LOG_FATAL_IF(condition, expression)        LOG_IF("FATAL", condition, expression)
   #define LOG_ERROR_IF(condition, expression)        LOG_IF("ERROR", condition, expression)
   #define LOG_WARNING_IF(condition, expression)      LOG_IF("WARNING", condition, expression)
   #define LOG_INFORMATION_IF(condition, expression)  LOG_IF("INFORMATION", condition, expression)
   #define LOG_DEBUG_IF(condition, expression)        LOG_IF("DEBUG", condition, expression)
   #define LOG_VERBOSE_IF(condition, expression)      LOG_IF("VERBOSE", condition, expression)
   #define LOG_NONE_IF(condition, expression)         LOG_IF("NONE", condition, expression)


   #define LOG_FATAL_NR(uNumber, expression)       LOG_NR(uNumber, "FATAL", expression)
   #define LOG_ERROR_NR(uNumber, expression)       LOG_NR(uNumber, "ERROR", expression)
   #define LOG_WARNING_NR(uNumber, expression)     LOG_NR(uNumber, "WARNING", expression)
   #define LOG_INFORMATION_NR(uNumber, expression) LOG_NR(uNumber, "INFORMATION", expression)
   #define LOG_DEBUG_NR( uNumber, expression)      LOG_NR(uNumber, "DEBUG", expression)
   #define LOG_VERBOSE_NR(uNumber, expression)     LOG_NR(uNumber, "VERBOSE", expression)
   #define LOG_NONE_NR(uNumber, expression)        LOG_NR(uNumber, "NONE", expression)


   #define LOG_FATAL_IF_NR( uNumber, condition, expression)        LOG_IF_NR(uNumber, "FATAL", condition, expression)
   #define LOG_ERROR_IF_NR( uNumber, condition, expression)        LOG_IF_NR(uNumber, "ERROR", condition, expression)
   #define LOG_WARNING_IF_NR( uNumber, condition, expression)      LOG_IF_NR(uNumber, "WARNING", condition, expression)
   #define LOG_INFORMATION_IF_NR( uNumber, condition, expression)  LOG_IF_NR(uNumber, "INFORMATION", condition, expression)
   #define LOG_DEBUG_IF_NR( uNumber, condition, expression)        LOG_IF_NR(uNumber, "DEBUG", condition, expression)
   #define LOG_VERBOSE_IF_NR( uNumber, condition, expression)      LOG_IF_NR(uNumber, "VERBOSE", condition, expression)
   #define LOG_NONE_IF_NR( uNumber, condition, expression)         LOG_IF_NR(uNumber, "NONE", condition, expression)

#endif
