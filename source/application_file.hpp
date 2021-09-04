#pragma once
#include <vector>
#include <string_view>
#include <format>
#include <regex>

#include <boost/regex.hpp>

#include "gd_utf8_string.hpp"

namespace application { namespace file {

#  ifdef BOOST_RE_REGEX_HPP
	extern std::pair<bool, std::string> Replace( gd::utf8::string& stringText, const boost::regex& regexMatch, std::string_view stringInsert, uint32_t uFlags );
	extern std::pair<bool, std::string> Erase( gd::utf8::string& stringText, const boost::regex& regexMatch, uint32_t uFlags );
#  endif
	extern std::pair<bool, std::string> Replace( gd::utf8::string& stringText, const std::regex& regexMatch, std::string_view stringInsert, uint32_t uFlags );
	extern std::pair<bool, std::string> Erase( gd::utf8::string& stringText, const std::regex& regexMatch, uint32_t uFlags );

	class CFile;

/**
 * ## CFile ===================================================================
 */

	class CSection
	{
	public:
		CSection() {}
		CSection( CFile* pFile, gd::utf8::string& stringTag, gd::utf8::string& stringCode ): m_pFile(pFile), m_stringTag(stringTag), m_stringCode(stringCode) {}
		CSection( CFile* pFile, gd::utf8::string&& stringTag, gd::utf8::string&& stringCode ): m_pFile(pFile), m_stringTag(stringTag), m_stringCode(stringCode) {}
		CSection( const CSection& o ): m_pFile( o.m_pFile ), m_stringTag( o.m_stringTag ), m_stringCode( o.m_stringCode ) { };
		CSection( CSection&& o ) noexcept : m_pFile( o.m_pFile ), m_stringTag( std::move( o.m_stringTag ) ), m_stringCode( std::move( o.m_stringCode ) ) { 
			o.m_pFile = nullptr; 
		};
		~CSection() {};

	public:
		void code( gd::utf8::string stringCode ) { m_stringCode = stringCode;  }
		gd::utf8::string code() { return m_stringCode; }
		const gd::utf8::string& code() const { return m_stringCode; }


	public:
		void SetGroup( gd::utf8::string&& m_stringTag ) { m_stringTag = std::move( m_stringTag ); }
		void SetCode( gd::utf8::string&& stringCode ) { m_stringCode = std::move( stringCode ); }

		bool HasGroup( std::string_view m_stringTag ) const noexcept;
		/// Add group to section, if multiple groups then enclose each group in between square brackets "[groupname]"
		void AddGroup( std::string_view stringTag ) { m_stringTag.append( std::format( "[{}]", stringTag ) ); }

		/// Split section into two sections and add them as child's. 
		void Split( std::size_t uPosition ) { Split( std::vector< std::size_t >( { uPosition } ) ); }
		void Split( std::vector<std::size_t> vectorPosition ) { Split( vectorPosition, false ); }
		void Split( std::vector<std::size_t> vectorPosition, bool bKeep );
		gd::utf8::string Join() { return Join( std::string_view() ); }
		gd::utf8::string Join( std::string_view stringTag );

		/// ## Replace all matched text parts from regular expression in string
#     ifdef BOOST_RE_REGEX_HPP
		std::pair<bool, std::string>  Replace( const boost::regex& regexMatch, std::string_view stringInsert, uint32_t uFlags ) { return application::file::Replace( m_stringCode, regexMatch, stringInsert, uFlags ); }
		std::pair<bool, std::string>  Replace( const boost::regex& regexMatch, std::string_view stringInsert ) { return application::file::Replace( m_stringCode, regexMatch, stringInsert, boost::regex_constants::match_default ); }
#		endif
		std::pair<bool, std::string>  Replace( const std::regex& regexMatch, std::string_view stringInsert , uint32_t uFlags ) { return application::file::Replace( m_stringCode, regexMatch, stringInsert, uFlags ); }
		std::pair<bool, std::string>  Replace( const std::regex& regexMatch, std::string_view stringInsert ) { return application::file::Replace( m_stringCode, regexMatch, stringInsert, std::regex_constants::match_default ); }


		/// ## Erase all matched text parts from regular expression in string
#     ifdef BOOST_RE_REGEX_HPP
		std::pair<bool, std::string>  Erase( const boost::regex& regexMatch, uint32_t uFlags ) { return application::file::Erase( m_stringCode, regexMatch, uFlags ); }
		std::pair<bool, std::string>  Erase( const boost::regex& regexMatch ) { return application::file::Erase( m_stringCode, regexMatch, boost::regex_constants::match_default ); }
#		endif
		std::pair<bool, std::string>  Erase( const std::regex& regexMatch, uint32_t uFlags ) { return application::file::Erase( m_stringCode, regexMatch, uFlags ); }
		std::pair<bool, std::string>  Erase( const std::regex& regexMatch ) { return application::file::Erase( m_stringCode, regexMatch, std::regex_constants::match_default ); }

		void SECTION_Append( gd::utf8::string m_stringTag, gd::utf8::string stringText ) { m_vectorSection.push_back( CSection( m_pFile, m_stringTag, stringText ) ); }
		auto SECTION_At( std::size_t uIndex ) const { return m_vectorSection[ uIndex ]; }
		auto SECTION_Begin() { return m_vectorSection.begin(); }
		auto SECTION_Begin() const { return m_vectorSection.cbegin(); }
		auto SECTION_End() { return m_vectorSection.end(); }
		auto SECTION_End() const { return m_vectorSection.cend(); }
		auto SECTION_CEnd() const { return m_vectorSection.cend(); }
		auto SECTION_Size() const { return m_vectorSection.size(); }
		auto SECTION_Empty() const { return m_vectorSection.empty(); }

	public:
		CFile* m_pFile = nullptr;		/// Parent - each section is connected to the owning file object
		gd::utf8::string m_stringTag;/// Code group, this is used to filter section parts when working with code
		gd::utf8::string m_stringCode;/// Section code different file operations are working on
		std::vector<CSection> m_vectorSection;	///< file sections, file can be split in one or more sections
	};

/**
 * ## CFile ===================================================================
 */

	class CFile
	{
	public:
		CFile() {};
		CFile( std::string_view stringPath ) : m_stringPath( stringPath ) { SetNameFromPath(); }
		CFile( const CFile& o ): m_stringName( o.m_stringName ), m_vectorSection( o.m_vectorSection ) {}
		CFile( CFile&& o ) noexcept: m_stringName( std::move( o.m_stringName ) ), m_vectorSection( std::move( o.m_vectorSection ) ) {}
		~CFile() {};

	public:
      const std::string& name() const { return m_stringName; }
      std::string name() { return m_stringName; }
      void name( std::string_view stringName ) { m_stringName = stringName; }


	public:
		std::pair<bool, std::string> FILE_Load( std::string stringFileName, std::string_view stringName );

		void SetNameFromPath();

		void SECTION_Append( gd::utf8::string stringText ) { SECTION_Append( gd::utf8::string(), stringText ); }
		void SECTION_Append( gd::utf8::string stringTag, gd::utf8::string stringText ) { m_vectorSection.push_back( CSection( this, stringTag, stringText ) ); }
		auto SECTION_At( std::size_t uPosition ) const { return m_vectorSection[ uPosition ]; }
		auto SECTION_Begin() { return m_vectorSection.begin(); }
		auto SECTION_Begin() const { return m_vectorSection.cbegin(); }
		auto SECTION_End() { return m_vectorSection.end(); }
		auto SECTION_End() const { return m_vectorSection.cend(); }
		auto SECTION_CEnd() const { return m_vectorSection.cend(); }
		auto SECTION_Size() const { return m_vectorSection.size(); }
		auto SECTION_Empty() const { return m_vectorSection.empty(); }
		//template<typename STRING>
		//void SECTION_Append( STRING m_stringTag, STRING stringText ) { SECTION_Append( gd::utf8::string( m_stringTag ), gd::utf8::string( stringText ) ); }


      /**
       * Replace parts in sections
       */
      ///@{
#     ifdef BOOST_RE_REGEX_HPP
		std::pair<bool, std::string> SECTION_Replace( const boost::regex& regexMatch, std::string_view stringInsert, std::string_view stringTag, uint32_t uFlags );
		std::pair<bool, std::string> SECTION_Replace( const boost::regex& regexMatch, std::string_view stringInsert, std::string_view stringTag ) { return SECTION_Replace( regexMatch, stringInsert, stringTag, boost::regex_constants::match_default ); }
		std::pair<bool, std::string> SECTION_Replace( const boost::regex& regexMatch, std::string_view stringInsert ) { return SECTION_Replace( regexMatch, stringInsert, std::string_view() ); }
#		endif
		std::pair<bool, std::string> SECTION_Replace( const std::regex& regexMatch, std::string_view stringInsert, std::string_view stringTag, uint32_t uFlags );
		std::pair<bool, std::string> SECTION_Replace( const std::regex& regexMatch, std::string_view stringInsert, std::string_view stringTag ) { return SECTION_Replace( regexMatch, stringInsert, stringTag, std::regex_constants::match_default ); }
		std::pair<bool, std::string> SECTION_Replace( const std::regex& regexMatch, std::string_view stringInsert ) { return SECTION_Replace( regexMatch, stringInsert, std::string_view() ); }
      ///@}


      /**
       * Erase parts in sections
       */
      ///@{
#     ifdef BOOST_RE_REGEX_HPP
		std::pair<bool, std::string> SECTION_Erase( const boost::regex& regexMatch, std::string_view stringTag, uint32_t uFlags );
		std::pair<bool, std::string> SECTION_Erase( const boost::regex& regexMatch, std::string_view stringTag ) { return SECTION_Erase( regexMatch, stringTag, boost::regex_constants::match_default ); }
		std::pair<bool, std::string> SECTION_Erase( const boost::regex& regexMatch ) { return SECTION_Erase( regexMatch, std::string_view() ); }
#		endif
		std::pair<bool, std::string> SECTION_Erase( const std::regex& regexMatch, std::string_view stringTag, uint32_t uFlags );
		std::pair<bool, std::string> SECTION_Erase( const std::regex& regexMatch, std::string_view stringTag ) { return SECTION_Erase( regexMatch, stringTag, std::regex_constants::match_default ); }
		std::pair<bool, std::string> SECTION_Erase( const std::regex& regexMatch ) { return SECTION_Erase( regexMatch, std::string_view() ); }
      ///@}

	public:
		std::string m_stringName;					///< file name
		std::string m_stringPath;					///< full file path if file is used
		std::vector<CSection> m_vectorSection;	///< file sections, file can be split in one or more sections

	};

	/**
	 * @brief Hold folder information
	*/
	class CFolder
	{
	public:
		CFolder() {}
		CFolder( std::string_view stringAlias, std::string_view stringFolder): m_stringAlias( stringAlias ), m_stringFolder( stringFolder ) {}
		CFolder(const CFolder& o): m_stringAlias(o.m_stringAlias), m_stringFolder(o.m_stringFolder) {}
		~CFolder() {}

	public:
		const std::string& alias() const noexcept { return m_stringAlias; }
		const std::string& folder() const noexcept { return m_stringFolder; }

		/// Return if folder is empty or not
		bool IsEmpty() const { return m_stringFolder.empty(); }

	public:
		std::string m_stringAlias;		///< Alias used to work with managed folder
		std::string m_stringFolder;	///< Name to folder

	};




} }
