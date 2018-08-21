#ifndef READ_TRANSACTION_H
#define READ_TRANSACTION_H
#include <iostream>
#include <fstream>
#include <string>

using namespace std;
class ReadTransaction
{
private:
    ifstream m_db;

public:
    ReadTransaction(string filepath)
    {
        m_db.open(filepath);
        if (!m_db)
        {
            cerr<< "[ERROR] ReadTransaction() : Input file " + filepath + " could not be opened!\n";
            exit(1);
        }
    }

    ~ReadTransaction()
    {
        m_db.close();
    }

    size_t nextTransaction(vector<int>& next)
    {
        string next_line;
        if(getline(m_db, next_line).eof()) // Read is complete
        {
            return 0;
        }
        else // next_line should be processed as vector<int> next
        {
            size_t last_position = 0;
            size_t current_position = next_line.find(' ');
            next.clear(); // Clear the parsed vector before processing

            while (current_position != string::npos)
            {
                next.push_back(stoi(next_line.substr(last_position, current_position-last_position)));
                last_position = current_position + 1;
                current_position = next_line.find(' ', last_position);
            }

            next.push_back(stoi(next_line.substr(
                            last_position,
                            (min(current_position, next_line.size())-last_position + 1)
                            )));
            return next_line.size();
        }
    }
};

#endif
