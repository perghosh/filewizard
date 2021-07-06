
#include "gd_utf8_string.hpp"
#include "..\include\gd_utf8_string.hpp"

namespace gd { namespace utf8 { 

string::buffer string::m_pbuffer_empty[] = {0,0,0,0,-1, 0};

string::string( gd::utf8::buffer bufferStack ) {                              assert( bufferStack.m_uSize > sizeof( string::buffer ) );
   m_pbuffer = reinterpret_cast<string::buffer*>( bufferStack.m_pBuffer );
   m_pbuffer->capacity( bufferStack.m_uSize - sizeof( string::buffer ) );
   m_pbuffer->flags( eBufferStorageStack | eBufferStorageSingle );
   m_pbuffer->size( 0 );
   m_pbuffer->count( 0 );
   m_pbuffer->set_reference( 1 );
}

void string::push_back( uint8_t ch )
{
   allocate( 1 );

   auto pbszEnd = c_buffer_end();

   auto uSize = convert( ch, pbszEnd );
   pbszEnd[uSize] = '\0';

   m_pbuffer->size( m_pbuffer->size() + uSize );
   m_pbuffer->count( m_pbuffer->count() + 1 );
}

/**
 * @brief copy string object
 * @param o string object that is copied
*/
void string::copy(string& o)
{
   string::release(m_pbuffer);
   m_pbuffer = o.m_pbuffer;

   if(m_pbuffer != string::m_pbuffer_empty)
   {
      if(o.m_pbuffer->is_refcount() == true)
      {
         m_pbuffer->add_reference();
      }
      else
      {
         _clone( o );
      }
   }
}


string& string::assign( const char* pbszText, uint32_t uLength )
{
   uint32_t uSize = gd::utf8::size( pbszText, uLength );
   if( uSize > m_pbuffer->capacity() ) allocate( uSize - m_pbuffer->capacity() );

   convert_ascii( pbszText, m_pbuffer->c_buffer() );

   m_pbuffer->size( m_pbuffer->size() + uSize );
   m_pbuffer->count( m_pbuffer->count() + uLength );

   return *this;
}

string& string::assign( const value_type* pbszText, uint32_t uSize, uint32_t uCount )
{
   string::release( m_pbuffer );
   m_pbuffer = string::m_pbuffer_empty;

   if( uSize > m_pbuffer->capacity() ) allocate_exact( uSize );

   memcpy( c_buffer(), pbszText, uSize );
   m_pbuffer->count( uCount );

   return *this;
}


void string::push_back( uint16_t ch )
{
   allocate( sizeof(uint16_t) );

   auto pbszEnd = c_buffer_end();
   auto uSize = convert( ch, pbszEnd );

   pbszEnd[uSize] = '\0';

   m_pbuffer->size( m_pbuffer->size() + uSize );
   m_pbuffer->count( m_pbuffer->count() + 1 );
}


/**
 * @brief append character store in 32 bit value
 * @param ch character added to string
 */
void string::push_back( uint32_t ch )
{
   allocate( sizeof(uint32_t) );                                                // add four bytes, max size for utf32_t character

   auto pbszEnd = c_buffer_end();
   auto uSize = convert( ch, pbszEnd );

   pbszEnd[uSize] = '\0';

   m_pbuffer->size( m_pbuffer->size() + uSize );
   m_pbuffer->count( m_pbuffer->count() + 1 );
}

string& string::append( const char* pbszText, uint32_t uLength )
{
   uint32_t uSize = gd::utf8::size( pbszText, uLength );
   allocate( uSize );

   auto pbszEnd = c_buffer_end();
   convert_ascii( pbszText, pbszEnd );

   m_pbuffer->size( m_pbuffer->size() + uSize );
   m_pbuffer->count( m_pbuffer->count() + uLength );

   return *this;
}

/**
 * @brief allocate new buffer for string if needed
 * @param uSize added size string needs to have
*/
void string::allocate(uint32_t uSize)
{
   uSize += sizeof(string::buffer);
   //if( uSize + m_uSize >= m_uSizeBuffer )
   if( uSize + m_pbuffer->size() >= m_pbuffer->capacity() )
   {
      auto _size = uSize + m_pbuffer->size();

      if( _size < 64 ) _size = 64;
      else if( _size < 1024 ) _size = 1024;
      else
      {
         uint32_t uAdd = _size % 4096;
         _size += uAdd;

         if( uAdd < 64 ) _size += 4096;
      }

      uint8_t* puNew = new uint8_t[_size];
      memcpy( puNew, m_pbuffer, _size );
      if( m_pbuffer != string::m_pbuffer_empty ) string::release( m_pbuffer );
      m_pbuffer = reinterpret_cast<string::buffer*>( puNew );
      m_pbuffer->set_reference( 1 );
      m_pbuffer->capacity( _size - sizeof(string::buffer) );
   }
}

/**
 * @brief Allocate buffer and specify the exact buffer size
 * @param uSize size buffer gets
*/
void string::allocate_exact(uint32_t uSize)
{                                                                               assert( uSize < 0x01000000 ); // realistic !!
   buffer* pbufferOld = m_pbuffer;

   // ## allocate buffer
   auto _size = uSize + sizeof(string::buffer);
   uint8_t* puNew = new uint8_t[_size + 1];                                     // exact size + zero ending
   m_pbuffer = reinterpret_cast<string::buffer*>( puNew );

   if( string::is_empty( m_pbuffer ) == false )                                 // if old buffer has a valid string, copy
   {
      m_pbuffer->capacity( uSize );
      if( pbufferOld->size() > 0 )
      {
         if( pbufferOld->size() > uSize )                                       // if string is larger then we need to walk backwards to find where to cut, utf8 remember... a character may be stored in multiple bytes
         {
            auto uCount = pbufferOld->count();
            auto pubszEnd = pbufferOld->c_buffer_end();
            while( uCount > 0 && pubszEnd - pbufferOld->c_buffer() > uSize )
            {
               pubszEnd = move::previous( pubszEnd );
               uCount--;
            }

            uSize = static_cast<uint32_t>( pubszEnd - pbufferOld->c_buffer() );
            m_pbuffer->count( uCount );
            m_pbuffer->size( uSize );
         }

         memcpy( m_pbuffer->c_buffer(), pbufferOld->c_buffer(), uSize );
         m_pbuffer->c_buffer()[uSize] = '\0';
         string::release( pbufferOld );
      }

      pbufferOld->release();
   }
   else
   {
      m_pbuffer->reset( uSize );
   }
}

/**
 * @brief clone string
 * This will allways create a new copy of string, no reference copy
 * @param o string that is copied
*/
void string::_clone(const string& o)
{
   m_pbuffer = o.m_pbuffer;

   if(m_pbuffer != string::m_pbuffer_empty)
   {
      allocate_exact(o.size());
      m_pbuffer->flags(0);
      m_pbuffer->size(o.size());
      m_pbuffer->count(o.count());
      m_pbuffer->set_reference(1);
      memcpy(c_buffer(), o.c_str(), size());
   }
}


} }
