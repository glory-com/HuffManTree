#include "Huffman.h"
#include<bits/stdc++.h> 
using namespace std ; 


Node::Node(char c , int f) : ch(c) , freq(f) , left(nullptr) , right(nullptr) {} 
HuffManTree::HuffManTree() : root(nullptr) , originalSize(0) , compressSize(0) {} 
BitWriter::BitWriter(ostream& os) : out(os), buffer(0), count(0) {}
BitReader::BitReader(ifstream& is) : in(is), buffer(0), count(0) {} 


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
        totalChars++ ; 
    }
    in.close() ; 
    
    if (freqs.empty()) return;

    buildTree(freqs) ; 

    ofstream codeOut(codeFile , ios::binary) ; 
    if(!codeOut){
        cerr << "ERROR:CANT OPEN CODE FILE" << endl; 
        return; 
    }

    codeOut << "HEAD " << totalChars << endl ; 

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
    ofstream out(outFile , ios::binary) ; 
    if(!out){
        cerr << "ERROR:CANT OPEN OUT FILE" << endl; 
        return; 
    }

    BitWriter bw(out) ; 
    compressSize = 0 ; 

    while(in.get(ch)){
        const string& code = huffManCode.at(ch) ; 
        for(char bc : code){
            bw.writeBit(bc - '0') ; 
        } 
        compressSize += code.length() ; 
    }
    bw.flush() ;

    in.close() ; 
    out.close() ; 

    long long compressedBytes = (compressSize + 7) / 8 ; 
    double rate = (double) compressedBytes / originalSize * 100.0 ; 
    

    cout << "COMPRESSRATE = " << fixed << setprecision(2) << rate << "%" << endl ; 

    if(verbose){
        showTreeInfo() ; 
    }
}

void HuffManTree::decode(const string& inputFile , const string& codeFile , const string& outFile , bool verbose){
    ifstream codeIn(codeFile , ios::binary) ; 
    if(!codeIn){
        cerr << "ERROR: CANT OPEN CODE FILE" << endl ;
        return;
    }
    
    map<char , int> freqs ; 
    string keyStr, codeStr;
    int freq;
    long long DecodeChars = 0; 
    
    string Tag ; 
    codeIn >> Tag ; 
    if(Tag == "HEAD"){
        codeIn >> DecodeChars ; 
    }

    while(codeIn >> keyStr >> freq >> codeStr){
        char c = static_cast<char>(stoi(keyStr));
        freqs[c] = freq;
    }

    codeIn.close() ; 

    if (freqs.empty()) {
        cerr << "ERROR: CODE FILE EMPTY OR INVALID" << endl;
        return;
    }

    buildTree(freqs) ; 

    ifstream in(inputFile , ios::binary) ;
    ofstream out(outFile , ios::binary) ; 

    if(!in || !out){
        cerr << "ERROR: CANT OPEN FILE" << endl ;
        return ; 
    }


    BitReader br(in) ;
    long long decodedCount = 0 ;  

    NodePtr current = root ; 
    char ch ; 
    
    while (decodedCount < DecodeChars) {

        int bit = br.readBit();
        if (bit == -1) break; 

        if (bit == 0) current = current->left;
        else current = current->right;

        if (!current) break; 

        if (current->isLeaf()) {
            if (decodedCount == DecodeChars)
                break;
            out.put(current->ch);
            decodedCount++;
            current = root;
        }
    }

    in.close() ; 
    out.close() ; 

    if(verbose){
        showTreeInfo() ; 
    }
}

void HuffManTree::showTreeInfo() const {
    showWPL() ; 
    printTree(root , "" , true) ; 
}

string HuffManTree::getCode(const char& c) const{
    if (!c) return "";
    auto it = huffManCode.find(c);
    if(it != huffManCode.end()){
        return it->second;
    }
    return "";
}

void HuffManTree::showWPL() const { 
    int wpl = calcWPL(root , 0) ;
    cout << "the WPL of this Huffman tree is : "  << wpl << endl ;  

}

void HuffManTree::printTree(const NodePtr& node, const std::string& prefix, bool isLeft) const {
    if (node == nullptr) {
        return;
    }


    std::cout << prefix;
    std::cout << (isLeft ? "|-- " : "`-- "); 


    std::cout << "[";
    if (node->isLeaf()) {
        char ch = node->ch;

        if (ch == '\n') std::cout << "\\n";
        else if (ch == '\r') std::cout << "\\r";
        else if (ch == ' ') std::cout << "Sp";
        else if (ch >= 32 && ch <= 126) std::cout << ch ;
        else std::cout << (int)(unsigned char)ch; 

        std::cout << "]:" << node->freq << ' ' << getCode(ch) ; 
    } else {
        std::cout << "O]:" << node->freq; 
    }
    std::cout << std::endl;

    std::string newPrefix = prefix + (isLeft ? "|   " : "    ");

    if (node->right) {
  
        printTree(node->right, newPrefix, false); 
    }


    if (node->left) {

        printTree(node->left, newPrefix, true); 
    }
}


void BitWriter::writeBit(int bit){
    buffer = buffer << 1 ; 
    if(bit){
        buffer = buffer | 1 ; 
    }
    count++ ;
    if(count == 8){
        count = 0 ; 
        out.put(buffer) ; 
        buffer = 0 ; 
    }

}

void BitWriter::flush() { 
    if(count > 0){
        buffer = buffer << (8 - count) ; 
        out.put(buffer) ; 
        count = 0 ; 
        buffer = 0 ;
    }
}

int BitReader::readBit(){
    if(count == 0){
        char c  ; 
        if(!in.get(c)) return -1 ; 
        buffer = static_cast<unsigned char>(c) ;
        count = 8 ; 
    }

    int bit = (buffer >> (count - 1)) & 1 ; 
    count -- ; 
    return bit ;
}

