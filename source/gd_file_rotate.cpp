#include <fstream>
#include <filesystem>


#include "gd_utf8.hpp"

#include "gd_file_rotate.h"

#pragma warning(disable: 4996)


_GD_FILE_ROTATE_BEGIN


void backup_history::common_construct(const backup_history& o)
{
   m_uMaxHistoryCount = o.m_uMaxHistoryCount;
   m_stringConfigureFileName = o.m_stringConfigureFileName;
}

/*----------------------------------------------------------------------------- file_read_date_name_s */ /**
 * Read list of files from file history.
 * Format for file history read is
 * 
 * date format read, same as javascript `toISOString`
 * sample `2011-10-05T14:48:00.000Z` [link](https://tc39.es/ecma262/multipage/numbers-and-dates.html#sec-date-time-string-format)
- `YYYY` : 4-digit year.
- `MM` : 2-digit month (where January is 01 and December is 12)
- `DD` : 2-digit date (0 to 31)
- `-` : Date delimiters.
- `T` : Indicates the start of time.
- `HH` : 24-digit hour (0 to 23)
- `mm` : Minutes (0 to 59)
- `ss` : Seconds (0 to 59) 
- `Z`: If Z is present, date will be set to UTC. If Z is not present, it’ll be Local Time. (This only applies if time is provided.)

*DATETIME FILENAME*
 ```
2011-10-05T14:48:00Z C:\backup\filename_01.log
2011-10-06T18:25:00Z C:\backup\filename_02.log
2011-10-06T20:11:00Z C:\backup\filename_03.log
2011-10-07T04:04:00Z C:\backup\filename_04.log
 ```
 * \param stringFileName file name containing history information
 * \return std::vector< std::pair< std::string, std::string> > vector having dates and filenames, like history
 */
std::vector<std::pair< std::string, std::string>> backup_history::file_read_date_name_s(const std::string_view& stringFileName)
{
   // vector where time value as string and file name is places from each read line
   std::vector<std::pair<std::string, std::string>> vectorDateAndName;

   std::fstream fstreamDateName(stringFileName.data(), std::ios::in);            // open history file

   if( (bool)fstreamDateName.is_open() == true )
   {
      std::string stringTime;    // read time value
      std::string stringFile;    // read file name
      std::string stringLine;    // read line from file
      while( std::getline(fstreamDateName, stringLine) )
      {
         if( stringLine.empty() == false && std::isdigit(stringLine[0]) != 0 )
         {
            // ## try to read time and file name from line
            const char* pbsz;
            const char* pbszLine = stringLine.c_str();                           // ->2011-10-05T14:48:00.000Z C:\backup\filename_01.log
            if( *pbszLine > '9' || *pbszLine < '0' ) continue;                   // first character in line needs to be a number
            pbsz = gd::utf8::move::next_space(pbszLine);                         // -> C:\backup\filename_01.log
            if( pbsz == pbszLine ) continue;                                     

            stringTime.assign(pbszLine, pbsz);
            pbszLine = pbsz;
            pbsz = gd::utf8::move::next_non_space(pbszLine);                     // ->C:\backup\filename_01.log
            if( pbsz == pbszLine ) continue;
            
            pbszLine = pbsz;
            stringFile.assign(pbszLine, stringLine.c_str() + stringLine.length());

            vectorDateAndName.push_back( {stringTime, stringFile} );             // add time and file name to vector (pair value)
         }
      }
   }

   return vectorDateAndName;

}


/*----------------------------------------------------------------------------- file_write_text_s */ /**
 * Write text to file, text written is stored in vector and all strings in vector is
 * written to file where newline is added to each string written to file
 * \param stringFileName name of file written to
 * \param vectorText string that will be written to file
 * \param bAppend if true text is appended
 * \return std::pair<bool, std::string>
 */
std::pair<bool, std::string> backup_history::file_write_text_s(const std::string_view& stringFileName, const std::vector<std::string>& vectorText, bool bAppend)
{
   enum { eBufferErrorSize = 100 };
   unsigned uOpenMode = std::ios::out;                                           // how to open file (in what mode, write and maybe append)
   if( bAppend == true ) uOpenMode |= std::ios::app;

   std::fstream fstreamText(stringFileName.data(), uOpenMode);


   if( (bool)fstreamText.is_open() == true )
   {
      for( auto it : vectorText )
      {
         fstreamText << it;
         fstreamText << "\n";
      }

      fstreamText.close();
   }
   else
   {
      std::string stringError;
      stringError.reserve( eBufferErrorSize );
      strerror_s(&stringError[0], eBufferErrorSize, errno);
      return {false, stringError };
   }


   return {true, ""};
}

/*----------------------------------------------------------------------------- file_write_date_name_s */ /**
 * write date and name information from vector into file
 * \param stringFileName name of file to write to
 * \param vectorDateName date and name values to write to file
 * \param bAppend if true then append to file
 * \return std::pair<bool, std::string> true if ok, false and error information if error
 */
std::pair<bool, std::string> backup_history::file_write_date_name_s(const std::string_view& stringFileName, const std::vector<std::pair<std::string, std::string>>& vectorDateName, bool bAppend )
{
   enum { eBufferErrorSize = 100 };
   unsigned uOpenMode = std::ios::out;                                           // how to open file (in what mode, write and maybe append)
   if( bAppend == true ) uOpenMode |= std::ios::app;

   std::fstream fstreamDateName(stringFileName.data(), uOpenMode);

   if( (bool)fstreamDateName.is_open() == true )
   {
      for( auto it : vectorDateName )
      {
         fstreamDateName << std::get<0>(it);
         fstreamDateName << "  ";
         fstreamDateName << std::get<1>(it);
         fstreamDateName << "\n";
      }

      fstreamDateName.close();
   }
   else
   {
      std::string stringError;
      stringError.reserve(eBufferErrorSize);
      strerror_s(&stringError[0], eBufferErrorSize, errno);
      return { false, stringError };
   }

   return { true, "" };
}

/*----------------------------------------------------------------------------- file_backup_as_temporary */ /**
 * clone file to temporary file as backup.
 * remember that this file is only temporary and should not been seen as permanent backup.
 * just functionality if you need to modify a file and need to revert if file modification
 * wasn't successful.
 * 
 * \param stringFileName file to backup
 * \return std::string name of backup file, keep for later if restore is needed or to delete if everything went well
 */
std::string backup_history::file_backup_as_temporary(const std::string_view& stringFileName)
{
   const char* pbszTemporaryFileName = std::tmpnam(nullptr);                     // generate temporary file name
   if( pbszTemporaryFileName != nullptr )
   {
      auto bOk = std::filesystem::copy_file(stringFileName, pbszTemporaryFileName);// create clone as generated temp file
      return bOk ? pbszTemporaryFileName : "";                                   //
   }
                                                                                 assert( false );
   return "";
}

/*----------------------------------------------------------------------------- file_delete_backup */ /**
 * Remove backup file (or any other file)
 * \param stringFileName name of file to be removed
 * \return bool true if file was removed, false if not
 */
bool backup_history::file_delete_backup(const std::string_view& stringFileName)
{
   int iResult = std::remove( stringFileName.data() );
   if( iResult == 0 ) return true;
   return false;
}


/*----------------------------------------------------------------------------- find_max_index */ /**
 * find max index for file names in vector, check the second string value in pair for index in file name
 * \param vectorPairText vector of pair strings where name is checked for index
 * \return int integer value with largest index value read from file name
 */
int backup_history::find_max_index(const std::vector< std::pair< std::string, std::string> >& vectorPairText)
{
   auto _get_index = [](const std::string& stringText) -> int {
      auto uPosition = stringText.rfind('.');
      if( uPosition != std::string::npos )
      {
         uPosition--;                                                            // move to character before "."
         auto pbszBegin = stringText.c_str();
         auto pbszDigit = pbszBegin + uPosition + 1;                             // pointer to digit
         auto pbszDigitEnd = pbszDigit;
         pbszDigit--;
         while( pbszDigit != pbszBegin && std::isdigit(*pbszDigit) != 0 ) { pbszDigit--; }
         pbszDigit++;

         std::string_view stringNumber(pbszBegin + 1, pbszDigitEnd);
         auto iIndex = std::atoi(pbszDigit);
         return iIndex;
      }
      return -1;
   };


   int iMax = 0;
   for( auto it : vectorPairText )
   {
      const auto& stringFile = it.second;
      int iIndex = _get_index( stringFile );
      if( iIndex > iMax ) iMax = iIndex;öä---
   }
   return iMax;
}

/*----------------------------------------------------------------------------- file_backup_log */ /**
 * Backup log file, generates a new name for log and copies the main log file to the new generated name
 * \param stringFileName log file that is backed up
 * \param stringBackupName name for backup'ed log file, this name is modified with index and time before copy is done
 * \param uIndex new index for backup log
 * \return std::pair<bool, std::string> true and name if ok, false and error information if error
 */
std::pair<bool, std::string> backup_history::file_backup_log(const std::string_view& stringFileName, const std::string_view& stringBackupName, unsigned uIndex, unsigned uOptions )
{
   // ## check if file exists
   bool bExist = std::filesystem::exists(stringFileName);
   if( bExist == false )
   {
      std::string stringError{ "file not found: log file with name \"" };
      stringError += stringFileName;
      stringError += "\" is missing";
      return { false, string };
   }

   // ## build new file name 

   std::string stringDirectory;
   const size_t uSlashPosition = stringFileName.rfind('\\');
   if( std::string::npos != uSlashPosition ) { stringDirectory = stringFileName.substr(0, uSlashPosition); }

   std::string stringBackupLog = 
   auto bOk = std::filesystem::copy_file(stringFileName, pbszTemporaryFileName);// create clone as generated temp file



}

_GD_FILE_ROTATE_END