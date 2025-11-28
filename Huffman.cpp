#include "Huffman.h"
#include<bits/stdc++.h> 
using namespace std ; 


Node::Node(char c , int f) : ch(c) , freq(f) , left(nullptr) , right(nullptr) {} ; 
HuffManTree::HuffManTree() : root(nullptr) , originalSize(0) , compressSize(0) {}


int Node::getFreq() const {
    return freq ;
}

char Node::getChar() const {
    return ch ;
}

bool Node::isLeaf() const {
    return left == nullptr && right == nullptr ;
}


void HuffManTree::generateCodes(const NodePtr& node ,string str){
    if(node == nullptr) return ; 

    if(node -> isLeaf()){
        huffManCode[node -> ch] = str ; 
    }
    generateCodes(node -> left , str + '0') ; 
    generateCodes(node -> right , str + '1') ; 
}

int HuffManTree::calcWPL(const NodePtr& node , int depth) const {
    if(node == nullptr) return 0 ; 
    if(node -> isLeaf()){
        return node -> freq * depth ; 
    }
    return calcWPL(node -> left , depth + 1) + calcWPL(node -> right , depth + 1) ; 
}

void HuffManTree::printTree(const NodePtr& node , int indent) const {

    if (node != nullptr) {
        if (node->right) printTree(node->right , indent + 4);
        
        if (indent) {
            cout << setw(indent) << " ";
        }
        
        if (node->right) cout << " /\n" << setw(indent) << " ";
        
        if (node->isLeaf()) {
            if (node->ch == '\n') cout << "\\n" << "(" << node->freq << ")\n";
            else if (node->ch == ' ') cout << "Sp" << "(" << node->freq << ")\n";
            else cout << node->ch << "(" << node->freq << ")\n";
        } else {
            cout << "O" << "(" << node->freq << ")\n";
        }

        if (node->left) {
            cout << setw(indent) << " " << " \\\n";
            printTree(node->left , indent + 4);
        }
    }
}

void HuffManTree::buildTree(const map<char , int>& freqs){
    freqMap = freqs ;

    auto comp = [](const NodePtr& l , const NodePtr& r) {
        return l->getFreq() > r->getFreq();
    } ; 
    priority_queue<NodePtr , vector<NodePtr> , decltype(comp)> pq(comp) ; 

    for(const auto& pair : freqs){
        pq.push(make_shared<Node>(pair.first, pair.second));
    }

    if(pq.empty()) return ; 
    if(pq.size() == 1){
        auto  t = pq.top() ; 
        pq.pop() ; 
        root = make_shared <Node>('\0' , t -> getFreq()) ; 
        root -> left = t ; 
        return ; 
    }

    while(pq.size() > 1){
        auto left = pq.top() ; pq.pop() ; 
        auto right = pq.top() ; pq.pop() ; 
        auto parent = make_shared <Node>('\0' , left -> getFreq() + right -> getFreq()) ; 

        parent -> left = left ; 
        parent -> right = right ; 

        pq.push(parent) ; 
    }

    root = pq.top() ; 
    generateCodes(root , "") ; 
} 

void HuffManTree::encode(const string& inputFile , const string& codeFile , const string& outFile , bool verbose){
    ifstream in(inputFile , ios::binary) ; 

    if(!in){
        cerr << "ERROR:CANT OPEN FILE" << endl ; 
        return; 
    }

    char ch ; 
    map<char , int> freqs ; 
    originalSize = 0 ; 

    while(in.get(ch)){
        freqs[ch]++ ;       
        originalSize++ ; 
    }
    in.close() ; 
    
    if (freqs.empty()) return;

    buildTree(freqs) ; 

    ofstream codeOut(codeFile) ; 
    if(!codeOut){
        cerr << "ERROR:CANT OPEN CODE FILE" << endl; 
        return; 
    }

    for (const auto& pair : freqs) {
        codeOut << static_cast<int>(static_cast<unsigned char>(pair.first)) ;
        codeOut << " " ; 
        codeOut << pair.second ; 
        codeOut << " " ; 
        codeOut << huffManCode.at(pair.first);
        codeOut << endl; 
    }
    codeOut.close() ; 


    in.open(inputFile , ios::binary) ; 
    ofstream out(outFile) ; 
    if(!out){
        cerr << "ERROR:CANT OPEN OUT FILE" << endl; 
        return; 
    }

    compressSize = 0 ; 

    while(in.get(ch)){
        const string& code = huffManCode.at(ch) ; 
        out << code ; 
        compressSize += code.length() ; 
    }

    in.close() ; 
    out.close() ; 
    
    double rate = 0.0;
    if (originalSize > 0) {
        rate = static_cast<double>(compressSize) / (originalSize * 8) * 100.0 ; 
    }
    
    cout << "COMPRESSRATE = " << fixed << setprecision(2) << rate << "%" << endl ; 

    if(verbose){
        showTreeInfo() ; 
    }
}

void HuffManTree::decode(const string& inputFile , const string& codeFile , const string& outFile , bool verbose){
    ifstream codeIn(codeFile) ; 
    if(!codeIn){
        cerr << "ERROR: CANT OPEN CODE FILE" << endl ;
        return;
    }
    
    map<char , int> freqs ; 
    string s ; 
    int freq ; 
    string code ; 
    
    while(codeIn >> s >> freq >> code){
        char ch = static_cast<char>(stoi(s));
        freqs[ch] = freq;
    }
    codeIn.close() ; 

    if (freqs.empty()) {
        cerr << "ERROR: CODE FILE EMPTY OR INVALID" << endl;
        return;
    }

    buildTree(freqs) ; 

    ifstream in(inputFile) ;
    ofstream out(outFile , ios::binary) ; 

    if(!in || !out){
        cerr << "ERROR: CANT OPEN FILE" << endl ;
        return ; 
    }

    NodePtr current = root ; 
    char ch ; 
    
    while(in.get(ch)){
        if(ch != '0' && ch != '1') continue ;
        
        if(ch == '0') current = current -> left ; 
        else if(ch == '1') current = current -> right ; 
        
        if (!current) break; 

        if(current -> isLeaf()){
            char originalChar = current -> getChar() ; 
            out.put(originalChar) ;         
            current = root ; 
        }
    }

    in.close() ; 
    out.close() ; 

    if(verbose){
        showTreeInfo() ; 
    }
}
void HuffManTree::showTreeInfo() const {
    cout << "showTreeInfo function not yet implemented." << endl;
}

string HuffManTree::getCode(const char& c) const{
    if (!c) return "";
    auto it = huffManCode.find(c);
    if(it != huffManCode.end()){
        return it->second;
    }
    return "";
}

