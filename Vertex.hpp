#pragma once

#ifndef VERTEX_HPP
#define VERTEX_HPP

#define _QUICK_OPERATOR_DEF0(op) inline Vertex operator##op (Vertex Second) { return Vertex(this->m_fData[0] op Second.m_fData[0], this->m_fData[1] op Second.m_fData[1], this->m_fData[2] op Second.m_fData[2]); }
#define _QUICK_OPERATOR_DEF1(op) inline Vertex operator##op (float n) { return Vertex(this->m_fData[0] op n, this->m_fData[1] op n, this->m_fData[2] op n); }
#define _QUICK_OPERATOR_DEF2(op) inline Vertex operator##op##= (float n) { return Vertex(this->m_fData[0] op n, this->m_fData[1] op n, this->m_fData[2] op n); }

#pragma pack(push, 1)
class Vertex	// sizeof(Vertex) = sizeof(float[3]) = 12
{
public:

	float m_fData[3]; // Ensure contiguity

	inline Vertex()
	{
		m_fData[0] = 0.0f;
		m_fData[1] = 0.0f;
		m_fData[2] = 0.0f;
	}

	inline Vertex(float x, float y, float z)
	{
		m_fData[0] = x;
		m_fData[1] = y;
		m_fData[2] = z;
	}

	_QUICK_OPERATOR_DEF0(+);
	_QUICK_OPERATOR_DEF0(-);
	_QUICK_OPERATOR_DEF0(*);
	_QUICK_OPERATOR_DEF0(/);

	_QUICK_OPERATOR_DEF1(+);
	_QUICK_OPERATOR_DEF1(-);
	_QUICK_OPERATOR_DEF1(*);
	_QUICK_OPERATOR_DEF1(/);

	_QUICK_OPERATOR_DEF2(+);
	_QUICK_OPERATOR_DEF2(-);
	_QUICK_OPERATOR_DEF2(*);
	_QUICK_OPERATOR_DEF2(/);

	
};
#pragma pack(pop)

#endif