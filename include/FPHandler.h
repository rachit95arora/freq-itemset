#ifndef FP_HANDLER_H
#define FP_HANDLER_H

#include <algorithm>
#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <list>
#include <stack>
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

    FPNode(const FPNode* original):
        m_parent(original->m_parent),
        m_auxillary(original->m_auxillary),
        m_id(original->m_id),
        m_count(original->m_count)
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
    }

    FPNode* createShadowNode(FPNode* shadow_child, FPTree* shadow_tree)
    {
        FPNode* original = shadow_child->m_parent;
        bool created = false;
        if (original->m_auxillary) //Shadow Parent already exists
        {
            original->m_auxillary->m_count+=shadow_child->m_count;
            shadow_child->m_parent = original->m_auxillary;
        }
        else
        {
            created = true;
            FPNode* copy = new FPNode(original);
            copy->m_count = shadow_child->m_count;
            copy->m_auxillary = original;
            original->m_auxillary = copy;
            shadow_child->m_parent = copy;
            auto& tree_index = shadow_tree->m_tree_index;
            if(tree_index.find(copy->m_id) == tree_index.end())
            {
                list<FPNode*> nodel;
                FPNode* index_node = new FPNode();
                index_node->m_id = copy->m_id;
                nodel.push_back(index_node);
                nodel.push_back(copy);
                tree_index.emplace(copy->m_id, nodel);
            }
            else
            {
                tree_index[copy->m_id].push_back(copy);
            }
        }
        shadow_child->m_auxillary->m_auxillary = nullptr; // Reset original node's auxillary
        shadow_child->m_auxillary = nullptr;              // Reset shadow node's auxillary

        return created?(original->m_auxillary):nullptr;
    }

    void createShadowTree(stack<FPNode*> &dfs_stack, FPTree* new_tree)
    {
        while(!dfs_stack.empty())
        {
            auto node = dfs_stack.top();
            dfs_stack.pop();
            if((node->m_parent) && (node->m_parent->m_count > 0))
            {
                // Create shadow copy of parent               
                auto new_parent = createShadowNode(node, new_tree);
                if(new_parent)
                    dfs_stack.push(new_parent);
            }
            else if(node->m_parent) // Parent is null node
            {
                node->m_parent = new_tree->m_root;
            }
        }

        // Update index node counts in new_tree
        auto& tree_index = new_tree->m_tree_index;
        for (auto iter : tree_index)
        {
            auto& total_count = (*iter.second.begin())->m_count;
            total_count = 0;
            auto node_iter = iter.second.begin();
            while(++node_iter != iter.second.end())
            {
                total_count += (*node_iter)->m_count;
            }
        }
    }

    void fpGrowth(FPTree* fptree, vector<vector<int>> & freq_set)
    {
        auto& tree_index = fptree->m_tree_index;
        if(tree_index.size() == 0) //Empty Tree
        {
            return;
        }

        for(auto& iter : tree_index)
        {
            int iter_count = ((*(iter.second.begin()))->m_count);
            int iter_arg = ((*(iter.second.begin()))->m_id);

            if((((double)iter_count)/m_total_transactions) >= m_min_support)
            {
                auto node_list = tree_index[iter_arg];
                FPTree* new_tree = new FPTree();
                auto node = node_list.begin();
                stack<FPNode*> dfs_stack;
                list<FPNode*> delete_list;
                while(++node != node_list.end())
                {
                    FPNode* copy = new FPNode(*node);
                    copy->m_auxillary = copy;
                    dfs_stack.push(copy);
                    delete_list.push_back(copy);
                }

                createShadowTree(dfs_stack, new_tree);

                for( auto node: delete_list )
                {
                    delete node;
                }
                delete_list.clear();
                vector<vector<int>> new_set;
                fpGrowth(new_tree, new_set);
                for(auto& pat_set : new_set)
                {
                    pat_set.push_back(iter_arg);
                    freq_set.push_back(pat_set);
                }
                vector<int> singleton = { iter_arg };
                freq_set.push_back(singleton);
                delete new_tree;
            }
        }

    }

public:
    FPHandler(string input_file, string output_file, double min_support):
        m_input_filename(input_file),
        m_output_filename(output_file),
        m_min_support(min_support),
        m_total_transactions(0),
        m_total_ids(0)
    {
    }

    void generateFrequentItemSet()
    {
        generateMainFpTree();
        vector<vector<int> > freq_set;
        fpGrowth(m_fptree, freq_set);

        WriteFrequentItems* write_helper = new WriteFrequentItems(m_output_filename);
        write_helper->printItems(freq_set);
        delete write_helper;

    }

    ~FPHandler()
    {
        delete m_fptree;
    }

};
#endif
