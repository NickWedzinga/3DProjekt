#pragma once
#ifndef QUADTREE_H
#define QUADTREE_H

#include "includes.h"
#include "Billboard.h"


class QuadTree
{
private:
	struct Node
	{
		std::vector<uint> index;
		XMINT2 bottomLeft;
		XMINT2 topRight;
	};
public:
	QuadTree(int levels);
	~QuadTree();
	uint FindParent(int nodeIndex);
	void FindChildren(int nodeIndex, int* children);
	uint GetFirstLeaf();
	uint GetNumOfNodes();
	uint GetNumOfLeaves();
	XMINT2 GetBottomLeft(uint index);
	XMINT2 GetTopRight(uint index);
	void pushVertexIndex(uint treeIndex, uint index);
	void Culling(uint index, XMVECTOR planes[6], Billboard* billboard);
	
private:
	void setTreeCoordinates(uint start, uint level);
	float DistanceToPoint(XMVECTOR plane, XMINT2 point);

	int levels;
	int firstLeaf;
	std::vector<Node> tree;
};

#endif // !QUADTREE_H
