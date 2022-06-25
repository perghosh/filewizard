#include "gd_sql_query.h"

_GD_SQL_QUERY_BEGIN


/*----------------------------------------------------------------------------- table_add */ /**
 * Adds table with table properties found in vector
 * \param vectorTable vector with pair items where each pair represents one property for table
 * \return table* pointer to added table
 */
query::table* query::table_add(const std::vector< std::pair<std::string_view, gd::variant_view> >& vectorTable)
{
   table tableAdd;
   for( auto it : vectorTable )
   {
      tableAdd.set(it.first, it.second);
   }

   m_vectorTable.push_back( std::move( tableAdd ) );
   return &m_vectorTable.back();
}

/*----------------------------------------------------------------------------- field_add */ /**
 * Add field to query
 * \param vectorField vector with field properties, each property has a name and a value
 * \return gd::sql::query::field* pointer to added field
 */
query::field* query::field_add(const std::vector< std::pair<std::string_view, gd::variant_view> >& vectorField)
{
   field fieldAdd(*table_get(0));
   for( auto it : vectorField )
   {
      fieldAdd.set(it.first, it.second);
   }

   m_vectorField.push_back(std::move(fieldAdd));
   return &m_vectorField.back();
}

query::field* query::field_get( const std::pair<std::string_view, gd::variant_view>& pairField )
{
   for( auto it = std::begin(m_vectorField); it != std::end(m_vectorField); it++ )
   {
      if( it->compare(pairField) == true ) return &(*it);
   }

   return nullptr;
}



/*----------------------------------------------------------------------------- sql_get_select */ /**
 * Build `SELECT` text for fields in query
 * \return std::string text for fields selected in query
 */
std::string query::sql_get_select() const
{
   std::string stringSelect; // generated select string with fields used in query

   for( auto it = field_begin(); it != field_end(); it++ )
   {
      if( stringSelect.empty() == false ) stringSelect += ", ";

      const table* ptable = table_get_for_key(it->get_table_key());            assert( ptable != nullptr );
      if( ptable->has("alias") == true )
      {
         stringSelect += ptable->alias();
         stringSelect += ".";
      }

      stringSelect += it->name();
      if( it->has("alias") == true )
      {
         stringSelect += " ";
         stringSelect += it->alias();
      }
   }
   return stringSelect;
}


/*----------------------------------------------------------------------------- sql_get_from */ /**
 * Build "FROM" text for tables in query
 * \return std::string
 */
std::string query::sql_get_from() const
{
   std::string stringFrom; // generated from string with tables used in query

   if( m_vectorTable.size() == 1 )
   {
      auto ptableFrom = table_get();
      if( ptableFrom->has("schema") == true )
      {
         stringFrom += ptableFrom->schema();
         stringFrom += ".";
      }

      stringFrom += ptableFrom->name();
   }

   return stringFrom;
}


_GD_SQL_QUERY_END
