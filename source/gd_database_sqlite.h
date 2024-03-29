/*
| name | description |
| ---- | ----------- |
| `` |  |
| `` |  |
| `` |  |
*/

#pragma once

#include <cassert>
#include <cstring>
#include <string>
#include <string_view>
#include <type_traits>

#include "gd_variant.h"
#include "gd_variant_view.h"
#include "gd_arguments.h"
#include "gd_database_types.h"
#include "gd_database_record.h"

#include "sqlite/sqlite3.h"



#ifndef _GD_DATABASE_SQLITE_BEGIN
#define _GD_DATABASE_SQLITE_BEGIN namespace gd { namespace database { namespace sqlite {
#define _GD_DATABASE_SQLITE_END } } }
_GD_DATABASE_SQLITE_BEGIN
#else
_GD_DATABASE_SQLITE_BEGIN
#endif


/**
 * \brief
 *
 *
 *
 \code
 \endcode
 */
class database
{
// ## construction -------------------------------------------------------------
public:
   database() : m_psqlite3{ nullptr } {}
   database( sqlite3* psqlite3 ) : m_psqlite3 { psqlite3 }, m_bOwner{ true } {}
   database( void* psqlite3 ) : m_psqlite3{ (sqlite3*)psqlite3 }, m_bOwner{ true } {}
   database( void* psqlite3, bool bOwner ) : m_psqlite3{ (sqlite3*)psqlite3 }, m_bOwner{ bOwner } {}

   // copy
   database(const database& o) { common_construct(o); }
   database(database&& o) noexcept { common_construct( std::move(o) ); }
   // assign
   database& operator=(const database& o) { common_construct(o); return *this; }
   database& operator=(database&& o) noexcept { common_construct(o); return *this; }

   ~database() { close(); }
private:
// common copy
   void common_construct(const database& o) {
      m_psqlite3 = o.m_psqlite3;
   }
   void common_construct(database&& o) noexcept {
      m_psqlite3 = o.m_psqlite3;
      o.m_psqlite3 = nullptr;
   }

// ## operator -----------------------------------------------------------------
public:


// ## methods ------------------------------------------------------------------
public:
/** \name GET/SET
*///@{
   sqlite3* get_sqlite3() const { return m_psqlite3; }

//@}

/** \name OPERATION
*///@{
   /// Open sqlite database, if no sqlite database at file location one will be created
   std::pair<bool, std::string> open(const std::string_view& stringFileName);

   /// Execute sql, any sql
   std::pair<bool, std::string> execute(const std::string_view& stringQuery);

   /// get last inserted key
   gd::variant get_insert_key() const;

   /// get number of changes in last call to database
   gd::variant get_change_count() const;

   /// close sqlite connection if open
   void close() { if( m_bOwner == true ) { close_s( m_psqlite3 ); } m_psqlite3 = nullptr; }

   /// Release internal pointer to sqlite database
   sqlite3* release() { sqlite3* psqlite3 = m_psqlite3; m_psqlite3 = nullptr; return psqlite3; }

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
   bool m_bOwner;          ///< If pointer to sqlite database connection is owned (then it is deleted when object is destroyed)
   sqlite3* m_psqlite3;    ///< pointer to sqlite database connection


// ## free functions ------------------------------------------------------------
public:

   static std::pair<sqlite3*, std::string> open_s(const std::string_view& stringFileName, int iFlags);
   static std::pair<sqlite3*, std::string> open_s( const std::string_view& stringFileName ) { return open_s( stringFileName, 0 ); }
   static std::pair<bool, std::string> execute_s(sqlite3* psqlite, const std::string_view& stringQuery);
   static void close_s(sqlite3* psqlite);
};

inline std::pair<bool, std::string> database::execute(const std::string_view& stringQuery) { return database::execute_s(m_psqlite3, stringQuery); }




/**
 * \brief
 *
 *
 *
 \code
 \endcode
 */
class cursor
{
public:
   enum enumState { row = 0x0001 };

// ## construction -------------------------------------------------------------
public:
   cursor(): m_uState(0), m_pstmt(nullptr), m_pdatabase(nullptr) {}
   cursor( database* pdatabase ) : m_uState( 0 ), m_pstmt( nullptr ), m_pdatabase( pdatabase ) { assert( pdatabase != nullptr ); assert( pdatabase->get_sqlite3() != nullptr ); }
   // copy
   cursor(const cursor& o) { common_construct(o); }
   cursor(cursor&& o) noexcept { common_construct(o); }
   // assign
   cursor& operator=(const cursor& o) { common_construct(o); return *this; }
   cursor& operator=(cursor&& o) noexcept { common_construct(o); return *this; }

   ~cursor() { close(); }
private:
   // common copy
   void common_construct(const cursor& o) {}
   void common_construct(cursor&& o) noexcept {}

// ## operator -----------------------------------------------------------------
public:
   // ## Index operators, returns variant_view with value from column
   //    Using index to column or column name it is possible to access column value
   //    matching index or name.
   gd::variant_view operator[](unsigned uIndex) const { return get_variant_view(uIndex); }
   gd::variant_view operator[](const std::string_view& stringName) const { return get_variant_view(stringName); }

   /// get internal record that has information about columns
   operator const record& () const { return m_recordRow; }


// ## methods ------------------------------------------------------------------
public:
/** \name GET/SET
*///@{
   unsigned get_column_count() const { return m_recordRow.size(); }
//@}

/** \name OPERATION
*///@{
   /// Open SQL SELECT query
   std::pair<bool, std::string> open(const std::string_view& stringSql);

   void update() { update(0, m_recordRow.size()); }
   void update( unsigned uFrom, unsigned uTo );

   /// go to next row
   std::pair<bool, std::string> next();
   /// check if row is valid
   bool is_valid_row() const { return (m_uState & row) == row; }

   /// close statement if open
   void close() {
      if(m_pstmt != nullptr) { 
         ::sqlite3_finalize(m_pstmt); m_pstmt = nullptr; 
         m_uState = 0; 
         m_recordRow.clear();
      }
   }


   // ## `variant` methods, return value(s) as variants
   std::vector<gd::variant> get_variant() const;
   gd::variant get_variant( unsigned uColumnIndex ) const;
   std::vector<gd::variant_view> get_variant_view() const;
   gd::variant_view get_variant_view( unsigned uColumnIndex ) const;
   gd::variant_view get_variant_view( const std::string_view& stringName ) const;


   int get_index( const std::string_view& stringName ) const;

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
   unsigned m_uState;            ///< cursor state
   sqlite3_stmt* m_pstmt;        ///< sqlite statement for active result
   database* m_pdatabase;        ///< database cursor reads data from 
   record m_recordRow;           ///< buffer used to store data from active row


// ## free functions ------------------------------------------------------------
public:
   static unsigned get_column_type_s( const char* pbszColumnType );
   static unsigned get_column_ctype_s( const char* pbszColumnType );

};


_GD_DATABASE_SQLITE_END