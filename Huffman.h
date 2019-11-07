/*
 * File Name: Huffman.h
 * Date: 10/18/2019
 * Course: EECS 2510 Non-Linear Data Structures
 * Author: Mike Baldwin
 * Brief Description: This file contains the definitions of the Huffman encoding class as well as the definitions and declarations of the Node structure.
*/

#pragma once

#include <string>
#include <fstream>

using namespace std;

class Huffman
{
	/*
	 * Huffman class. Contains everything needed to encode and decode files.
	*/

public:
	void EncodeFile(string inputFilePath, string outputFilePath);
	void DecodeFile(string inputFilePath, string outputFilePath);
	void MakeTreeBuilder(string inputFilePath, string outputFilePath);
	void EncodeFileWithTree(string inputFilePath, string outputFilePath, string treeFilePath);
	void DisplayHelp();

private:
	struct Node //Node structure
	{
		Node* left; // Left child node pointer
		Node* right; // Right child node pointer
		int symbol; // What symbol is in this node (yes, I know it's an integer)
		int count; // The frequency count of the node

		Node(int symbol, int count)
		{
			/* Node constructor, only used in allocating begging node structure */
			this->symbol = symbol;
			this->count = count;
			this->left = nullptr;
			this->right = nullptr;
		};
		Node(Node* left, Node* right)
		{
			/* Node constructor, used when beuilding subtrees */
			symbol = left->symbol + right->symbol;
			count = left->count + right->count;
			this->left = left;
			this->right = right;
		};

		bool IsLeaf() { /* Is this node a leaf or not */return left == nullptr && right == nullptr; };
	};

	Node* BuildTree(ifstream &inputStream, ofstream &outputStream, unsigned char rows[]);
	Node* BuildTree(unsigned char rows[]);
	void CalculateFrequencyCounts(ifstream& inputStream, Node *nodes[]);
	void BuildSubTree(Node* nodes[], unsigned char rows[], int rowIndex);
	void BuildSubFromRows(Node* nodes[], unsigned char rows[], int rowIndex);
	void TraverseAndBuild(Node *node, string bstring, string* bStrings);
	void EncodeAndWrite(ifstream& inputStream, ofstream& outputStream, string* bStrings);
	void DecodeAndWrite(ifstream& inputStream, ofstream& outputStream, Node* root);
	string FindPaddingBits(string* bStrings, int paddingLength);
};