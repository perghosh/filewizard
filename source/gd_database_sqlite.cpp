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

std::pair<bool, std::string> database::execute_s(sqlite3* psqlite, const std::string_view& stringQuery)
{
   char* pbszError = nullptr;
   auto iExecuteResult = ::sqlite3_exec(psqlite, stringQuery.data(), nullptr, nullptr, &pbszError );
   if( iExecuteResult != SQLITE_OK )
   {
      std::string stringError = pbszError;
      ::sqlite3_free( pbszError );
      return { false, std::move(stringError) };
   }

   return { true, std::string() };
}

void database::close_s( sqlite3* psqlite )
{
   if( psqlite != nullptr )
   {
      ::sqlite3_close(psqlite);
   }
}



/**
 * @brief 
 * @param stringSql 
 * @return 
*/
std::pair<bool, std::string> recordset::open(const std::string_view& stringSql)
{                                                                             assert( m_pdatabase != nullptr );
   sqlite3_stmt* pstmt;
   const char* pbszTail;
   auto iResult = ::sqlite3_prepare_v2(m_pdatabase->get_sqlite3(), stringSql.data(), (int)stringSql.length(), &pstmt, &pbszTail);
   if( iResult != SQLITE_OK )
   {
      auto pbszError = ::sqlite3_errmsg(m_pdatabase->get_sqlite3());
      return { false, pbszError };
   }

   // ## Collect information about columns in result for buffers storing result data
   auto iColumnCount = ::sqlite3_column_count( pstmt );
   for( auto i = 0; i < iColumnCount; i++ )
   {
      unsigned uColumnType = eColumnTypeCompleteUnknown;
      const char* pbszColumnType = ::sqlite3_column_decltype(pstmt, i);
      if( pbszColumnType != nullptr )                                         // if declared type is valid then try to get type from type name
      {
         uColumnType = get_column_type_s( pbszColumnType );
      }
   }

   return { true, "" };
}


/*----------------------------------------------------------------------------- get_column_type */ /**
 * Returns the type for this column. It will first try to check the declared type which was used
 * when the table was created. The reason is that SQLite allows any type to be stored in any column.
 * If that fails, the column's real type is returned. If all else fails, utf-8 type is returned.
 * \param iColumn Column number, 0-based
 * \return The column's type
 */
unsigned recordset::get_column_type_s( const char* pbszColumnType )
{
   switch( *pbszColumnType )
   {
   case 'B' : return pbszColumnType[2] == 'N' ? eColumnTypeCompleteBinary : eColumnTypeCompleteInt64;// BINARY|BIT|BIGINT
   case 'D' : return pbszColumnType[1] == 'E' ? eColumnTypeCompleteCDouble : eColumnTypeCompleteInt64;// DECIMAL | DATE
   case 'F' : return eColumnTypeCompleteCDouble;                                                   // FLOAT
   case 'G' : return eColumnTypeCompleteBinary;                                                    // GUID
   case 'I' : return eColumnTypeCompleteInt64;                                                     // INTEGER
   case 'N' : return pbszColumnType[1] == 'V' ? eColumnTypeCompleteUtf8String : eColumnTypeCompleteCDouble; // NVARCHAR | NUMERIC
   case 'R' : return eColumnTypeCompleteCDouble;                                                   // REAL
   case 'T' : return pbszColumnType[1] == 'E' ? eColumnTypeCompleteUtf8String : eColumnTypeCompleteInt64; // TEXT | TIME
   case 'U' : return eColumnTypeCompleteInt64;                                                     // UTCTIME | UTCDATETIME
   case 'V' : return pbszColumnType[3] == 'C' ? eColumnTypeCompleteUtf8String : eColumnTypeCompleteBinary; // VARCHAR | VARBIN
   }

   return eColumnTypeCompleteUtf8String;
}





_GD_DATABASE_SQLITE_END

