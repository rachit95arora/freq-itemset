#ifndef APRIORI_HANDLER_H
#define APRIORI_HANDLER_H

#include <string>
#include <iostream>
#include <unordered_set>
using namespace std;

#include "ReadTransaction.h"
typedef pair< vector<int>, int> candidate;
class AprioriHandler {
private:
    string m_input_filename, m_output_filename;
    double m_min_support;
    int m_total_transactions;
    vector<vector<candidate>> m_cand_lists, m_freq_lists;
public:

    AprioriHandler(string input_file, string output_file, double min_support)
    {
        m_input_filename = input_file;
        m_output_filename = output_file;
        m_min_support = min_support;
        m_total_transactions = 0;
        m_cand_lists.clear();
        m_freq_lists.clear();
    }

    void initPass();
    void getFrequentFromCandidate(int index);
};

inline void AprioriHandler::initPass()
{
    ReadTransaction* read_helper = new ReadTransaction(m_input_filename);
    m_cand_lists.resize(1);
    unordered_set<int> seen_integers;
    while(true)
    {
        vector<int> next_transaction;
        if (read_helper->nextTransaction(next_transaction) > 0)
        {
            ++m_total_transactions;
            for(int element : next_transaction)
            {
                if (seen_integers.find(element) == seen_integers.end()) // Add a new candidate
                {
                    seen_integers.insert(element);
                    vector<int> new_candidate_vector = { element };
                    m_cand_lists[0].emplace_back(new_candidate_vector, 0);
                }
            }
        }
        else
            break;
    }
}

inline void AprioriHandler::getFrequentFromCandidate(int index)
{
    for(candidate next: m_cand_lists[index])
    {
        if(next.second/((double) m_total_transactions) >= m_min_support) // Qualifies as frequent
        {
            m_freq_lists[index].push_back(next);
        }
    }
}

#endif
