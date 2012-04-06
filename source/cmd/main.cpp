#include <windows.h>

#include "commandline.h"

#define BOOST_FILESYSTEM_NO_DEPRECATED
#include <boost/filesystem.hpp>

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include "FDB_LIB.h"


CommandLine cmdline;


using namespace std;

bool Process();
bool List(bool include_crc);

int main( int argc, const char* argv[] )
{
     try
    {
        if (!cmdline.Parse(argc, argv))  return -1;
    }
    catch(...)
    {
        return -1;
    }    

    
    bool okay = false;

	if (cmdline.list_only || cmdline.list_only_with_crc) 
	    okay = List(cmdline.list_only_with_crc);
	else
		okay = Process();

    ReleaseRedux();


    if (!okay)
    {
        cerr << "nothing extracted\n";
        cmdline.HelpOut();
        return -1;
    }

    return 0;
}

/////////////////////////////////////
bool ProcessDir(string filename,  const boost::regex& filter);
bool ProcessFile(boost::filesystem::path filename,  const boost::regex& filter);

bool Process()
{
    bool had_error = false;

    for (vector<string>::iterator f = cmdline.fbd_files.begin(); f!=cmdline.fbd_files.end();++f )
        had_error |= !ProcessDir(*f, cmdline.filter);

	return !had_error;
}

bool ProcessDir(string filename, const boost::regex& filter)
{
    boost::filesystem::path basepath(filename);
    basepath.remove_filename();


    WIN32_FIND_DATA FindFileData;
    HANDLE hFind;

    hFind = FindFirstFile(filename.c_str(), &FindFileData);
    if (hFind == INVALID_HANDLE_VALUE)  return false;

    bool something_extracted=false;
    do 
    {
        printf("processing %s\n",FindFileData.cFileName);
        something_extracted |= ProcessFile(basepath / FindFileData.cFileName, filter);
    } while (FindNextFile(hFind,&FindFileData));

    FindClose(hFind);

    return something_extracted;
}

bool ProcessFile(boost::filesystem::path filename, const boost::regex& filter)
{
    bool extracted = false;
    FDBPackage fdb( filename.generic_string().c_str() );

    FDBPackage::e_export_format ex_format=FDBPackage::EX_NONE;
    if (cmdline.raw_data)   ex_format = (FDBPackage::e_export_format)(ex_format + FDBPackage::EX_RAW);
    if (cmdline.db_sql_out) ex_format = (FDBPackage::e_export_format)(ex_format + FDBPackage::EX_SQLITE3);
    if (cmdline.lua_out)    ex_format = (FDBPackage::e_export_format)(ex_format + FDBPackage::EX_LUA);
    if (cmdline.csv_out)    ex_format = (FDBPackage::e_export_format)(ex_format + FDBPackage::EX_CSV);
    

    for (size_t id=0;id<fdb.GetFileCount();++id)
    {
        const char*fname = fdb.GetFileName(id);
        
        if (boost::regex_search(fname, filter, boost::match_any)) 
        { 
            boost::filesystem::path output(cmdline.output_directory);
            output /= fname; // TODO: get real name (in case of DB it may differ)

            boost::filesystem::create_directories(output.parent_path());


            if (!cmdline.overwrite)
            {
                if (boost::filesystem::exists(output))
                {
                    if (cmdline.neveroverwrite) continue;
                    char command=0;
                    while (string("ynas").find(command)== string::npos)
                    {
                        cout << fname << " exists.\n Overwrite? [y]es,[n]o,[a]lways yes,[s]kip all\n";
                        command=_getch();
                    }

                    cout <<command<<"\n";
                    if (command=='n') continue;
                    if (command=='s') { cmdline.neveroverwrite=true; continue; }
                    if (command=='a') cmdline.overwrite=true;
                }
            }

            bool res = fdb.ExtractFile(id, output.generic_string().c_str(), ex_format );
            extracted |= res;

            if (cmdline.verbose)
            {
                if (!res) cout << fname << "-ERROR\n";
                else      cout << fname <<"\n";
            }

        }
    }

    return extracted;
}

/////////////////////////////////////
bool ListDir(string filename,  const boost::regex& filter, bool include_crc);
bool ListFile(boost::filesystem::path filename,  const boost::regex& filter, bool include_crc);

bool List(bool include_crc)
{
    bool had_error = false;

    for (vector<string>::iterator f = cmdline.fbd_files.begin(); f!=cmdline.fbd_files.end();++f )
        had_error |= !ListDir(*f, cmdline.filter, include_crc);

	return !had_error;
}

bool ListDir(string filename, const boost::regex& filter, bool include_crc)
{
    boost::filesystem::path basepath(filename);
    basepath.remove_filename();


    WIN32_FIND_DATA FindFileData;
    HANDLE hFind;

    hFind = FindFirstFile(filename.c_str(), &FindFileData);
    if (hFind == INVALID_HANDLE_VALUE)  return false;

    bool something_extracted=false;
    do 
    {
        something_extracted |= ListFile(basepath / FindFileData.cFileName, filter, include_crc);
    } while (FindNextFile(hFind,&FindFileData));

    FindClose(hFind);

    return something_extracted;
}

bool ListFile(boost::filesystem::path filename, const boost::regex& filter, bool include_crc)
{
    bool extracted = false;
    FDBPackage fdb( filename.generic_string().c_str() );

    for (size_t id=0;id<fdb.GetFileCount();++id)
    {
        const char*fname = fdb.GetFileName(id);
        
        if (boost::regex_search(fname, filter, boost::match_any)) 
        { 
            boost::filesystem::path output(cmdline.output_directory);
            output /= fname; // TODO: get real name (in case of DB it may differ)


			if (include_crc)
			{
				DWORD crc32 = fdb.CalcCRC32(id);
				cout << output.generic_string() <<"\t"<< setw(8) << setfill('0') << hex << crc32 <<"\n";
			}
			else
			{
				cout << output <<"\n";
			}
            extracted = true;
        }
    }

    return extracted;
}

