#include <iostream>
#include <cstring>
#include <vector>
#include "MemoryPool.h"

int main()
{
    std::cout << "Network Packet Buffer Pool\n\n";

    const size_t BLOCK_SIZE = 512;
    const size_t BLOCK_COUNT = 8;

    MemoryPool pool(BLOCK_SIZE, BLOCK_COUNT);

    std::cout << "Block Size:       " << pool.blockSize() << " bytes\n";
    std::cout << "Blocks:           " << BLOCK_COUNT << "\n";
    std::cout << "Total Capacity:   " << pool.capacity() << " bytes\n\n";

    // --- Allocate a few blocks and show their addresses ---
    void* packet1 = pool.allocate();
    void* packet2 = pool.allocate();
    void* packet3 = pool.allocate();

    std::cout << "Packet 1 allocated: " << packet1 << "\n";
    std::cout << "Packet 2 allocated: " << packet2 << "\n";
    std::cout << "Packet 3 allocated: " << packet3 << "\n\n";

    std::cout << "Available blocks: " << pool.availableBlocks() << "\n";
    std::cout << "Allocated blocks: " << pool.allocatedBlocks() << "\n\n";

    // --- Write and read back binary data ---
    unsigned char packetData[] =
    {
        0x45, 0x00, 0x00, 0x3C,
        0xAB, 0xCD, 0x12, 0x34
    };

    std::memcpy(packet1, packetData, sizeof(packetData));
    std::cout << "Binary packet written to Packet 1.\n\n";

    unsigned char readBack[sizeof(packetData)];
    std::memcpy(readBack, packet1, sizeof(packetData));

    std::cout << "Read back from Packet 1: ";
    for (unsigned char byte : readBack)
    {
        std::cout << std::hex << "0x" << static_cast<int>(byte) << " ";
    }
    std::cout << std::dec << "\n\n";

    // --- Release a block ---
    pool.deallocate(packet2);
    std::cout << "Packet 2 released.\n\n";

    std::cout << "Available blocks: " << pool.availableBlocks() << "\n";
    std::cout << "Allocated blocks: " << pool.allocatedBlocks() << "\n\n";

    // --- Demonstrate memory reuse ---
    void* packet4 = pool.allocate();
    std::cout << "Packet 4 allocated: " << packet4 << "\n";
    if (packet4 == packet2)
    {
        std::cout << "Packet 4 reused the previously released block.\n\n";
    }
    else
    {
        std::cout << "Packet 4 did not reuse the released block.\n\n";
    }

    // --- Exhaust the pool ---
    std::cout << "Attempting to exhaust pool...\n\n";
    std::vector<void*> allBlocks;
    while (true)
    {
        void* block = pool.allocate();
        if (block == nullptr)
        {
            break;
        }
        allBlocks.push_back(block);
    }

    std::cout << "No blocks available.\n";
    void* failedAlloc = pool.allocate();
    if (failedAlloc == nullptr)
    {
        std::cout << "allocate() returned nullptr.\n\n";
    }

    std::cout << "Available blocks: " << pool.availableBlocks() << "\n";
    std::cout << "Allocated blocks: " << pool.allocatedBlocks() << "\n\n";

    // --- Demonstrate double deallocation is rejected ---
    std::cout << "Attempting double deallocation...\n\n";
    void* someBlock = allBlocks.empty() ? nullptr : allBlocks.front();
    if (someBlock != nullptr)
    {
        bool first = pool.deallocate(someBlock);
        bool second = pool.deallocate(someBlock);
        std::cout << "First deallocation result:  " << (first ? "true" : "false") << "\n";
        std::cout << "Second deallocation result: " << (second ? "true" : "false") << "\n";
        if (!second)
        {
            std::cout << "Double deallocation rejected.\n";
        }
    }

    return 0;
}
