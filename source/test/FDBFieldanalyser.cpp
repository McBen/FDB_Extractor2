#include <Windows.h>
#include "FDBPackage.h"

#include "gtest/gtest.h"
#include "FDBFieldAnalyzer.h"

class FDBFieldAnalyzerTest : public ::testing::Test
{
	public:
        void Test_Worldmaptable()
        {
            field_list fields;
            GetFieldDefinition("data\\worldmaptable.db", fields);

            AssertFieldType(fields[0], FDB_DBField::F_DWORD,0);
            AssertFieldType(fields[1], FDB_DBField::F_STRING,4);
            AssertFieldType(fields[2], FDB_DBField::F_STRING,68);
            AssertFieldType(fields[3], FDB_DBField::F_DWORD,132);
            AssertFieldType(fields[4], FDB_DBField::F_DWORD,136);
            AssertFieldType(fields[5], FDB_DBField::F_DWORD,140);
            AssertFieldType(fields[6], FDB_DBField::F_INT,144);
        }


        void Test_exptable()
        {
            field_list fields;
            GetFieldDefinition("data\\exptable.db", fields);

            // Table has no Header/Name info
            AssertFieldType(fields[0], FDB_DBField::F_DWORD,0);   // guid
            AssertFieldType(fields[1], FDB_DBField::F_DWORD,4);   // level
            // .... more
            AssertFieldType(fields[6], FDB_DBField::F_DWORD,24);   // quest_xp_base
            AssertFieldType(fields[7], FDB_DBField::F_DWORD,28);   // quest_money_base
            // .... more
        }

        void Test_runeobject()
        {
            field_list fields;
            GetFieldDefinition("data\\runeobject.db", fields);

            AssertFieldType(fields, "runegroup", FDB_DBField::F_DWORD,368);
        }

        void Test_questnpcobject()
        {
            field_list fields;
            GetFieldDefinition("data\\questnpcobject.db", fields);

            AssertFieldType(fields, "guid", FDB_DBField::F_DWORD,0);
            // .... more
            AssertFieldType(fields, "zoneid", FDB_DBField::F_INT,1124);
            // .... more
            AssertFieldType(fields, "zoneid_bk", FDB_DBField::F_INT,1140);
            // .... more
        }

        void Test_questdetailobject()
        {
            field_list fields;
            GetFieldDefinition("data\\questdetailobject.db", fields);

            AssertFieldType(fields, "guid", FDB_DBField::F_DWORD,0);

            AssertFieldType(fields, "icheck_lv", FDB_DBField::F_DWORD,160);
            AssertFieldType(fields, "icheck_lowerlv", FDB_DBField::F_DWORD,164);

            AssertFieldType(fields, "icheck_objid1", FDB_DBField::F_DWORD,200);
            AssertFieldType(fields, "icheck_objid2", FDB_DBField::F_DWORD,204);
            AssertFieldType(fields, "icheck_objid3", FDB_DBField::F_DWORD,208);
            AssertFieldType(fields, "icheck_objid4", FDB_DBField::F_DWORD,212);
            AssertFieldType(fields, "icheck_objid5", FDB_DBField::F_DWORD,216);

            AssertFieldType(fields, "icheck_loop", FDB_DBField::F_BOOL,240);

            AssertFieldType(fields, "irequest_itemid1", FDB_DBField::F_DWORD,340);
            AssertFieldType(fields, "irequest_itemid2", FDB_DBField::F_DWORD,344);
            AssertFieldType(fields, "irequest_itemid3", FDB_DBField::F_DWORD,348);
            AssertFieldType(fields, "irequest_itemid4", FDB_DBField::F_DWORD,352);
            AssertFieldType(fields, "irequest_itemid5", FDB_DBField::F_DWORD,356);
            AssertFieldType(fields, "irequest_itemid6", FDB_DBField::F_DWORD,360);
            AssertFieldType(fields, "irequest_itemid7", FDB_DBField::F_DWORD,364);
            AssertFieldType(fields, "irequest_itemid8", FDB_DBField::F_DWORD,368);
            AssertFieldType(fields, "irequest_itemid9", FDB_DBField::F_DWORD,372);
            AssertFieldType(fields, "irequest_itemid10", FDB_DBField::F_DWORD,376);
            AssertFieldType(fields, "irequest_itemval1", FDB_DBField::F_DWORD,380);
            AssertFieldType(fields, "irequest_itemval2", FDB_DBField::F_DWORD,384);
            AssertFieldType(fields, "irequest_itemval3", FDB_DBField::F_DWORD,388);
            AssertFieldType(fields, "irequest_itemval4", FDB_DBField::F_DWORD,392);
            AssertFieldType(fields, "irequest_itemval5", FDB_DBField::F_DWORD,396);
            AssertFieldType(fields, "irequest_itemval6", FDB_DBField::F_DWORD,400);
            AssertFieldType(fields, "irequest_itemval7", FDB_DBField::F_DWORD,404);
            AssertFieldType(fields, "irequest_itemval8", FDB_DBField::F_DWORD,408);
            //AssertFieldType(fields, "irequest_itemval9", FDB_DBField::F_DWORD,412);
            //AssertFieldType(fields, "irequest_itemval10", FDB_DBField::F_DWORD,416);
            AssertFieldType(fields, "irequest_killid1", FDB_DBField::F_DWORD,420);
            AssertFieldType(fields, "irequest_killid2", FDB_DBField::F_DWORD,424);
            AssertFieldType(fields, "irequest_killid3", FDB_DBField::F_DWORD,428);
            AssertFieldType(fields, "irequest_killid4", FDB_DBField::F_DWORD,432);
            AssertFieldType(fields, "irequest_killid5", FDB_DBField::F_DWORD,436);
            AssertFieldType(fields, "irequest_killval1", FDB_DBField::F_DWORD,440);
            AssertFieldType(fields, "irequest_killval2", FDB_DBField::F_DWORD,444);
            AssertFieldType(fields, "irequest_killval3", FDB_DBField::F_DWORD,448);
            AssertFieldType(fields, "irequest_killval4", FDB_DBField::F_DWORD,452);
            AssertFieldType(fields, "irequest_killval5", FDB_DBField::F_DWORD,456);

            AssertFieldType(fields, "irequest_questid", FDB_DBField::F_DWORD,468);

            AssertFieldType(fields, "iprocess_killtogetitem_objid1", FDB_DBField::F_DWORD,632);
            AssertFieldType(fields, "iprocess_killtogetitem_objid2", FDB_DBField::F_DWORD,636);
            AssertFieldType(fields, "iprocess_killtogetitem_objid3", FDB_DBField::F_DWORD,640);
            AssertFieldType(fields, "iprocess_killtogetitem_objid4", FDB_DBField::F_DWORD,644);
            AssertFieldType(fields, "iprocess_killtogetitem_objid5", FDB_DBField::F_DWORD,648);
            AssertFieldType(fields, "iprocess_killtogetitem_objid6", FDB_DBField::F_DWORD,652);
            AssertFieldType(fields, "iprocess_killtogetitem_objid7", FDB_DBField::F_DWORD,656);
            AssertFieldType(fields, "iprocess_killtogetitem_objid8", FDB_DBField::F_DWORD,660);
            AssertFieldType(fields, "iprocess_killtogetitem_objid9", FDB_DBField::F_DWORD,664);
            AssertFieldType(fields, "iprocess_killtogetitem_objid10", FDB_DBField::F_DWORD,668);

            AssertFieldType(fields, "iprocess_killtogetitem_getitem1", FDB_DBField::F_DWORD,712);
            AssertFieldType(fields, "iprocess_killtogetitem_getitem2", FDB_DBField::F_DWORD,716);
            AssertFieldType(fields, "iprocess_killtogetitem_getitem3", FDB_DBField::F_DWORD,720);
            AssertFieldType(fields, "iprocess_killtogetitem_getitem4", FDB_DBField::F_DWORD,724);
            AssertFieldType(fields, "iprocess_killtogetitem_getitem5", FDB_DBField::F_DWORD,728);
            AssertFieldType(fields, "iprocess_killtogetitem_getitem6", FDB_DBField::F_DWORD,732);
            AssertFieldType(fields, "iprocess_killtogetitem_getitem7", FDB_DBField::F_DWORD,736);
            AssertFieldType(fields, "iprocess_killtogetitem_getitem8", FDB_DBField::F_DWORD,740);
            AssertFieldType(fields, "iprocess_killtogetitem_getitem9", FDB_DBField::F_DWORD,744);
            AssertFieldType(fields, "iprocess_killtogetitem_getitem10", FDB_DBField::F_DWORD,748);

            AssertFieldType(fields, "ireward_itemid1", FDB_DBField::F_DWORD,832);
            AssertFieldType(fields, "ireward_itemid2", FDB_DBField::F_DWORD,836);
            AssertFieldType(fields, "ireward_itemid3", FDB_DBField::F_DWORD,840);
            AssertFieldType(fields, "ireward_itemid4", FDB_DBField::F_DWORD,844);
            AssertFieldType(fields, "ireward_itemid5", FDB_DBField::F_DWORD,848);
            AssertFieldType(fields, "ireward_itemval1", FDB_DBField::F_DWORD,852);
            AssertFieldType(fields, "ireward_itemval2", FDB_DBField::F_DWORD,856);
            AssertFieldType(fields, "ireward_itemval3", FDB_DBField::F_DWORD,860);
            AssertFieldType(fields, "ireward_itemval4", FDB_DBField::F_DWORD,864);
            // AssertFieldType(fields, "ireward_itemval5", FDB_DBField::F_DWORD,868);

            AssertFieldType(fields, "ireward_money", FDB_DBField::F_DWORD,892);
            AssertFieldType(fields, "ireward_exp", FDB_DBField::F_DWORD,896);
            // .... more
        }

        void Test_imageobject()
        {
            field_list fields;
            GetFieldDefinition("data\\imageobject.db", fields);

            AssertFieldType(fields, "guid", FDB_DBField::F_DWORD,0);
            AssertFieldType(fields[1], FDB_DBField::F_DWORD,4);
            AssertFieldType(fields, "outputlanguage", FDB_DBField::F_INT,8);
            AssertFieldType(fields, "name", FDB_DBField::F_DWORD,12); // unused
            AssertFieldType(fields, "name_plural", FDB_DBField::F_DWORD,16); // unused
            AssertFieldType(fields, "imageid", FDB_DBField::F_DWORD,20);
            AssertFieldType(fields, "note", FDB_DBField::F_DWORD,32);
            AssertFieldType(fields, "shortnote", FDB_DBField::F_DWORD,36);
            AssertFieldType(fields, "imagetype", FDB_DBField::F_DWORD,144);
            AssertFieldType(fields, "actfield", FDB_DBField::F_STRING,148);
            AssertFieldType(fields,"actworld", FDB_DBField::F_STRING,276);
            AssertFieldType(fields,"modelsize", FDB_DBField::F_DWORD,404);
            // .... more
        }

        void Test_armorobject()
        {
            field_list fields;
            GetFieldDefinition("data\\armorobject.db", fields);

            AssertFieldType(fields, "guid", FDB_DBField::F_DWORD,0);
            AssertFieldType(fields[1], FDB_DBField::F_DWORD,4);
            AssertFieldType(fields, "outputlanguage", FDB_DBField::F_INT,8);
            AssertFieldType(fields, "name", FDB_DBField::F_DWORD,12);
            AssertFieldType(fields, "name_plural", FDB_DBField::F_DWORD,16);
            AssertFieldType(fields, "imageid", FDB_DBField::F_DWORD,20);
            AssertFieldType(fields[6], FDB_DBField::F_DWORD,24);
            AssertFieldType(fields[7], FDB_DBField::F_BOOL,28);
            AssertFieldType(fields, "note", FDB_DBField::F_DWORD,32);
            AssertFieldType(fields, "shortnote", FDB_DBField::F_DWORD,36);
            AssertFieldType(fields, "mode", FDB_DBField::F_DWORD,40);
            AssertFieldType(fields, "pricestype", FDB_DBField::F_DWORD,44);
            AssertFieldType(fields, "selltype", FDB_DBField::F_BOOL,48);
            AssertFieldType(fields, "cost", FDB_DBField::F_DWORD,52);    // floor(v/10) = gold
            // AssertFieldType(fields[14], FDB_DBField::F_BOOL,56);
            AssertFieldType(fields, "rare", FDB_DBField::F_DWORD,64);

            // .... more
            AssertFieldType(fields, "limitlv", FDB_DBField::F_DWORD,88); // min required level
            // .... more
            AssertFieldType(fields, "armorpos", FDB_DBField::F_DWORD,180); // where the armor must be placed
            // .... more
            AssertFieldType(fields, "suitid", FDB_DBField::F_DWORD,392);
            // .... more
        }

        void Test_weaponobject()
        {
            field_list fields;
            GetFieldDefinition("data\\weaponobject.db", fields);

            AssertFieldType(fields, "guid", FDB_DBField::F_DWORD,0);
            AssertFieldType(fields[1], FDB_DBField::F_DWORD,4);
            AssertFieldType(fields, "outputlanguage", FDB_DBField::F_INT,8);
            AssertFieldType(fields, "name", FDB_DBField::F_DWORD,12); // unused
            AssertFieldType(fields, "name_plural", FDB_DBField::F_DWORD,16); // unused
            AssertFieldType(fields, "imageid", FDB_DBField::F_DWORD,20);
            AssertFieldType(fields[6], FDB_DBField::F_DWORD,28);
            AssertFieldType(fields, "note", FDB_DBField::F_DWORD,32);
            AssertFieldType(fields, "shortnote", FDB_DBField::F_DWORD,36);
            // .... more
        }

/////////////////////////////////////////////////////////
// all what follows: I only test the values which I use in some tool scripts


/////////////////////////////////////////////////////////

        void AssertFieldType(const FDB_DBField& field, FDB_DBField::field_type ft, DWORD position)
        {
            EXPECT_EQ(field.type, ft);
            EXPECT_EQ(field.position, position);
        }

        void AssertFieldType(field_list& fields, const std::string& name, FDB_DBField::field_type ft, DWORD position)
        {
            for (field_list::const_iterator field=fields.cbegin();field!=fields.cend();++field)
            {
                if (field->name == name)
                {
                    EXPECT_EQ(field->type, ft);
                    EXPECT_EQ(field->position, position);
                    return;
                }
            }

            FAIL() << "field not found: "<< name.c_str();
        }

		void GetFieldDefinition(const char* filename,  field_list& fields)
		{
			FDBPackage datafdb("../resources/test_data/data.fdb");
			size_t index = datafdb.FindFile(filename);
			ASSERT_NE(index, (size_t)-1);

			FDBPackage::file_info s_info;
			BYTE* data;
			size_t data_size;

			int res = datafdb.GetFileData(index, data, data_size, &s_info);
			ASSERT_EQ(res, 0);

			FDBFieldAnalyzer analyse(s_info, data);
			analyse.Do(fields);
		}
};


TEST_F(FDBFieldAnalyzerTest, FDBFieldAnalyzerTest) {
	Test_Worldmaptable();
	Test_armorobject();
	Test_exptable();
	Test_imageobject();
	Test_questdetailobject();
	Test_questnpcobject();
	Test_weaponobject();
	Test_runeobject();
}


