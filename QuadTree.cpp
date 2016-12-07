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

float QuadTree::DistanceToPoint(XMVECTOR plane, XMINT2 point)
{
	XMFLOAT4 temp;
	XMStoreFloat4(&temp, plane);
	return temp.x*point.x + temp.z*point.y + temp.w;
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

void QuadTree::Culling(uint index, XMVECTOR planes[6], Billboard* billboard)
{
	bool oneCornerInside = true, bool frustumInsideNode = false;
	float distance[16];
	XMINT2 corners[4];
	corners[0] = GetBottomLeft(index);						//Bottom left
	corners[1] = GetTopRight(index);						//Top right
	corners[2] = XMINT2(corners[1].x, corners[0].y);		//Bottom right
	corners[3] = XMINT2(corners[0].x, corners[1].y);		//Top left


	for (uint i = 0; i < 4; ++i)
	{
		for (uint j = 0; j < 4; ++j)
		{
			distance[i * 4 + j] = DistanceToPoint(planes[i], corners[j]);
			if (distance[i] < 0)
				oneCornerInside = false;
		}
	}

	if (oneCornerInside || frustumInsideNode)
	{
		int children[4];
		FindChildren(index, children);
		if (children != nullptr)
		{
			for (uint i = 0; i < 4; ++i)
			{
				Culling(children[i], planes, billboard); //this is where the recursion occurs + magic
			}
		}
	}
	else
	{
		uint size = tree[index].index.size()
		for(uint i; i < size; ++i)
		billboard->unused.push_back(billboard); //FIXA HÄR: göra get/set-vertices i mesh, ta bort vertiser från mesh
	}


	/*
	

	for( i = tree[0]; i < 341; i++)
	if(DIstanceToPoint(plane[0], bottomleft) > 0 &&
	if(bottomleft > planes[0] && bottomleft < planes[1] && bottomleft > planes[4] && bottomleft < planes[5]
	bottomleft är innanför frustum
	else if (bottomright > planes[0] && bottomright < planes[1] && bottomright > planes[4] && bottomright < planes[5]
	bottomright är innanför frustum
	else if (topleft > planes[0] && topleft < planes[1] && topleft > planes[4] && topleft < planes[5]
	topleft är innanför frustum
	else if (topright > planes[0] && topright < planes[1] && topright > planes[4] && topright < planes[5]
	topright är innanför frustum
	*/
	/*
	IsInside(0)


	{
	bottomright = (topright.x, bottomleft.y)
	topleft = (bottomleft.x, topright.y)

	}
	*/

}

