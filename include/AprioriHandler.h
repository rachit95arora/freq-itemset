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

    void generateFrequentItemSet();
    void generateCandidates(int index);
    void initPass();
    void getFrequentFromCandidate(int index);
};

void AprioriHandler::generateFrequentItemSet()
{
    initPass();
    getFrequentFromCandidate(0);
    for(int k=1; m_freq_lists[k-1].size()>0; k++)
    {
        generateCandidates(k);
        ReadTransaction* read_helper = new ReadTransaction(m_input_filename);
        vector<int> next_transaction;
        while(read_helper->nextTransaction(next_transaction) > 0)
        {
            for(candidate current : m_cand_lists[index])
            {
                bool current_present = true;
                vector<int> candidate_vector = current.first;
                for(int elem : candidate_vector)
                {
                    bool found = false;
                    for(int i = 0; i<next_transaction.size(); i++)
                    {
                        if(next_transaction[i] == elem)
                        {
                            found = true;
                            break;
                        }
                    }
                    if(!found)
                    {
                        current_present = false;
                    }
                }
                if(current_present)
                {
                    ++current.second;
                }
            }
            next_transaction.clear();
        }
        delete read_helper;

        getFrequentFromCandidate(k);
    }

}

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
    delete read_helper;
}

void AprioriHandler::generateCandidates(int index)
{
    if( index + 1 > m_cand_lists.size())
    {
        m_cand_lists.resize(index+1);
    }
    for(int i = 0; i<m_freq_lists[index-1].size(); i++)
    {
        for(int j = i+1; j<m_freq_lists[index-1].size(); j++)
        {
            vector<int> next_candidate_vector;

            // Join F1, F2
            int left = 0, right = 0, merged = 0;
            vector<int> left_f = m_freq_lists[index-1][i].first;
            vector<int> right_f = m_freq_lists[index-1][j].first;
            for(int merged = 0; merged<=index; merged++) // WARNING: Makes implicit sorted assumption
            {
                if(left_f[left] == right_f[right]) // Element match
                {
                    next_candidate_vector.push_back(left_f[left]);
                    ++left;
                    ++right;
                }
                else if(left_f[left] < right_f[right])
                {
                    next_candidate_vector.push_back(left_f[left]);
                    ++left;
                }
                else
                {
                    next_candidate_vector.push_back(right_f[right]);
                    ++right;
                }
            }
            if((left<index) || (right<index)) // Multiple mismatches, continue
            {
                continue;
            }

            // Prune next_candidate_vector if a subset not frequent
            bool prune = false;
            for(int i = 0; i<=index; i++) //Choose element to skip
            {
                prune = true;
                for(int j = 0; j<m_freq_lists[index-1].size(); j++)
                {
                    vector<int> possible_subset = m_freq_lists[index-1][j].first;
                    int k=0;
                    while(k<index)
                    {
                        if(((k<index) && (possible_subset[k] != next_candidate_vector[k]))
                            || ((k>=index) && (possible_subset[k] != next_candidate_vector[k+1])))
                            continue;
                    }
                    prune = false;
                    break;
                }
                if(prune)
                    break;
            }
            if(prune)
            {
                continue;
            }

            // next_candidate_vector fit for addition
            m_cand_lists[index].emplace_back(next_candidate_vector, 0);
        }
    }
}

inline void AprioriHandler::getFrequentFromCandidate(int index)
{
    if (index + 1 > m_cand_lists.size())
    {
        cerr<<"[ERROR] AprioriHandler::getFrequentFromCandidate() : Candidate List shorter than index!\n";
    }
    else if( index + 1 > m_freq_lists.size())
    {
        m_freq_lists.resize(index+1);
    }
    for(candidate next: m_cand_lists[index])
    {
        if(next.second/((double) m_total_transactions) >= m_min_support) // Qualifies as frequent
        {
            m_freq_lists[index].push_back(next);
        }
    }
}

#endif
