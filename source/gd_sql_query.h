#pragma once

#include <cassert>
#include <string>
#include <string_view>
#include <vector>
#include <type_traits>

#include "gd_arguments.h"
#include "gd_variant.h"
#include "gd_variant_view.h"


#ifndef _GD_SQL_QUERY_BEGIN
#define _GD_SQL_QUERY_BEGIN namespace gd::sql {
#define _GD_SQL_QUERY_END }
_GD_SQL_QUERY_BEGIN
#else
_GD_SQL_QUERY_BEGIN
#endif

using namespace gd::argument;

/*-----------------------------------------*/ /**
 * \brief sql dialect used to generate sql code
 *
 *
 */
enum enumSqlDialect
{
   eSqlDialectSqlServer    = 1,
   eSqlDialectSqlite       = 2,
   eSqlDialectPostgreSql   = 3,
   eSqlDialectMySql        = 4,

};

/*-----------------------------------------*/ /**
 * \brief how to format sql
 *
 *
 */
enum enumFormat
{
   eFormatUseQuotes        = (1 << 0),
   eFormatAddASKeyword     = (1 << 1),
   eFormatAddINNERKeyword  = (1 << 2),

};


/*-----------------------------------------*/ /**
 * \brief constant values for describing what type of join to use
 *
 *
 */
enum enumJoin
{
   eJoinUnknown = 0,
   eJoinInner = 1,
   eJoinLeft,
   eJoinRight,
   eJoinFull,
};

enum enumOperatorTypeNumber
{
   eOperatorTypeNumberEqual = 0,        // =
   eOperatorTypeNumberNotEqual = 1,     // !=
   eOperatorTypeNumberLess = 2,         // <
   eOperatorTypeNumberLessEqual = 3,    // <=
   eOperatorTypeNumberGreater = 4,      // >
   eOperatorTypeNumberGreaterEqual = 5, // >=
   eOperatorTypeNumberLike = 6,         // ..=..
   eOperatorTypeNumberLikeBegin = 7,    // ..=
   eOperatorTypeNumberLikeEnd = 8,      // =..
   eOperatorTypeNumberNull = 9,         // IS NULL
   eOperatorTypeNumberNotNull = 10,     // IS NOT NULL
   eOperatorTypeNumberIn = 11,          // IN
   eOperatorTypeNumberNotIn = 12,       // NOT IN
   eOperatorTypeNumberEND,              // Used to check for max valid operator number
};

enum enumOperatorGroupType
{
   eOperatorGroupTypeBoolean    = 0x00000100,   // boolean value
   eOperatorGroupTypeNumber     = 0x00000200,   // number value
   eOperatorGroupTypeDate       = 0x00000400,   // date value
   eOperatorGroupTypeString     = 0x00000800,   // text value
   eOperatorGroupTypeBinary     = 0x00001000,   // binary
};

enum enumOperator
{
   eOperatorEqual =                 eOperatorTypeNumberEqual | eOperatorGroupTypeBoolean | eOperatorGroupTypeNumber | eOperatorGroupTypeDate | eOperatorGroupTypeString | eOperatorGroupTypeBinary,
   eOperatorNotEqual =              eOperatorTypeNumberNotEqual | eOperatorGroupTypeBoolean | eOperatorGroupTypeNumber | eOperatorGroupTypeDate | eOperatorGroupTypeString | eOperatorGroupTypeBinary,
   eOperatorLess =                  eOperatorTypeNumberLess | eOperatorGroupTypeNumber | eOperatorGroupTypeDate | eOperatorGroupTypeString,
   eOperatorLessEqual =             eOperatorTypeNumberLessEqual | eOperatorGroupTypeNumber | eOperatorGroupTypeDate | eOperatorGroupTypeString,
   eOperatorGreater =               eOperatorTypeNumberGreater | eOperatorGroupTypeNumber | eOperatorGroupTypeDate | eOperatorGroupTypeString,
   eOperatorGreaterEqual =          eOperatorTypeNumberGreaterEqual | eOperatorGroupTypeNumber | eOperatorGroupTypeDate | eOperatorGroupTypeString,
   eOperatorLike =                  eOperatorTypeNumberLike | eOperatorGroupTypeString,
   eOperatorLikeBegin =             eOperatorTypeNumberLikeBegin | eOperatorGroupTypeString,
   eOperatorLikeEnd =               eOperatorTypeNumberLikeEnd | eOperatorGroupTypeString,
   eOperatorNull =                  eOperatorTypeNumberNull | eOperatorGroupTypeBoolean | eOperatorGroupTypeNumber | eOperatorGroupTypeDate | eOperatorGroupTypeString | eOperatorGroupTypeBinary,
   eOperatorNotNull =               eOperatorTypeNumberNotNull | eOperatorGroupTypeBoolean | eOperatorGroupTypeNumber | eOperatorGroupTypeDate | eOperatorGroupTypeString | eOperatorGroupTypeBinary,
   eOperatorIn =                    eOperatorTypeNumberIn | eOperatorGroupTypeBoolean | eOperatorGroupTypeNumber | eOperatorGroupTypeDate | eOperatorGroupTypeString | eOperatorGroupTypeBinary,
   eOperatorNotIn =                 eOperatorTypeNumberNotIn | eOperatorGroupTypeBoolean | eOperatorGroupTypeNumber | eOperatorGroupTypeDate | eOperatorGroupTypeString | eOperatorGroupTypeBinary,

   eOperatorError =                 0xffffffff,
};

enum enumOperatorMask
{
   eOperatorMaskNumber = 0x000000ff,
};

/**
 * \brief message level to print, how severe information sent to printers
 * 
 */
enum enumSqlPart
{
   //                        3       2 2       1 1
   //                        1       4 3       6 5       8 7       0       
   eSqlPartSelect =        0b0000'0000'0000'0000'0000'0001'0000'0000,
   eSqlPartFrom =          0b0000'0000'0000'0000'0000'0010'0000'0000,
   eSqlPartWhere =         0b0000'0000'0000'0000'0000'0100'0000'0000,
   eSqlPartLimit =         0b0000'0000'0000'0000'0000'1000'0000'0000,
   eSqlPartInsert =        0b0000'0000'0000'0000'0001'0000'0000'0000,
   eSqlPartUpdate =        0b0000'0000'0000'0000'0010'0000'0000'0000,
   eSqlPartDelete =        0b0000'0000'0000'0000'0100'0000'0000'0000,
   eSqlPartOrderBy =       0b0000'0000'0000'0000'1000'0000'0000'0000,
   eSqlPartGroupBy =       0b0000'0000'0000'0001'0000'0000'0000'0000,
   eSqlPartWith =          0b0000'0000'0000'0010'0000'0000'0000'0000,
   eSqlPartHaving =        0b0000'0000'0000'0100'0000'0000'0000'0000,
};

enum enumSql
{
   eSqlSelect =            eSqlPartSelect | eSqlPartFrom | eSqlPartWhere | eSqlPartOrderBy | eSqlPartGroupBy | eSqlPartWith | eSqlPartLimit,
   eSqlDelete =            eSqlPartDelete | eSqlPartFrom | eSqlPartWhere,
};


/**
 * \brief 
 *
 *
 *
 \code
 \endcode
 */
class query 
{
   
public:
   /*-----------------------------------------*/ /**
    * \brief information for tables used in query
    *
    *
    */
   struct table 
   {
      table() {}
      table( unsigned uTable ): m_uKey(uTable) {}
      table(unsigned uTable, std::string_view stringName): m_uKey(uTable) { append("name", stringName); }
      table( const table& o ) { common_construct( o ); }
      table( table&& o ) noexcept { common_construct( o ); }
      table& operator=( const table& o ) { common_construct( o ); return *this; }
      table& operator=( table&& o ) noexcept { common_construct( o ); return *this; }
   
      void common_construct(const table& o) { m_uKey = o.m_uKey; m_iReferenceCount = o.m_iReferenceCount; m_argumentsTable = o.m_argumentsTable; }
      void common_construct( table&& o ) noexcept { m_uKey = o.m_uKey; m_iReferenceCount = o.m_iReferenceCount; m_argumentsTable = std::move( o.m_argumentsTable ); }

      operator unsigned() const { return m_uKey;  }
      

      std::string name() const { return m_argumentsTable["name"].get_string(); }
      std::string alias() const { return m_argumentsTable["alias"].get_string(); }
      std::string schema() const { return m_argumentsTable["schema"].get_string(); }
      std::string owner() const { return m_argumentsTable["owner"].get_string(); }
      std::string join() const { return m_argumentsTable["join"].get_string(); }

      const arguments& get_arguments() const { return m_argumentsTable; }

      template<typename VALUE>
      table& append(std::string_view stringName, const VALUE& v) { m_argumentsTable.append(stringName, v); return *this; }
      table& set(std::string_view stringName, const gd::variant_view& v) { m_argumentsTable.set(stringName, v); return *this; }
      bool has(std::string_view stringName) const { return m_argumentsTable.find(stringName) != nullptr; }
      bool compare(const std::pair<std::string_view, gd::variant_view>& pairMatch) const { return m_argumentsTable.find(pairMatch) != nullptr; }

      bool compare(unsigned uKey) const { return m_uKey == uKey; }

      void add_reference() { m_iReferenceCount++; }
   
      // attributes
      public:
         unsigned m_uKey = 0;        ///< key to table used by other object in query (field belongs to table)
         int m_iReferenceCount = 0;  ///< if table is in use by other items
         arguments m_argumentsTable; ///< all table properties 
   };


   /*-----------------------------------------*/ /**
    * \brief information for fields used in query
    *
    *
    */
   struct field 
   {
      field() {}
      explicit field(unsigned uTable) : m_uTableKey(uTable) {}
      field(unsigned uTable, std::string_view stringName): m_uTableKey(uTable) { append("name", stringName); }
      field(unsigned uTable, std::string_view stringName, std::string_view stringAlias): m_uTableKey(uTable) { append("name", stringName); append("alias", stringAlias); }
      field( const field& o ) { common_construct( o ); }
      field( field&& o ) noexcept { common_construct( o ); }
      field& operator=( const field& o ) { common_construct( o ); return *this; }
      field& operator=( field&& o ) noexcept { common_construct( o ); return *this; }
   
      void common_construct(const field& o) { m_uTableKey = o.m_uTableKey; m_argumentsField = o.m_argumentsField; }
      void common_construct( field&& o ) noexcept { m_uTableKey = o.m_uTableKey; m_argumentsField = std::move( o.m_argumentsField ); }

      unsigned get_table_key() const { return m_uTableKey; }

      std::string name() const { return m_argumentsField["name"].get_string(); }
      std::string alias() const { return m_argumentsField["alias"].get_string(); }
      std::string raw() const { return m_argumentsField["raw"].get_string(); }

      template<typename VALUE>
      field& append(std::string_view stringName, const VALUE& v) { m_argumentsField.append(stringName, v); return *this; }
      field& append_argument( std::string_view stringName, gd::variant_view v) { m_argumentsField.append_argument(stringName, v); return *this; }
      template<typename VALUE>
      field& set(std::string_view stringName, const VALUE& v) { m_argumentsField.set(stringName, v); return *this; }
      bool has(std::string_view stringName) const { return (m_argumentsField.find(stringName) != nullptr); }
      bool compare( const std::pair<std::string_view, gd::variant_view>& pairMatch) const { return m_argumentsField.find(pairMatch) != nullptr; }
      bool compare(const table* pTable) const { return m_uTableKey == *pTable; }

   
      // attributes
      public:
         unsigned m_uTableKey = 0;   ///< table that owns field
         arguments m_argumentsField; ///< all field properties 
   };

   /*-----------------------------------------*/ /**
    * \brief 
    *
    *
    */
   struct condition 
   {
      condition() {}
      explicit condition(unsigned uTable) : m_uTableKey(uTable) {}
      condition( const condition& o ) { common_construct( o ); }
      condition( condition&& o ) noexcept { common_construct( o ); }
      condition& operator=( const condition& o ) { common_construct( o ); return *this; }
      condition& operator=( condition&& o ) noexcept { common_construct( o ); return *this; }

      operator arguments() const { return m_argumentsCondition; }
   
      void common_construct(const condition& o) { m_uTableKey = o.m_uTableKey; m_argumentsCondition = o.m_argumentsCondition; }
      void common_construct(condition&& o) noexcept { m_uTableKey = o.m_uTableKey; m_argumentsCondition = std::move(o.m_argumentsCondition); }

      std::string name() const { return m_argumentsCondition["name"].get_string(); }
      std::string value() const { return m_argumentsCondition["value"].get_string(); }
      std::string raw() const { return m_argumentsCondition["raw"].get_string(); }

      unsigned get_table_key() const { return m_uTableKey; }
      unsigned get_operator() const { return m_argumentsCondition["operator"].get_uint(); }

      template<typename VALUE>
      condition& append(std::string_view stringName, const VALUE& v) { m_argumentsCondition.append(stringName, v); return *this; }
      condition& append_argument(std::string_view stringName, gd::variant_view v) { m_argumentsCondition.append_argument(stringName, v); return *this; }
      template<typename VALUE>
      condition& set(std::string_view stringName, const VALUE& v) { m_argumentsCondition.set(stringName, v); return *this; }
      bool has(std::string_view stringName) const { return (m_argumentsCondition.find(stringName) != nullptr); }
      bool compare(const std::pair<std::string_view, gd::variant_view>& pairMatch) const { return m_argumentsCondition.find(pairMatch) != nullptr; }
      /// compare named value with sent condition, if both condition values for name match return true, otherwise false
      bool compare(const std::string_view& stringName, const condition* pconditionCompareTo) const { return m_argumentsCondition.compare( stringName, *pconditionCompareTo ); }
      

   
      // attributes
      public:
         unsigned m_uTableKey = 0;   ///< table that owns condition
         arguments m_argumentsCondition; ///< all condition properties 
   };

// ## construction -------------------------------------------------------------
public:
   query() {}
   query( unsigned uFormatOptions ) : m_uFormatOptions(uFormatOptions) {}
   // copy
   query( const query& o ) { common_construct( o ); }
   query( query&& o ) noexcept { common_construct( o ); }
   // assign
   query& operator=( const query& o ) { common_construct( o ); return *this; }
   query& operator=( query&& o ) noexcept { common_construct( o ); return *this; }
   
	~query() {}
private:
   // common copy
   void common_construct(const query& o);
   void common_construct(query&& o) noexcept;

// ## operator -----------------------------------------------------------------
public:
   

// ## methods ------------------------------------------------------------------
public:
/** \name GET/SET
*///@{
   
//@}

/** \name TABLE
*///@{
   const table* table_get() const { return &m_vectorTable[0]; }
   const table* table_get(const gd::variant_view& variantTable) const;
   table* table_get( const std::pair<std::string_view, gd::variant_view>& pairField );
   const table* table_get_for_key(unsigned uTableKey) const;
   table* table_add(std::string_view stringName);
   table* table_add(const std::vector< std::pair<std::string_view, gd::variant_view> >& vectorTable );
   std::size_t table_size() const { return m_vectorTable.size(); }
   bool table_empty() const { return m_vectorTable.empty(); }

   std::vector<table>::iterator table_begin() { return m_vectorTable.begin(); }
   std::vector<table>::const_iterator table_begin() const { return m_vectorTable.cbegin(); }
   std::vector<table>::iterator table_end() { return m_vectorTable.end(); }
   std::vector<table>::const_iterator table_end() const { return m_vectorTable.cend(); }
//@}


/** \name FIELD
*///@{
   // ## add fields to query
   field* field_add(std::string_view stringName) { return field_add(gd::variant_view(0u), stringName, std::string_view()); }
   field* field_add(const gd::variant_view& variantTable, std::string_view stringName) { return field_add(gd::variant_view(0u), stringName, std::string_view()); }
   field* field_add(const gd::variant_view& variantTable, std::string_view stringName, std::string_view stringAlias);
   field* field_add(const std::vector< std::pair<std::string_view, gd::variant_view> >& vectorField) { return field_add( gd::variant_view(0u), vectorField ); }
   field* field_add(const gd::variant_view& variantTable, const std::vector< std::pair<std::string_view, gd::variant_view> >& vectorField );
   void field_add_many(const std::vector< std::vector< std::pair<std::string_view, gd::variant_view> > >& vectorVectorField );

   // ## get field in query
   const field* field_get(unsigned uIndex) const { assert(uIndex < m_vectorField.size()); return &m_vectorField[uIndex]; }
   field* field_get(unsigned uIndex) { assert(uIndex < m_vectorField.size()); return &m_vectorField[uIndex]; }
   field* field_get( const std::pair<std::string_view, gd::variant_view>& pairField );

   // ## various field operations and iterator

   std::size_t field_size() const { return m_vectorField.size(); }
   bool field_empty() const { return m_vectorField.empty(); }

   std::vector<field>::iterator field_begin() { return m_vectorField.begin();  }
   std::vector<field>::const_iterator field_begin() const { return m_vectorField.cbegin();  }
   std::vector<field>::iterator field_end() { return m_vectorField.end(); }
   std::vector<field>::const_iterator field_end() const { return m_vectorField.cend(); }
//@}

/** \name CONDITION
*///@{
   condition* condition_add(std::string_view stringName, const gd::variant_view& variantOperator, const gd::variant_view& variantValue);
   condition* condition_add(const gd::variant_view& variantTable, std::string_view stringName, const gd::variant_view& variantOperator, const gd::variant_view& variantValue);
   condition* condition_add(const std::vector< std::pair<std::string_view, gd::variant_view> >& vectorCondition) { return condition_add( gd::variant_view(0u), vectorCondition ); }
   condition* condition_add(const gd::variant_view& variantTable, const std::vector< std::pair<std::string_view, gd::variant_view> >& vectorCondition );
   condition* condition_add_(const table* ptable, std::string_view stringName, const gd::variant_view& variantOperator, const gd::variant_view& variantValue);
//@}



/** \name OPERATION
*///@{
   template <typename VALUE>
   void set_attribute( const std::string_view& stringName, const VALUE& value_ ) { m_argumentsAttribute.set( stringName, value_ ); }
   gd::variant_view distinct() const { return m_argumentsAttribute["distinct"].get_variant_view(); }
   gd::variant_view limit() const { return m_argumentsAttribute["limit"].get_variant_view(); }


   /// Generate key values for internal data in query
   unsigned next_key() { return ++m_uNextKey; };
   // sql_update(), sql_update( iDbType )
   // sql_insert(), sql_insert( iDbType )

   void sql_set_dialect( enumSqlDialect eSqlDialect ) { m_eSqlDialect = eSqlDialect; }

   std::string sql_get_select() const;
   std::string sql_get_from() const;
   std::string sql_get_where() const;
   std::string sql_get_insert() const;
   std::string sql_get_update() const;
   std::string sql_get_delete() const;
   std::string sql_get_groupby() const;
   std::string sql_get_orderby() const;
   std::string sql_get_limit() const;
   std::string sql_get_with() const;

   std::string sql_get( enumSql eSql ) const;
   std::string sql_get( enumSql eSql, const unsigned* puPartOrder ) const;

   
//@}

protected:
/** \name INTERNAL
*///@{
   
//@}

// ## attributes ----------------------------------------------------------------
public:
   enumSqlDialect m_eSqlDialect;    ///< sql dialect (brand) used to generate sql
   unsigned m_uNextKey = 0;         ///< used to generate keys
   unsigned m_uFormatOptions;       ///< How to format query, has flags from `enumFormat`
   std::vector<table> m_vectorTable;///< list of tables used to generate query
   std::vector<field> m_vectorField;///< list of fields used to generate query
   std::vector<condition> m_vectorCondition;///< list of conditions used to generate query
   arguments m_argumentsAttribute;  ///< Attributes are values like `limit`, `distinct`


   static unsigned m_puPartOrder_s[];

// ## free functions ------------------------------------------------------------------
public:
   // ## SQL key words and type numbers
   static enumJoin get_join_type_s(const std::string_view& stringJoin);
   static std::string_view sql_get_join_text_s(enumJoin eJoinType);

   // ## SQL WHERE operator
   static enumOperator get_where_operator_number_s(std::string_view stringOperator);
   static enumOperator get_where_operator_number_s(const gd::variant_view& variantOperator);
   static unsigned get_where_operator_text_s(unsigned uOperator, char* pbBuffer);
   static void print_condition_values_s( std::string& stringValues,  const std::vector<const condition*>& vectorCondition);

   // ## Condition methods
   /// Find all conditions for same field and same operator
   static std::vector<std::size_t> condition_find_all_for_operataor_s(const std::vector<condition>& vectorCondtion, const condition* pconditionMatch, unsigned uBegin);

   // ## flag methods
   template<typename FLAG>
   static bool flag_has_s(unsigned uTest, FLAG uFlag);

   // ## format methods
   static void format_add_and_surround_s(std::string& stringValue, const std::string_view& stringAdd, char chCharacter);

};

/// Add table
inline query::table* query::table_add(std::string_view stringName) {
   m_vectorTable.push_back( table( next_key(), stringName ) );
   return &m_vectorTable.back();
}

/// Return table pointer for table key if found, nullptr if not found
inline const query::table* query::table_get_for_key(unsigned uTableKey) const {
   for( auto it = m_vectorTable.begin(); it != m_vectorTable.end(); it++ ) {
      if( it->compare(uTableKey) == true ) return &(*it);
   }
   return nullptr;
}

/// Add field with name and alias


inline void query::field_add_many(const std::vector< std::vector< std::pair<std::string_view, gd::variant_view> > >& vectorVectorField) {
   for( auto it : vectorVectorField ) field_add(it);
}

template<typename FLAG>
bool query::flag_has_s(unsigned uTest, FLAG uFlag) { 
   static_assert( sizeof(FLAG) >= 4, "Value isn't compatible with unsigned (4 byte)");
   return (uTest & (unsigned)uFlag) == 0; 
}

/// add surrounded value to string, like XXXX => "XXXX"
inline void query::format_add_and_surround_s(std::string& stringText, const std::string_view& stringAdd, char chCharacter) {
   stringText += chCharacter;
   stringText += stringAdd;
   stringText += chCharacter;
}




_GD_SQL_QUERY_END // namespace _GD_CALCULATE_PARSE_BEGIN