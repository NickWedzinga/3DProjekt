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
		std::vector<uint> indices; //indices for the billboard vertex vector array
		XMFLOAT3 bottomLeft;
		XMFLOAT3 topRight;
		XMFLOAT3 center;
		XMFLOAT3 halfDiagonal;
	};
public:
	QuadTree(int levels);
	~QuadTree();
	uint FindParent(uint nodeIndex);
	void FindChildren(uint nodeIndex, int* children);
	uint GetFirstLeaf();
	uint GetNumOfNodes();
	uint GetNumOfLeaves();
	XMFLOAT3 GetBottomLeft(uint index);
	XMFLOAT3 GetTopRight(uint index);
	XMFLOAT3 GetCenter(uint index);
	void pushVertexIndex(uint treeIndex, uint index);
	void Culling(uint index, Camera* camera, Billboard* billboard);
	int getLevels();
	void FillLeaves(int index, uint bbsPerNode);
	bool HitBoundingBox(uint index, XMVECTOR* frustum);

private:
	void setTreeCoordinates(uint start, uint level);
	int levels;
	int firstLeaf;
	std::vector<Node> tree;
};

#endif // !QUADTREE_H
