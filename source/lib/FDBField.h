#pragma once


class FDB_DBField
{
	public:
		enum field_type
		{
			F_BOOL,
			F_INT,
			F_DWORD,
			F_FLOAT,
			F_BYTE,
			F_STRING,
			F_ARRAY,
			F_WORD,

			NOF_FIELD_TYPE
		};

		std::string name;
		field_type type;
		DWORD position;
		DWORD size;
		std::string comment;

	public:

		FDB_DBField();
		FDB_DBField(const std::string& _name, field_type _type, DWORD _pos, DWORD _size);

		bool HasComment() const;
		bool HasCustomComment() const;
		bool IsConstCommment() const;
		void SetCommentConst(const std::string&);
		void SetComment(const std::string&);

		bool operator<(const FDB_DBField& j) const;
};

typedef std::vector<FDB_DBField> field_list;
extern field_list empty_field_list; 

