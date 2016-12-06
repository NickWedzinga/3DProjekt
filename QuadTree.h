#pragma once
#ifndef QUADTREE_H
#define QUADTREE_H

#include "includes.h"

class QuadTree
{
private:
	struct Node
	{
		std::vector<uint> index;
	};
public:
	QuadTree(int levels);
	~QuadTree();
	int FindParent(int nodeIndex);
	void FindChildren(int nodeIndex, int* children);
	uint GetFirstLeaf();
	uint GetNumOfNodes();
	uint GetNumOfLeaves();
	void pushVertexIndex(uint treeIndex, uint index);
	
private:
	void CreateQuadTree();

	int levels;
	int firstLeaf;
	std::vector<Node> tree;
};

#endif // !QUADTREE_H
