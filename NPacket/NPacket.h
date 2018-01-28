/*---------------------------------------------------------------

N Packet.

��Ʈ��ũ ��Ŷ�� Ŭ����.
�����ϰ� ��Ŷ�� ������� ����Ÿ�� In, Out �Ѵ�.

�÷����� ���� ������ ����� �ٸ��Ƿ�, ��Ʈ��ũ ��Ŷ ���۽ÿ� ������ �߻��� �� �ִ�.
�׸��Ͽ� �÷����� �������� ���� ���������� ������ ����.

--------------------------------------------------------

NPacket::_ValueSizeCheck()

�Լ��� ���� 1ȸ ȣ���Ͽ� �� ������ ������ Ȯ���� �ʼ��� �ؾ��Ѵ�.
FALSE ���Ͻ� �����ڿ��� �����ض�.


�Ʒ� ������� �ٸ� ��� ������ �ÿ����� �߻��Ѵ�.

C					C#				TYPE
char				sbyte			signed 1Byte
unsigned char		byte			unsigned 1Byte
WCHAR				char			unicode char 2Byte

short				short			signed 2Byte
unsigned short		ushort			unsigned 2Byte

int					int				signed 4Byte
unsigned int		uint			unsigned 4Byte
float				float			float 4Byte

__int64				long			signed 8Byte
unsigned __int64	ulong			unsigned 8Byte

--------------------------------------------------------


- ����.

CNPacket cPacket;

�ֱ�.
clPacket << 40030;	or	clPacket << iValue;		(int �ֱ�)
clPacket << 3;		or	clPacket << byValue;	(BYTE �ֱ�)
clPacket << 1.4;	or	clPacket << fValue;		(float �ֱ�)

����.
clPacket >> iValue;		(int ����)
clPacket >> byValue;	(BYTE ����)
clPacket >> fValue;		(float ����)

!.	���ԵǴ� ����Ÿ FIFO ������ �����ȴ�.
ť�� �ƴϹǷ�, �ֱ�(<<).����(>>) �� ȥ���ؼ� ����ϸ� �ȵȴ�.

----------------------------------------------------------------*/
#ifndef  __N_PACKET__
#define  __N_PACKET__

class CNPacket
{
public:

	/*---------------------------------------------------------------
	AyaPacket Enum.

	----------------------------------------------------------------*/
	enum enN_PACKET
	{
		eBUFFER_DEFAULT = 10000,	// ��Ŷ�� �⺻ ���� ������.
		eSTRING_MAX = 1024,		// ���ڿ� �Է½� �ִ����

		eSBYTE = 1,
		eBYTE = 1,
		eWCHAR = 2,

		eSHORT = 2,
		eUSHORT = 2,
		eINT = 4,
		eUINT = 4,
		eFLOAT = 4,

		eINT64 = 8,		// C# ���� long ��.
		eUINT64 = 8
	};

	//////////////////////////////////////////////////////////////////////////
	// ������, �ı���.
	//
	// Parameters:
	//				---------------------------------------------------------
	//				(int) ���� ������.
	//				---------------------------------------------------------
	//				(const CNPacket &) ��Ŷ�� ���縦 ���� ��Ŷ ���۷���.
	// Return:
	//////////////////////////////////////////////////////////////////////////
	CNPacket()
	{
		Initial();
	}


	CNPacket(int iBufferSize)
	{
		Initial(iBufferSize);
	}


	CNPacket(const CNPacket &clSrcPacket)
	{
		m_chpBuffer = new unsigned char[clSrcPacket.m_iBufferSize];
		StringCchPrintfA((STRSAFE_LPSTR)m_chpBuffer,
			clSrcPacket.m_iBufferSize,
			(STRSAFE_LPCSTR)clSrcPacket.m_chpBuffer
			);

		m_iBufferSize = clSrcPacket.m_iBufferSize;

		m_chpDataFieldStart = m_chpBuffer;
		m_chpDataFieldEnd = m_chpBuffer;

		m_chpReadPos = m_chpBuffer + (clSrcPacket.m_chpReadPos - clSrcPacket.m_chpBuffer);
		m_chpWritePos = m_chpBuffer + (clSrcPacket.m_chpWritePos - clSrcPacket.m_chpBuffer);

		m_iDataSize = clSrcPacket.m_iDataSize;
	}

	virtual			~CNPacket()
	{
		Release();
	}


	//////////////////////////////////////////////////////////////////////////
	// �� �ý����� ���� ���� üũ.
	//
	// Parameters: ����.
	// Return: (bool)true,false.
	//////////////////////////////////////////////////////////////////////////
	static bool		_ValueSizeCheck(void)
	{
		// 1 byte ���� 
		if (eSBYTE != sizeof(char) || eBYTE != sizeof(unsigned char)) return false;

		// 2 byte ���� 
		if (eWCHAR != sizeof(WCHAR) || eSHORT != sizeof(short) || eUSHORT != sizeof(unsigned short)) return false;

		// 4 byte ���� 
		if (eINT != sizeof(int) || eUINT != sizeof(unsigned int) || eFLOAT != sizeof(float)) return false;

		// 8 byte ���� 
		if (eINT64 != sizeof(__int64) || eUINT64 != sizeof(unsigned __int64)) return false;
		return true;
	}


	//////////////////////////////////////////////////////////////////////////
	// ��Ŷ �ʱ�ȭ.
	//
	// �޸� �Ҵ��� ���⼭ �ϹǷ�, �Ժη� ȣ���ϸ� �ȵȴ�. 
	//
	// Parameters: (int)BufferSize.
	// Return: ����.
	//////////////////////////////////////////////////////////////////////////
	void			Initial(int iBufferSize = eBUFFER_DEFAULT)
	{
		m_chpBuffer = new unsigned char[iBufferSize];

		m_iBufferSize = iBufferSize;

		m_chpDataFieldStart = m_chpBuffer + 5;
		m_chpDataFieldEnd = m_chpDataFieldStart;

		m_chpReadPos = m_chpDataFieldStart;
		m_chpWritePos = m_chpDataFieldStart;

		m_bHeaderExist = false;

		m_iDataSize = 0;

		m_lRefCnt = 0;
	}


	//////////////////////////////////////////////////////////////////////////
	// ��Ŷ  �ı�.
	//
	// Parameters: ����.
	// Return: ����.
	//////////////////////////////////////////////////////////////////////////
	void			Release(void)
	{
		delete[] m_chpBuffer;
	}



	//////////////////////////////////////////////////////////////////////////
	// ��Ŷ û��.
	//
	// Parameters: ����.
	// Return: ����.
	//////////////////////////////////////////////////////////////////////////
	void			Clear(void)
	{
		m_chpReadPos = m_chpDataFieldStart;
		m_chpWritePos = m_chpDataFieldStart;

		m_iDataSize = 0;

		m_bHeaderExist = false;
	}



	//////////////////////////////////////////////////////////////////////////
	// ���� ������ ���.
	//
	// Parameters: ����.
	// Return: (int)��Ŷ ���� ������ ���.
	//////////////////////////////////////////////////////////////////////////
	int				GetBufferSize(void) { return m_iBufferSize; }

	//////////////////////////////////////////////////////////////////////////
	// ���� ������� ������ ���.
	//
	// Parameters: ����.
	// Return: (int)������� ����Ÿ ������.
	//////////////////////////////////////////////////////////////////////////
	const int		GetDataSize(void) const { return m_iDataSize; }



	//////////////////////////////////////////////////////////////////////////
	// ����(������) ������ ���.
	//
	// Parameters: ����.
	// Return: (unsigned char *)���� ������.
	//////////////////////////////////////////////////////////////////////////
	unsigned char	*GetBufferPtr(void) const { return m_chpDataFieldStart; }



	//////////////////////////////////////////////////////////////////////////
	// ����(���) ������ ���.
	//
	// Parameters: ����.
	// Return: (unsigned char *)���� ������.
	//////////////////////////////////////////////////////////////////////////
	unsigned char	*GetBufferHeaderPtr(void) const { return m_chpReadPos; }



	//////////////////////////////////////////////////////////////////////////
	// ���� Pos �̵�. (�����̵��� �ȵ�)
	// GetBufferPtr �Լ��� �̿��Ͽ� �ܺο��� ������ ���� ������ ������ ��� ���. 
	//
	// Parameters: (int) �̵� ������.
	// Return: (int) �̵��� ������.
	//////////////////////////////////////////////////////////////////////////
	int				MoveWritePos(int iSize)
	{
		unsigned int iWriteIndex = m_chpWritePos - m_chpBuffer;

		if ((iWriteIndex + iSize) > m_iBufferSize)			return 0;

		m_chpWritePos += iSize;
		m_iDataSize += iSize;

		return iSize;
	}

	int				MoveReadPos(int iSize)
	{
		unsigned int iReadIndex = m_chpWritePos - m_chpBuffer;

		if ((iReadIndex + iSize) > m_iBufferSize)			return 0;

		m_chpReadPos += iSize;
		m_iDataSize -= iSize;

		return iSize;
	}


	//////////////////////////////////////////////////////////////////////////
	// ��� ����(5Byte)
	// ��Ŷ ��� ����. 
	//
	// Parameters: (int) �̵� ������.
	// Return: (int) �̵��� ������.
	//////////////////////////////////////////////////////////////////////////
	void			SetHeader(char *pHeader)
	{
		if (m_bHeaderExist)				return;

		memcpy_s(m_chpBuffer, 5, pHeader, 5);

		m_chpReadPos -= 5;
		m_iDataSize += 5;
	}


	//////////////////////////////////////////////////////////////////////////
	// ��� ����(�ϴ� 5Byte ���Ϸ�)
	// ��Ŷ ��� ����. 
	//
	// Parameters: (int) �̵� ������.
	// Return: (int) �̵��� ������.
	//////////////////////////////////////////////////////////////////////////
	void SetCustomHeader(char *pHeader, int iCustomHeaderSize)
	{
		if (m_bHeaderExist || iCustomHeaderSize > 5)				return;

		unsigned char * chpHeaderPos = m_chpBuffer + iCustomHeaderSize;

		memcpy_s(chpHeaderPos, iCustomHeaderSize, pHeader, iCustomHeaderSize);

		m_chpReadPos -= 2;
		m_iDataSize += 2;
	}


	//////////////////////////////////////////////////////////////////////////
	// ��� ����(2Byte, ���� ������)
	// ��Ŷ ��� ����. 
	//
	// Parameters: (int) �̵� ������.
	// Return: (int) �̵��� ������.
	//////////////////////////////////////////////////////////////////////////
	void SetCustomShortHeader(unsigned short Header)
	{
		if (m_bHeaderExist)				return;

		unsigned char * chpHeaderPos = m_chpBuffer + 3;

		memcpy_s(chpHeaderPos, 2, &Header, 2);

		m_chpReadPos -= 2;
		m_iDataSize += 2;
	}


	//////////////////////////////////////////////////////////////////////////
	// ��Ŷ �Ҵ�.
	// ��Ŷ�� �޸� Ǯ���� �Ҵ� �޾Ƽ� ���. 
	//
	// Parameters: ����.
	// Return: (CNPacket *) ��Ŷ ������.
	//////////////////////////////////////////////////////////////////////////
	static CNPacket *Alloc()
	{
		CNPacket *pAllocPacket = new CNPacket;

		pAllocPacket->addRef();
		pAllocPacket->Clear();

		return pAllocPacket;
	}


	//////////////////////////////////////////////////////////////////////////
	// ��Ŷ ��ȯ.
	// ��Ŷ�� �޸� Ǯ�� ��ȯ. 
	//
	// Parameters: ����.
	// Return: ����.
	//////////////////////////////////////////////////////////////////////////
	void Free()
	{
		int result = InterlockedDecrement((LONG *)&m_lRefCnt);

		if (0 == result)
		{
			delete this;
		}
	}


	//////////////////////////////////////////////////////////////////////////
	// �Ҵ� ī��Ʈ ����. 
	//
	// Parameters: ����.
	// Return: ����.
	//////////////////////////////////////////////////////////////////////////
	void addRef()
	{
		InterlockedIncrement((LONG *)&m_lRefCnt);
	}




	/* ============================================================================= */
	// ������ ���۷�����.
	/* ============================================================================= */
	CNPacket	&operator = (CNPacket &clSrcPacket)
	{
		m_chpBuffer = new unsigned char[clSrcPacket.m_iBufferSize];
		StringCchPrintfA((STRSAFE_LPSTR)m_chpBuffer,
			clSrcPacket.m_iBufferSize,
			(STRSAFE_LPCSTR)clSrcPacket.m_chpBuffer
			);

		m_iBufferSize = clSrcPacket.m_iBufferSize;

		m_chpDataFieldStart = m_chpBuffer;
		m_chpDataFieldEnd = m_chpBuffer;

		m_chpReadPos = m_chpBuffer + (clSrcPacket.m_chpReadPos - clSrcPacket.m_chpBuffer);
		m_chpWritePos = m_chpBuffer + (clSrcPacket.m_chpWritePos - clSrcPacket.m_chpBuffer);

		m_iDataSize = clSrcPacket.m_iDataSize;
	}
	
	void CNPacket::operator << (char chValue){ Put(chValue); }
	void CNPacket::operator << (unsigned char byValue){ Put(byValue); }
	void CNPacket::operator << (WCHAR wchValue){ Put(wchValue); }
	void CNPacket::operator << (short shValue){ Put(shValue); }
	void CNPacket::operator << (int iValue){ Put(iValue); }
	void CNPacket::operator << (unsigned int iValue){ Put(iValue); }
	void CNPacket::operator << (float fValue){ Put(fValue); }
	void CNPacket::operator << (__int64 i64Value){ Put(i64Value); }
	void CNPacket::operator << (unsigned __int64 i64Value){ Put(i64Value); }
	void CNPacket::operator << (WCHAR *szString){ Put(szString); }

	void CNPacket::operator >> (char &chValue){ Get(chValue); }
	void CNPacket::operator >> (unsigned char &byValue){ Get(byValue); }
	void CNPacket::operator >> (WCHAR &wchValue){ Get(wchValue); }
	void CNPacket::operator >> (short &shValue){ Get(shValue); }
	void CNPacket::operator >> (int &iValue){ Get(iValue); }
	void CNPacket::operator >> (unsigned int &iValue){ Get(iValue); }
	void CNPacket::operator >> (float &fValue){ Get(fValue); }
	void CNPacket::operator >> (__int64 &i64Value){ Get(i64Value); }
	void CNPacket::operator >> (unsigned __int64 &i64Value){ Get(i64Value); }
	void CNPacket::operator >> (WCHAR *szString){ Get(szString, eSTRING_MAX); }


	//////////////////////////////////////////////////////////////////////////
	// �ֱ�.	�� ���� Ÿ�Ը��� ��� ����.
	//////////////////////////////////////////////////////////////////////////
	int Put(char chValue)				// C# - sbyte	// signed 1Byte
	{
		return PutData((unsigned char *)&chValue, eBYTE);
	}

	int Put(unsigned char byValue)		// C# - byte	// unsigned 1Byte
	{
		return PutData((unsigned char *)&byValue, eBYTE);
	}

	int Put(WCHAR wchValue)				// C# - char	// unicode char 2Byte
	{
		return PutData((unsigned char *)&wchValue, eWCHAR);
	}

	int Put(short shValue)				// C# - short	// signed 2Byte
	{
		return PutData((unsigned char *)&shValue, eSHORT);
	}

	int Put(unsigned short wValue)		// C# - ushort	// unsigned 2Byte
	{
		return PutData((unsigned char *)&wValue, eUSHORT);
	}

	int Put(int iValue)					// C# - int		// signed 4Byte
	{
		return PutData((unsigned char *)&iValue, eINT);
	}

	int Put(unsigned int iValue)		// C# - uint	// unsigned 4Byte
	{
		return PutData((unsigned char *)&iValue, eUINT);
	}
	int Put(float fValue)				// C# - float	// float 4Byte
	{
		return PutData((unsigned char *)&fValue, eFLOAT);
	}

	int Put(__int64 i64Value)			// C# - long	// signed 8Byte
	{
		return PutData((unsigned char *)&i64Value, eINT64);
	}

	int Put(unsigned __int64 i64Value)	// C# - ulong	// unsigned 8Byte
	{
		return PutData((unsigned char *)&i64Value, eUINT64);
	}

	int	Put(WCHAR *szString)
	{
		if (eSTRING_MAX < wcslen(szString))
			return 0;
		else
			return PutData((unsigned char *)szString, (wcslen(szString) + 1)* eWCHAR);
	}

	//int	Put(Proud::String &String);		// ���ڿ� �ֱ�

	//////////////////////////////////////////////////////////////////////////
	// ����.	�� ���� Ÿ�Ը��� ��� ����.
	//////////////////////////////////////////////////////////////////////////

	int Get(unsigned char &byValue) const
	{
		return GetData((unsigned char *)&byValue, eBYTE);
	}

	int Get(char &chValue) const
	{
		return GetData((unsigned char *)&chValue, eBYTE);
	}

	int Get(WCHAR &wchValue) const
	{
		return GetData((unsigned char *)&wchValue, eWCHAR);
	}

	int Get(short &shValue) const
	{
		return GetData((unsigned char *)&shValue, eSHORT);
	}

	int Get(unsigned short &wValue) const
	{
		return GetData((unsigned char *)&wValue, eUSHORT);
	}

	int Get(int &iValue) const
	{
		return GetData((unsigned char *)&iValue, eINT);
	}

	int Get(unsigned int &iValue) const
	{
		return GetData((unsigned char *)&iValue, eUINT);
	}

	int Get(float &fValue) const
	{
		return GetData((unsigned char *)&fValue, eFLOAT);
	}

	int Get(__int64 &i64Value) const			// C# - long
	{
		return GetData((unsigned char *)&i64Value, eINT64);
	}

	int Get(unsigned __int64 &i64Value) const	// C# - ulong
	{
		return GetData((unsigned char *)&i64Value, eUINT64);
	}

	int	Get(WCHAR *szString, int iBuffer) const
	{
		int iCnt = 0;

		do{
			szString[iCnt] = *((WCHAR *)m_chpReadPos);
			m_chpReadPos += eWCHAR;
			m_iDataSize -= eWCHAR;
			iCnt++;
		} while (*((WCHAR *)m_chpReadPos) != L'\0');

		szString[iCnt] = L'\0';

		return iCnt * eWCHAR;
	}

	//int	Get(Proud::String &String) const;		// ���ڿ� ����

	//////////////////////////////////////////////////////////////////////////
	// ����Ÿ ���.
	//
	// Parameters: (unsigned char *)Dest ������. (int)Size.
	// Return: (int)������ ������.
	//////////////////////////////////////////////////////////////////////////
	int					GetData(unsigned char *bypDest, int iSize) const
	{
		int iCnt;

		if ((m_chpWritePos - m_chpReadPos) < iSize)				return 0;

		for (iCnt = 0; iCnt < iSize; iCnt++)
		{
			bypDest[iCnt] = *(m_chpReadPos++);
			m_iDataSize--;
		}

		return iCnt;
	}

	//////////////////////////////////////////////////////////////////////////
	// ����Ÿ ����.
	//
	// Parameters: (unsigned char *)Src ������. (int)SrcSize.
	// Return: (int)������ ������.
	//////////////////////////////////////////////////////////////////////////
	int					PutData(unsigned char *bypSrc, int iSrcSize)
	{
		int iCnt;

		if (((m_chpBuffer + m_iBufferSize) - m_chpWritePos) < iSrcSize)
			return 0;

		for (iCnt = 0; iCnt < iSrcSize; iCnt++)
		{
			*(m_chpWritePos++) = bypSrc[iCnt];
			m_iDataSize++;
		}

		return iCnt;
	}




protected:

	//------------------------------------------------------------
	// ��Ŷ���� / ���� ������.
	//------------------------------------------------------------
	unsigned char				*m_chpBufferExpansion;

	unsigned char				*m_chpBuffer;
	int							m_iBufferSize;
	//------------------------------------------------------------
	// ��Ŷ���� ���� ��ġ.	(�� Ŭ���� ������ ������� ������, Ȯ�强�� ���� ���)
	//------------------------------------------------------------
	unsigned char				*m_chpDataFieldStart;
	unsigned char				*m_chpDataFieldEnd;


	//------------------------------------------------------------
	// ������ ���� ��ġ, ���� ��ġ.
	//------------------------------------------------------------
	mutable unsigned char		*m_chpReadPos;
	mutable unsigned char		*m_chpWritePos;


	//------------------------------------------------------------
	// ���� ���ۿ� ������� ������.
	//------------------------------------------------------------
	mutable int					m_iDataSize;


	//------------------------------------------------------------
	// ��� ���� �÷���
	//------------------------------------------------------------
	bool						m_bHeaderExist;

	//------------------------------------------------------------
	// �Ҵ� ī����
	//------------------------------------------------------------
	long						m_lRefCnt;
};


#endif
