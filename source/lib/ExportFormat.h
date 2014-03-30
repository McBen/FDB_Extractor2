#pragma once

#include "FDBFile.h"
#include "FDBFieldManager.h"

class DBExport
{
	protected:
		FILE* outf;
		std::string filename;
		std::string realfilename;
		bool not_first_value;

	public:
		DBExport(const std::string& _filename);
		virtual ~DBExport();
		void Close();

		virtual void TableStart(const char* table_name);
		virtual void TableField(const std::string& name, uint32_t position, FDB_DBField::field_type type, size_t size ) {}
		virtual void TableEnd() {}
		virtual void EntryHeader() {}
		virtual void EntryStart() {}
		virtual void EntryField(FDB_DBField::field_type type, void*data) {}
		virtual void EntryEnd() {}
		virtual void EntryFooter() {}
};


class DBExport_CSV : public DBExport
{
	public:
		DBExport_CSV(const std::string& _filename);
		void TableField(const std::string& name, uint32_t position, FDB_DBField::field_type type, size_t size );
		void TableEnd();
		void EntryStart();
		void EntryField(FDB_DBField::field_type type, void*data);
		void EntryEnd();
};

class DBExport_Sqlite3 : public DBExport
{
	private:
		const char* table_name;

	public:
		DBExport_Sqlite3(const std::string& _filename);
		void TableStart(const char* _table_name);
		void TableField(const std::string& name, uint32_t position, FDB_DBField::field_type type, size_t size );
		void TableEnd();
		void EntryStart();
		void EntryField(FDB_DBField::field_type type, void*data);
		void EntryEnd();
};

class DBExport_MySQL: public DBExport
{
	private:
		const char* table_name;
		int nof_export_lines;

	public:
		DBExport_MySQL(const std::string& _filename);
		void TableStart(const char* _table_name);
		void TableField(const std::string& name, uint32_t position, FDB_DBField::field_type type, size_t size );
		void TableEnd();
		void EntryHeader();
		void EntryStart();
		void EntryField(FDB_DBField::field_type type, void*data);
		void EntryEnd();
		void EntryFooter();
};
