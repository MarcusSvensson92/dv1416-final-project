#include "StdAfx.h"
#include "Buffer.h"

Buffer::Buffer(void) { }

Buffer::~Buffer(void)
{
	RELEASE(m_buffer);
}

void Buffer::init(ID3D11Device* device, const BufferInitDesc bufferInitDesc)
{
	m_elementSize = bufferInitDesc.elementSize;
	m_type		  = bufferInitDesc.type;

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage		  = bufferInitDesc.usage;
	bufferDesc.ByteWidth	  = bufferInitDesc.elementCount * m_elementSize;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags	  = 0;

	switch (m_type)
	{
	case VertexBuffer:
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		break;
	case IndexBuffer:
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		break;
	case ConstantBufferVS:
	case ConstantBufferGS:
	case ConstantBufferPS:
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		break;
	}

	if (bufferInitDesc.usage == D3D11_USAGE_DYNAMIC)
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = bufferInitDesc.data;

	device->CreateBuffer(&bufferDesc, &initData, &m_buffer);
}

void Buffer::apply(ID3D11DeviceContext* deviceContext)
{
	switch (m_type)
	{
	case VertexBuffer:
	{
		UINT stride = m_elementSize;
		UINT offset	= 0;
		deviceContext->IASetVertexBuffers(0, 1, &m_buffer, &stride, &offset);
		break;
	}
	case IndexBuffer:
		deviceContext->IASetIndexBuffer(m_buffer, DXGI_FORMAT_R32_UINT, 0);
		break;
	case ConstantBufferVS:
		deviceContext->VSSetConstantBuffers(0, 1, &m_buffer);
		break;
	case ConstantBufferGS:
		deviceContext->GSSetConstantBuffers(0, 1, &m_buffer);
		break;
	case ConstantBufferPS:
		deviceContext->PSSetConstantBuffers(0, 1, &m_buffer);
		break;
	}
}

void* Buffer::map(ID3D11DeviceContext* deviceContext)
{
	D3D11_MAPPED_SUBRESOURCE resource;
	deviceContext->Map(m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	return resource.pData;
}

void Buffer::unmap(ID3D11DeviceContext* deviceContext)
{
	deviceContext->Unmap(m_buffer, 0);
}