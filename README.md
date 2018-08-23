# freq-itemset

Implementation of:
* Apriori Algorithm
* FP-Growth Algorithm

for frequent itemset mining in C++

## Project Structure
The project contains the following files and folders:

* **compile.sh**     : sh script to compile C++ code
* **2014CS50292.sh** : sh script to run apriori/fptree/plot relative performance
* **Makefile**       : make targets to compile/clean source ( uses O3 )
* **plot.py**        : python script to plot relative performance of the two algorithms
* **include**        : c++ header files containing code for mining algorithms
  * AprioriHandler.h  -> Handler for running apriori algorithm
  * FPHandler.h       -> Handler for running fpgrowth algorithm
  * ReadTransaction.h -> Handler for reading one transaction in memory at a time
* **src**            : c++ source file directory
  * Main.cpp          -> Entry point. Uses handler header files in include  

## Observations

## Usage
* Compilation: `sh compile.sh`
* Run:
  * Apriori: `sh 2014CS50292.sh input-file support% -apriori output-file`
  * FPTree : `sh 2014CS50292.sh input-file support% -fptree output-file`
  * Plotting relative performance
      ```
      sh 2014CS50292.sh input-file -plot
      ```

