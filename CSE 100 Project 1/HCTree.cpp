#include <queue>
#include <vector>
#include <fstream>
#include "HCTree.hpp"
#include <string>
#include <algorithm>
using namespace std;

HCNode* successor(HCNode* current)
{
    if(current == NULL)
    {
        return NULL;
    }

    if(current->c1 != NULL)
    {
        current = current->c1;
        while(current->c0 != NULL)
        {
            current = current->c0;
        }
        return current;
    }
    else
    {
        while(current->p != NULL)
        {
            if(current->p->c0 != current)
            {
                current = current->p;
            }
            else
            {
                return current->p;
            }
        }
        return NULL;
    }
}

void HCTree::build(const vector<int>& freqs)
{
    priority_queue<HCNode*, vector<HCNode*>, HCNodePtrComp> pq;


    for(unsigned int i = 0; i < freqs.size(); i++)
    {
        if(freqs[i] != 0)
        {
            leaves[i] = new HCNode(freqs[i], i);
            pq.push(leaves[i]);
        }
    }

    while(pq.size() > 1)
    {
        HCNode* t1 = pq.top();
        pq.pop();
        HCNode* t2 = pq.top();
        pq.pop();

        HCNode* inter = new HCNode(t1->count + t2->count, t1->symbol);

        inter->c0 = t1;
        inter->c1 = t2;
        t1->p = inter;
        t2->p = inter;
        
        pq.push(inter);
    }
    if(pq.size() == 1)
    {
        root = pq.top();
    }
}

HCTree::~HCTree()
{
    HCNode* curr = root;
    if(curr != NULL)
    {
        while(curr->c0 != NULL)
        {
            curr = curr->c0;
        }

        while(successor(curr) != NULL)
        {
            HCNode* temp = successor(curr);
            delete curr;
            curr = temp;
        }
        delete curr;
    }
}

void HCTree::encode(unsigned char symbol, FancyOutputStream & out) const
{
    HCNode* curr = leaves[symbol];
    string encode = "";
    while(curr != root && curr != NULL)
    {
        if(curr->p->c0 == curr)
        {
            encode = encode + "0";
        }
        else
        {
            encode = encode + "1";
        }

        curr = curr->p;
    }
    
    for(int i = encode.length() - 1; i >= 0; i--)
    {
        int bit = encode[i] - 48;
        out.write_bit(bit);
    }
}

unsigned char HCTree::decode(FancyInputStream & in) const
{
    HCNode* curr = root;

    while(curr->c0 != NULL || curr->c1 != NULL)
    {
        int bit = in.read_bit();
        if(bit == 0 && curr->c0 != NULL)
        {
            curr = curr->c0;
        }
        else if(bit == 1 && curr->c1 != NULL)
        {
            curr = curr->c1;
        }
        else
        {
            return curr->symbol;
        }
    }

    return curr->symbol;
}
