/*
BlockList.cpp
Austin McCowan
4/2025
Provides functionality for BlockList.hpp
*/
#include "BlockNode.hpp"
#include "BlockList.hpp"
#include <iostream>
#include <vector>
using namespace std;

string BlockList::generateBlock(Status stat, int size) {
    string id = to_string(++_prog_id_gen);
    _storage[id] = BlockNode( id, stat, size);
    return id;
}

void BlockList::dump() {
    string variables, freeList;
    BlockNode* it = &_storage["ORIGIN"];
    // Get free list ready for presentation
    while (it != NULL) {
        if (it->_status != ORIGIN) {
            if ((it->_status == FREE) && (it->_nextBlock != NULL)) {
                freeList += to_string(it->_start) + "(" + to_string(it->_size) + ") [" + to_string(it->_refcount) + "], ";
            }
            else if (it->_status == FREE) {
                freeList += to_string(it->_start) + "(" + to_string(it->_size) + ") [" + to_string(it->_refcount) + "]\n";
            }
        }
        it = it->_nextBlock;
    }

    // Get variables ready for presentation
    for (const auto& it : _refArr) {
        variables += it.first + ":" + to_string(it.second->_start) + "(" + to_string(it.second->_size) + ") [" + to_string(it.second->_refcount) + "]\n";
    }

    cout << "Variables:\n" << variables << "Free List:\n" << freeList << "============================================================================" << endl;

}

void BlockList::compress() {
    vector<string> tracker;                 // Keep track of all the free blocks. We will mass delete them and congregate them later.
    BlockNode* it = &_storage["ORIGIN"];    // iterates through the linked list.

    // Move through linked list, compressing any free list sections.
    while (it != NULL) {
        if ((it->_status == FREE) && (it->_prevBlock->_status == FREE)) {
            it->_prevBlock->_nextBlock = it->_nextBlock;
            it->_prevBlock->_end = it->_end;
            it->_prevBlock->_size += it->_size;
            tracker.push_back(it->_id);
            if (it->_nextBlock != NULL) {
                it->_nextBlock->_prevBlock = it->_prevBlock;
            }
        }
        it = it->_nextBlock;
    }
    // Destroy now useless free blocks
    for (string val : tracker) {
        _storage.erase(val);
    }
}

// Called within main. Releases the memory referenced by the variable
void BlockList::free(std::string var){
    // Check if the variable even exists still. Accounts for variables that were never referenced prior due to no memory available.
    if (_refArr.find(var) != _refArr.end()) {
        // If anything is attached to the variable, free it from memory.
        if (_refArr[var] != NULL) {
            BlockNode* block = _refArr[var];
            block->_refcount--;
            if (block->_refcount < 1) { free(block); }
            _refArr.erase(var); 
        }
    }
}

// Called within BlockList.
void BlockList::free(BlockNode* block){
    block->_status = FREE;
}

void BlockList::allocation(std::string variable, int alloc) { 
    // Look for variable within reference array
    if (_refArr.find(variable) == _refArr.end()) {
        // If we do not find it. See if we can add it. Create it + plus a new block. Generated blocks start out at refcount 1.
        string placement = check(alloc);
        if ( placement.compare("-1") != 0 ) {
            BlockNode* freeBlock = &_storage[placement];
            string id = generateBlock(BASIC, alloc);
            _refArr[variable] = &_storage[id];
            slot(_refArr[variable], freeBlock);
        }

    // Variable was found within reference arr    
    } else {
        // It does exist. Therefore we must decrement the variables prev block refcount + create new block.
        BlockNode* targ = _refArr[variable];
        targ->_refcount--;

        // Check if we can free previous allocation. If so, update possible placement options.
        if (targ->_refcount < 1) { free(targ); }

        // Check and see if we can add new block.
        string placement = check(alloc);
        if ( placement.compare("-1") != 0 ) {
            BlockNode* freeBlock = &_storage[placement];
            string id = generateBlock(BASIC, alloc);
            _refArr[variable] = &_storage[id];
            slot(_refArr[variable], freeBlock);
        }
    }                                                             
}

// Check if there are any available slots in freeList given a size. Then return the 
string BlockList::check(int size) {
    BlockNode* b = &_storage["ORIGIN"];
    while (b != NULL) {
        // Can we fit block in this empty space?
        if ((b->_size >= size) && (b->_status == FREE)) { return b->_id; }
        // Move on to next block
        else { b = b->_nextBlock; }
    }

    return "-1"; // If no spots exist, return -1

}

// Slot in a block
void BlockList::slot(BlockNode* block, BlockNode* freeSpace) {
    // Rearrange positions / sizes
    block->_start = freeSpace->_start;
    block->_end = block->_start + block->_size;
    freeSpace->_start += block->_size;
    freeSpace->_size = freeSpace->_end - freeSpace->_start;

    // Rearrange pointers
    BlockNode* prev = freeSpace->_prevBlock;
    prev->_nextBlock = block;
    block->_prevBlock = prev;
    freeSpace->_prevBlock = block;
    block->_nextBlock = freeSpace;

    // Check and see if the free space is now filled (i.e., has zero capacity). If so, delete it.
    if (freeSpace->_size <= 0) {
        if (freeSpace->_nextBlock != NULL) { block->_nextBlock = freeSpace->_nextBlock; }
        else { block->_nextBlock = NULL; }
        _storage.erase(freeSpace->_id);
    }
}

// Slot in a free space (This should really only be the initial free space of the program)
void BlockList::freeslot() {
    string id = generateBlock(FREE, _capacity);
    BlockNode* freeBlock = &_storage[id];
    BlockNode* origin = &_storage["ORIGIN"];
    freeBlock->_refcount = 0;
    freeBlock->_start = 0;
    freeBlock->_end = _capacity;
    freeBlock->_prevBlock = origin;
    origin->_nextBlock = freeBlock;   
}

// Make the referrer reference whatever the referred does.
void BlockList::reference(std::string referrer, std::string referred) {
    // If referrer does not exist and referred exists
    if ( (_refArr.find(referrer) == _refArr.end()) && (_refArr.find(referred) != _refArr.end())) {
        _refArr[referrer] = _refArr[referred];
        _refArr[referrer]->_refcount++;

    // Referred and referrer exist
    } else if ((_refArr.find(referrer) != _refArr.end()) && (_refArr.find(referred) != _refArr.end())) {
        // Check if the referrer and referred are the same. Also check that referred exists.
        if ( (referrer.compare(referred) != 0)) {
            // Handle the referrer's previous obligations
            BlockNode* targ = _refArr[referrer];
            targ->_refcount--;
            if (targ->_refcount < 1) { free(targ); }

            // New obligations
            _refArr[referrer] = _refArr[referred];
            _refArr[referrer]->_refcount++;

        } // else: failed to comply with any conditions. Do nothing

    // Referrer exists but referred does not exist. ( The program will not create ghost variables. i.e., variables pointing to nothing )
    } else if ((_refArr.find(referrer) != _refArr.end()) && (_refArr.find(referred) == _refArr.end())) {
        // Handle the referrer's previous obligations
        BlockNode* targ = _refArr[referrer];
        targ->_refcount--;
        if (targ->_refcount < 1) { free(targ); }
        _refArr[referrer] = NULL;

        // Infact, remove this variable entirely. No point in this existing anymore.
        _refArr.erase(referrer);
    
    // Neither exist. (the program will not create ghost variables. i.e., variables pointing to nothing)
    } else {
        // Do nothing.
    }
}  