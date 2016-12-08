#include "QuadTree.h"
#include "camera.h"

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
	return temp.x*point.x + temp.y * 0 + temp.z*point.y + temp.w;
}


uint QuadTree::FindParent(uint nodeIndex)
{
	return ((nodeIndex + 3)/4) - 1;
}

void QuadTree::FindChildren(uint nodeIndex, int* children) //remember to check children for nullptr when calling
{
	if (nodeIndex < firstLeaf)
	{
		for (int i = 1; i < 5; i++)
		{
			children[i - 1] = nodeIndex * 4 + i;
		}
	}
	else
	{
		children[0] = -1;
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

void QuadTree::Culling(uint index,Camera* camera, Billboard* billboard)
{
	XMFLOAT3 camPos = camera->getPos();
	bool oneCornerInside = false, frustumInsideNode = false;
	float distance[4];
	XMINT2 corners[4];
	corners[0] = GetBottomLeft(index);						//Bottom left
	corners[3] = GetTopRight(index);						//Top right
	corners[1] = XMINT2(corners[3].x, corners[0].y);		//Bottom right
	corners[2] = XMINT2(corners[0].x, corners[3].y);		//Top left


	for (uint i = 0; i < 4; ++i)
	{
		for (uint j = 0; j < 4; ++j)
		{
			distance[j] = DistanceToPoint(camera->plane[j], corners[i]);
		}
		if (distance[0] >= 0 && distance[1] >= 0 && distance[2] >= 0 && distance[3] >= 0)
			oneCornerInside = true;
	}
	if (tree[index].bottomLeft.x <= camPos.x && tree[index].topRight.x >= camPos.x && tree[index].bottomLeft.y <= camPos.z && tree[index].topRight.y >= camPos.z)
	{
		frustumInsideNode = true;
	}

	if (oneCornerInside || frustumInsideNode)
	{
		int children[4];
		FindChildren(index, children);
		if (children[0] != -1)
		{
			for (uint i = 0; i < 4; ++i)
			{
				Culling(children[i], camera, billboard); //this is where the recursion occurs + magic (just a joke, Francisco)
			}
		}
		else
		{
			uint size = tree[index].index.size();
			for (uint i = 0; i < size; ++i)
			{
				billboard->used.push_back(billboard->vertices[tree[index].index[i]]);
			}
		}
	}
}

