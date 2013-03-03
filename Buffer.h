#ifndef BUFFER_H
#define BUFFER_H

#include "StdAfx.h"

enum BufferType
{
	VertexBuffer,
	IndexBuffer,
	ConstantBufferVS,
	ConstantBufferGS,
	ConstantBufferPS
};

struct BufferInitDesc
{
	D3D11_USAGE usage;
	UINT		elementSize;
	UINT		elementCount;
	BufferType	type;
	void*		data;
};

class Buffer
{
public:
	Buffer(void);
	~Buffer(void);

	void init(ID3D11Device* device, const BufferInitDesc bufferInitDesc);
	void apply(ID3D11DeviceContext* deviceContext);

	void* map(ID3D11DeviceContext* deviceContext);
	void unmap(ID3D11DeviceContext* deviceContext);
private:
	ID3D11Buffer* m_buffer;

	UINT	   m_elementSize;
	BufferType m_type;
};

#endif