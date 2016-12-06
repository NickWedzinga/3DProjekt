#pragma once
#ifndef QUADTREE_H
#define QUADTREE_H

#include "includes.h"

class QuadTree
{
private:
	struct Node
	{
		std::vector<int> index;
	};
public:
	QuadTree(int levels);
	~QuadTree();

private:
	void CreateQuadTree();
	int FindParent(int nodeIndex);
	void FindChildren(int nodeIndex, int* children);

	int levels;
	std::vector<Node> tree;
};

#endif // !QUADTREE_H
