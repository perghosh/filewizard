#pragma once

#include <cassert>
#include <cstring>
#include <string>
#include <string_view>
#include <vector>
#include <memory>


#include "gd_database_types.h"


#ifndef _GD_DATABASE_BEGIN
#define _GD_DATABASE_BEGIN namespace gd { namespace database {
#define _GD_DATABASE_END } }
_GD_DATABASE_BEGIN
#else
_GD_DATABASE_BEGIN
#endif


/**
 * \brief keep constant strings in one single buffer
 *
 * `names` is used to stor constant text values used to work with results from
 * database. text values that do not changed during work.
 * *buffer format, how name is stored in memory `0` = zero terminator*
 * 'SSname1_value0SSname2_value0SSname3_value0SSname4_value0'
 * - SS = lenght is stored in two bytes as `unsighed short` 
 * - nameX-value = name text as utf8
 * - 0 = zero terminator
 * 
 */
struct names
{
   names(): m_uSize{0}, m_uMaxSize{0}, m_pbBufferNames{nullptr} {}
   names( names&& o ) {
      m_uSize = o.m_uSize;
      m_uMaxSize = o.m_uMaxSize;
      m_pbBufferNames = o.m_pbBufferNames;
      o.m_pbBufferNames = nullptr;
   }
   names& operator=( names&& o ) noexcept { 
      m_uSize = o.m_uSize;
      m_uMaxSize = o.m_uMaxSize;
      m_pbBufferNames = o.m_pbBufferNames;
      o.m_pbBufferNames = nullptr;
      return *this; 
   }
   ~names() { delete [] m_pbBufferNames; }

   operator const char* () const { return m_pbBufferNames; }

   uint16_t add( std::string_view stringName );
   std::string_view get( unsigned uOffset ) const { return get_name_s( m_pbBufferNames, uOffset ); }
   uint16_t last_position() const noexcept { return m_uSize; }
   void reserve( unsigned uSize );
   void clear() {
      delete [] m_pbBufferNames; 
      m_uSize = 0; m_uMaxSize = 0; m_pbBufferNames = nullptr;
   }

   
   uint16_t m_uSize;       ///< value size in bytes
   unsigned m_uMaxSize;    ///< total buffer size in bytes
   char* m_pbBufferNames;  ///< buffer where names are stored

   static const unsigned m_uBufferGrowBy_s = 256;

   /// get name from offset in buffer 
   static std::string_view get_name_s( const char* pbBuffer, unsigned uOffset ) { assert( uOffset < 0x90000 ); // realistic ?
      return std::string_view( pbBuffer + uOffset, *(uint16_t*)(pbBuffer + (uOffset - sizeof(uint16_t))) ); 
   }
};

/**
 * \brief buffer used to store values from result in database
 *
 */
struct buffers
{
   buffers(): m_uSize{0}, m_uMaxSize{0}, m_pbBufferPrimitve{nullptr} {}
   buffers( buffers&& o ) {
      m_uSize = o.m_uSize;
      m_uMaxSize = o.m_uMaxSize;
      m_vectorBuffer = std::move( o.m_vectorBuffer );
      m_pbBufferPrimitve = o.m_pbBufferPrimitve;
      o.m_pbBufferPrimitve = nullptr;
   }
   buffers& operator=( buffers&& o ) noexcept { 
      m_uSize = o.m_uSize;
      m_uMaxSize = o.m_uMaxSize;
      m_vectorBuffer = std::move( o.m_vectorBuffer );
      m_pbBufferPrimitve = o.m_pbBufferPrimitve;
      o.m_pbBufferPrimitve = nullptr;
      return *this; 
   }

   ~buffers() { 
      delete [] m_pbBufferPrimitve; 
   }

   // ## Methods that manages primitives (fixed values, max size buffer)
   /// Add fixed buffer 
   uint32_t primitive_add( unsigned uType, unsigned uSize );
   
   uint8_t* primitive_data() const { return m_pbBufferPrimitve; }
   uint8_t* primitive_data_end() const { return m_pbBufferPrimitve + m_uSize; }
   uint8_t* primitive_data_offset( unsigned uOffset ) const { return m_pbBufferPrimitve + uOffset; }

   unsigned primitive_resize( unsigned uType, unsigned uSize );

   // ## Derived buffers is used to store buffers that varies in size. 
   /// return pointer to derived data buffer at index
   uint8_t* derived_data( unsigned uIndex ) const { return m_vectorBuffer[uIndex].get(); }
   uint8_t* derived_data_value( unsigned uIndex ) const { return m_vectorBuffer[uIndex].get() + m_uDerivedOffsetStart_s; }
   uint8_t* derived_resize( unsigned uIndex, unsigned uSize );

   // 
   uint16_t derived_add( unsigned uColumnType, unsigned uSize );

   void clear() {
      delete [] m_pbBufferPrimitve; 
      m_uSize = 0; m_uMaxSize = 0; m_pbBufferPrimitve = nullptr;
      m_vectorBuffer.clear();
   }

   unsigned m_uSize;       ///< value size in bytes
   unsigned m_uMaxSize;    ///< total buffer size in bytes
   uint8_t* m_pbBufferPrimitve; ///< pointer to buffer where fixed values are stored
   std::vector< std::unique_ptr<uint8_t> > m_vectorBuffer;  ///< used to store values with flexible sizes, first dword in pointer holds current size
   static const unsigned m_uBufferGrowBy_s = 128;
   static const unsigned m_uStartSizeForDerivedBuffer_s = 128; ///< min size for buffer storing values withoud max column size
   static const unsigned m_uDerivedOffsetStart_s = sizeof( unsigned ) * 2; ///< derived buffer distance to value (derived = value with any size)


   // ## Buffer methods. All buffers have same format four bytes storing size, four bytes storing type and then value
   //    Buffer layout = SSSSTTTT......... where SSSS = four bytes (unsigned) for length, TTTT = four bytes (unsigned) for type

   /// Get value position from root position in buffer or move pointer
   static uint8_t* buffer_get_value_from_root_s( uint8_t* puBuffer ) { return puBuffer + sizeof( unsigned ) + sizeof( unsigned ); }
   static uint8_t* buffer_get_root_from_value_s( uint8_t* puBuffer ) { return puBuffer - (sizeof( unsigned ) + sizeof( unsigned )); }

   // ## size in buffer (buffer store size at first position)
   [[nodiscard]] static unsigned buffer_size_s( uint8_t* puBuffer ) { return *( unsigned* )puBuffer; }

};



/**
 * \brief Store one row of database information
 *
 *
 *
 \code
 \endcode
 */
class record
{
public:
   /**
    * \brief Information about each column in record
    *
    * `column` has information needed to work with data for each column in record.
    * Each column has a type, size, name and alias if needed.
    * 
    * Underlying logic is designed for speed and minimize memory use
    *
    */
   struct column
   {
      enum { eStateNull = 0x01, eStateFixed = 0x02 };

      column() { memset( this, 0, sizeof(column) ); }
      
      bool is_null() const { return (m_uState & eStateNull) == eStateNull; }
      void set_null( bool bNull ) {  if( bNull == true ) { m_uState |= eStateNull; } else { m_uState &= ~eStateNull; } }
      /// Check if column value is stored in fixed buffer
      bool is_fixed() const { return m_uState & eStateFixed; }
      void set_fixed( bool bFixed ) { if( bFixed == true ) { m_uState |= eStateFixed; } else { m_uState &= ~eStateFixed; } }

      void type( unsigned uType ) { m_uType = uType; }
      [[nodiscard]] unsigned type() const noexcept { return m_uType; }
      void ctype( unsigned uCType ) { m_uCType = uCType; }
      [[nodiscard]] unsigned ctype() const noexcept { return m_uCType; }
      void size( unsigned uSize ) { m_uSize = uSize; }
      [[nodiscard]] unsigned size() const noexcept { return m_uSize; }
      [[nodiscard]] uint16_t name() const { return m_uNameOffset; }
      void name( uint16_t uOffset ) { m_uNameOffset = uOffset; }
      [[nodiscard]] std::string_view name( const char* pbBuffer ) const noexcept { assert( m_uNameOffset != 0 ); return names::get_name_s( pbBuffer, m_uNameOffset ); }
      //std::string_view name() const noexcept { assert( m_uNameOffset != 0 ); return names::get_name_s( m_pbBufferNames, m_uNameOffset ); }
      [[nodiscard]] uint16_t alias() const { return m_uAliasOffset; }
      void alias( uint16_t uOffset ) { m_uAliasOffset = uOffset; }
      [[nodiscard]] std::string_view alias( const char* pbBuffer ) const noexcept { assert( m_uAliasOffset != 0 ); return names::get_name_s( pbBuffer, m_uAliasOffset ); }
      //std::string_view alias() const noexcept { assert( m_uAliasOffset != 0 ); return names::get_name_s( m_pbBufferNames, m_uAliasOffset ); }
      void value( unsigned uOffset ) { m_uValueOffset = uOffset; }
      [[nodiscard]] unsigned value() const noexcept { return m_uValueOffset; }


      unsigned m_uState;   // colum state
      unsigned m_uType;    // native value type
      unsigned m_uCType;   // c value type
      unsigned m_uSize;    // current value size 
      uint16_t m_uNameOffset; // offset to location for name in buffer
      uint16_t m_uAliasOffset;// offset to location for alias in buffer
      unsigned m_uValueOffset;// offset to value in buffer

   };

// ## construction -------------------------------------------------------------
public:
   record() {}
   // copy
   record( const record& o ) { common_construct( o ); }
   record( record&& o ) noexcept { common_construct( std::move(o) ); }
   // assign
   record& operator=( const record& o ) { common_construct( o ); return *this; }
   record& operator=( record&& o ) noexcept { common_construct( std::move(o) ); return *this; }

   ~record() {}
private:
   // common copy
   void common_construct( const record& o ) {}
   void common_construct( record&& o ) noexcept {
      m_vectorColumn = std::move( o.m_vectorColumn );
      m_namesColumn = std::move( o.m_namesColumn );
      m_buffersValue = std::move( o.m_buffersValue );
   }

// ## operator -----------------------------------------------------------------
public:


// ## methods ------------------------------------------------------------------
public:
/** \name GET/SET
*///@{

//@}

/** \name OPERATION
*///@{

   record& add( unsigned uColumnType, const std::string_view& stringName ) { return add( uColumnType, stringName, std::string_view() ); }
   record& add( unsigned uColumnType, const std::string_view& stringName, const std::string_view& stringAlias ) { return add( uColumnType, size_s(uColumnType), stringName, stringAlias ); }
   record& add( unsigned uColumnType, unsigned uSize, const std::string_view& stringName) { return add( uColumnType, uSize, stringName, std::string_view() ); }
   record& add( unsigned uColumnType, unsigned uSize, const std::string_view& stringName, const std::string_view& stringAlias );

   [[nodiscard]] unsigned size() const noexcept { return (unsigned)m_vectorColumn.size(); }

   // ## access columns
   const column* get_column( unsigned uIndex ) const { return &m_vectorColumn[uIndex]; }
   column* get_column( unsigned uIndex ) { return &m_vectorColumn[uIndex]; }
   int get_column_index_for_name( const std::string_view& stringName ) const;

   // ## access columns buffers
   /// get pointer to buffer value for column with fixed max size
   uint8_t* buffer_get( unsigned uIndex ) const;
   /// get pointer to buffer for column with variable size
   uint8_t* buffer_get_detached( unsigned uIndex ) const { return m_buffersValue.derived_data( uIndex ); }

   // ## access column aliases
   std::string_view alias_get( unsigned uIndex ) const;
   std::vector<std::string_view> alias_get() const;

   // ## access column names
   std::string_view name_get( unsigned uIndex ) const;
   std::vector<std::string_view> name_get() const;

   uint8_t* resize( unsigned uIndex, unsigned uSize ) { return m_buffersValue.derived_resize( uIndex, uSize ); }

   void clear();
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
   std::vector<column> m_vectorColumn; /// vector with columns used to store information about each column record has

   names m_namesColumn;    ///< names, aliases for columns in record. this works like a datastore for const text values
   buffers m_buffersValue; ///< store values for columns record manages

// ## free functions ------------------------------------------------------------
public:
   // ## size in bytes storing values for column type
   [[nodiscard]] static unsigned size_s( enumColumnTypeNumber eType );
   [[nodiscard]] static unsigned size_s( unsigned uType ) { return size_t( (enumColumnTypeNumber)(uType & 0x000000ff) ); }

};

/**
 * @brief Find index for column name
 * @param stringName column name index is returned for
 * @return int index to column if found, otherwise -1
*/
inline int record::get_column_index_for_name( const std::string_view& stringName ) const {
   for( auto it = std::begin( m_vectorColumn ), itEnd = std::end( m_vectorColumn ); it != itEnd; it++ ) {
      auto u = it->name();
      auto name_ = it->name( m_namesColumn );
      if( name_ == stringName )
      {
         return ( int )std::distance( std::begin( m_vectorColumn ), it );
      }
   }
   return -1;
}



_GD_DATABASE_END