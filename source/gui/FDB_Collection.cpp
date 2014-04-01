#include "stdafx.h"

#include <string>
#include <algorithm>

#include "FDB_Collection.h"
#include "wx/filename.h"
#include "wx/dir.h"
#include "Export_DLG.h"

using namespace std;

FDB_Collection::FDB_Collection()
{
}

FDB_Collection::~FDB_Collection()
{
	Close();
}

bool FDB_Collection::Open(const wxString& filename_pattern)
{
	bool res = false;
	wxString f = wxFindFirstFile(filename_pattern);
	while ( !f.empty() )
	{
		FDBPackage* new_fdb=new FDBPackage(f.mb_str());
		if (new_fdb->GetFileCount()>0)
		{
			packages.push_back(new_fdb);
			res = true;
		}
		else
		{
			delete (new_fdb);
		}

		f = wxFindNextFile();
	}

	//if (res)	UpdateDirectories();

	return res;
}

bool FDB_Collection::OpenDefault()
{
	Close();

	wxString dir = wxString::FromUTF8(GetROMInstallDir().c_str());
	if (wxDirExists(dir))
		return Open(dir + wxT("fdb/*.fdb"));
	else
		return false;
}

void FDB_Collection::Close()
{
	for (vector<FDBPackage*>::iterator pack=packages.begin();pack!=packages.end();++pack) delete(*pack);
	packages.clear();
	directories.clear();
}

void FDB_Collection::GetSubDirectories(const wxString& base_path, wxArrayString& names, std::vector<bool>& has_childs)
{
    const char separator = wxFileName::GetPathSeparator();

	names.clear();
	has_childs.clear();

	for (vector<FDBPackage*>::iterator ipack=packages.begin();ipack!=packages.end();++ipack)
	{
		FDBPackage* pack = *ipack;

		size_t count = pack->GetFileCount();
		for (size_t i = 0;i<count;++i)
		{
			wxString name=wxString::FromUTF8(pack->GetFileName(i));
			wxString rest;
			if (name.StartsWith(base_path,&rest))
			{
				int sep_pos = rest.Find(separator);
				if (sep_pos != wxNOT_FOUND)
				{
					wxString cur_dir = rest.Left(sep_pos);

					int sep_pos2 = rest.Find(separator);
					int index = names.Index(cur_dir);
					if (index==wxNOT_FOUND)
					{
						names.Add(cur_dir);
						has_childs.push_back(sep_pos2 != wxNOT_FOUND);
					}
					else
					{
						if (sep_pos2 != wxNOT_FOUND)
						{
							has_childs[index]=true;
						}
					}
				}
			}
		}
	}
}

void FDB_Collection::GetFileInfos(const wxString& base_path, std::vector<FDBPackage::file_info>& infos)
{
    const char separator = wxFileName::GetPathSeparator();

	infos.clear();

	for (vector<FDBPackage*>::iterator ipack=packages.begin();ipack!=packages.end();++ipack)
	{
		FDBPackage* pack = *ipack;

		size_t count = pack->GetFileCount();
		for (size_t i = 0;i<count;++i)
		{
			wxString name=wxString::FromUTF8(pack->GetFileName(i));
			wxString rest;
			if ( name.StartsWith(base_path, &rest) )
			{
				if (rest.find(separator)==wxNOT_FOUND)
				{
					FDBPackage::file_info info;
					pack->GetFileInfo(i,info);
					infos.push_back(info);
				}
			}
		}
	}
}

wxString FDB_Collection::ExtractFile(const wxString& fname, const wxString& destname)
{
	for (vector<FDBPackage*>::iterator ipack=packages.begin();ipack!=packages.end();++ipack)
	{
		size_t idx = (*ipack)->FindFile(fname.mb_str());
		if (idx!=-1)
		{
			FDBFile* file = (*ipack)->GetFile(idx);
			if (file)
			{
				FDBPackage::e_export_format e= file->DefaultFormat();
				bool res=  file->WriteToFile(destname.mb_str(), e);
				wxString res_name = wxString::FromUTF8( file->GetTargetName(destname.mb_str(), e).c_str());
				delete(file);

				if (res)	return res_name;
			}
		}
	}

	return wxT("");
}

int FDB_Collection::CalcFileCount(const wxString& src_dir, const wxArrayString& files)
{
	if (files.size()>0) return files.size();

	size_t count=0;

	for (vector<FDBPackage*>::iterator ipack=packages.begin();ipack!=packages.end();++ipack)
	{
		FDBPackage* pack = *ipack;

		size_t fc = pack->GetFileCount();
		for (size_t i = 0;i<fc;++i)
		{
			wxString name=wxString::FromUTF8(pack->GetFileName(i));
			if (name.StartsWith(src_dir)) ++count;
		}
	}

	return count;
}

bool FDB_Collection::ExtractMultipleFiles(const wxString& src_dir, const wxString& dest_dir, const wxArrayString& files)
{
	int total = CalcFileCount(src_dir, files);

	Export_DLG dlg(NULL);
	dlg.SetMax(total);
	dlg.Show();

	bool res=false;

	for (vector<FDBPackage*>::iterator ipack=packages.begin();ipack!=packages.end();++ipack)
	{
		FDBPackage* pack = *ipack;

		size_t count = pack->GetFileCount();
		for (size_t i = 0;i<count;++i)
		{
            wxString full_name=wxString::FromUTF8(pack->GetFileName(i));
			wxString file_name;
			if (full_name.StartsWith(src_dir,&file_name))
			{
				if ( (files.size()==0) || (files.Index(file_name)!= wxNOT_FOUND) )
				{
					wxFileName dest(dest_dir + wxFileName::GetPathSeparator());
					if (files.size()>0) dest.AppendDir(file_name);
					else                dest.AppendDir(full_name);

					dest.Mkdir(511,wxPATH_MKDIR_FULL);
					res |= pack->ExtractFile(i,dest.GetFullPath().mb_str(), FDBPackage::EX_NONE);
					dlg.Step(full_name);

					if (dlg.IsCanceled)
					{
						return false;
					}
				}
			}
		}
	}

	return res;
}
