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
		tree[0].bottomLeft = XMFLOAT3(0, 0, 0);
		tree[0].topRight = XMFLOAT3(TWIDTH - 1, 115, THEIGHT - 1);
		tree[0].center = XMFLOAT3((TWIDTH - 1) / 2, 115 / 2, (THEIGHT - 1) / 2);
		tree[0].halfDiagonal = XMFLOAT3(tree[0].center.x - tree[0].bottomLeft.x, 115 / 2, tree[0].center.z - tree[0].bottomLeft.z); //kanske ska vara bottomleft - center
		nuIndex = 1;
	}
	if (levels != 0)
	{
		uint parent = FindParent(nuIndex);		//0
		XMFLOAT3 min = tree[parent].bottomLeft;	//0,0
		XMFLOAT3 max = tree[parent].topRight;		//255, 255
		uint iter = 0;
		for (uint i = 0; i < 2; ++i)
		{
			for (uint j = 0; j < 2; ++j)
			{
				tree[nuIndex + iter].bottomLeft = XMFLOAT3(min.x + ((max.x - min.x + 1) / 2) * j, 0.0f,  min.z + ((max.z - min.z + 1) / 2) * i);
				tree[nuIndex + iter].topRight = XMFLOAT3(max.x - ((max.x - min.x + 1) / 2) * (1 - j), 115,  max.z - ((max.z - min.z + 1) / 2) * (1 - i));
				tree[nuIndex + iter].center = XMFLOAT3(tree[nuIndex + iter].bottomLeft.x + (tree[nuIndex + iter].topRight.x - tree[nuIndex + iter].bottomLeft.x) / 2, 115 / 2, tree[nuIndex + iter].bottomLeft.z + (tree[nuIndex + iter].topRight.z - tree[nuIndex + iter].bottomLeft.z) / 2);
				//fel halfDiagonal
				tree[nuIndex + iter].halfDiagonal = XMFLOAT3(tree[nuIndex + iter].center.x - tree[nuIndex + iter].bottomLeft.x, tree[nuIndex + iter].center.y - tree[nuIndex + iter].bottomLeft.y, tree[nuIndex + iter].center.z - tree[nuIndex + iter].bottomLeft.z);
				if (level != levels - 1)
					setTreeCoordinates((nuIndex + iter) * 4 + 1, level + 1);
				++iter;
			}
		}
	}
}

float QuadTree::DistanceToPoint(XMVECTOR plane, XMINT3 point)
{
	XMFLOAT4 temp;
	XMStoreFloat4(&temp, plane);
	return temp.x * point.x + temp.y * point.y + temp.z * point.z + temp.w;
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

XMFLOAT3 QuadTree::GetBottomLeft(uint index)
{
	return tree[index].bottomLeft;
}

XMFLOAT3 QuadTree::GetTopRight(uint index)
{
	return tree[index].topRight;
}

XMFLOAT3 QuadTree::GetCenter(uint index)
{
	return tree[index].center;
}

void QuadTree::pushVertexIndex(uint treeIndex, uint index)
{
	tree[treeIndex].index.push_back(index);
}

void QuadTree::Culling(uint index, Camera* camera, Billboard* billboard)
{
	XMFLOAT3 camPos;
	if (!camera->lockLight)
		camPos = camera->getPos();
	else
	{
		XMStoreFloat3(&camPos, camera->cData.camPos);
	}

	//bool oneCornerInside = false;/*, frustumInsideNode = false;*/
	////float distance[4];
	//XMFLOAT3 corners[8];
	//corners[0] = GetBottomLeft(index);						//Bottom bottom left
	//corners[6] = GetTopRight(index);						//Top top right
	//corners[1] = XMFLOAT3(corners[6].x, 0, corners[0].z);	//Bottom bottom right
	//corners[2] = XMFLOAT3(corners[6].x, 0, corners[6].z);	//Bottom top right
	//corners[3] = XMFLOAT3(corners[0].x, 0, corners[6].z);	//Bottom top left
	//corners[4] = XMFLOAT3(corners[0].x, 115, corners[0].z);	//Top bottom left
	//corners[5] = XMFLOAT3(corners[6].x, 115, corners[0].z);	//Top bottom right
	//corners[7] = XMFLOAT3(corners[0].x, 115, corners[6].y);		//Top top left


	//for (uint i = 0; i < 4; ++i)
	//{
	//	for (uint j = 0; j < 4; ++j)
	//	{
	//		for (uint k = 0; k < 2; ++k)
	//		{
	//			XMINT3 tempCorner = XMINT3(corners[i].x, 115 * k, corners[i].y);
	//			//distance[j] = DistanceToPoint(camera->plane[j], tempCorner/*corners[i]*/); Behövs inte längre
	//		}
	//	}
	//	//if (distance[0] >= 0 && distance[1] >= 0 && distance[2] >= 0 && distance[3] >= 0) Behövs inte längre
	//		//oneCornerInside = true;
	//}

	//if (!oneCornerInside)
	//{
	//	camera->SetFrustumCoordinates();

	//	for (int i = 0; i < 8; ++i)
	//	{
	//		XMFLOAT3 frustumCorner;
	//		XMStoreFloat3(&frustumCorner, camera->nearAndFarVertices[i]);
	//		//if (camera->rayPlaneIntersect(corners)) Behövs inte längre
	//		if (HitBoundingBox)
	//		{
	//			oneCornerInside = true;
	//			break;
	//		}
	//	}
	//}

	/*if ((tree[index].bottomLeft.x <= camPos.x && tree[index].topRight.x >= camPos.x && tree[index].bottomLeft.y <= camPos.z && tree[index].topRight.y >= camPos.z) && !oneCornerInside) Behövs inte längre
	{
		frustumInsideNode = true;
	}*/

	//if (oneCornerInside)
	if (HitBoundingBox(index, camera->plane))
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

int QuadTree::getLevels()
{
	return levels;
}

void QuadTree::FillLeaves(int index, uint bbsPerNode)
{
	int children[4];
	FindChildren(index, children);
	XMFLOAT3 min = GetBottomLeft(index);
	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			if (index < (firstLeaf - pow(4, levels - 1)))
			{
				FillLeaves(children[i * 2 + j], bbsPerNode);
			}
			else
			{
				for (uint k = 0; k < bbsPerNode; k++)
				{
					uint indexPos = k + bbsPerNode * j + sqrt(GetNumOfLeaves()) * bbsPerNode * i + min.z * bbsPerNode + min.x; // min.z var min.y tidigare
					tree[children[i * 2 + j]].index.push_back(indexPos);
				}
			}
		}
	}
}

bool QuadTree::HitBoundingBox(uint index, XMVECTOR * frustumVec)
{
	bool result = false;
	float e = 0;
	float s = 0;
	XMFLOAT4 planeNor;
	XMFLOAT3 planeAbs;

	for (uint i = 0; i < 6; i++)
	{
		XMStoreFloat4(&planeNor, frustumVec[i]);
		planeAbs = XMFLOAT3(sqrt(pow(planeNor.x, 2)), sqrt(pow(planeNor.y, 2)), sqrt(pow(planeNor.z, 2)));
		e = tree[index].halfDiagonal.x * planeAbs.x + tree[index].halfDiagonal.y * planeAbs.y + tree[index].halfDiagonal.z * planeAbs.z;
		s = tree[index].center.x * planeNor.x + tree[index].center.y * planeNor.y + tree[index].center.z * planeNor.z + planeNor.w;
		//if (s > 0)
		//	s = -s;
		if (s - e > 0)
			result = false;

		else if (s + e < 0)
		{
			result = true;
			break;
		}

		else
		{
			result = true;
			break;
		}
		
	}

	return result;
}

