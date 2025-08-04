/*
BlockList.hpp
Austin McCowan
4/2025
Header file for BlockList.hpp
BlockList handles blockNodes and their references
*/

#ifndef _BLOCK_LIST_HPP_
#define _BLOCK_LIST_HPP_

#include <vector>
#include <unordered_map>
#include <string>
#include "BlockNode.hpp"
#include <climits>

class BlockList {
    private:
        std::unordered_map<std::string, BlockNode> _storage;   // merely the storage of BlockNodes
        std::unordered_map<std::string, BlockNode*> _refArr;   // Keeps track of what variable is pointing at what
        int _capacity;                                         // Keeps track of the maximum capacity of memory
        unsigned int _prog_id_gen;                             // Helps with generating block ID's. Block ID doesnt matter.
                                                               // Only becomes an issue if you have for some reason created billions of blocks, reaching UINT_MAX.                                                    

    public:
        // Constructor
        BlockList() :
            _storage(),
            _refArr(),
            _capacity(0),
            _prog_id_gen(0)
        {
            // Immediately create and push origin into storage.
            _storage["ORIGIN"] = ( BlockNode() );
        }
    
    void resize(int val) { _capacity = val; }

    void dump();

    void compress();

    void free(std::string block);

    void free(BlockNode* block);

    void allocation(std::string variable, int alloc);

    std::string check(int size);

    void slot(BlockNode* block, BlockNode* freeSpace);

    void freeslot();

    void reference(std::string referrer, std::string referred); 

    int capacity() { return _capacity; }

    std::string generateBlock(Status stat, int size);

};

#endif