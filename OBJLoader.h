#ifndef OBJLOADER_H
#define OBJLOADER_H


#include <SimpleMath.h>
#include <SimpleMath.inl>

//#include "bth_image.h"
#include <sstream> //obj
#include <fstream> //obj
#include <vector> //obj
#include <DDSTextureLoader.h>
#include <WICTextureLoader.h>



struct CONSTANT_BUFFER2
{
	DirectX::XMFLOAT4 KD;
	DirectX::XMFLOAT4 KA;
	DirectX::XMFLOAT4 KS;
};
struct VertexData { float x, y, z, u, v, x2, y2, z2; }; //behövs globalt så att draw kan sättas dynamiskt

class Object
{
public:
	Object();
	~Object();

	void LoadObject(ID3D11Device* gDevice);
	std::vector<VertexData> triangleVertices; //behövs globalt så att draw kan sättas dynamiskt	
	CONSTANT_BUFFER2 materialData;
	ID3D11Buffer* VertexBuffer;
	ID3D11ShaderResourceView* textureView;
private:
   void MTLLoader(std::string mtlfile, ID3D11Device* gDevice);
   void Texture(std::string material, ID3D11Device* gDevice);
};

#endif