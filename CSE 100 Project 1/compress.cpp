#include <vector>
#include "Helper.hpp"
#include "HCTree.hpp"

int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        error("Only 3 command line arguments allowed");
    }

    FancyInputStream* in = new FancyInputStream(argv[1]);
    vector<int> freqs = vector<int>(256);
    int byte = in->read_byte();

    while(byte != -1)
    {
        freqs[byte]++;
        byte = in->read_byte();
    }

    HCTree* huff = new HCTree();

    huff->build(freqs);

    FancyOutputStream* out = new FancyOutputStream(argv[2]);

    if(in->filesize() != 0)
    {
        for(unsigned int i = 0; i < freqs.size(); i++)
        {
            char* c;
            c = (char*) & freqs[i];
            for(int j = 0; j < 3; j++)
            {
                out->write_byte(c[j]);
            }
        }
    }

    in->reset();
    
    byte = in->read_byte();
    
    while(byte != -1)
    {
        huff->encode(byte, *out);
        byte = in->read_byte();
    }

    delete out;
    delete in;
    delete huff;

    return 0;
}
