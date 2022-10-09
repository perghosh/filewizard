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


/** ---------------------------------------------------------------------------
 * @brief Add primitive value, primitive value is same as value that has a fixed max buffer size
 * To avoid to allocate to much memory there is one logic to add primitive (fixed max size values)
 * and another to logic used to manage values that can be "any" size and grow dynamically if needed.
 * @param uColumnType type of column
 * @param uSize max size buffer needs to be to store value
 * @return offset position in buffer storing fixed values
*/
uint32_t buffers::primitive_add( unsigned uColumnType, unsigned uSize )
{                                                                              assert( uSize != 0 ); // if 0 then value is "derived" (flexible size)
   uint8_t* pbBuffer = nullptr;
   unsigned uSizeOld = m_uSize;

   unsigned uSizeAndExtra = uSize + sizeof( uSize ) + sizeof( uColumnType );   // needed size in buffer for primitive/fixed type

   // ## allocate storage if needed
   std::tie( m_pbBufferPrimitve, m_uMaxSize ) = reserve_g<uint8_t,m_uBufferGrowBy_s>( m_pbBufferPrimitve, uSizeAndExtra + m_uSize, m_uMaxSize );
   
   uint8_t* pbValue = m_pbBufferPrimitve + uSizeOld;
   *(unsigned*)pbValue = uSize;                                                // buffer size
   pbValue += sizeof(unsigned);
   *(unsigned*)pbValue = uColumnType;                                          // value type

   m_uSize += uSizeAndExtra;                                                   // add current buffer size for fixed buffers

   return uSizeOld + sizeof( uColumnType ) + sizeof( uSize );                  // return offset position to location in buffer where value is stored
}

/** ---------------------------------------------------------------------------
 * @brief Add "primitive"derived" value, value that are able to store any number of bytes
 * Derived buffers are buffers used for columns that do not have any max value in them.
 * Before reading value from result into column there is a check to see if buffer is large
 * enought, if not relocation is done and this is only done for column buffer.
 * Fixed buffers are store in `m_pbBufferPrimitve` as one long serie of data (same memory block)
 * @param uColumnType type of column
 * @param uSize max size buffer needs to be to store value
 * @return offset position in buffer storing fixed values
*/
uint16_t buffers::derived_add(unsigned uColumnType, unsigned uSize)
{
   if( uSize == 0 ) uSize = m_uStartSizeForDerivedBuffer_s;                    // Set default start size if no size is specified

   unsigned uSizeAndExtra = uSize + sizeof( uSize ) + sizeof( uColumnType );

   std::unique_ptr<uint8_t> pBuffer( new uint8_t[uSizeAndExtra] );
   uint8_t* puBuffer = pBuffer.get();

   *( unsigned* )puBuffer = uSize;                                             // Set max size that can be store in buffer
   puBuffer += sizeof( unsigned );
   *(unsigned*)puBuffer = uColumnType;                                         // Set column type

   m_vectorBuffer.push_back( std::move( pBuffer ) );

   return (uint16_t)m_vectorBuffer.size() - 1;
}

/** ---------------------------------------------------------------------------
 * @brief Reize buffer holding fixed values, all fixed values are stored in one single buffer large enough to store all fixed values
 * @param uType number for value type
 * @param uSize max size for value 
 * @return previous size for buffer
*/
unsigned buffers::primitive_resize( unsigned uType, unsigned uSize )
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

/** ---------------------------------------------------------------------------
 * @brief Resize derived buffers, derived buffers are used for values that can have any buffer size
 * @param uIndex index to buffer that needs to be resized
 * @param uSize new size for buffer (has to be larger than previous buffer)
 * @return pointer to new buffer
*/
uint8_t* buffers::derived_resize( unsigned uIndex, unsigned uSize )
{                                                                              assert( uIndex < m_vectorBuffer.size() );
   const uint8_t* puBuffer = m_vectorBuffer[uIndex].get();                     assert( *( unsigned* )puBuffer < uSize );

   uSize = uSize + (m_uStartSizeForDerivedBuffer_s - (uSize % m_uStartSizeForDerivedBuffer_s));

   unsigned uOldSize = *( unsigned* )puBuffer;
   uint8_t* puBiggerBuffer = new uint8_t[uSize + sizeof(unsigned) + sizeof(unsigned)];// create new buffer with new size
   memcpy( puBiggerBuffer, puBuffer, uOldSize + sizeof( unsigned ) * 2 );      // copy old buffer information into new buffer
   *( unsigned* )puBiggerBuffer = uSize;
   m_vectorBuffer[uIndex].reset( puBiggerBuffer );                             // replace buffer
   return puBiggerBuffer;
}


record& record::add( unsigned uColumnType, unsigned uSize, const std::string_view& stringName, const std::string_view& stringAlias )
{
   column columnAdd;
   unsigned uValueOffset{0};

   columnAdd.type( uColumnType );
   columnAdd.size( uSize );
   if( stringName.empty() == false ) columnAdd.name( m_namesColumn.add( stringName ) );
   if( stringAlias.empty() == false ) columnAdd.alias( m_namesColumn.add( stringAlias ) );

   if( uSize > 0 )                                                             // do we have a fixed value? (size is specified)
   {
      uValueOffset = m_buffersValue.primitive_add( uColumnType, uSize );       // add buffer for fixed size type
      columnAdd.value( uValueOffset );
      columnAdd.set_fixed( true );
   }
   else
   {
      uValueOffset = m_buffersValue.derived_add( uColumnType, uSize );         // add buffer for fixed size type
      columnAdd.value( uValueOffset );
   }

   m_vectorColumn.push_back( columnAdd );
   
   return *this;
}

/** ---------------------------------------------------------------------------
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


/**
 * @brief empty columns
*/
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
