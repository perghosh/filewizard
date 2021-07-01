#pragma once

namespace application { namespace file {

	class CFile;

	class CSection
	{
	public:
		CSection();
		~CSection();

	public:
		CFile* m_pFile = nullptr;

	};

	CSection::CSection()
	{
	}

	CSection::~CSection()
	{
	}


	class CFile
	{
	public:
		CFile() {};
		~CFile() {};

	public:
		std::vector<CSection> m_vectorSection;

	};


} }
