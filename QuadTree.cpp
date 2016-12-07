#include "QuadTree.h"

QuadTree::QuadTree(int levels)
{
	Node temp;
	this->levels = levels;
	int nodes = 0;
	for (int i = 0; i <= levels; ++i)
	{
		nodes += pow(4, i);
		if (i == levels - 1)
			firstLeaf = nodes;
	}
	for (int i = 0; i < nodes; ++i)
	{
		tree.push_back(temp);
	}
	setTreeCoordinates(0, 0);
}

QuadTree::~QuadTree()
{

}

void QuadTree::setTreeCoordinates(uint index, uint level)	//0, 0
{
	uint nuIndex = index;
	if (index == 0)
	{
		tree[0].bottomLeft = XMINT2(0, 0);
		tree[0].topRight = XMINT2(TWIDTH - 1, THEIGHT - 1);
		nuIndex = 1;
	}
	if (levels != 0)
	{
		uint parent = FindParent(nuIndex);		//0
		XMINT2 min = tree[parent].bottomLeft;	//0,0
		XMINT2 max = tree[parent].topRight;		//255, 255
		uint iter = 0;
		for (uint i = 0; i < 2; ++i)
		{
			for (uint j = 0; j < 2; ++j)
			{
				tree[nuIndex + iter].bottomLeft = XMINT2(min.x + ((max.x - min.x) / 2) * j, min.y + ((max.y - min.y) / 2) * i);
				tree[nuIndex + iter].topRight = XMINT2(max.x - ((max.x - min.x + 1) / 2) * (1 - j), max.y - ((max.y - min.y + 1) / 2) * (1 - i));
				if (level != levels - 1)
					setTreeCoordinates((nuIndex + iter) * 4 + 1, level + 1);
				++iter;
			}
		}
	}
}


uint QuadTree::FindParent(int nodeIndex)
{
	return ((nodeIndex + 3)/4) - 1;
}

void QuadTree::FindChildren(int nodeIndex, int* children) //remember to check children for nullptr when calling
{
	if (nodeIndex < firstLeaf)
	{
		for (int i = 1; i < 5; i++)
		{
			children[i] = nodeIndex * 4 + i;
		}
	}
	else
	{
		children = nullptr;
	}
}

uint QuadTree::GetFirstLeaf()
{
	return firstLeaf;
}

uint QuadTree::GetNumOfNodes()
{
	return tree.size();
}

uint QuadTree::GetNumOfLeaves()
{
	return GetNumOfNodes() - GetFirstLeaf();
}

XMINT2 QuadTree::GetBottomLeft(uint index)
{
	return tree[index].bottomLeft;
}

XMINT2 QuadTree::GetTopRight(uint index)
{
	return tree[index].topRight;
}

void QuadTree::pushVertexIndex(uint treeIndex, uint index)
{
	tree[treeIndex].index.push_back(index);
}

