#include "AprioriHandler.h"
#include <iostream>
#include "FPHandler.h"
using namespace std;
int main()
{
    cerr<<"AprioriHandler.cpp : Running Apriori\n";
    AprioriHandler* apriori_h = new AprioriHandler("/Users/rachit/Documents/retail.dat", "/Users/rachit/Documents/retail.out", 0.2);
    apriori_h->generateFrequentItemSet();
    delete apriori_h;

    cerr<<"AprioriHandler.cpp : Running FPGrowth\n";
    FPHandler* fp_h = new FPHandler("/Users/rachit/Documents/retail.dat", "/Users/rachit/Documents/retail_fp.out", 0.2);
    fp_h->generateFrequentItemSet();
    delete fp_h;
    return 0;
}
