g++ main.cpp Huffman.cpp -o HuffMan.exe

.\Huffman.exe -e .\TextBase\Hamlet.txt code.txt .\out.txt

.\Huffman.exe -e .\TextBase\random.txt code.txt .\out.txt

.\Huffman.exe -d  .\out.txt .\code.txt .\decoderFile.txt 

.\Huffman.exe -v -e .\TextBase\text.txt code.txt .\out.txt

.\Huffman.exe -v -d .\out.txt .\code.txt .\decoderFile.txt
