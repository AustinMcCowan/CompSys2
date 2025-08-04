/*
BlockNode.hpp
Austin McCowan
4/2025
Header file for BlockNode
BlockNodes will retain all information regarding a block. This includes
pointing to the next basic blocknode, its start and end positions, reference count, et cetera.
*/

#ifndef _BLOCK_NODE_HPP_
#define _BLOCK_NODE_HPP_

#include <vector>
#include <string>

// "Free" represents a block considered as free space. "Basic" is a block that is considered storing value. Only Basic blocks can have a reference count above 0.
// "Origin" is the start of the linked list. Due to its interaction with the rest of the storage structure, it receives its own tag. All of its values are basically 0.
// Blocks can start at index 0, and this helps with that.
enum Status { FREE, BASIC, ORIGIN };

class BlockNode {
    // Only use public class. This file is only accessible in BlockTree.hpp
    public:
        Status _status;                 // Helps identify type of block. Useful when iterating through linked list.
        std::string _id;                // Helps identify block.     
        unsigned int _refcount;         // How much is referrencing this block. If this ever becomes zero, immolate.
        signed int _start;              // Represents starting address in memory. 
        signed int _end;                // Represents ending address in memory.
        signed int _size;               // Keeps track of size. If this goes to or below zero, it is because it is a free space slated for compression/removal

        // Note. Due to the how this structure operates, Every block only looks at a block with a higher addr, thus maintaining a sorted state (from low addr to high)
        BlockNode* _nextBlock;          // Simply points to the next block in order (Taking into consideration ALL blocks)
        BlockNode* _prevBlock;          // Simply points to the previous block in order

        // Constructor. Most of these values will be immediately filled after being entered into BlockTree. So no need to really worry about anything
        BlockNode(std::string id, Status stat, int size) :
            _status(stat),
            _id(id),
            _refcount(1),
            _start(),
            _end(),
            _size(size),
            _nextBlock(NULL),
            _prevBlock(NULL)
        {}

        // Complete block setup if needed.
        BlockNode(std::string id, Status stat, int size, int start, int end) :
            _status(stat),
            _id(id),
            _refcount(1),
            _start(start),
            _end(end),
            _size(size),
            _nextBlock(NULL),
            _prevBlock(NULL)
        {}

        // Origin declaration
        BlockNode() :
            _status(ORIGIN),
            _id("ORIGIN"),
            _refcount(0),
            _start(0),
            _end(0),
            _size(0),
            _nextBlock(NULL),
            _prevBlock(NULL)
        {}
};

#endif