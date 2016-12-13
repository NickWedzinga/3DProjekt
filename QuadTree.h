#pragma once
#ifndef QUADTREE_H
#define QUADTREE_H

#include "includes.h"
#include "Billboard.h"

class Camera;	//Because circular includes, forward declaration of Camera because Camera has QuadTree included.

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
	uint FindParent(uint nodeIndex);
	void FindChildren(uint nodeIndex, int* children);
	uint GetFirstLeaf();
	uint GetNumOfNodes();
	uint GetNumOfLeaves();
	XMINT2 GetBottomLeft(uint index);
	XMINT2 GetTopRight(uint index);
	void pushVertexIndex(uint treeIndex, uint index);
	void Culling(uint index, Camera* camera, Billboard* billboard);
	int getLevels();
	void FillLeaves(int index, uint bbsPerNode);

private:
	void setTreeCoordinates(uint start, uint level);
	float DistanceToPoint(XMVECTOR plane, XMINT3 point);

	int levels;
	int firstLeaf;
	std::vector<Node> tree;
};

#endif // !QUADTREE_H
