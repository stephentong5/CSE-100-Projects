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

    if(in->filesize() != 0)
    {
        for(unsigned int i = 0; i < freqs.size(); i++)
        {
            for(int j = 0; j < 3; j++)
            {
                freqs[i] += (in->read_byte()) << 8*j;
            }
        }
    }

    HCTree* huff = new HCTree();

    huff->build(freqs);

    FancyOutputStream* out = new FancyOutputStream(argv[2]);

    if(huff->root != NULL)
    {
        for(int i = 0; i < huff->root->count; i++)
        {
            char symbol = huff->decode(*in);
            out->write_byte(symbol);
        }
    }

    delete out;
    delete in;
    delete huff;
    
    return 0;
}
