#pragma once

/*

!!!!!!!!!!!!!!!!!!!! WARNING: INCLUDE THIS CLASS BEFORE "inc.hpp" !!!!!!!!!!!!!!!!!!!!

*/


#ifndef DXVBOS_HPP
#define DXVBOS_HPP

#include <vector>
#include <wrl/client.h>

#include "inc.hpp"
#include "Vertex.hpp"

template <unsigned long nVertices, typename T = Vertex>
class DXVBO
{
	Microsoft::WRL::ComPtr<ID3D10Buffer> m_pInternalBuffer;
	
	T m_vecVertices[nVertices];

	unsigned long m_nVertCount = 0;

public:

	DXVBO()
	{

	}
	
	~DXVBO()
	{

	}

	bool AddVertex(Vertex Vert)
	{
		m_vecVertices[m_nVertCount] = Vert;
		m_nVertCount++;

		return true;
	}
	
	bool AddVertex(Vertex* pVert)
	{
		if (!pVert)
		{
			::MessageBoxA(0, "ERROR: pVert was zero.", "FATAL ERROR", 0);
			return false;
		}

		m_vecVertices[m_nVertCount] = *pVert;
		m_nVertCount++;

		return true;
	}

	bool CreateBuffer(ID3D10Device* pD3DDevice)
	{
		/*
		Vertex OurVertices[3 * 2] =
		{

			{ -1.f, -1.f, 0.f },
			{ 1.f, -1.f, 0.f },
			{ 1.f, 1.f, 0.f },
			{ -1.f, -1.f, 0.f },
			{ -1.f, 1.f, 0.f },
		};*/

		D3D10_BUFFER_DESC bd = { 0 };
		bd.ByteWidth = sizeof(Vertex) * m_nVertCount;  // use the vector size over what the programmer supplied to prevent 
																// misalignment in the case of the buffer not being filled.
		bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;

		D3D10_SUBRESOURCE_DATA srd = { m_vecVertices, 0, 0 };

		pD3DDevice->CreateBuffer(&bd, &srd, &m_pInternalBuffer);

		return true;
	}
	
	bool DrawBuffer(ID3D10Device* pD3DDevice, D3D_PRIMITIVE_TOPOLOGY Topology = (D3D_PRIMITIVE_TOPOLOGY)5) // 5 => D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
	{
		if (!m_pInternalBuffer.Get())
		{
			::MessageBoxA(0, "ERROR!! m_pInternalBuffer was zero!!", "Fatal error", 0);
			exit(-1);
		}

		// set the vertex buffer
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		pD3DDevice->IASetVertexBuffers(0, 1, m_pInternalBuffer.GetAddressOf(), &stride, &offset);
		pD3DDevice->IASetPrimitiveTopology(Topology);
		pD3DDevice->Draw(6, 0);

		return true;
	}

	ID3D10Buffer* GetBuffer()
	{
		return m_pInternalBuffer.Get();
	}

	bool ScaleVerts(float fNewScale)
	{
		if (m_nVertCount == 0)
		{
			return false;
		}

		for (unsigned long i = 0; i < m_nVertCount; i++)
		{
			m_vecVertices[i] *= fNewScale;
		}

		return true;
	}

};

#endif