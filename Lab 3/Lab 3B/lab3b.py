# import required modules
from sets import Set

class Inode:
	# initialize instance variables unique to each Inode instance
	def __init__(self, inodeNumber, numberOfLinks, numBlocks):
		self.inodeNumber = inodeNumber
		self.referenceList = []  # what directory entry is pointing to this inode; this is the list of all directory entries containing this inode;  <directory inode, entry number> 
		self.numberOfLinks = numberOfLinks  # the number of links contained by this inode
		self.blockPointers = []  # the block pointers (12 direct and 3 indirect) contained by this inode
		self.numBlocks = numBlocks # the number of blocks used by this inode

class Block:
    # initialize instance variables unique to each Block instance
	def __init__(self, blockNumber):
		self.blockNumber = blockNumber
		self.referenceList = []  # what are the inodes that are pointing to this block; <inode #, indirect block #, entry number>
		# IMPORTANT: currently I just have <inode #, entry #>

class Superblock:
	# initialize instance variables unique to the Superblock instance
	def __init__(self, magicNumber, totalNumInodes, totalNumBlocks, blockSize, fragmentSize, blocksPerGroup, inodesPerGroup, fragmentsPerGroup, firstDataBlock):
		self.magicNumber = magicNumber
		self.totalNumInodes = totalNumInodes 
		self.totalNumBlocks = totalNumBlocks 
		self.blockSize = blockSize
		self.fragmentSize = fragmentSize
		self.blocksPerGroup = blocksPerGroup
		self.inodesPerGroup = inodesPerGroup
		self.fragmentsPerGroup = fragmentsPerGroup
		self.firstDataBlock = firstDataBlock

# global variables
# all of our ".csv" files containing the data of the file system
supercsv = open("super.csv", "r")
groupcsv = open("group.csv", "r")
bitmapcsv = open("bitmap.csv", "r")
inodecsv = open("inode.csv", "r")
directorycsv = open("directory.csv", "r")
indirectcsv = open("indirect.csv", "r")
# output file
output = open("lab3b_check.txt", "w")
# sets and lists containing our data
freeInodeSet = set()
freeBlockSet = set()
allocatedInodes = []  # list containing allocated inode instances
allocatedBlocks = [] # list containing instances of the block class --> group descriptor block, etc. can be stored here too 
indirectList = [] # contains the indirect blocks --> tuple or hashmap that stores block number, entry number, and pointer
directoryList = [] # contains child_directory_inode, parent_directory_inode

# read from the superblock
supercsvLine = supercsv.readline()
supercsvLine = supercsvLine.split(',')
superblock = Superblock(int(supercsvLine[0], 16), int(supercsvLine[1]), int(supercsvLine[2]), int(supercsvLine[3]), int(supercsvLine[4]), int(supercsvLine[5]), int(supercsvLine[6]), int(supercsvLine[7]), int(supercsvLine[8]))

# contains all of the block pointers for the block bitmaps
blockBitmapPointers = []
# contains all of the block pointers for the inode bitmaps
inodeBitmapPointers = []

# main function
def main():
	# figure out which block pointers correspond to block bitmaps by getting the 5th column for group.csv
	for line in groupcsv:
		# a list of all the entries in a single line of groupcsv
		groupcsvLine = line.split(',')
		# the 5th entry contains the block bitmap block pointer
		blockBitmapPointers.append(int(groupcsvLine[5], 16))
		# the 4th entry contains the inode bitmap block pointer
		inodeBitmapPointers.append(int(groupcsvLine[4], 16))

	# store all of the free blocks into freeBlockSet and all of the free inodes into freeInodeSet
	# get a list of all of the free blocks by checking which ones correspond to the block bitmap pointers
	for line in bitmapcsv:
		bitmapcsvLine = line.split(',')
		# check if this line corresponds to a block bitmap block pointer
		if int(bitmapcsvLine[0], 16) in blockBitmapPointers:
			# get free block pointer
			freeBlockSet.add(int(bitmapcsvLine[1]))
		else:
			# get free inode pointer
			freeInodeSet.add(int(bitmapcsvLine[1]))

	# store the all of the indirect blocks into the indirect list 
	for line in indirectcsv:
		indirectcsvLine = line.split(',')
		# store all of the data of the indirect block entry in a tuple
		newIndirectBlockEntry = (int(indirectcsvLine[0], 16), int(indirectcsvLine[1]), int(indirectcsvLine[2], 16))
		indirectList.append(newIndirectBlockEntry)

	# store all of the inodes
	for line in inodecsv:
		inodecsvLine = line.split(',')
		newInode = Inode(int(inodecsvLine[0]), int(inodecsvLine[5]), int(inodecsvLine[10]))
		newInode.blockPointers = inodecsvLine[11:26]
		for pointer in newInode.blockPointers:
			pointer = int(pointer, 16)
		# go through all of the data blocks of the inode
		registerBlocksForInode(newInode)
		# add the inode to the list of allocated inodes
		allocatedInodes.append(newInode)

	# go through all of the directory entries
	for line in directorycsv:
		directorycsvLine = line.split(',')
		parentInodeNumber = int(directorycsvLine[0])
		entryNumber = int(directorycsvLine[1])
		childInodeNumber = int(directorycsvLine[4])
		# get the entry and use strip() to get rid of whitespace and quotation marks
		entryName = directorycsvLine[5]
		entryName = entryName.replace('"', '').strip()

		# parentInodeNumber == 2 means root directory
		if parentInodeNumber == 2 or (entryNumber > 1 and parentInodeNumber != childInodeNumber):
			directoryList.append((childInodeNumber, parentInodeNumber));  
		# check to see if the child node is in the list of allocated inodes
		isInodeAllocated = 0
		for inode in allocatedInodes:
			if inode.inodeNumber == childInodeNumber:
				# add the directory entry to the reference list
				inode.referenceList.append((parentInodeNumber, entryNumber))
				# indicate that the inode is already allocated
				isInodeAllocated = 1
		# at the end of the loop; can see that the inode has not been allocated yet, so error
		if isInodeAllocated == 0:
			output.write("UNALLOCATED INODE < {} > REFERENCED BY DIRECTORY < {} > ENTRY < {} >\n".format(childInodeNumber, parentInodeNumber, entryNumber))

		# check '.'; child inode should point to parent inode number
		if entryNumber == 0 and parentInodeNumber != childInodeNumber:
			output.write("INCORRECT ENTRY IN < {} > NAME < {} > LINK TO < {} > SHOULD BE < {} >\n".format(parentInodeNumber, str(entryName), childInodeNumber, parentInodeNumber))

	 	# check '..';  need to get grandparentInode (parentInodeNumber of parentInodeNumber) by looking up inside of directory list and it should be equal to childInodeNumber
	 	for entry in directoryList:
	 		if entry[0] == parentInodeNumber:
	 			# this is okay because the only blocks that have their own scope are classes and function definitions
	 			# get the parent of the parent --> grandparent
	 			grandparentInodeNumber = entry[1]
	 			break 

		if entryNumber == 1 and childInodeNumber != grandparentInodeNumber:
			output.write("INCORRECT ENTRY IN < {} > NAME < {} > LINK TO < {} > SHOULD BE < {} >\n".format(parentInodeNumber, str(entryName), childInodeNumber, grandparentInodeNumber))

	# check the blocks for errors
	for block in allocatedBlocks:
		# blocks are used by more than one inodes
		if len(block.referenceList) > 1:
			# sort the reference list in place (sorts based on first element of tuple, then second, etc.)
			block.referenceList.sort()
			output.write("MULTIPLY REFERENCED BLOCK < {} > BY".format(int(block.blockNumber, 16)))
			for reference in block.referenceList:
				output.write(" INODE < {} > ENTRY < {} >".format(reference[0], reference[1]))
			output.write("\n")
		else:
			if int(block.blockNumber, 16) in freeBlockSet: 
				output.write("UNALLOCATED BLOCK < {} > REFERENCED BY".format(int(block.blockNumber, 16)))
				for reference in block.referenceList:
					output.write(" INODE < {} > ENTRY < {} >".format(reference[0], reference[1]))
				output.write("\n") 

	# check the inodes for errors
	for inode in allocatedInodes:
		# there is no file corresponding to this inode in the file system
		if inode.inodeNumber > 10 and len(inode.referenceList) == 0:
			# find out what block the corresponding bitmap should be in by 
			# find the first group where the number of free inodes is less than the number of indoes per group
			# we already read groupcsv, so move the file cursor back to the beginning
			groupcsv.seek(0)
			# the min of the range of inode numbers for this group
			inodesSeen = 0
			for line in groupcsv:
				# a list of all the entries in a single line of groupcsv
				groupcsvLine = line.split(',')
				# the max of the range of inode numbers for this group
				newInodesSeen = inodesSeen + int(superblock.inodesPerGroup)
				# the inode number is within the range
				if inode.inodeNumber >= inodesSeen and inode.inodeNumber < newInodesSeen:
					# use the free inode bitmap block as the free list
					freeList = int(groupcsvLine[4], 16)
					output.write("MISSING INODE < {} > SHOULD BE IN FREE LIST < {} >\n".format(inode.inodeNumber, freeList)) 
					break
				inodesSeen = newInodesSeen
		# link count does not reflect the number of directory entries that point to them
		if len(inode.referenceList) != inode.numberOfLinks:
			output.write("LINKCOUNT < {} > IS < {} > SHOULD BE < {} >\n".format(inode.inodeNumber, inode.numberOfLinks, len(inode.referenceList)))
		
# go through all blocks for one inode
def registerBlocksForInode(inode):
	# check the number of blocks filled
	if inode.numBlocks < 12:
		# don't need to go through any indirect blocks
		for i in range(0, inode.numBlocks):
			# error checking
			if int(inode.blockPointers[i], 16) == 0 or int(inode.blockPointers[i], 16) > superblock.totalNumBlocks:
				output.write("INVALID BLOCK < {} > IN INODE < {} > ENTRY < {} >\n".format(inode.blockPointers[i], inode.inodeNumber, i))
			# the block is valid, so save it
			else:
				registerBlock(inode.blockPointers[i], inode.inodeNumber, i)

	else:
		# save all of the direct blocks
		for i in range(0, 12):
			# error checking
			if int(inode.blockPointers[i], 16) == 0 or int(inode.blockPointers[i], 16) > superblock.totalNumBlocks:
				output.write("INVALID BLOCK < {} > IN INODE < {} > ENTRY < {} >\n".format(inode.blockPointers[i], inode.inodeNumber, i))
			else:
				registerBlock(inode.blockPointers[i], inode.inodeNumber, i)
		# also save all of the indirect blocks (need separate look since they are allowed to be 0)
		for i in range(12, 15):
			if inode.blockPointers[i] != 0:
				if int(inode.blockPointers[i], 16) > superblock.totalNumBlocks:
					output.write("INVALID BLOCK < {} > IN INODE < {} > ENTRY < {} >\n".format(inode.blockPointers[i], inode.inodeNumber, i))
				else:
					# only output the indirect pointers that aren't 0
					if int(inode.blockPointers[i], 16) != 0:
						registerBlock(inode.blockPointers[i], inode.inodeNumber, i)

# put one block in the allocatedBlocks list (need to do this for every block)
def registerBlock(blockPointer, inodeNumber, entryNumber):
	# create a new entry for the reference list for this block
	reference = (inodeNumber, entryNumber)
	# looks to see if the allocatedBlocks already has an entry with same blockPointer
	for block in allocatedBlocks:
		# have already seen the block before, so just update the reference list
		if block.blockNumber == blockPointer:
			block.referenceList.append(reference)
			return
	# first time seeing the block, so create a new block instance and save it in the list
	newBlock = Block(blockPointer)
	newBlock.referenceList.append(reference)
	allocatedBlocks.append(newBlock)

# execute the main function (this executes only if run as a script, not when imported)
if __name__ == "__main__":
    main()
