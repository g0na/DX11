#include "Base.h"

CBase::CBase()
{
}

_uint CBase::AddRef()
{
	return ++m_iRefCnt;
}

_uint CBase::Release()
{
	// 레퍼런스 카운트가 0 이라면 삭제해야하므로 0을 리턴
	if (m_iRefCnt == 0)
	{
		Free();

		delete this;

		return 0;
	}
	else
		// 0이 아니라면 감소하기 전의 레퍼런스 카운트를 반환
		return m_iRefCnt--;
}

void CBase::Free()
{
}