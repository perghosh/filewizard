#pragma once

#include <cassert>
#include <string>
#include <string_view>
#include <vector>
#include <type_traits>



#ifndef _GD_FILE_ROTATE_BEGIN
#define _GD_FILE_ROTATE_BEGIN namespace gd { namespace file { namespace rotate {
#define _GD_FILE_ROTATE_END } } }
_GD_FILE_ROTATE_BEGIN
#else
_GD_FILE_ROTATE_BEGIN
#endif

#pragma warning( disable : 26495 )


/*
TODO:
- Read files from configuration file
- check date

*/

/**
 * \brief 
 *
 *
 *
 \code
 \endcode
 */
class backup_history 
{
public:
   /*-----------------------------------------*/ /**
    * \brief information about backup files
    *
    * Read information about backup files from configuration and store it in 
    * file objects.
    */
   struct file
   {
      file() {}
      file( time_t iBackupTime, const std::string_view& stringFileName ): m_iBackupTime(iBackupTime), m_stringFileName(stringFileName) {}
      // attributes
      public:
         time_t m_iBackupTime;         ///< time when file was backup was done (64 bit integer)
         std::string m_stringFileName; ///< backup file name
   };

// ## construction -------------------------------------------------------------
public:
   backup_history() {}
   explicit backup_history(unsigned uMaxHistoryCount) : m_uMaxHistoryCount(uMaxHistoryCount) {}
   backup_history( const std::string_view& stringConfigureFileName ): m_stringConfigureFileName(stringConfigureFileName) {}
   // copy
   backup_history( const backup_history& o ) { common_construct( o ); }
   backup_history( backup_history&& o ) noexcept { common_construct( o ); }
   // assign
   backup_history& operator=( const backup_history& o ) { common_construct( o ); return *this; }
   backup_history& operator=( backup_history&& o ) noexcept { common_construct( o ); return *this; }
   
	~backup_history() {}
private:
   // common copy
   void common_construct(const backup_history& o);
   void common_construct( backup_history&& o ) noexcept {}

// ## operator -----------------------------------------------------------------
public:
   

// ## methods ------------------------------------------------------------------
public:
/** \name GET/SET
*///@{
   
//@}

/** \name OPERATION
*///@{
   
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
   unsigned m_uMaxHistoryCount;
   std::string m_stringFolder;
   std::string m_stringConfigureFileName;
   
// ## free functions ------------------------------------------------------------
public:
   // ## file operations.  read and write values from files. we are working with dates and file names

   // reads date value and file name from each line
   static std::vector< std::pair< std::string, std::string> > file_read_date_name_s(const std::string_view& stringFileName);

   // write text line or lines from vector to file
   static std::pair<bool, std::string> file_write_text_s(const std::string_view& stringFileName, const std::vector< std::string >& vectorText, bool bAppend);
   // write date and name found in vector to file
   static std::pair<bool, std::string> file_write_date_name_s(const std::string_view& stringFileName, const std::vector< std::pair< std::string, std::string> >& vectorDateName, bool bAppend);

   // backup file to temporary file
   static std::string file_backup_as_temporary(const std::string_view& stringFileName);
   static  std::pair<bool, std::string> file_backup_log(const std::string_view& stringFileName, const std::string_view& stringBackupName, unsigned uIndex) { return file_backup_log(stringFileName, stringBackupName, uIndex, 0); }
   static std::pair<bool, std::string> file_backup_log( const std::string_view& stringFileName, const std::string_view& stringBackupName, unsigned uIndex, unsigned uOptions );
   static bool file_delete_backup( const std::string_view& stringFileName );

   // ## history statistics, find information in files containing history information

   // calculate largest number among files (tries to find largest number in names)
   static int find_max_index( const std::vector< std::pair< std::string, std::string> >& vectorDateName );

   


};


_GD_FILE_ROTATE_END