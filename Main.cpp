/*
 * File Name: Main.cpp
 * Date: 10/18/2019
 * Course: EECS 2510 Non-Linear Data Structures
 * Author: Mike Baldwin
 * Brief Description: This file contains the command line parsing logic; passing that data onto the Huffman encoding functions.
*/

#include <string>
#include <iostream>
#include <fstream>
#include <ctime>
#include <time.h>
#include <iomanip>
#include "Huffman.h"

using namespace std;

int GetFileExtensionSize(string filePath);
streamoff GetFileSize(string filePath);

int main(int argc, char* argv[])
{
	/*
	 * Main function. Parses the cmd args and passes that onto the huffman class.
	*/

	clock_t start = clock();
	Huffman huffman = Huffman(); // Huffman instance
	string command, inputFilePath, outputFilePath, treeBuilderFilePath, secondOutputFilePath; // Argument declarations

	// Pull the args from the supplied cmd args
	if (argc > 1)
		command = argv[1];
	if (argc > 2)
		inputFilePath = argv[2];
	if (argc > 3)
	{
		outputFilePath = argv[3];
		treeBuilderFilePath = argv[3];
	}
	if (argc > 4)
		secondOutputFilePath = argv[4];

	// Check to make sure a non-empty input file path was supplied
	if (inputFilePath.empty())
	{
		cout << "Input file path is empty" << endl;
		return -1;
	}

	// Check to make sure the input and output paths don't point to the same file!
	if (inputFilePath == outputFilePath)
	{
		cout << "Input and Output paths cannot equal" << endl;
		return -1;
	}

	// Test the 'first' arg to see what it is
	if (command.empty())
	{
		cout << "Invalid arguments" << endl;
	}
	else if (command == "-e")
	{
		// If the output path isn't supplied, we have the technology to generate it
		if (outputFilePath.empty())
		{
			int extSize = GetFileExtensionSize(inputFilePath);
			if (extSize > 0)
				outputFilePath = inputFilePath.substr(0, inputFilePath.length() - extSize) + ".huf";
			else
				outputFilePath = inputFilePath + ".huf";
		}

		huffman.EncodeFile(inputFilePath, outputFilePath);
	}
	else if (command == "-d")
	{
		// Check to make sure a non-empty output file path was supplied
		if (outputFilePath.empty())
		{
			cout << "Output file path is empty" << endl;
			return -1;
		}

		huffman.DecodeFile(inputFilePath, outputFilePath);
	}
	else if (command == "-t")
	{
		// If the output path isn't supplied, we have the technology to generate it
		if (outputFilePath.empty())
		{
			int extSize = GetFileExtensionSize(inputFilePath);
			if (extSize > 0)
				outputFilePath = inputFilePath.substr(0, inputFilePath.length() - extSize) + ".htree";
			else
				outputFilePath = inputFilePath + ".htree";
		}

		huffman.MakeTreeBuilder(inputFilePath, outputFilePath);
	}
	else if (command == "-et")
	{
		// Check to make sure a non-empty tree-builder file path was supplied
		if (treeBuilderFilePath.empty())
		{
			cout << "Tree-Builder file path is empty" << endl;
			return -1;
		}

		// Check to make sure the input and output paths don't point to the same file!
		if (inputFilePath == secondOutputFilePath)
		{
			cout << "Input and Output paths cannot equal" << endl;
			return -1;
		}

		// Check to make sure the input and output paths don't point to the same file!
		if (inputFilePath == treeBuilderFilePath)
		{
			cout << "Input and Tree-Builder paths cannot equal" << endl;
			return -1;
		}

		// Check to make sure the input and output paths don't point to the same file!
		if (secondOutputFilePath == treeBuilderFilePath)
		{
			cout << "Output and Tree-Builder paths cannot equal" << endl;
			return -1;
		}

		// If the output path isn't supplied, we have the technology to generate it
		if (secondOutputFilePath.empty())
		{
			int extSize = GetFileExtensionSize(inputFilePath);
			if (extSize > 0)
				secondOutputFilePath = inputFilePath.substr(0, inputFilePath.length() - extSize) + ".huf";
			else
				secondOutputFilePath = inputFilePath + ".huf";
		}

		huffman.EncodeFileWithTree(inputFilePath, secondOutputFilePath, treeBuilderFilePath);
	}
	else if (command == "-h" || command == "-?" || command == "-help")
	{
		huffman.DisplayHelp();
		return 0;
	}

	// Calculate the bytes read and written based upon file sizes and commands. Write that data to the console.
	clock_t end = clock();
	double elapsed = ((double)(end - start)) / CLOCKS_PER_SEC;
	streamoff inputBytes = command == "-et" ? GetFileSize(inputFilePath) + GetFileSize(treeBuilderFilePath) : GetFileSize(inputFilePath); // Get the input (COMBINED) bytes
	streamoff outputBytes = command == "-et" ? GetFileSize(secondOutputFilePath) : GetFileSize(outputFilePath); // Get the output bytes
	cout << "Time: " << setprecision(4) << elapsed << " seconds. " << inputBytes << " bytes in / " << outputBytes << " bytes out" << endl; // MUST set precision of the output stream before writing the time!!!
	return 0;
}

int GetFileExtensionSize(string filePath)
{
	/*
	 * Helper function to find the extension of a file (it it exists), from a file path
	 */

	size_t lastDot = filePath.find_last_of('.'); // Find the last dot character
	if (lastDot != string::npos) // If found, get the size of the extension and return that, else return 0
		return filePath.length() - lastDot;
	return 0;
}

streamoff GetFileSize(string filePath)
{
	/*
	 * Helper function to find the file size of a file (in bytes), using a path to said file
	 */

	ifstream stream(filePath, ios::binary | ios::ate); // Open the stream at the end, in binary
	streamoff size = stream.is_open() ? (int)stream.tellg() : 0; // If it opened correctly, get the head position, else 0
	stream.close(); // Close the file to free resources
	return size; // Return the found size, or zero
}