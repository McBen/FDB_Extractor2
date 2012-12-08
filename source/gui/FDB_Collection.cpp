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

bool FDB_Collection::Open(const char* filename_pattern)
{
	bool res = false;
	wxString f = wxFindFirstFile(filename_pattern);
	while ( !f.empty() )
	{
		FDBPackage* new_fdb=new FDBPackage(f.c_str());
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

	string dir = GetROMInstallDir();
	if (wxDirExists(dir))
		return Open((dir + "fdb/*.fdb").c_str());
	else
		return false;
}

void FDB_Collection::Close()
{
	for (vector<FDBPackage*>::iterator pack=packages.begin();pack!=packages.end();++pack) delete(*pack);
	packages.clear();
	directories.clear();
}

void FDB_Collection::UpdateDirectories()
{
	for (vector<FDBPackage*>::iterator pack=packages.begin();pack!=packages.end();++pack)
	{
		size_t count = (*pack)->GetFileCount();
		for (size_t i = 0;i<count;++i)
		{
			wxString path;
			wxFileName::SplitPath((*pack)->GetFileName(i),NULL,&path,NULL,NULL);
			directories.insert(path.ToStdString());
		}
	}
}

void FDB_Collection::GetSubDirectories(const char* base_path, vector<string>& names, vector<bool>& has_childs)
{
	int len = strlen(base_path);
	names.clear();
	has_childs.clear();

	for (vector<FDBPackage*>::iterator ipack=packages.begin();ipack!=packages.end();++ipack)
	{
		FDBPackage* pack = *ipack;

		size_t count = pack->GetFileCount();
		for (size_t i = 0;i<count;++i)
		{
			const char* name = pack->GetFileName(i);
			if (strncmp(base_path,name,len)==0)
			{
				const char* p1 = strchr(name+len,'\\');
				if (p1) 
				{
					string cur_dir(name+len, p1-(name+len));

					const char* p2 = strchr(p1+1,'\\');
					vector<string>::iterator pos = std::find(names.begin(), names.end(), cur_dir);
					if (pos ==names.end())
					{
						names.push_back(cur_dir);
						has_childs.push_back(p2!=NULL);
					}
					else if (p2!=NULL) 
					{
						has_childs[std::distance(names.begin(),pos)]=true;
					}
				}
			}
		}
	}
}

void FDB_Collection::GetFileInfos(const char* base_path, std::vector<FDBPackage::file_info>& infos)
{
	int len = strlen(base_path);
	infos.clear();

	for (vector<FDBPackage*>::iterator ipack=packages.begin();ipack!=packages.end();++ipack)
	{
		FDBPackage* pack = *ipack;

		size_t count = pack->GetFileCount();
		for (size_t i = 0;i<count;++i)
		{
			const char* name = pack->GetFileName(i);
			if (strncmp(base_path,name,len)==0)
			{
				if ( strchr(name+len,'\\')==NULL) 
				{
					FDBPackage::file_info info;
					pack->GetFileInfo(i,info);
					infos.push_back(info);
				}
			}
		}
	}
}

string FDB_Collection::ExtractFile(const char* fname, const char* destname)
{
	for (vector<FDBPackage*>::iterator ipack=packages.begin();ipack!=packages.end();++ipack)
	{
		size_t idx = (*ipack)->FindFile(fname);
		if (idx!=-1)
		{
			FDBFile* file = (*ipack)->GetFile(idx);
			if (file)
			{
				FDBPackage::e_export_format e= file->DefaultFormat();
				bool res=  file->WriteToFile(destname, e);
				std::string res_name = file->GetTargetName(destname, e);
				delete(file);

				if (res)	return res_name;
			}
		}
	}

	return "";
}

int FDB_Collection::CalcFileCount(const char* src_dir, const wxArrayString& files)
{
	if (files.size()>0) return files.size();

	size_t count=0;
	int len = strlen(src_dir);

	for (vector<FDBPackage*>::iterator ipack=packages.begin();ipack!=packages.end();++ipack)
	{
		FDBPackage* pack = *ipack;

		size_t fc = pack->GetFileCount();
		for (size_t i = 0;i<fc;++i)
		{
			const char* name = pack->GetFileName(i);
			if (strncmp(src_dir,name,len)==0) ++count;
		}
	}

	return count;
}

bool FDB_Collection::ExtractMultipleFiles(const char* src_dir, const char* dest_dir, const wxArrayString& files)
{
	int total = CalcFileCount(src_dir, files);
	int done=0;

	Export_DLG dlg(NULL);
	dlg.SetMax(total);
	dlg.Show();

	bool res=false;
	int len = strlen(src_dir);

	for (vector<FDBPackage*>::iterator ipack=packages.begin();ipack!=packages.end();++ipack)
	{
		FDBPackage* pack = *ipack;

		size_t count = pack->GetFileCount();
		for (size_t i = 0;i<count;++i)
		{
			const char* name = pack->GetFileName(i);
			if (strncmp(src_dir,name,len)==0)
			{
				if ( (files.size()==0) || (files.Index(name+len)!= wxNOT_FOUND) )
				{
					wxFileName dest(string(dest_dir)+string("/")+string(name));
					if (files.size()>0) dest = string(dest_dir)+string("/")+string(name+len);

					wxDir::Make(dest.GetPath(),511,wxPATH_MKDIR_FULL);
					res |= pack->ExtractFile(i,dest.GetFullPath().c_str(), FDBPackage::EX_NONE);
					dlg.Step(name);

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
