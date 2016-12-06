#include "QuadTree.h"

QuadTree::QuadTree(int levels)
{
	Node temp;
	this->levels = levels;
	int nodes = 0;
	for (int i = 0; i < levels; ++i)
	{
		nodes += pow(4, i);
	}
	for (int i = 0; i < nodes; ++i)
	{
		tree.push_back(temp);
	}
}

QuadTree::~QuadTree()
{

}

void QuadTree::CreateQuadTree()
{

}

int QuadTree::FindParent(int nodeIndex)
{
	return ((nodeIndex + 3)/4)-1;
}

void QuadTree::FindChildren(int nodeIndex, int* children)
{
	for (int i = 1; i < 5; i++)
	{
		children[i] = nodeIndex * 4 + i;
	}
}

