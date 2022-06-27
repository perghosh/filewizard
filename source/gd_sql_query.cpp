#include "gd_sql_query.h"

/**
* \file gd_sql_query.h
* *Sample how to create SQL using two tables called `TActivity` and `TCustomer`.*
```cpp
{
   gd::sql::query query;      // create query object
   // add two tables
   query.table_add({ {"name","TActivity"}, {"schema","application"}, {"alias","Activity1"} });
   query.table_add({ {"name","TCustomer"}, {"schema","application"}, {"alias","Customer1"}, {"join","Activity1.ActivityK=Customer1.CustomerK"} });
   // add two fields
   query.field_add("Activity1", { {"name", "ActivityK"}, {"alias", "ID"} });
   query.field_add("Customer1", { {"name", "FName"}, {"alias", "CustomerName"} });

   // generate SQL
   auto stringSQL = std::string("SELECT ");
   stringSQL += query.sql_get_select();
   stringSQL += "\nFROM ";
   stringSQL += query.sql_get_from();
   std::cout << stringSQL << std::endl;
}
```
*/

_GD_SQL_QUERY_BEGIN

const query::table* query::table_get(const gd::variant_view& variantTableIndex) const
{
   if( variantTableIndex.is_integer() == true )
   {
      unsigned uIndex = variantTableIndex.get_uint();                            assert( uIndex < m_vectorTable.size() );
      return &m_vectorTable[uIndex];
   }
   else
   {
      std::string stringTable = variantTableIndex.get_string();
      /// ## Find table matching table name
      for( auto it = std::begin(m_vectorTable); it != std::end(m_vectorTable); it++ )
      {
         if( it->get_arguments().compare({ "name", stringTable }) == true ) return &(*it);
      }

      // ## Find table matching alias
      for( auto it = std::begin(m_vectorTable); it != std::end(m_vectorTable); it++ )
      {
         if( it->get_arguments().compare({ "alias", stringTable }) == true ) return &(*it);
      }
   }

   return nullptr;
}


query::table* query::table_get(const std::pair<std::string_view, gd::variant_view>& pairTable)
{
   for( auto it = std::begin(m_vectorTable); it != std::end(m_vectorTable); it++ )
   {
      if( it->compare(pairTable) == true ) return &(*it);
   }

   return nullptr;
}


/*----------------------------------------------------------------------------- table_add */ /**
 * Adds table with table properties found in vector
 * \param vectorTable vector with pair items where each pair represents one property for table
 * \return table* pointer to added table
 */
query::table* query::table_add(const std::vector< std::pair<std::string_view, gd::variant_view> >& vectorTable)
{
   table tableAdd(next_key());
   for( auto it : vectorTable )
   {
      tableAdd.set(it.first, it.second);
   }

   m_vectorTable.push_back( std::move( tableAdd ) );
   return &m_vectorTable.back();
}

/*----------------------------------------------------------------------------- field_add */ /**
 * Add field and set field name
 * \param variantTable index to table field belongs to
 * \param stringName name for field
 * \param stringAlias alias for field
 * \return query::field* pointer to added field
 */
query::field* query::field_add(const gd::variant_view& variantTable, std::string_view stringName, std::string_view stringAlias) 
{
   auto ptable = table_get(variantTable);                                        assert(ptable != nullptr);

   m_vectorField.push_back(field(*ptable, stringName));

   auto pfield = &m_vectorField.back();
   if( stringAlias.empty() == false ) pfield->append("alias", stringAlias);

   return pfield;
}

/*----------------------------------------------------------------------------- field_add */ /**
 * Add field to query
 * \param variantTable index to table field belongs to
 * \param vectorField vector with field properties, each property has a name and a value
 * \return gd::sql::query::field* pointer to added field
 */
query::field* query::field_add(const gd::variant_view& variantTable, const std::vector< std::pair<std::string_view, gd::variant_view> >& vectorField)
{
   auto ptable = table_get(variantTable);                                        assert(ptable != nullptr);

   field fieldAdd( *ptable );                                                    // create field object that is added to query

   for( auto it : vectorField )
   {  // append values for field added to query
      fieldAdd.append_argument(it.first, it.second);                             // first = name, second = value
   }

   m_vectorField.push_back(std::move(fieldAdd));                                 // add to list with fields
   return &m_vectorField.back();                                                 // return pointer to added field
}

/*----------------------------------------------------------------------------- condition_add */ /**
 * Add condition to query
 * \param variantTable index to table condition belongs to
 * \param stringName condition field name
 * \param variantOperator condition operator
 * \param variantValue condition value
 * \return condition* pointer to added condition
 */
gd::sql::query::condition* query::condition_add(const gd::variant_view& variantTable, std::string_view stringName, const gd::variant_view& variantOperator, const gd::variant_view& variantValue)
{
   auto ptable = table_get(variantTable);                                        assert(ptable != nullptr);

   condition conditionAdd(*ptable);                                              // create condition object that is added to query
   conditionAdd.append("name", stringName);


   enumOperator eOperator = get_where_operator_number_s(variantOperator);        assert( eOperator != eOperatorError );
   conditionAdd.append("operator", eOperator);
   conditionAdd.append_argument("value", variantValue);

   m_vectorCondition.push_back(std::move(conditionAdd));                         // add to list with fields
   return &m_vectorCondition.back();                                             // return pointer to added condition
}

/*----------------------------------------------------------------------------- condition_add */ /**
 * Add condition to query
 * \param variantTable index to table field belongs to
 * \param vectorField vector with field properties, each property has a name and a value
 * \return gd::sql::query::field* pointer to added field
 */
query::condition* query::condition_add(const gd::variant_view& variantTable, const std::vector< std::pair<std::string_view, gd::variant_view> >& vectorCondition)
{
   auto ptable = table_get(variantTable);                                        assert(ptable != nullptr);

   condition conditionAdd( *ptable );                                            // create field object that is added to query

   for( auto it : vectorCondition )
   {  // append values for field added to query
      if( it.first[0] == 'o' && it.first == "operator" )
      {
         enumOperator eOperator = get_where_operator_number_s(it.second);        assert( eOperator != eOperatorError );
         conditionAdd.append("operator", eOperator);
      }
      else { conditionAdd.append_argument(it.first, it.second); }                // first = name, second = value
   }

   m_vectorCondition.push_back(std::move(conditionAdd));                         // add condition to list
   return &m_vectorCondition.back();                                             // return pointer to added condition
}



/*----------------------------------------------------------------------------- field_get */ /**
 * return field based on property value for field
 * \param pairField name and value that are matched for field searched for
 * \return field* pointer to field if found, nullptr if not found
 */
query::field* query::field_get( const std::pair<std::string_view, gd::variant_view>& pairField )
{
   for( auto it = std::begin(m_vectorField); it != std::end(m_vectorField); it++ )
   {
      if( it->compare(pairField) == true ) return &(*it);                        // compare if field matches named property and property value, if match return pointer to field
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

      const table* ptable = table_get_for_key(it->get_table_key());            assert( ptable != nullptr ); // no table found for key indicates internal error for query object, this shouldn't happen
      if( ptable->has("alias") == true )
      {
         stringSelect += ptable->alias();
         stringSelect += ".";
      }

      stringSelect += it->name();
      if( it->has("alias") == true )                                             // found alias ?
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
{                                                                                assert( m_vectorTable.empty() == false ); // don't call this if no tables added to query
   std::string stringFrom; // generated from string with tables used in query

   auto fAddTableName = [](const table* ptable, std::string& stringFrom) -> void {
      if( ptable->has("schema") == true )
      {
         stringFrom += ptable->schema();
         stringFrom += ".";
      }

      stringFrom += ptable->name();
      if( ptable->has("alias") == true )                                         // found alias ?
      {
         stringFrom += " ";
         stringFrom += ptable->alias();
      }
   };

   enumJoin eJoinDefault = eJoinInner;                                           // default join if join isn't, specified for table
   unsigned uTableIndex = 0;
   for( auto itTable = std::begin(m_vectorTable); itTable != std::end(m_vectorTable); itTable++ )
   {
      if( uTableIndex != 0 )
      {
         stringFrom += "\n";                                                     // new line

         enumJoin eJoin = eJoinDefault;
         stringFrom += sql_get_join_text_s(eJoin);
      }

      fAddTableName( &(*itTable), stringFrom);                                   // add table to string

      if( uTableIndex != 0 )
      {
         stringFrom += " ON ";
         std::string stringJoin = itTable->join();
         stringFrom += stringJoin;
      }

      uTableIndex++;
   }

   return stringFrom;
}

/*----------------------------------------------------------------------------- sql_get_join_type_s */ /**
 * Get join type (constant value for join) for text
 * \param stringJoin join text join type is returned for
 * \return gd::sql::enumJoin type of join that join text is
 */
gd::sql::enumJoin query::sql_get_join_type_s(std::string_view stringJoin)
{
   auto pbszJoin = stringJoin.data();
   while( *pbszJoin <= 32 && *pbszJoin != '\0' ) pbszJoin++;
   switch( *pbszJoin )
   {
   case 'i' :
   case 'I' :
   case 'j' :
   case 'J' :
      return eJoinInner;
   case 'l':
   case 'L':
      return eJoinLeft;
   case 'r':
   case 'R':
      return eJoinRight;
   case 'f':
   case 'F':
      return eJoinFull;
   }

   return eJoinUnknown;
}

/*----------------------------------------------------------------------------- sql_get_join_text_s */ /**
 * Return join string for join type
 * \param eJoinType type of join text is returned for
 * \return std::string_view string with join text
 */
std::string_view query::sql_get_join_text_s(enumJoin eJoinType)
{
   switch( eJoinType )
   {
   case eJoinInner: return std::string_view( " INNER JOIN " );
   case eJoinLeft:  return std::string_view( " LEFT JOIN " );
   case eJoinRight: return std::string_view( " RIGHT JOIN " );
   case eJoinFull:  return std::string_view( " FULL JOIN " );
   }
                                                                               assert(false);
   return std::string_view();
}

/*----------------------------------------------------------------------------- get_where_operator_number_s */ /**
 * get where operator number
 * \param stringOperator
 * \return gd::sql::enumOperator
 */
enumOperator query::get_where_operator_number_s(std::string_view stringOperator)
{                                                                                assert( stringOperator.empty() == false );
   auto pbszOperator = stringOperator.data();
   switch( *pbszOperator )
   {
   case '=': return eOperatorEqual;
   case '!': return eOperatorNotEqual;
   case '<': {
      if( *(pbszOperator + 1) == '\0' ) return eOperatorLess;
      else if( *(pbszOperator + 1) == '=' ) return eOperatorLessEqual;
   }
   break;
   case '>': {
      if( *(pbszOperator + 1) == '\0' ) return eOperatorGreater;
      else if( *(pbszOperator + 1) == '=' ) return eOperatorGreaterEqual;
   }
   break;
   case 'e': return eOperatorEqual;
   case 'n': {
      if( stringOperator == "notequal" ) return eOperatorEqual;                  // !=
      else if( stringOperator == "null" ) return eOperatorNull;                  // IS NULL
      else if( stringOperator == "notnull" ) return eOperatorNotNull;            // IS NOT NULL
      else if( stringOperator == "notin" ) return eOperatorNotIn;                // NOT IN
   }
   break;

   }
                                                                                 assert( false );
   return eOperatorError;
}

enumOperator query::get_where_operator_number_s(const gd::variant_view& variantOperator)
{
   enumOperator eOperator;
   if( variantOperator.is_number() ) eOperator = static_cast<enumOperator>(variantOperator.get_uint());
   else if( variantOperator.is_char_string() ) eOperator = get_where_operator_number_s(std::string_view( variantOperator.c_str() ));
   else { eOperator = get_where_operator_number_s(std::string_view(variantOperator.get_string())); }

   return eOperator;
}

_GD_SQL_QUERY_END
