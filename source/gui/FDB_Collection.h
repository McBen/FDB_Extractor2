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

		void GetSubDirectories(const wxString& base_path, wxArrayString& names, std::vector<bool>& has_childs);
		void GetFileInfos(const wxString& base_path, std::vector<FDBPackage::file_info>& infos);

		wxString ExtractFile(const wxString& fname, const wxString& destname);
		bool ExtractMultipleFiles(const wxString& src_dir, const wxString& dest_dir, const wxArrayString& files);

	private:
		int  CalcFileCount(const wxString& src_dir, const wxArrayString& files);

	private:
		std::vector<FDBPackage*> packages;
		std::set<std::string> directories;

};
