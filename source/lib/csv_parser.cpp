
#include "csv_parser.hpp"
using namespace std;

#define DELIMITER ';'
#define TEXT_DELIMITER '"'

void csvline_populate(vector<string> &record, const string& line);

bool csvparse(ifstream& in, vector<string>& row)
{
    string line;
	if (!getline(in, line) || in.fail()) return false;

    csvline_populate(row, line);
    return true;
}

void csvline_populate(vector<string> &record, const string& line)
{
    int linepos=0;
    int inquotes=false;
    char c;
    int linemax=line.length();
    string curstring;
    record.clear();
       
    while(line[linepos]!=0 && linepos < linemax)
    {
       
        c = line[linepos];
       
        if (!inquotes && curstring.length()==0 && c==TEXT_DELIMITER)
        {
            //beginquotechar
            inquotes=true;
        }
        else if (inquotes && c=='"')
        {
            //quotechar
            if ( (linepos+1 <linemax) && (line[linepos+1]==TEXT_DELIMITER) )
            {
                //encountered 2 double quotes in a row (resolves to 1 double quote)
                curstring.push_back(c);
                linepos++;
            }
            else
            {
                //endquotechar
                inquotes=false;
            }
        }
        else if (!inquotes && c==DELIMITER)
        {
            //end of field
            record.push_back( curstring );
            curstring="";
        }
        else if (!inquotes && (c=='\r' || c=='\n') )
        {
            record.push_back( curstring );
            return;
        }
        else
        {
            curstring.push_back(c);
        }
        linepos++;
    }
    record.push_back( curstring );
    return;
}