#pragma once
#include <vector>
#include <string_view>
#include <regex>

#include "gd_utf8_string.hpp"

namespace application { namespace file {

	extern std::pair<bool, std::string> Erase( gd::utf8::string& stringText, const std::regex& regexMatch );

	class CFile;

	class CSection
	{
	public:
		CSection() {}
		CSection( CFile* pFile, gd::utf8::string& stringGroup, gd::utf8::string& stringCode ): m_pFile(pFile), m_stringGroup(stringGroup), m_stringCode(stringCode) {}
		CSection( CFile* pFile, gd::utf8::string&& stringGroup, gd::utf8::string&& stringCode ): m_pFile(pFile) {}
		CSection( const CSection& o ): m_pFile( o.m_pFile ), m_stringGroup( o.m_stringGroup ), m_stringCode( o.m_stringCode ) { };
		CSection( CSection&& o ) noexcept : m_pFile( o.m_pFile ), m_stringGroup( std::move( o.m_stringGroup ) ), m_stringCode( std::move( o.m_stringCode ) ) { 
			o.m_pFile = nullptr; 
		};
		~CSection() {};

	public:
		void SetGroup( gd::utf8::string&& stringGroup ) { m_stringGroup = std::move( stringGroup ); }
		void SetCode( gd::utf8::string&& stringCode ) { m_stringCode = std::move( stringCode ); }

		bool HasGroup( std::string_view stringGroup ) const noexcept { return true; }

		std::pair<bool, std::string>  Erase( const std::regex& regexMatch ) { return application::file::Erase( m_stringCode, regexMatch ); }


	public:
		CFile* m_pFile = nullptr;		/// Parent - each section is connected to the owning file object
		gd::utf8::string m_stringGroup;/// Code group, this is used to filter section parts when working with code
		gd::utf8::string m_stringCode;/// Section code different file operations are working on

	};

	class CFile
	{
	public:
		CFile() {};
		CFile( std::string_view stringName ): m_stringName( stringName ) {}
		CFile( const CFile& o ): m_stringName( o.m_stringName ), m_vectorSection( o.m_vectorSection ) {}
		CFile( CFile&& o ) noexcept: m_stringName( std::move( o.m_stringName ) ), m_vectorSection( std::move( o.m_vectorSection ) ) {}
		~CFile() {};

	public:
		std::pair<bool, std::string> FILE_Load( std::string stringFileName, std::string_view stringName );

		void SECTION_Append( gd::utf8::string stringGroup, gd::utf8::string stringText ) { m_vectorSection.push_back( CSection( this, stringGroup, stringText ) ); }
		//template<typename STRING>
		//void SECTION_Append( STRING stringGroup, STRING stringText ) { SECTION_Append( gd::utf8::string( stringGroup ), gd::utf8::string( stringText ) ); }

		std::pair<bool, std::string> SECTION_Erase( const std::regex& regexMatch, std::string_view stringGroup );

	public:
		std::string m_stringName;					///< file name
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
