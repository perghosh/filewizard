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
std::pair<sqlite3*, std::string> database::open_s(const std::string_view& stringFileName, int iFlags)
{
   if( iFlags == 0 ) iFlags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_FULLMUTEX;
   
   sqlite3* psqlite3 = nullptr;

   auto iOpenResult = ::sqlite3_open_v2(stringFileName.data(), &psqlite3, iFlags, nullptr );
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



/** ---------------------------------------------------------------------------
 * @brief Open select statement
 * @param stringSql SELECT query cursor will handle
 * @return true if ok otherwise false and error information
*/
std::pair<bool, std::string> cursor::open(const std::string_view& stringSql)
{                                                                              assert( m_pdatabase != nullptr );
   close();                                                                    // close statement if there is one active open statement
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
      auto pbszName = ::sqlite3_column_name( pstmt, i );
      auto uSize = value_size_g( uColumnType );
      if( uSize == 0 && pbszColumnType != nullptr ) uSize = value_size_g( pbszColumnType );

      //uint8_t* pBuffer = m_recordRow.
      m_recordRow.add( uColumnType, uSize, pbszName );
   }

   m_pstmt = pstmt;

   iResult = ::sqlite3_step( pstmt );
   if( iResult == SQLITE_ROW )
   {
      update();
      m_uState |= row;
   }
   else if( iResult < SQLITE_ROW )   
   {
      auto pbszError = ::sqlite3_errmsg(m_pdatabase->get_sqlite3());
      return { false, pbszError };
   }
   else
   {

   }

   return { true, "" };
}

/** -----------------------------------------------------------------------------------------------
 * @brief Update column buffers
 * @param uFrom start on column to update buffer
 * @param uTo end before this column updating buffers
*/
void cursor::update( unsigned uFrom, unsigned uTo )
{                                                                             assert( m_pstmt != nullptr );
   unsigned uType;
   uint8_t* pbBuffer;
   int iValueSize = 0;

   for( auto u = uFrom; u < uTo; u++ )
   {
      int iType = ::sqlite3_column_type( m_pstmt, u );
      gd::database::record::column* pcolumn = m_recordRow.get_column( u );
      if (iType != SQLITE_NULL)
      {
         pcolumn->set_null(false);
         uType = pcolumn->type();
         if( pcolumn->is_fixed() == true )
         {
            pbBuffer = m_recordRow.get_value_buffer( u );                      assert( pbBuffer != nullptr ); // get buffer to column
         }
         else
         {
            // ## Compare buffer size with value size, if buffer size is smaller than
            //    needed value size, then increase buffer size.
            //    Remember that first 8 bytes in buffer has length as unsigned and type ans unsigned.
            pbBuffer = m_recordRow.get_detached_buffer( pcolumn->value() ); 
            unsigned uBufferSize = buffers::buffer_size_s( pbBuffer );
            iValueSize = ::sqlite3_column_bytes(m_pstmt, u);
            if( iValueSize > ( int )uBufferSize ) 
            {
               pbBuffer = m_recordRow.resize( pcolumn->value(), iValueSize + 1 );// increase buffer size, add one for zero termination
            }
            pbBuffer = buffers::buffer_get_value_from_root_s( pbBuffer );
#ifdef _DEBUG
            auto buffer_length_d = *( unsigned* )buffers::buffer_get_root_from_value_s( pbBuffer );
            auto value_length_d = (unsigned)::sqlite3_column_bytes( m_pstmt, u );
            assert( buffer_length_d >= value_length_d );
#endif // _DEBUG

         }

         switch( uType )
         {
         case eColumnTypeCompleteInt64:
            *(int64_t*)pbBuffer = ::sqlite3_column_int64( m_pstmt, u );
            break;
         case eColumnTypeCompleteCDouble:
            *(double*)pbBuffer = ::sqlite3_column_double( m_pstmt, u );
            break;
         case eColumnTypeCompleteUtf8String: {
            const unsigned char* pbValue = ::sqlite3_column_text(m_pstmt, u);  // get text value in table

            if( iValueSize == 0 )                                              // if text value lenght hasn't been retrieved then this must be a fixed buffer
            {
               // ## Copy text from buffer in sqlite db
               int iSize = ::sqlite3_column_bytes( m_pstmt, u );
               memcpy( pbBuffer, pbValue, iSize );
               pbBuffer[iSize] = '\0';
               pcolumn->size( iSize );
            }
            else
            {
               int iSize = iValueSize;
               memcpy( pbBuffer, pbValue, iSize );
               pbBuffer[iSize] = '\0';
            }
         }
         break;

         default:
            assert( false );
         }
      }
      else
      {
         pcolumn->set_null(true);
      }
   }
}

/** -----------------------------------------------------------------------------------------------
 * @brief Go to next row
 * @return 
*/
std::pair<bool, std::string> cursor::next()
{
   int iResult = ::sqlite3_step(m_pstmt);
   if(iResult == SQLITE_ROW) 
   { 
      m_uState |= row; 
      update();
   }
   else if( iResult == SQLITE_DONE )
   {
      m_uState &= ~row; 
   }

   return std::pair<bool, std::string>();
}

/** -----------------------------------------------------------------------------------------------
 * @brief Collects values from active row and place them in returned vector with `variant`
 * @return std::vector<gd::variant> get all values from current row
*/
std::vector<gd::variant> cursor::get_variant() const
{
   std::vector<gd::variant> vectorValue;
   for( auto u = 0u, uTo = size(); u < uTo; u++ )
   { 
      vectorValue.push_back( get_variant( u ) );
   }
   return vectorValue;
}

/** -----------------------------------------------------------------------------------------------
 * @brief Get value in specified column
 * @param uColumnIndex Index to column where retured value i value
 * @return gd::variant value is placed and returned in variant
*/
gd::variant cursor::get_variant( unsigned uColumnIndex ) const
{                                                                             assert( uColumnIndex < size() );
   const gd::database::record::column* pcolumn = m_recordRow.get_column( uColumnIndex );

   if (pcolumn->is_null() == false)
   {
      unsigned uType = pcolumn->type();
      uint8_t* pbBuffer = m_recordRow.get_value_buffer( uColumnIndex );        // get buffer to column
      switch( uType )
      {
      case eColumnTypeCompleteInt32: return gd::variant( (int32_t)*(int64_t*)pbBuffer);
      case eColumnTypeCompleteInt64: return gd::variant( *(int64_t*)pbBuffer );
      case eColumnTypeCompleteCDouble: return gd::variant( *(double*)pbBuffer );
      case eColumnTypeCompleteString: return gd::variant( (const char*)pbBuffer, (size_t)pcolumn->size() );
      case eColumnTypeCompleteUtf8String: return gd::variant( (const char*)pbBuffer, (size_t)pcolumn->size() );
         
      default: assert(false);
      }
   }

   return gd::variant();
}

/** -----------------------------------------------------------------------------------------------
 * @brief Collects values from active row and place them in returned vector with `variant_view`
 * @return std::vector<gd::variant_view> get all values from current row
*/
std::vector<gd::variant_view> cursor::get_variant_view() const
{
   std::vector<gd::variant_view> vectorValue;
   for( auto u = 0u, uTo = size(); u < uTo; u++ )
   { 
      vectorValue.push_back( get_variant_view( u ) );
   }
   return vectorValue;
}

/** -----------------------------------------------------------------------------------------------
 * @brief Get value in specified column
 * @param uColumnIndex Index to column where retured value i value
 * @return gd::variant_view value is placed and returned in variant_view
*/
gd::variant_view cursor::get_variant_view( unsigned uColumnIndex ) const
{                                                                             assert( uColumnIndex < size() );
   const gd::database::record::column* pcolumn = m_recordRow.get_column( uColumnIndex );

   if (pcolumn->is_null() == false)
   {
      unsigned uType = pcolumn->type();
      uint8_t* pbBuffer = m_recordRow.get_value_buffer( uColumnIndex );        // get buffer to column
      switch( uType )
      {
      case eColumnTypeCompleteInt32: return gd::variant_view( (int32_t)*(int64_t*)pbBuffer);
      case eColumnTypeCompleteInt64: return gd::variant_view( *(int64_t*)pbBuffer );
      case eColumnTypeCompleteCDouble: return gd::variant_view( *(double*)pbBuffer );
      case eColumnTypeCompleteString: return gd::variant_view( (const char*)pbBuffer, (size_t)pcolumn->size() );
      case eColumnTypeCompleteUtf8String: return gd::variant_view( (const char*)pbBuffer, (size_t)pcolumn->size() );
         
      default: assert(false);
      }
   }

   return gd::variant_view();
}

/** -----------------------------------------------------------------------------------------------
 * @brief Get value in named column
 * @param stringName column name where retured value i value
 * @return gd::variant_view value is placed and returned in variant_view
*/
gd::variant_view cursor::get_variant_view( const std::string_view& stringName ) const
{
   int iColumnIndex = get_index( stringName );
   if( iColumnIndex != -1 ) return get_variant_view( iColumnIndex );

   return gd::variant_view();
}


/**
 * @brief return index to column based field name
 * @param stringName name column index is returned for
 * @return int index to column if found, otherwise -1
*/
int cursor::get_index( const std::string_view& stringName ) const
{
   return m_recordRow.get_column_index_for_name( stringName );
}


/*----------------------------------------------------------------------------- get_column_type */ /**
 * Returns the type for this column. It will first try to check the declared type which was used
 * when the table was created. The reason is that SQLite allows any type to be stored in any column.
 * If that fails, the column's real type is returned. If all else fails, utf-8 type is returned.
 * \param iColumn Column number, 0-based
 * \return The column's type
 */
unsigned cursor::get_column_type_s( const char* pbszColumnType )
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
   case 'S' : return eColumnTypeCompleteInt64;                                                     // SMALLINT
   case 'T' : return pbszColumnType[1] == 'E' ? eColumnTypeCompleteUtf8String : eColumnTypeCompleteInt64; // TEXT | TIME
   case 'U' : return eColumnTypeCompleteInt64;                                                     // UTCTIME | UTCDATETIME
   case 'V' : return pbszColumnType[3] == 'C' ? eColumnTypeCompleteUtf8String : eColumnTypeCompleteBinary; // VARCHAR | VARBIN
   }

   return eColumnTypeCompleteUtf8String;
}

unsigned cursor::get_column_ctype_s( const char* pbszColumnType )
{
   switch( *pbszColumnType )
   {
   case 'B' : {
      if( pbszColumnType[2] == 'T' ) return eColumnTypeCompleteBit;                                // BIT
      if( pbszColumnType[2] == 'G' ) return eColumnTypeCompleteInt64;                              // BIGINT
      return eColumnTypeCompleteBinary;                                                            // BINARY 
   }
   case 'D' : return pbszColumnType[1] == 'E' ? eColumnTypeCompleteCDouble : eColumnTypeCompleteDateTime;// DECIMAL | DATE
   case 'F' : return eColumnTypeCompleteCDouble;                                                   // FLOAT
   case 'G' : return eColumnTypeCompleteGuid;                                                      // GUID
   case 'I' : return eColumnTypeCompleteInt32;                                                     // INTEGER
   case 'N' : return pbszColumnType[1] == 'V' ? eColumnTypeCompleteUtf8String : eColumnTypeCompleteCDouble; // NVARCHAR | NUMERIC
   case 'R' : return eColumnTypeCompleteCDouble;                                                   // REAL
   case 'S' : return eColumnTypeCompleteInt16;                                                     // SMALLINT
   case 'T' : {
      if( pbszColumnType[2] == 'M' ) return eColumnTypeCompleteTime;                               // TIME
      if( pbszColumnType[2] == 'N' ) return eColumnTypeCompleteInt8;                               // TINYINT
      return eColumnTypeCompleteUtf8String;                                                        // TEXT 
   }
   case 'U' : return eColumnTypeCompleteInt64;                                                     // UTCTIME | UTCDATETIME
   case 'V' : return pbszColumnType[3] == 'C' ? eColumnTypeCompleteUtf8String : eColumnTypeCompleteBinary; // VARCHAR | VARBIN
   }

   return eColumnTypeCompleteUtf8String;
}


// TINYINT = 0 - 255





_GD_DATABASE_SQLITE_END

