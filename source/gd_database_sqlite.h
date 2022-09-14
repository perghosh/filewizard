#pragma once

#include <cassert>
#include <string>
#include <string_view>
#include <type_traits>

#include "gd_variant.h"
#include "gd_variant_view.h"
#include "gd_arguments.h"
#include "gd_database_types.h"

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
   database( sqlite3* psqlite3 ) : m_psqlite3{ psqlite3 } {}

   // copy
   database(const database& o) { common_construct(o); }
   database(database&& o) noexcept { common_construct(o); }
   // assign
   database& operator=(const database& o) { common_construct(o); return *this; }
   database& operator=(database&& o) noexcept { common_construct(o); return *this; }

   ~database() { close(); }
private:
// common copy
   void common_construct(const database& o) {}
   void common_construct(database&& o) noexcept {}

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

   /// close sqlite connection if open
   void close() { close_s(m_psqlite3); m_psqlite3 = nullptr; }

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
   sqlite3* m_psqlite3;


// ## free functions ------------------------------------------------------------
public:

   static std::pair<sqlite3*, std::string> open_s(const std::string_view& stringFileName);
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
class recordset
{
// ## construction -------------------------------------------------------------
public:
   recordset(): m_pdatabase(nullptr) {}
   recordset( database* pdatabase ): m_pdatabase(pdatabase) {}
   // copy
   recordset(const recordset& o) { common_construct(o); }
   recordset(recordset&& o) noexcept { common_construct(o); }
   // assign
   recordset& operator=(const recordset& o) { common_construct(o); return *this; }
   recordset& operator=(recordset&& o) noexcept { common_construct(o); return *this; }

   ~recordset() {}
private:
   // common copy
   void common_construct(const recordset& o) {}
   void common_construct(recordset&& o) noexcept {}

// ## operator -----------------------------------------------------------------
public:


// ## methods ------------------------------------------------------------------
public:
/** \name GET/SET
*///@{

//@}

/** \name OPERATION
*///@{
   std::pair<bool, std::string> open(const std::string_view& stringSql);

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
   database* m_pdatabase;        ///< database recordset reads data from 


// ## free functions ------------------------------------------------------------
public:
   static unsigned get_column_type_s( const char* pbszColumnType );

};



_GD_DATABASE_SQLITE_END