/*
 * File Name: Huffman.cpp
 * Date: 10/18/2019
 * Course: EECS 2510 Non-Linear Data Structures
 * Author: Mike Baldwin
 * Brief Description: This file contains the main implementation of my Huffman encoding algorithm and class.
*/

#include <string>
#include <iostream>
#include <fstream>
#include "Huffman.h"
#include "LookupTables.h"

using namespace std;

// This constant is used for fine tuning the speed/memory ratio. It represents how large the read/write buffers should be when accessing files
const int READ_WRITE_BUFFER_SIZE = 8 * 1024 * 1024; // TotalBytes = #ofMegaBytes * 1024(KB) * 1024(B)

void Huffman::EncodeFile(string inputFilePath, string outputFilePath)
{
	/*
	 * Encodes a file specified by inputFilePath to an output file, specified by outputFilePath (optional)
	 */

	 // Open the input file and check that it opened correctly
	ifstream inputStream;
	inputStream.open(inputFilePath, ios::binary);
	if (!inputStream.is_open())
	{
		cout << "Input file cannot be opened! Please check that the drive exists and that it is NOT read-only." << endl;
		return;
	}

	// Open the output file and check that it opened correctly
	ofstream outputStream;
	outputStream.open(outputFilePath, ios::binary);
	if (!outputStream.is_open())
	{
		cout << "Output file cannot be opened! Please check that the drive exists and that it is NOT read-only." << endl;
		return;
	}

	unsigned char rows[510]; // This is the tree-builder rows that are written to a .htree file
	Node* root; // Root node, no need to have it declared in the class as it's only really used here
	string bStrings[256]; // Array of all the possible string paths to each of the 256 characters in the huffman tree (once it's built!)
	string tempBString; // Temporary variable, only used for providing a spot for each traversal to write to, it used for calculating the paths to each leaf node.

	root = BuildTree(inputStream, outputStream, rows); // Build the tree, no output to file!
	TraverseAndBuild(root, tempBString, bStrings); // Traverse through the entire tree, building up the paths and storing them in the bStrings array
	EncodeAndWrite(inputStream, outputStream, bStrings); // Go back through the file, converting and writing all the data to the outputStream

	// Close the streams
	if (inputStream.is_open())
		inputStream.close();
	if (outputStream.is_open())
		outputStream.close();
}

void Huffman::DecodeFile(string inputFilePath, string outputFilePath)
{
	/*
	 * Decodes a file located at inputFilePath and writes the now decoded file to outputFilePath
	 */

	 // Open the input file and check that it opened correctly
	ifstream inputStream;
	inputStream.open(inputFilePath, ios::binary);
	if (!inputStream.is_open())
	{
		cout << "Input file cannot be opened! Please check that the drive exists and that it is NOT read-only." << endl;
		return;
	}

	// Open the output file and check that it opened correctly
	ofstream outputStream;
	outputStream.open(outputFilePath, ios::binary);
	if (!outputStream.is_open())
	{
		cout << "Output file cannot be opened! Please check that the drive exists and that it is NOT read-only." << endl;
		return;
	}

	// Declare, init, and read the tree-builder info from the inputStream file
	unsigned char rows[510];
	inputStream.read((char*)&rows, 510);
	Node* root = BuildTree(rows);
	DecodeAndWrite(inputStream, outputStream, root);

	// Close the streams
	if (inputStream.is_open())
		inputStream.close();
	if (outputStream.is_open())
		outputStream.close();
}

void Huffman::MakeTreeBuilder(string inputFilePath, string outputFilePath)
{
	/*
	 * Generates a .htree file from an input file (supplied by inputFilePath) and writes the output to an configurable outputFilePath.
	*/

	// Open the input file and check that it opened correctly
	ifstream inputStream;
	inputStream.open(inputFilePath, ios::binary);
	if (!inputStream.is_open())
	{
		cout << "Input file cannot be opened! Please check that the drive exists and that it is NOT read-only." << endl;
		return;
	}

	// Open the output file and check that it opened correctly
	ofstream outputStream;
	outputStream.open(outputFilePath, ios::binary);
	if (!outputStream.is_open())
	{
		cout << "Output file cannot be opened! Please check that the drive exists and that it is NOT read-only." << endl;
		return;
	}

	// Declare our local var rows and call the build tree function
	unsigned char rows[510];
	BuildTree(inputStream, outputStream, rows);

	// Close the streams
	if (inputStream.is_open())
		inputStream.close();
	if (outputStream.is_open())
		outputStream.close();
}

void Huffman::EncodeFileWithTree(string inputFilePath, string outputFilePath, string treeBuilderFilePath)
{
	/*
	 * Encodes a file, specified by inputFilePath, using the tree building data that will found at treeBuilderFilePath.
	 * Then it will output the encoded data into a file at outputFilePath (optional).
	 */

	 // Open the input file and check that it opened correctly
	ifstream inputStream;
	inputStream.open(inputFilePath, ios::binary);
	if (!inputStream.is_open())
	{
		cout << "Input file cannot be opened! Please check that the drive exists and that it is NOT read-only." << endl;
		return;
	}

	// Open the input file and check that it opened correctly
	ifstream inputTreeStream;
	inputTreeStream.open(treeBuilderFilePath, ios::binary);
	if (!inputTreeStream.is_open())
	{
		cout << "Input Tree-Builder file cannot be opened! Please check that the drive exists and that it is NOT read-only." << endl;
		return;
	}

	// Open the output file and check that it opened correctly
	ofstream outputStream;
	outputStream.open(outputFilePath, ios::binary);
	if (!outputStream.is_open())
	{
		cout << "Output file cannot be opened! Please check that the drive exists and that it is NOT read-only." << endl;
		return;
	}

	// Declare, init, and read the tree-builder info from the inputStream file
	unsigned char rows[510];
	inputTreeStream.read((char*)&rows, 510);
	Node* root = BuildTree(rows);
	string bStrings[256];
	string tempBString;

	outputStream.write((char*)rows, 510); // Write the tree builder data as the header
	TraverseAndBuild(root, tempBString, bStrings); // Traverse through the entire tree, building up the paths and storing them in the bStrings array
	EncodeAndWrite(inputStream, outputStream, bStrings); // Go back through the file, converting and writing all the data to the outputStream

	// Close the streams
	if (inputStream.is_open())
		inputStream.close();
	if (inputTreeStream.is_open())
		inputStream.close();
	if (outputStream.is_open())
		outputStream.close();
}

void Huffman::DisplayHelp()
{
	/*
	 * Displays the useful functionality of the program to the user
	*/

	cout << "-e file1 [file2]			: Encodes File1, placing the output into File2 (optional)" << endl;
	cout << "-d file1 file2				: Decodes File1, placing it into File2" << endl;
	cout << "-t file1 [file2]			: Produces the tree builder file from File1 and places it into File2 (optional)" << endl;
	cout << "-et file1 file2 [file3]	: Encode file1 using a prebuild tree in file2, and placing the output inot file3 (optional)" << endl;
	cout << "-help || -? || -h			: displays this list of available commands" << endl;
}

Huffman::Node* Huffman::BuildTree(ifstream& inputStream, ofstream& outputStream, unsigned char rows[])
{
	/*
	 * Builds the tree from the input file stream and outputs it to the output file stream.
	*/

	// Declare my return var and the temporary nodes array 
	Node* root = nullptr;
	Node* nodes[256];

	// Pre-allocate the nodes array.
	for (int i = 0; i < 256; i++)
		nodes[i] = new Node(i, 0);

	// Build up the freq array
	CalculateFrequencyCounts(inputStream, nodes);

	// Loop over the nodes, building not only the individual subtrees, but also saving the tree-building info
	for (int i = 0, rowIndex = 0; i < 255; i++, rowIndex += 2)
		BuildSubTree(nodes, rows, rowIndex);

	// Find the root, it *might* be at index 0, if not, search the tree
	root = nodes[0];
	if (root == nullptr)
		for (int i = 0; i < 256; i++)
			if (nodes[i] != nullptr)
				root = nodes[i];

	// Dump the whole row array into a file
	outputStream.write((char*)rows, 510);

	// Return the root back up the call stack
	return root;
}

Huffman::Node* Huffman::BuildTree(unsigned char rows[])
{
	/*
	 * Builds the tree from tree-building data supplied in the rows array. Returns the root pointer
	 */

	 // Declare my vars
	Node* root = nullptr;
	Node* nodes[256];

	// Pre-allocate the nodes array.
	for (int i = 0; i < 256; i++)
		nodes[i] = new Node((unsigned char)i, 0);

	// Loop over the nodes building not only the individual subtrees
	for (int i = 0, rowIndex = 0; i < 255; i++, rowIndex += 2)
		BuildSubFromRows(nodes, rows, rowIndex);

	// Find the root, it *might* be at index 0, if not, search the tree
	root = nodes[0];
	if (root == nullptr)
		for (int i = 0; i < 256; i++)
			if (nodes[i] != nullptr)
				root = nodes[i];

	// Return the root back up the call stack
	return root;
}

void Huffman::CalculateFrequencyCounts(ifstream& inputStream, Node* nodes[])
{
	/*
	 * Builds up an array of character frequences from an input file.
	*/

	// This is where we get our first glimps into the file buffering. We will load the file in a rotating buffer

	unsigned char* buffer = new unsigned char[READ_WRITE_BUFFER_SIZE]; // We HAVE to dynamically allocate this to avoid a stack overflow, it must be put onto the heap!!!

	while (!inputStream.eof()) // Keep looping until the end of file is reached
	{
		inputStream.read((char*)buffer, READ_WRITE_BUFFER_SIZE); // Read 'up to' the buffer size, the actual bytes read might be less
		for (streamsize i = 0; i < inputStream.gcount(); i++) // Loop over however many bytes were read last by the inputStream
		{
			unsigned char c = buffer[i]; // Get the next character in the buffer
			nodes[c]->count++; // Increment the count of whatever character was read
		}
	}

	// Free the mems
	delete[] buffer;
}

void Huffman::BuildSubTree(Node* nodes[], unsigned char rows[], int rowIndex)
{
	/*
	 * Finds the lowest and second lowest nodes, parents them to a new 'parent' node.
	 */

	int lowest = INT_MAX; // Lowest 'count' encountered in this pass
	int secondLowest = INT_MAX; // Second lowest 'count' found in the pass
	int lowestIndex = -1; // The index where the lowest 'coun' was found
	int secondLowestIndex = -1; // The index where the second lowest 'count' was found

	// Find the lowest count node
	for (int i = 0; i < 256; i++)
	{
		if (nodes[i] == nullptr) // If the node is null, continue with the next iteration
			continue;
		if (nodes[i]->count < lowest) // If this nodes 'count' is lower than the currently lowest 'count', then set the lowest to this new lowest, and the lowest index to this current index
		{
			lowest = nodes[i]->count;
			lowestIndex = i;
		}
	}

	// Find the second lowest count node
	for (int i = 0; i < 256; i++)
	{
		if (nodes[i] == nullptr) // If the node is null, continue with the next iteration
			continue;
		if (nodes[i]->count < secondLowest && i != lowestIndex) // If this nodes 'count' is lower than the currently secondLowest 'count', then set the secondLowest to this new secondLowest, and the lowest secondLowestIndex to this current index
		{
			secondLowest = nodes[i]->count;
			secondLowestIndex = i;
		}
	}

	// Declare which node is the left & right child as well as their specific indices.
	int leftIndex = lowestIndex < secondLowestIndex ? lowestIndex : secondLowestIndex < lowestIndex ? secondLowestIndex : lowestIndex; // Basically min(lowestIndex, secondLowestIndex)
	int rightIndex = leftIndex == lowestIndex ? secondLowestIndex : lowestIndex; // Assign whatever index was NOT assigned to leftIndex previously
	Node* leftNode = nodes[leftIndex]; // Get the left node at the lowest index
	Node* rightNode = nodes[rightIndex]; // Get the right node at the second lowest index

	// Set the correct row info, create a new parent node, with the children in tow, null out the old spot of the right child.
	rows[rowIndex] = leftIndex; // Set the row number to whatever the leftIndex was
	rows[rowIndex + 1] = rightIndex; // Set the next row number to the rightIndex
	nodes[leftIndex] = new Node(leftNode, rightNode); // Create a parent node, having the leftNode and rightNode as the left and right children
	nodes[rightIndex] = nullptr; // Set the rightIndex to null, for the next pass
}

void Huffman::BuildSubFromRows(Node* nodes[], unsigned char rows[], int rowIndex)
{
	/*
	 * Builds a subtree from a list of nodes, rows, and the rowIndex. Used for rebuilding the tree when decoding.
	*/

	int leftIndex = rows[rowIndex]; // Gets the first index form the first row
	int rightIndex = rows[rowIndex + 1]; // Gets the second index from the rowIndex + 1
	Node* leftNode = nodes[leftIndex]; // left Index into the nodes array is the left child
	Node* rightNode = nodes[rightIndex]; // Right index into the nodes array is the right child
	nodes[leftIndex] = new Node(leftNode, rightNode); // Create a parent node and set the left and right children
	nodes[rightIndex] = nullptr; // Set the right nodes index to null, allows us to search for the root later
}

void Huffman::TraverseAndBuild(Node* node, string bstring, string* bStrings)
{
	/*
	 * Non-trivial recursive traversal function. It builds up the individual path strings for each character in the tree.
	*/

	// Make sure we arn't going to get a null pointer exception
	if (node != nullptr)
	{
		// If the left child is not null, traverse through that subtree
		if (node->left != nullptr)
		{
			bstring.push_back('0'); // Add a '0' when moving down a left branch
			TraverseAndBuild(node->left, bstring, bStrings); // Traverse down the left subtree
			bstring.pop_back(); // Remove a '0' when moving back up a left branch
		}

		// If the current node is a leaf, save the binary direction pathing string to our array, at the correct location
		if (node->IsLeaf())
		{
			bStrings[node->symbol] = bstring;
		}

		// If the right child is not null, traverse through that subtree
		if (node->right != nullptr)
		{
			bstring.push_back('1'); // Add a '1' when moving down a right branch
			TraverseAndBuild(node->right, bstring, bStrings); // Traverse down the right subtree
			bstring.pop_back(); // Remove a '1' when moving back up a right branch
		}
	}
}

void Huffman::EncodeAndWrite(ifstream& inputStream, ofstream& outputStream, string* bStrings)
{
	/*
	 * Runs through the input file, converting the characters to their binary path equivilent (as per the tree), then outputs it all to a file (with buffering).
	*/

	// Reset the stream back to the beginning
	inputStream.clear();
	inputStream.seekg(0, ios::beg);

	unsigned char* inputBuffer = new unsigned char[READ_WRITE_BUFFER_SIZE]; // Declare an input buffer used for reading in chunks of input data, MUST BE ALLOCATED ON HEAP!!
	unsigned char* outputBuffer = new unsigned char[READ_WRITE_BUFFER_SIZE]; // So this buffer is only written when either full, or the entire inputfile has been read, MUST BE ALLOCATED ON HEAP!!
	string binaryToCharBuffer; // Used for holding the 8 'bits' of each byte to be written to the larger output buffer
	int outputBufferIndex = 0; // Output buffer index, keeping track of the buffer positioning
	bool forceOutput = false; // When the end of the file is reached and the padding bits have been calculated, this bool will flip to true, to force the large output buffer to write to file

	/*
	* Keep looping until the end of the input file is reached
	* Each loop we do the following:
	*	1. Read 'up to' the buffer size, the actual bytes read might be less
	*	2. Loop over however many bytes were read last by the inputStream (gcount())
	*	3. Get the next character in the inputBuffer
	*	4. Find the correct binary string in our generated lookup table
	*	5. Store those binary 'bits' from our lookup table into the binaryToCharBuffer 'string' in groupings of 8 'bits' each
	*	6. Everytime we fill the smallOutputBuffer (8-bits), we convert that base-2 string into a base10 unsigned char and store that in the large outputBuffer
	*	7. Once the larger ouputBuffer is filled, we dump that to the outputStream
	*	8. At the very end we calculate padding bits if needed (see comments below).
	*/
	while (!inputStream.eof())
	{
		inputStream.read((char*)inputBuffer, READ_WRITE_BUFFER_SIZE);
		for (streamsize i = 0; i < inputStream.gcount(); i++)
		{
			unsigned char c = inputBuffer[i];
			string bString = bStrings[c];

			for (int j = 0; j < bString.length(); j++)
			{
				binaryToCharBuffer.push_back(bString.c_str()[j]); // Yes, substr would be faster, but I want to maintain a count added between inner loops without added complexity

				/*
				 * If the end of the input file has been reached, and we have read the last input byte stored in the inputBuffer
				 * AND we have already written the last bit from the binary string lookup table.
				 * Then we will calculate the padding needed to make our last byte written out a total of 8-bits
				 * And we will flip the forceOutputWrite flag, to force our outputBuffer to write to file whatever it has currently has.
				*/
				if (inputStream.eof() && i == inputStream.gcount() - 1 && j == bString.length() - 1)
				{
					forceOutput = true;

					if (binaryToCharBuffer.length() < 8)
					{
						int paddingLength = 8 - binaryToCharBuffer.length(); // Calculate padding length
						string padd = FindPaddingBits(bStrings, paddingLength); // Find the required padding bits
						for (int k = 0; k < padd.length(); k++)
						{
							binaryToCharBuffer.push_back(padd.c_str()[k]); // 'Padd' the end of the last byte
						}
					}
				}

				/*
				 * If the small output buffer is full we do a few things:
					1. Convert the binaryToCharBuffer (which is all '1's and '0's to a unsigned char
					2. Set that converted character into the main output buffer
					3. If the larger output buffer is full, dump it to the output stream and reset it's index (head)
				*/
				if (binaryToCharBuffer.length() == 8)
				{
					// Convert the binaryToCharBuffer into an unsigned char
					unsigned char outChar = 0;
					for (int k = 0; k < 8; k++) // Loop over all the binary digits (there's only 8)
						if (binaryToCharBuffer.c_str()[k] == '1')
							outChar |= ((unsigned char)1 << (7 - k)); // Shift it and Or it into place

					outputBuffer[outputBufferIndex] = outChar;
					outputBufferIndex++;
					binaryToCharBuffer.clear(); // Clear the small buffer in preperation for another byte

					if (outputBufferIndex == READ_WRITE_BUFFER_SIZE || (forceOutput && outputBufferIndex > 0)) // Need the (forceOutput && outputBufferIndex > 0) to prevent special cases
					{
						outputStream.write((char*)outputBuffer, outputBufferIndex);
						outputBufferIndex = 0;
					}
				}
			}
		}
	}

	// Free the memory
	delete[] outputBuffer;
	delete[] inputBuffer;
}

void Huffman::DecodeAndWrite(ifstream& inputStream, ofstream& outputStream, Node* root)
{
	/*
	 * Takes encoded input data from the inputStream, decodes it, and writes it out to the outputStream.
	 */

	Node* current = root; // This 'current' node is what is used to step through the tree
	unsigned char* inputBuffer = new unsigned char[READ_WRITE_BUFFER_SIZE]; // Declare an input buffer used for reading in chunks of input data, MUST BE ALLOCATED ON HEAP!!
	unsigned char* outputBuffer = new unsigned char[READ_WRITE_BUFFER_SIZE]; // So this buffer is only written when either full, or the entire inputfile has been read, MUST BE ALLOCATED ON HEAP!!
	int outputBufferIndex = 0; // Output buffer index, keeping track of the buffer positioning
	bool forceOutput = false; // When the end of the file is reached and the padding bits have been calculated, this bool will flip to true, to force the large output buffer to write to file

	/*
	* Keep looping until the end of the input file is reached
	* Each loop we do the following:
	*	1. Read 'up to' the buffer size, the actual bytes read might be less
	*	2. Loop over however many bytes were read last by the inputStream (gcount())
	*	3. Get the next character in the inputBuffer
	*	4. Then we use bitwise operation to split the data apart into it's indidvidual 'bits'
	*	5. Use those binary 'bits' from our buffer to step through the tree
	*	6. If the bit is '0', move to the left child; if '1', move to the right child
	*	7. Once a leaf node is reached, store the symbol at that leaf node into our output buffer
	*	8. Once the larger ouputBuffer is filled, we dump that to the outputStream
	*/
	while (!inputStream.eof())
	{
		inputStream.read((char*)inputBuffer, READ_WRITE_BUFFER_SIZE);
		for (streamsize i = 0; i < inputStream.gcount(); i++)
		{
			unsigned int byte = inputBuffer[i];
			int buffer[8]; // Declare and init a buffer to hold the byte data

			// All of this is splitting the data out into it's individual bits
			buffer[0] = (byte >> 7 & 1);
			buffer[1] = (byte >> 6 & 1);
			buffer[2] = (byte >> 5 & 1);
			buffer[3] = (byte >> 4 & 1);
			buffer[4] = (byte >> 3 & 1);
			buffer[5] = (byte >> 2 & 1);
			buffer[6] = (byte >> 1 & 1);
			buffer[7] = (byte >> 0 & 1);

			// Loop over the buffer
			for (int j = 0; j < 8; j++)
			{
				if (inputStream.eof() && i == inputStream.gcount() - 1 && j == 7) // If the end of the file is reached, and we are moving out last bit into position, force and output write
					forceOutput = true;

				// If the bit is a 0, move to the left child
				if (buffer[j] == 0)
					current = current->left;
				// If the bit is a 1, move to the right child
				if (buffer[j] == 1)
					current = current->right;
				// If we have moved to a leaf node, grab the nodes symbol, write the outputBuffer, and reset the current node to the root
				if (current->IsLeaf())
				{
					unsigned char symbol = current->symbol;
					current = root;

					outputBuffer[outputBufferIndex] = symbol; // Put the node symbol in the output buffer
					outputBufferIndex++;
				}

				if (outputBufferIndex == READ_WRITE_BUFFER_SIZE || (forceOutput && outputBufferIndex > 0)) // Need the (forceOutput && outputBufferIndex > 0) to prevent special cases
				{
					outputStream.write((char*)outputBuffer, outputBufferIndex); // Write the data buffer to the output stream
					outputBufferIndex = 0;
				}
			}
		}
	}

	// Free the memory
	delete[] outputBuffer;
	delete[] inputBuffer;
}

string Huffman::FindPaddingBits(string* bStrings, int paddingLength)
{
	/*
	 * Calculates a padding bit array to use.
	 * If for whatever reason, it failes to find one, it will first fall back to the lookup tables
	 * And then, if it still fails to find a proper solution, it will just pad with zeros and pray for mercy.
	*/

	string longestBString;
	for (int i = 0; i < 256; i++)
		if (bStrings[i].length() > longestBString.length())
			longestBString = bStrings[i];

	if (paddingLength + 1 < longestBString.length())
		return longestBString.substr(0, paddingLength); // We KNOW that for this htree, this path will lead nowhere.

	/*
	 * The padding calculations will almost never reach this point, but if they do, I want to be safe.
	 * The next hundred lines or so, are all pretty similiar. I have pre-generated lookup tables (generated from my own standalone program!)
	 * What each if statement clock is doing is 4 things:
	 *	1. The if statement first checks if the requested padding bit length is appropriate for it's own lookup table (there are 7 tables)
	 *	2. Next we will run through a loop of ALL the possible values in the lookup table
	 *	3. We will also run an inner loop of all the binaryStrings, comparing them with the current lookup table value, and if similar, we will flip a flag
	 *	4. If the flag is STILL false (no match was foound, which is good!) then we return the current lookup table string.
	*/
	bool matchFound = false; // Will be set to true if a padding combination is found while searching the lookup tables

	if (paddingLength == 1)
	{
		for (int i = 0; i < 2; i++) // Loop over pow(2,paddingLength)
		{
			for (int j = 0; j < 256; j++) // Loop over bStrings
				if (st1b[i] == bStrings[j])
					matchFound = true;
			if (!matchFound) // If this padding sequence matchs NO bString, we have a winner
				return st1b[i];
		}
	}
	if (paddingLength == 2)
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 256; j++)
				if (st2b[i] == bStrings[j])
					matchFound = true;
			if (!matchFound)
				return st2b[i];
		}
	}
	if (paddingLength == 3)
	{
		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 256; j++)
				if (st3b[i] == bStrings[j])
					matchFound = true;
			if (!matchFound)
				return st3b[i];
		}
	}
	if (paddingLength == 4)
	{
		for (int i = 0; i < 16; i++)
		{
			for (int j = 0; j < 256; j++)
				if (st4b[i] == bStrings[j])
					matchFound = true;
			if (!matchFound)
				return st4b[i];
		}
	}
	if (paddingLength == 5)
	{
		for (int i = 0; i < 32; i++)
		{
			for (int j = 0; j < 256; j++)
				if (st5b[i] == bStrings[j])
					matchFound = true;
			if (!matchFound)
				return st5b[i];
		}
	}
	if (paddingLength == 6)
	{
		for (int i = 0; i < 64; i++)
		{
			for (int j = 0; j < 256; j++)
				if (st6b[i] == bStrings[j])
					matchFound = true;
			if (!matchFound)
				return st6b[i];
		}
	}
	if (paddingLength == 7)
	{
		for (int i = 0; i < 128; i++)
		{
			for (int j = 0; j < 256; j++)
				if (st7b[i] == bStrings[j])
					matchFound = true;
			if (!matchFound)
				return st7b[i];
		}
	}

	// Backup option; This should never, ever, happen. Just pads with zeros.....ugh..
	string result;
	for (int i = 0; i < paddingLength; i++)
		result += '0';
	return result;
}