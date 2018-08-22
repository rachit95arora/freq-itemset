#ifndef FP_HANDLER_H
#define FP_HANDLER_H

#include <algorithm>
#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <list>
using namespace std;

#include "ReadTransaction.h"
struct FPNode
{
    int m_id;
    int m_count;
    FPNode* m_parent;
    list<FPNode*> m_children;
    FPNode* m_auxillary;
    FPNode():
        m_parent(nullptr),
        m_auxillary(nullptr),
        m_id(0),
        m_count(0)
    {
        m_children.clear();
    }
};

struct FPTree
{
    FPNode* m_root;
    unordered_map<int,list<FPNode*> > m_tree_index;
    FPTree()
    {
        m_root = new FPNode();
        m_root->m_count = -1; // Signifies null node
        m_tree_index.clear();
    }
    ~FPTree() // FPTree destruction will auto destruct all FPNodes in the tree
    {
        delete m_root;
        for (auto el = m_tree_index.begin(); el != m_tree_index.end(); ++el)
        {
            for (auto node : el->second)
            {
                delete node;
            }
            el->second.clear();
        }
    }
};

class FPHandler
{
private:
    string m_input_filename, m_output_filename;
    double m_min_support;
    int m_total_transactions, m_total_ids;
    FPTree* m_fptree;
    
    void addBranchToTree( vector<int>& branch )
    {
        auto parent = m_fptree->m_root;
        size_t index = 0;
        while (index < branch.size())
        {
            bool found = false;
            for(auto child : parent->m_children)
            {
                if(child->m_id == branch[index]) //Match, increment count and continue
                {
                    child->m_count++;
                    found = true;
                    parent = child;
                    break;
                }
            }
            if(!found)
            {
                FPNode* new_list_child = new FPNode();
                new_list_child->m_parent = parent;
                new_list_child->m_id = branch[index];
                new_list_child->m_count = 1;
                auto& tree_index = m_fptree->m_tree_index;
                if (tree_index.find(branch[index]) == tree_index.end())
                {
                    FPNode* indexnode = new FPNode();
                    indexnode->m_id = branch[index];
                    list<FPNode*> indexlist;
                    indexlist.push_back(indexnode);
                    tree_index.emplace(branch[index], indexlist);
                }
                tree_index[branch[index]].push_back(new_list_child);
                auto indexhead = tree_index[branch[index]].begin();
                (*indexhead)->m_count++;
                parent->m_children.push_back(new_list_child);
                parent = new_list_child;
            }
            index++;
        }
    }

    void generateMainFpTree()
    {
        unordered_map<int,int> id_to_freq;
        ReadTransaction* read_helper = new ReadTransaction(m_input_filename);
        vector<int> transaction;
        while(read_helper->nextTransaction(transaction) > 0)
        {
            m_total_transactions++;
            for(auto id : transaction)
            {
                if(id_to_freq.find(id) != id_to_freq.end())
                {
                    ++id_to_freq[id];
                }
                else
                {
                    id_to_freq.emplace(id,1);
                    m_total_ids++;
                }
            }
            transaction.clear();
        }

        function<bool(int left, int right)> compare_freq = 
            [ &id_to_freq ] (int left, int right) -> bool
            {
                int leftf = id_to_freq[left];
                int rightf = id_to_freq[right];
                return ((leftf > rightf) || ((leftf == rightf) && (left < right)));
            };

        m_fptree = new FPTree();
        read_helper->resetHead(); // Read transactions for the second time from disk
        while(read_helper->nextTransaction(transaction) > 0)
        {
            vector<int> branch;
            for ( auto id : transaction )
            {
                if ((id_to_freq[id]/((double) m_total_transactions)) >= m_min_support)
                    branch.push_back(id);
            }

            sort(branch.begin(), branch.end(), compare_freq);
            addBranchToTree(branch);
            transaction.clear();
        }
        
        delete read_helper;

        for (auto& iter : m_fptree->m_tree_index)
        {
            auto li = iter.second.begin();
            cerr<<(iter.first)<<' '<<((*li)->m_id)<<' '<<((*li)->m_count)<<endl;
        }cerr<<endl;
    }

public:
    FPHandler(string input_file, string output_file, double min_support):
        m_input_filename(input_file),
        m_output_filename(output_file),
        m_min_support(min_support),
        m_total_transactions(0),
        m_total_ids(0)
    {
        generateMainFpTree();
    }

    ~FPHandler()
    {
        delete m_fptree;
    }

};
#endif
