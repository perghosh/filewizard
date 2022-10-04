#include "gd_database_record.h"

_GD_DATABASE_BEGIN

template<int iGrowBy>
std::pair<uint8_t*, unsigned> reserve_g( uint8_t* pbBuffer, unsigned uSize, unsigned uMaxSize )
{
   if(uSize > uMaxSize)
   {
      unsigned uNewSize = uSize + (iGrowBy - uSize % iGrowBy);                 // new size + exgtra for avoiding to many allocations

      // ## create new buffer, copy from old buffer and update members
      uint8_t* pbNewBuffer = new uint8_t[uNewSize];
      if( uMaxSize > 0 )                                                       // do we need to copy existing data
      {                                                                        assert( pbBuffer != nullptr );
         memcpy(pbNewBuffer, pbBuffer, uMaxSize);
         delete [] pbBuffer;
      }
      return { pbNewBuffer, uNewSize };
   }

   return { pbBuffer, uMaxSize };
}

template<typename BYTE, int iGrowBy>
std::pair<BYTE*, unsigned> reserve_g( BYTE* pbBuffer, unsigned uSize, unsigned uMaxSize )
{
   auto _result = reserve_g<iGrowBy>( (uint8_t*)pbBuffer, uSize, uMaxSize );
   return { (BYTE*)_result.first, _result.second };
}

/** ---------------------------------------------------------------------------
 * @brief Add name to internal buffer that stores name
 * `names` object stores names (strings) that do not change, each name is store
 * in one single buffer.
 * @param stringName name added to buffer
 * @return offset position in buffer for stored name
*/
uint16_t names::add(std::string_view stringName)
{                                                                              assert( stringName.length() < 1000 ); // realistic
   auto uNameLLengthAndExtra = stringName.length() + 1 + sizeof(uint16_t);     // add name lenght and extra bytes to store lenght and zero ending
   reserve( m_uSize + (unsigned)uNameLLengthAndExtra );

   unsigned uNameOffset = last_position();                                     // get end position where new name is added
   char* pbName = m_pbBufferNames + uNameOffset;                               // get pointer
   *(uint16_t*)pbName = (uint16_t)stringName.length();                         // set name lenght before name (lenght is stored as uint16_t)
   pbName += sizeof(uint16_t);                                                 // move past name lenght in buffer
   uNameOffset += sizeof( uint16_t );                                          // add two bytes sizeof(uint16_t) storing name length, after this add name characters
   memcpy( pbName, stringName.data(), uNameLLengthAndExtra - sizeof(uint16_t) );// copy name and zero termination
   m_uSize += decltype(m_uSize)(uNameLLengthAndExtra);                         // set to end of name (where next name starts)

   return uNameOffset;                                                         // return offset position to where name was inserted
}

/** ---------------------------------------------------------------------------
 * @brief maker sure that internal buffer is big enough to store size
 * @param uSize needed size in internal buffer
*/
void names::reserve(unsigned uSize)
{
   // `reserve_g` is used to resize buffer if needed
   std::tie( m_pbBufferNames, m_uMaxSize ) = reserve_g<char,m_uBufferGrowBy_s>( m_pbBufferNames, uSize, m_uMaxSize );
}

// ============================================================================
// ==================================================================== buffers
// ============================================================================


uint32_t buffers::add_primitive( unsigned uColumnType, unsigned uSize )
{
   uint8_t* pbBuffer = nullptr;
   unsigned uSizeOld = m_uSize;

   unsigned uSizeAndExtra = uSize + sizeof( uColumnType ) + sizeof( uSize );   // needed size in buffer for primitive/fixed type

   // ## allocate storage if needed
   std::tie( m_pbBufferPrimitve, m_uMaxSize ) = reserve_g<uint8_t,m_uBufferGrowBy_s>( m_pbBufferPrimitve, uSizeAndExtra + m_uSize, m_uMaxSize );
   
   uint8_t* pbValue = m_pbBufferPrimitve + uSizeOld;
   *(unsigned*)pbValue = uColumnType;
   pbValue += sizeof(unsigned);
   *(unsigned*)pbValue = uSize;                                                // sice for buffer

   m_uSize += uSizeAndExtra;                                                   // add current buffer size for fixed buffers

   return uSizeOld + sizeof( uColumnType ) + sizeof( uSize );                  // return offset position to location in buffer where value is stored
}

unsigned buffers::reserve( unsigned uType, unsigned uSize )
{
   uint8_t* pbBuffer = nullptr;
   unsigned uSizeOld = m_uSize;

   if(uSize > m_uMaxSize)
   {
      unsigned uSizeAndExtra = uSize + sizeof( unsigned ) + sizeof( unsigned );
      std::tie( m_pbBufferPrimitve, m_uMaxSize ) = reserve_g<uint8_t,m_uBufferGrowBy_s>( m_pbBufferPrimitve, uSizeAndExtra, m_uMaxSize );
      m_uSize = uSizeAndExtra;
   }

   return uSizeOld;
}



/*

   size_t get_num_columns() const { return sqlite3_column_count( m_statement ); }
   int step() const { return sqlite3_step( m_statement ); }
   const char* column_name( size_t uIndex ) const { return sqlite3_column_name( m_statement, uIndex ); }
   int column_type( size_t uIndex ) const { return sqlite3_column_type( m_statement, uIndex ); }
   const char* column_decltype( size_t uIndex ) const { return sqlite3_column_decltype( m_statement, uIndex ); }
   __int64 column_int64( size_t uIndex ) const { return sqlite3_column_int64( m_statement, uIndex ); }
   double column_double( size_t uIndex ) const { return sqlite3_column_double( m_statement, uIndex ); }
   const void* column_blob( size_t uIndex ) const { return sqlite3_column_blob( m_statement, uIndex ); }
   size_t column_bytes( size_t uIndex ) const { return sqlite3_column_bytes( m_statement, uIndex ); }
   const char* column_text( size_t uIndex ) const { return (const char*)sqlite3_column_text( m_statement, uIndex ); }


*/



record& record::add( unsigned uColumnType, unsigned uSize, const std::string_view& stringName, const std::string_view& stringAlias )
{
   column columnAdd;
   unsigned uValueOffset{0};

   columnAdd.type( uColumnType );
   columnAdd.size( uSize );
   if( stringName.empty() == false ) columnAdd.name( m_namesColumn.add( stringName ) );
   if( stringAlias.empty() == false ) columnAdd.alias( m_namesColumn.add( stringAlias ) );

   if( uSize > 0 )
   {
      uValueOffset = m_buffersValue.add_primitive( uColumnType, uSize );       // add buffer for fixed size type
      columnAdd.value( uValueOffset );
   }

   m_vectorColumn.push_back( columnAdd );
   
   return *this;
}

/**
 * @brief return buffer for value buffer
 * @param uIndex column index buffer is returned for
 * @return 
*/
uint8_t* record::get_value_buffer( unsigned uIndex ) const
{
   const record::column* pcolumn = get_column( uIndex );
   if( pcolumn->size() != 0 )
   {
      return m_buffersValue.primitive_data_offset( pcolumn->value() );
   }

   return nullptr;
}

void record::clear()
{
   m_vectorColumn.clear();
}


/** ---------------------------------------------------------------------------
 * @brief return needed size to store value type, if 0 then value size is variable
 * @param eType type of value size is returned for
 * @return number of bytes needed to store value
*/
unsigned record::size_s( enumColumnTypeNumber eType )
{                                                                              assert( (unsigned)eType < 0xff );
   unsigned uSize = value_size_g( eType );

   return uSize;
}


_GD_DATABASE_END
