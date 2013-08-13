#pragma once

#include <set>

class FDB_Collection
{
	public:
		FDB_Collection();
		virtual ~FDB_Collection();

		bool Open(const wxString& filename_pattern);
		bool OpenDefault();
		void Close();

		void GetSubDirectories(const char* base_path, std::vector<std::string>& names, std::vector<bool>& has_childs);
		void GetFileInfos(const char* base_path, std::vector<FDBPackage::file_info>& infos);

		std::string ExtractFile(const char* fname, const char* destname);
		bool ExtractMultipleFiles(const char* src_dir, const wxString& dest_dir, const wxArrayString&);

	private:
		int  CalcFileCount(const char* src_dir, const wxArrayString& files);

	private:
		std::vector<FDBPackage*> packages;
		std::set<std::string> directories;

};