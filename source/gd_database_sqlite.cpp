#include <vector>


#include "gd_database_sqlite.h"


_GD_DATABASE_SQLITE_BEGIN

/** -----------------------------------------------------------------------------------------------
 * @brief 
 * @param stringFileName 
 * @return 
*/
std::pair<bool,std::string> database::open(const std::string_view& stringFileName)
{
   if( m_psqlite3 != nullptr ) close();

   auto [psqlite3, stringError] = open_s(stringFileName);
   if( psqlite3 != nullptr )
   {
      m_psqlite3 = psqlite3;
      return { true, std::string() };
   }
   
   return { false, stringError };
}





/** -----------------------------------------------------------------------------------------------
 * @brief open sql database for file name, if no database is found then create new database
 * @param stringFileName file name representing sqlite database
 * @return pointer to sqlite database or if error then add error information
*/
std::pair<sqlite3*, std::string> database::open_s(const std::string_view& stringFileName)
{
   sqlite3* psqlite3 = nullptr;
   auto iOpenResult = ::sqlite3_open(stringFileName.data(), &psqlite3);
   if( iOpenResult == SQLITE_OK ) return { psqlite3, std::string() };

   auto pbszError = ::sqlite3_errmsg(psqlite3);
   if( pbszError == nullptr ) pbszError = "unknown error";

   return { psqlite3, pbszError };
}

void database::close_s( sqlite3* psqlite )
{
   if( psqlite != nullptr )
   {
      ::sqlite3_close(psqlite);
   }
}



_GD_DATABASE_SQLITE_END

