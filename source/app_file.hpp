#pragma once
#include <vector>
#include <string_view>

#include "gd_utf8_string.hpp"

namespace application { namespace file {

	class CFile;

	class CSection
	{
	public:
		CSection() {}
		CSection( CFile* pFile, gd::utf8::string&& stringGroup, gd::utf8::string&& stringCode ): m_pFile(pFile) {}
		CSection( const CSection& o ): m_pFile( o.m_pFile ), m_stringGroup( o.m_stringGroup ), m_stringCode( o.m_stringCode ) { };
		CSection( CSection&& o ): m_pFile( o.m_pFile ), m_stringGroup( std::move( o.m_stringGroup ) ), m_stringCode( std::move( o.m_stringCode ) ) { 
			o.m_pFile = nullptr; 
		};
		~CSection() {};

	public:
		void SetGroup( gd::utf8::string&& stringGroup ) { m_stringGroup = std::move( stringGroup ); }
		void SetCode( gd::utf8::string&& stringCode ) { m_stringCode = std::move( stringCode ); }


	public:
		CFile* m_pFile = nullptr;		/// Parent - each section is connected to the owning file object
		gd::utf8::string m_stringGroup;/// Code group, this is used to filter section parts when working with code
		gd::utf8::string m_stringCode;/// Section code different file operations are working on

	};

	class CFile
	{
	public:
		CFile() {};
		~CFile() {};

	public:
		std::pair<bool, std::string> FILE_Load( std::string stringFileName, std::string_view stringName );

	public:
		std::vector<CSection> m_vectorSection;

	};


} }
