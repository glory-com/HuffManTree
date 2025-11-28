#ifndef HUFFMAN_H 
#define HUFFMAN_H 
#include<bits/stdc++.h> 
using namespace std ;

class Node ; 
using NodePtr = std::shared_ptr<Node> ;

class Node{
public:
    Node(char c ,int f) ; 

    int getFreq() const ; 
    char getChar() const ; 
    bool isLeaf() const ; 

    friend class HuffManTree;
private:
    char ch ; 
    int freq ; 
    NodePtr left ; 
    NodePtr right ; 


} ; 

class HuffManTree{
private: 
    NodePtr root ; 
    map<char , string> huffManCode ; 
    map<char , int> freqMap ; 
    long long originalSize ; 
    long long compressSize ; 

    void generateCodes(const NodePtr& node , string str) ; 
    int calcWPL(const NodePtr& node , int depth) const ; 
    void buildTree(const map<char , int>& freqs) ;



public:
    void encode(const string& inputFile , const string& codeFile , const string& outFIle , bool verbose) ;
    void decode(const string& inputFile , const string& codeFile , const string& outFIle , bool verbose) ; 
    HuffManTree() ; 
    string getCode(const char& c) const ; 
    void showWPL() const ; 
    void printTree(const NodePtr& node, const std::string& prefix, bool isLeft) const ; 
    void showTreeInfo() const ; 
};



#endif