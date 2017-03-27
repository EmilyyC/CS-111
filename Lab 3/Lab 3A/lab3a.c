#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// global struct so that the whole program can access the data from the super block
struct superblockData {
	uint16_t magicNumber;
	int totalNumInodes;
	int totalNumBlocks;
	unsigned int blockSize;
	int fragmentSize;
	int blocksPerGroup;
	int inodesPerGroup;
	int fragmentsPerGroup;
	int firstDataBlock;
	uint16_t inodeSize;
} superblockData;

// determine if a number is a power of two
int isPowerOfTwo(unsigned int n) {
	return (n && !(n & (n-1)));
}

// generate output for super.csv
void superblockAnalysis(int fileSystemImage) {
	// the superblock is always located at a byte offset of 1024 from the start of the volume
	int superblockOffset = 1024;
	int superblockSize = 1024;
	// read in all of the superblock (1024 bytes) into a buffer 
	void* superblock = malloc(superblockSize);  
	if (superblock == NULL) {
		perror("Could not allocate dynamic memory for the superblock"); exit(1);
	}
	if (pread(fileSystemImage, superblock, superblockSize, superblockOffset) == -1) {
		perror("Could not read the superblock"); exit(1);
	}

	// get the magic number (2 byte value)
	superblockData.magicNumber = *((uint16_t*)(superblock + 56));
	// get the total number of inodes
	superblockData.totalNumInodes = *((int*)(superblock + 0));
	// get the total number of blocks
	superblockData.totalNumBlocks = *((int*)(superblock + 4));
	// get the block size
	superblockData.blockSize = 1024 << *((int*)(superblock + 24));
	// get the fragment size
	if (*((int*)(superblock + 28)) > 0) 
		superblockData.fragmentSize = 1024 << *((int*)(superblock + 28));
	else
		superblockData.fragmentSize = 1024 >> -(*((int*)(superblock + 28)));
	// get the blocks per group
	superblockData.blocksPerGroup = *((int*)(superblock + 32));
	// get the inodes per group
	superblockData.inodesPerGroup = *((int*)(superblock + 40));
	// get the fragments per group
	superblockData.fragmentsPerGroup = *((int*)(superblock + 36));
	// get the first data block
	superblockData.firstDataBlock = *((int*)(superblock + 20));
	// get the size of each inode
	superblockData.inodeSize = *((uint16_t*)(superblock + 88));

	// perform sanity checking tests
	// check that the value of the magic number is correct
	if (superblockData.magicNumber != 0xEF53) {  
		printf("%d", &superblockData.magicNumber);
		perror("Incorrect value for magic number"); exit(1);
	}
	// check that the block size is reasonable (eg. power of 2 between 512 - 64k)
	if (!isPowerOfTwo(superblockData.blockSize) || (superblockData.blockSize < 512) || (superblockData.blockSize > 64000)) {
		perror("Invalid block size"); exit(1);
	}
	// check that total blocks and first data block are consistent with the file size
	struct stat fileInfo;
	if (fstat(fileSystemImage, &fileInfo) == -1) {
		perror("Cannot get file information"); exit(1);
	}
	if ((superblockData.firstDataBlock > fileInfo.st_size) || (superblockData.totalNumBlocks > fileInfo.st_size)) {
		perror("Total blocks and first data block are inconsistent with file size"); exit(1);
	}

	// check that blocks per group divide evenly into total blocks
	if ((superblockData.totalNumBlocks % superblockData.blocksPerGroup) != 0) {
		perror("Blocks per group does not divide evenly into total blocks");
	}

	// check that inodes per group divide evenly into total inodes
	if ((superblockData.totalNumInodes % superblockData.inodesPerGroup) != 0) {
		perror("Inodes per group does not divide evenly into total inodes");
	}

	// print out file system parameters from the super block to the CSV file
	FILE* supercsv = fopen("super.csv", "w");  // create a new file to write to
	if (fprintf(supercsv, "%x,%d,%d,%d,%d,%d,%d,%d,%d\n", superblockData.magicNumber, superblockData.totalNumInodes, superblockData.totalNumBlocks, superblockData.blockSize, superblockData.fragmentSize, superblockData.blocksPerGroup, superblockData.inodesPerGroup, superblockData.fragmentsPerGroup, superblockData.firstDataBlock) < 0) {
		perror("Error with writing to super.csv"); exit(1);
	}

	// free the memory
	free(superblock);
}

void groupDescriptorAnalysis(int fileSystemImage) {
	// every block group descriptor table contains all the information about all the block groups
	// the block group descriptor table starts on the first block following the superblock
	int blockGroupDescriptorOffset = 1024 + 1024;  // since superblock has offset of 1024 and size of 1024
	int blockGroupDescriptorSize = 32;
	int numGroups = superblockData.totalNumBlocks / superblockData.blocksPerGroup;
	// read in the whole block group descriptor table into a buffer 
	void* blockGroupDescriptor = malloc(numGroups * blockGroupDescriptorSize);  
	if (blockGroupDescriptor == NULL) {
		perror("Could not allocate dynamic memory for the block group descriptor"); exit(1);
	}
	if (pread(fileSystemImage, blockGroupDescriptor, blockGroupDescriptorSize * numGroups, blockGroupDescriptorOffset) == -1) {
		perror("Could not read the block group descriptor"); exit(1);
	}

	// open the group.csv, bitmap.csv, inode.csv files to contain our outputs
	FILE* groupcsv = fopen("group.csv", "w");  // create a new file to write to
	FILE* bitmapcsv = fopen("bitmap.csv", "w");  
	FILE* inodecsv = fopen("inode.csv", "w");  

	// counters to determine which blocks and which inodes are free
	// important to note that the first block is at index 0 so count needs to start at 1
	int blockCount = 1;
	int inodeCount = 1;

	// iterate thorugh all of the group descriptors
	int i;
	for (i = 0; i < numGroups; i++) {
		// CODE FOR PART 2
		// get the number of free blocks
		uint16_t numFreeBlocks = *((uint16_t*)(blockGroupDescriptor + 12));
		// get the number of free inodes
		uint16_t numFreeInodes = *((uint16_t*)(blockGroupDescriptor + 14));
		// get the number of directories
		uint16_t numDirectories = *((uint16_t*)(blockGroupDescriptor + 16));
		// get the (free) inode bitmap block
		int inodeBitmapBlock = *((int*)(blockGroupDescriptor + 4));
		// get the (free) block bitmap block
		int blockBitmapBlock = *((int*)(blockGroupDescriptor + 0));
		// get the inode table (start) block
		int inodeTableStartBlock = *((int*)(blockGroupDescriptor + 8));

		// perform sanity checking tests
		// check that bitmap and inode starting blocks are within the group
		int currBlockGroupOffset = i * superblockData.blocksPerGroup;
		if (inodeBitmapBlock < currBlockGroupOffset || inodeBitmapBlock > (currBlockGroupOffset + superblockData.blocksPerGroup)) {
			perror("Inode bitmap block is not within this block group"); exit(1);
		}
		if (blockBitmapBlock < currBlockGroupOffset || blockBitmapBlock > (currBlockGroupOffset + superblockData.blocksPerGroup)) {
			perror("Block bitmap block is not within this block group"); exit(1);
		}
		if (inodeTableStartBlock < currBlockGroupOffset || inodeTableStartBlock > (currBlockGroupOffset + superblockData.blocksPerGroup)) {
			perror("Inode table start block is not within this block group"); exit(1);
		}

		// print out file system parameters from the super block to the CSV file
		if (fprintf(groupcsv, "%d,%d,%d,%d,%x,%x,%x\n", superblockData.blocksPerGroup, numFreeBlocks, numFreeInodes, numDirectories, inodeBitmapBlock, blockBitmapBlock, inodeTableStartBlock) < 0) {
			perror("Error with writing to group.csv"); exit(1);
		}

		// CODE FOR PART 3
		// block bitmap and inode bitmap are both one block long, so use same procedure
		// the block bitmap is one block long, thus it contains block size * 8 bits = 1024 * 8 = 8192 bits
		// get the memory for the block bitmap block (8192/8 bytes since 8192 blocks and 8 bits per byte)
		void* blockBitmap = malloc(superblockData.blockSize);
		if (blockBitmap == NULL) {
			perror("Could not allocate dynamic memory for the block bitmap block"); exit(1);
		}
		if (pread(fileSystemImage, blockBitmap, 8192/8, blockBitmapBlock * superblockData.blockSize) == -1) {
			perror("Could not read the block bitmap block"); exit(1);
		}
		// get the memory for the inode bitmap block (2048/8 bytes since 2048 inodes and 8 bits per byte)
		void* inodeBitmap = malloc(superblockData.blockSize);
		if (inodeBitmap == NULL) {
			perror("Could not allocate dynamic memory for the inode bitmap block"); exit(1);
		}
		if (pread(fileSystemImage, inodeBitmap, 2048/8, inodeBitmapBlock * superblockData.blockSize) == -1) {
			perror("Could not read the inode bitmap block"); exit(1);
		}
		// need to check each bit of the block bitmap since each bit represents the current state of a block within that block group, where 1 means "used" and 0 "free/available"
		// the first block of this block group is represented by bit 0 of byte 0, the second by bit 1 of byte 0. The 8th block is represented by bit 7 (most significant bit) of byte 0 while the 9th block is represented by bit 0 (least significant bit) of byte 1

		// 8192/8 = 1024, we want to go over all of them so use a uint8_t to go 1 byte at a time
		int j;  // use as the byte offset from the beginning of the bitmap
		// go from left to right
		for (j = 0; j < 1024; j++) { 
			uint8_t blockBits = *((uint8_t*)(blockBitmap + j));
			// go over each of the 8 bits from right to left
			uint8_t currBit = 1;  
			int k;
			for (k = 0; k < 8; k++) {
				// the block is free
				if ((currBit & blockBits) == 0) {
					fprintf(bitmapcsv, "%x,%d\n", blockBitmapBlock, blockCount);
				}
				// increment iterator for which block we are checking
				blockCount++;
				// shifting an unsigned value to right is a logical shift
				currBit = currBit << 1;
			}
		}

		// store the inode offset for this group
		int currGroupInodeOffset = 0;
		// 2048/8 = 256, we want to go over all of them so use a uint8_t to go 1 byte at a time
		// use as the byte offset from the beginning of the bitmap
		// go from left to right
		for (j = 0; j < 256; j++) { 
			uint8_t inodeBits = *((uint8_t*)(inodeBitmap + j));
			// go over each of the 8 bits from right to left
			uint8_t currBit = 1;  
			int k;
			for (k = 0; k < 8; k++) {
				// the inode is free
				if ((currBit & inodeBits) == 0) {
					fprintf(bitmapcsv, "%x,%d\n", inodeBitmapBlock, inodeCount);
				}
				// CODE FOR PART 4
				// the inode is not free AKA it is already allocated
				else {
					int inodeNumber = inodeCount;
					// read in the data for the current inode
					void* currentInode = malloc(superblockData.inodeSize);
					if (currentInode == NULL) {
						perror("Could not allocate dynamic memory for the allocated inode"); exit(1);
					}
					if (pread(fileSystemImage, currentInode, superblockData.inodeSize, (inodeTableStartBlock * superblockData.blockSize + currGroupInodeOffset*superblockData.inodeSize) ) == -1) {
						perror("Could not read the allocated inode"); exit(1);
					}
					// get key information for the allocated inode
					uint16_t mode = *((uint16_t*)(currentInode + 0));
					uint16_t owner = *((uint16_t*)(currentInode + 2));
					uint16_t group = *((uint16_t*)(currentInode + 24));
					uint16_t linkCount = *((uint16_t*)(currentInode + 26));
					int creationTime = *((int*)(currentInode + 12));
					int modificationTime = *((int*)(currentInode + 16));
					int accessTime = *((int*)(currentInode + 8));
					int size = *((int*)(currentInode + 4));
					int numBlocks = *((int*)(currentInode + 28)) /2;  // must divide by 2 since the inode counts the number of 512 byte blocks, but the actual block size is 1024 bytes
					uint32_t* iBlock = (uint32_t*)(currentInode + 40);
					// get the file type from the mode
					char fileType;
					if (mode & 0x8000) {
						fileType = 'f';  // regular file
					}
					else if (mode & 0xA000) {
						fileType = 's';  // symbolic link
					}
					else if (mode & 0x4000) {
						fileType = 'd';  // directory
					}
					else {
						fileType = '?';  // all other types
					}
					// get the 15 block pointers
					uint32_t blockPointers[15];
					int l;
					for (l = 0; l < 15; l++) {
						// make sure the block pointer isn't NULL
						if ((iBlock + l) != NULL) {
							blockPointers[l] = *(iBlock + l);
							// check that the block number is in between the first data block number and the file system size (as indicated in the super block)
							if (blockPointers[l] > superblockData.totalNumBlocks) {
								perror("Block number is outside of range"); exit(1);
							}
						}
						else
							blockPointers[l] = 0;
					}
					// put the key information of the current inode in the inode.csv file
					fprintf(inodecsv, "%d,%c,%o,%d,%d,%d,%x,%x,%x,%d,%d", inodeNumber, fileType, mode,  owner, group, linkCount, creationTime, modificationTime, accessTime, size, numBlocks);			
					for (l = 0; l < 15; l++) {
						fprintf(inodecsv, ",%x", blockPointers[l]);
					}
					fprintf(inodecsv, "\n");  // finished printing everything for the current line

					// free the dynamic memory for the inode
					free(currentInode);
				}
				// update which inode in the group we are currently on
				currGroupInodeOffset++;
				// increment iterator for which block we are checking
				inodeCount++;
				// shifting an unsigned value to right is a logical shift
				currBit = currBit << 1;
			}
		}

		// free the dynamic memory for the blockBitmap and inodeBitmap
		free(blockBitmap);
		free(inodeBitmap);

		// increment by 32 bytes because that's the size of the block group descriptor
		// GCC allows you to add to void pointer by defining sizeof(void *) = 1
		blockGroupDescriptor += 32;
	}
}

void processDirectoryEntriesInBlock(int fileSystemImage, int blockPointer, FILE* directorycsv, int* entryNumber, int parentInodeNumber) {
	// block pointer to current block is not NULL, so check the entries there
	int currDirectoryEntryOffset = 0;
	// the directory entry offsets can't exceed the size of the block
	while (currDirectoryEntryOffset < superblockData.blockSize) {
		// get the first 8 bytes which contain all the directory entry data except the name, since the name is variable length
		void* directoryEntry = malloc(8);
		if (directoryEntry == NULL) {
			perror("Could not allocate dynamic memory for the directory entry"); exit(1);
		}
		if (pread(fileSystemImage, directoryEntry, 8, blockPointer * superblockData.blockSize + currDirectoryEntryOffset) == -1) {
			perror("Could not read the directory entry"); exit(1);
		}	
		// get the data from the first 8 bytes
		int directoryEntryInodeNumber = *((int*)(directoryEntry + 0));
		uint16_t recLen = *((uint16_t*)(directoryEntry + 4));
		uint8_t nameLen = *((uint8_t*)(directoryEntry + 6));

		// if inode number is zero, assume the directory entry is empty and don't include it in the output
		if (directoryEntryInodeNumber == 0) {
			currDirectoryEntryOffset += recLen;
			*entryNumber = *entryNumber + 1;
			continue;
		}

		// sanity checking
		// check if the entry length is reasonable
		if (recLen < 8 || recLen > 1024) {
			perror("Invalid directory entry length"); exit(1);
		}
		// see if name length is reasonable
		if (nameLen > recLen) {
			perror("Invalid name length for file entry"); exit(1);
		}
		// see if the inode number of the file entry is within the superblock specified range
		if (directoryEntryInodeNumber > superblockData.totalNumInodes) {
			perror("Invalid inode number of file "); exit(1);
		}

		// get the name (allocate extra byte so we can add nullbyte at the end)
		char* name = (char*)malloc((nameLen+1) * sizeof(char));
		if (name == NULL) {
			perror("Could not allocate dynamic memory for the name"); exit(1);
		}
		if (pread(fileSystemImage, name, nameLen, blockPointer * superblockData.blockSize + currDirectoryEntryOffset + 8) == -1) {
			perror("Could not read the name of the directory entry"); exit(1);
		}	
		// set the nullbyte for the name
			name[nameLen] = '\0';

		// print out the statistics for this directory entry
		fprintf(directorycsv, "%d,%d,%d,%d,%d,\"%s\"\n", parentInodeNumber, *entryNumber, recLen, nameLen, directoryEntryInodeNumber, name);			

		// update the directory entry number
		*entryNumber = *entryNumber + 1;
		// update the current directory offset
		currDirectoryEntryOffset += recLen;
	}
}

void directoryEntriesAnalysis(int fileSystemImage) {
	// every block group descriptor table contains all the information about all the block groups
	int blockGroupDescriptorOffset = 1024 + 1024;  // since superblock has offset of 1024 and size of 1024
	int blockGroupDescriptorSize = 32;
	int numGroups = superblockData.totalNumBlocks / superblockData.blocksPerGroup;
	// read in the whole block group descriptor table into a buffer 
	void* blockGroupDescriptor = malloc(numGroups * blockGroupDescriptorSize);  
	if (blockGroupDescriptor == NULL) {
		perror("Could not allocate dynamic memory for the block group descriptor"); exit(1);
	}
	if (pread(fileSystemImage, blockGroupDescriptor, blockGroupDescriptorSize * numGroups, blockGroupDescriptorOffset) == -1) {
		perror("Could not read the block group descriptor"); exit(1);
	}

	// create new files for output
	FILE* directorycsv = fopen("directory.csv", "w");  
	FILE* indirectcsv = fopen("indirect.csv", "w");  

	// counters to determine which blocks and which inodes are free
	// important to note that the first block is at index 0 so count needs to start at 1
	int inodeCount = 1;

	// iterate thorugh all of the group descriptors
	int i;
	for (i = 0; i < numGroups; i++) {
		// get the (free) inode bitmap block
		int inodeBitmapBlock = *((int*)(blockGroupDescriptor + 4));
		// get the inode table (start) block
		int inodeTableStartBlock = *((int*)(blockGroupDescriptor + 8));

		// get the memory for the inode bitmap block (2048/8 bytes since 2048 inodes and 8 bits per byte)
		void* inodeBitmap = malloc(superblockData.blockSize);
		if (inodeBitmap == NULL) {
			perror("Could not allocate dynamic memory for the inode bitmap block"); exit(1);
		}
		if (pread(fileSystemImage, inodeBitmap, 2048/8, inodeBitmapBlock * superblockData.blockSize) == -1) {
			perror("Could not read the inode bitmap block"); exit(1);
		}

		// store the inode offset for this group
		int currGroupInodeOffset = 0;
		// 2048/8 = 256, we want to go over all of them so use a uint8_t to go 1 byte at a time
		// use as the byte offset from the beginning of the bitmap
		// go from left to right
		int j;
		for (j = 0; j < 256; j++) { 
			uint8_t inodeBits = *((uint8_t*)(inodeBitmap + j));
			// go over each of the 8 bits from right to left
			uint8_t currBit = 1;  
			int k;
			for (k = 0; k < 8; k++) {
				// the inode is not free AKA it is already allocated
				if ((currBit & inodeBits) != 0) {
					int inodeNumber = inodeCount;
					// read in the data for the current inode
					void* currentInode = malloc(superblockData.inodeSize);
					if (currentInode == NULL) {
						perror("Could not allocate dynamic memory for the allocated inode"); exit(1);
					}
					if (pread(fileSystemImage, currentInode, superblockData.inodeSize, (inodeTableStartBlock * superblockData.blockSize + currGroupInodeOffset*superblockData.inodeSize) ) == -1) {
						perror("Could not read the allocated inode"); exit(1);
					}
					// get key information for the allocated inode
					uint16_t mode = *((uint16_t*)(currentInode + 0));
					uint32_t* iBlock = (uint32_t*)(currentInode + 40);
					uint16_t linkCount = *((uint16_t*)(currentInode + 26));
					// see if the inode is referencing a directory
					if (mode & 0x4000) {
						// Every file in the EXT2 file system is described by a single inode and each inode has a single unique number identifying it. The inodes for the file system are all kept together in inode tables. EXT2 directories are simply special files (themselves described by inodes) which contain pointers to the inodes of their directory entries.
						// save the inode number of the directory containing all the entries
						int parentInodeNumber = inodeNumber;
						// TOOD: fix the entryNumber bug?????
						int entryNumber = 0;
						// get the 15 block pointers
						uint32_t blockPointers[15];
						int l;
						for (l = 0; l < 15; l++) {
							// make sure the block pointer isn't NULL
							if ((iBlock + l) != NULL) 
								blockPointers[l] = *(iBlock + l);
							else
								blockPointers[l] = 0;
						}
						// get the directory entries (stored in a linked list); the start of the list is in the first data block
						// loop through all the direct block pointers
						for (l = 0; l < 12; l++) {
							// skip the null pointers
							if (blockPointers[l] == 0) {
								// still need to increment the number of entries we have seen
								entryNumber++;
								continue;
							}
							// the block pointer is not NULL, so process the directory entries in it
							processDirectoryEntriesInBlock(fileSystemImage, blockPointers[l], directorycsv, &entryNumber, parentInodeNumber);
						}
						// go through the indirect pointers
						if (blockPointers[12] != 0) {
							// get all the block pointers associated with the indirect block
							// read in the data for the current inode
							uint32_t* indirectBlockPointers = (uint32_t*)malloc(superblockData.blockSize);
							if (indirectBlockPointers == NULL) {
								perror("Could not allocate dynamic memory for the indirect block pointers"); exit(1);
							}
							if (pread(fileSystemImage, indirectBlockPointers, superblockData.blockSize, blockPointers[12] * superblockData.blockSize) == -1) {
								perror("Could not read the allocated block"); exit(1);
							}
							int numIndirectBlocks = superblockData.blockSize / 4;  // since 32 bits = 4 bytes
							// go through all the indirect blocks
							int i;
							for (i = 0; i < numIndirectBlocks; i++) {
								// skip the null pointers
								if (indirectBlockPointers[i] == 0) {
									entryNumber++;
									continue;
								}
								// validate that the indirect block pointer is within the superblock specified range
								if (indirectBlockPointers[i] > superblockData.totalNumBlocks) {
									perror("Indirect block number is outside of the superblock specified range"); exit(1);
								}
								// print out the statistics for this indirect block
								fprintf(indirectcsv, "%x,%d,%x\n", blockPointers[12], i, indirectBlockPointers[i]);
								// the block pointer is not NULL, so process the directory entries in it
								processDirectoryEntriesInBlock(fileSystemImage, indirectBlockPointers[i], directorycsv, &entryNumber, parentInodeNumber);
							}
						}
						// go through the doubly indirect pointers
						if (blockPointers[13] != 0) {
							// get all the block pointers associated with the indirect block
							// read in the data for the current inode
							uint32_t* doublyIndirectBlockPointers = (uint32_t*)malloc(superblockData.blockSize);
							if (doublyIndirectBlockPointers == NULL) {
								perror("Could not allocate dynamic memory for the indirect block pointers"); exit(1);
							}
							if (pread(fileSystemImage, doublyIndirectBlockPointers, superblockData.blockSize, blockPointers[13] * superblockData.blockSize) == -1) {
								perror("Could not read the allocated block"); exit(1);
							}
							int numDoublyIndirectBlocks = superblockData.blockSize / 4;  // since 32 bits = 4 bytes
							// go through all the indirect blocks
							int i;
							for (i = 0; i < numDoublyIndirectBlocks; i++) {
								// skip the null pointers
								if (doublyIndirectBlockPointers[i] == 0) {
									entryNumber++;
									continue;
								}
								if (doublyIndirectBlockPointers[i] > superblockData.totalNumBlocks) {
									perror("Doubly indirect block number is outside of the superblock specified range"); exit(1);
								}
								// print out the statistics for this doubly indirect block
								fprintf(indirectcsv, "%x,%d,%x\n", blockPointers[13], i, doublyIndirectBlockPointers[i]);
								// the block pointer is not NULL, so get the indirect pointers
								uint32_t* indirectBlockPointers = (uint32_t*)malloc(superblockData.blockSize);
								if (indirectBlockPointers == NULL) {
									perror("Could not allocate dynamic memory for the indirect block pointers"); exit(1);
								}
								if (pread(fileSystemImage, indirectBlockPointers, superblockData.blockSize, doublyIndirectBlockPointers[i] * superblockData.blockSize) == -1) {
									perror("Could not read the allocated inode"); exit(1);
								}
								int numIndirectBlocks = superblockData.blockSize / 4;  // since 32 bits = 4 bytes
								// go through all the indirect blocks
								int j;
								for (j = 0; j < numIndirectBlocks; j++) {
									// skip the null pointers
									if (indirectBlockPointers[j] == 0) {
										entryNumber++;
										continue;
									}
									if (indirectBlockPointers[j] > superblockData.totalNumBlocks) {
									perror("Indirect block number is outside of the superblock specified range"); exit(1);
									}
									// print out the statistics for this indirect block
									fprintf(indirectcsv, "%x,%d,%x\n", doublyIndirectBlockPointers[i], j, indirectBlockPointers[j]);
									// the block pointer is not NULL, so process the directory entries in it
									processDirectoryEntriesInBlock(fileSystemImage, indirectBlockPointers[j], directorycsv, &entryNumber, parentInodeNumber);
								}
							}

						}
						// go through the triply indirect pointers
						if (blockPointers[14] != 0) {
							uint32_t* triplyIndirectBlockPointers = (uint32_t*)malloc(superblockData.blockSize);
							if (triplyIndirectBlockPointers == NULL) {
								perror("Could not allocate dynamic memory for the indirect block pointers"); exit(1);
							}
							if (pread(fileSystemImage, triplyIndirectBlockPointers, superblockData.blockSize, blockPointers[14] * superblockData.blockSize) == -1) {
								perror("Could not read the allocated block"); exit(1);
							}
							int numTriplyIndirectBlocks = superblockData.blockSize / 4;  // since 32 bits = 4 bytes
							// go through all the indirect blocks
							int i;
							for (i = 0; i < numTriplyIndirectBlocks; i++) {
								// skip the null pointers
								if (triplyIndirectBlockPointers[i] == 0) {
									entryNumber++;
									continue;
								}
								if (triplyIndirectBlockPointers[i] > superblockData.totalNumBlocks) {
									perror("Triply indirect block number is outside of the superblock specified range"); exit(1);
								}
								// print out the statistics for this triply indirect block
								fprintf(indirectcsv, "%x,%d,%x\n", blockPointers[14], i, triplyIndirectBlockPointers[i]);
								// get all the block pointers associated with the indirect block
								// read in the data for the current inode
								uint32_t* doublyIndirectBlockPointers = (uint32_t*)malloc(superblockData.blockSize);
								if (doublyIndirectBlockPointers == NULL) {
									perror("Could not allocate dynamic memory for the indirect block pointers"); exit(1);
								}
								if (pread(fileSystemImage, doublyIndirectBlockPointers, superblockData.blockSize, triplyIndirectBlockPointers[i] * superblockData.blockSize) == -1) {
									perror("Could not read the allocated block"); exit(1);
								}
								int numDoublyIndirectBlocks = superblockData.blockSize / 4;  // since 32 bits = 4 bytes
								// go through all the indirect blocks
								int j;
								for (j = 0; j < numDoublyIndirectBlocks; j++) {
									// skip the null pointers
									if (doublyIndirectBlockPointers[j] == 0) {
										entryNumber++;
										continue;
									}
									if (doublyIndirectBlockPointers[j] > superblockData.totalNumBlocks) {
										perror("Doubly indirect block number is outside of the superblock specified range"); exit(1);
									}	
									// print out the statistics for this doubly indirect block
									fprintf(indirectcsv, "%x,%d,%x\n", triplyIndirectBlockPointers[i], j, doublyIndirectBlockPointers[j]);
									// the block pointer is not NULL, so get the indirect pointers
									uint32_t* indirectBlockPointers = (uint32_t*)malloc(superblockData.blockSize);
									if (indirectBlockPointers == NULL) {
										perror("Could not allocate dynamic memory for the indirect block pointers"); exit(1);
									}
									if (pread(fileSystemImage, indirectBlockPointers, superblockData.blockSize, doublyIndirectBlockPointers[j] * superblockData.blockSize) == -1) {
										perror("Could not read the allocated inode"); exit(1);
									}
									int numIndirectBlocks = superblockData.blockSize / 4;  // since 32 bits = 4 bytes
									// go through all the indirect blocks
									int k;
									for (k = 0; k < numIndirectBlocks; k++) {
										// skip the null pointers
										if (indirectBlockPointers[k] == 0) {
											entryNumber++;
											continue;
										}
										if (indirectBlockPointers[k] > superblockData.totalNumBlocks) {
											perror("Indirect block number is outside of the superblock specified range"); exit(1);
										}
										// print out the statistics for this indirect block
										fprintf(indirectcsv, "%x,%d,%x\n", doublyIndirectBlockPointers[j], k, indirectBlockPointers[k]);
										// the block pointer is not NULL, so process the directory entries in it
										processDirectoryEntriesInBlock(fileSystemImage, indirectBlockPointers[k], directorycsv, &entryNumber, parentInodeNumber);
									}
								}
							}
						}
					}	
					
					// free the dynamic memory for the inode
					free(currentInode);
				}
				// update which inode in the group we are currently on
				currGroupInodeOffset++;
				// increment iterator for which block we are checking
				inodeCount++;
				// shifting an unsigned value to right is a logical shift
				currBit = currBit << 1;
			}
		}

		// free dynamically allocated memory for the inode bitmap
		free(inodeBitmap);

		// increment by 32 bytes because that's the size of the block group descriptor
		// GCC allows you to add to void pointer by defining sizeof(void *) = 1
		blockGroupDescriptor += 32;
	}
}

int main(int argc, char **argv) {
	// read the file system image from the provided file name
	if (argc != 2) {
		perror("Invalid number of arguments specified"); exit(1);
	}
	char* fileSystemImageName = argv[1];
	// open the file system image file
	int fileSystemImage = open(fileSystemImageName, O_RDONLY);
	
	// part 1 
	superblockAnalysis(fileSystemImage);

	// parts 2, 3, & 4
	groupDescriptorAnalysis(fileSystemImage);

	// part 5 & 6 
	directoryEntriesAnalysis(fileSystemImage);
}