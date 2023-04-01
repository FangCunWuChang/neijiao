#include "stdafx.h"
#include "ADODB.h"


CAdoDB::CAdoDB()
{
	//����CoInitialize��ʼ��COM����
	::CoInitialize(NULL);
}

CAdoDB::~CAdoDB()
{
	//�ͷ�COM����
	::CoUninitialize();	
}

BOOL CAdoDB::IsOpen()
{

	try
	{
		return (m_pConn != NULL && (m_pConn->State & adStateOpen));
	}
	catch (_com_error e)//�쳣���� 
	{ 
		AfxMessageBox(e.Description()); 
	} 
	return FALSE;
}

BOOL CAdoDB::Open(LPCTSTR lpszConnect, long lOptions)
{
	//����Connection ����
	m_pConn.CreateInstance("ADODB.Connection");
	//��ʼ��Recordsetָ��
	m_pRs.CreateInstance(_uuidof(Recordset)); 
	m_strConn = lpszConnect;
	if (m_strConn.IsEmpty())
	{
		return FALSE;
	}
	if (IsOpen()) Close();
	try
	{
		// �������ݿ�
		return (m_pConn->Open(_bstr_t(m_strConn), "", "", lOptions) == S_OK);
	}
	catch (_com_error e)//�쳣���� 
	{ 
		AfxMessageBox(e.Description()); 
	} 
	return FALSE;
}

void CAdoDB::Close()
{
	try
	{
		if (m_pConn != NULL && IsOpen()) 
		{
			m_pConn->Close();
		}
	}
	catch (_com_error e)//�쳣���� 
	{ 
		AfxMessageBox(e.Description()); 
	} 
}

bool CAdoDB::BeginTrans()
{
	return (!FAILED(m_pConn->BeginTrans()));
}

bool CAdoDB::RollBackTrans()
{
	return (!FAILED(m_pConn->RollbackTrans()));
}

bool CAdoDB::CommitTrans()
{
	return (!FAILED(m_pConn->CommitTrans()));
}

_RecordsetPtr CAdoDB::Execute(LPCTSTR lpszSQL, long lOptions)//adCmdText
{
	_variant_t RecordsAffected; 
	try
	{
		return m_pConn->Execute(_bstr_t(lpszSQL),&RecordsAffected,adCmdText);
	}
	catch (_com_error e)//�쳣���� 
	{ 
		AfxMessageBox(e.Description()); 
	} 
	return FALSE;
}

BOOL CAdoDB::ExecuteQueryValue(LPCTSTR lpszSQL,CString& value)
{
	try
	{
		//�򿪼�¼��
		m_pRs->Open(_bstr_t(lpszSQL),(IDispatch*)m_pConn,adOpenDynamic,
			adLockOptimistic,adCmdText);
		if(!m_pRs->adoEOF) 
		{ 
			_variant_t thevalue = m_pRs->GetCollect(_variant_t((short)0));
			value = vartostr(thevalue);			
		} 
		m_pRs->Close(); 
		return TRUE;
	}
	catch (_com_error e)//�쳣���� 
	{ 
		AfxMessageBox(e.Description()); 
	} 
	return FALSE;
}

int CAdoDB::GetAdoItemCount(CString sql)
{
	CDStrs temp;
	this->ExecuteQuery(sql,temp);
	return (int)temp.size();
}

BOOL CAdoDB::ExecuteQuery(LPCTSTR lpszSQL,CDStrs& Fields)
{
	try
	{
		//�򿪼�¼��
m_pRs->Open(_bstr_t(lpszSQL),(IDispatch*)m_pConn,adOpenDynamic,
			adLockOptimistic,adCmdText);
		while(!m_pRs->adoEOF) 
		{ 
			CStrs strs;
			_variant_t thevalue ;
			//��ȡһ����¼�е������ֶ�ֵ���浽�ַ�������strs��
			int size = m_pRs->Fields->Count;
			for(long index = 0 ; index < m_pRs->Fields->Count; index++)
			{
				thevalue = m_pRs->GetCollect(_variant_t(index));
				CString temp = vartostr(thevalue);
				strs.push_back(temp);
			}
			//����һ����¼����ά�ַ���������
			Fields.push_back(strs);
			//ת����һ����¼
			m_pRs->MoveNext(); 
		} 
		m_pRs->Close(); 
		return TRUE;
	}
	catch (_com_error e)//�쳣���� 
	{ 
		AfxMessageBox(e.Description()); 
	} 
	return FALSE;
}

BOOL CAdoDB::ExecuteQuery(LPCTSTR lpszSQL,long Index,CStrs& Fields)
{
	try
	{
		//�򿪼�¼��
		m_pRs->Open(_bstr_t(lpszSQL),(IDispatch*)m_pConn,adOpenDynamic,
			adLockOptimistic,adCmdText);
		while(!m_pRs->adoEOF) 
		{ 
			_variant_t thevalue ;
			//��ȡһ����¼�е������ֶ�ֵ���浽�ַ�������strs��
			int size = m_pRs->Fields->Count;
			for(long index = 0 ; index < m_pRs->Fields->Count; index++)
			{
				if (Index == index)
				{	
					thevalue = m_pRs->GetCollect(_variant_t(index));
					CString temp = vartostr(thevalue);
					Fields.push_back(temp);
				}
			}
			/*
			//����һ����¼����ά�ַ���������
						Fields.push_back(strs);*/
			//ת����һ����¼
			m_pRs->MoveNext(); 
		} 
		m_pRs->Close(); 
		return TRUE;
	}
	catch (_com_error e)//�쳣���� 
	{ 
		AfxMessageBox(e.Description()); 
	} 
	return FALSE;
}

BOOL CAdoDB::ExecuteQuery(LPCTSTR lpszSQL,CStrs& Fields)
{
	try
	{
		//�򿪼�¼��
		m_pRs->Open(_bstr_t(lpszSQL),(IDispatch*)m_pConn,adOpenDynamic,
			adLockOptimistic,adCmdText);
		while(!m_pRs->adoEOF) 
		{ 
			_variant_t thevalue ;
			//��ȡһ����¼�е������ֶ�ֵ���浽�ַ�������strs��
			int size = m_pRs->Fields->Count;
			for(long index = 0 ; index < m_pRs->Fields->Count; index++)
			{
				thevalue = m_pRs->GetCollect(_variant_t(index));
				CString temp = vartostr(thevalue);
				Fields.push_back(temp);
			}
			if (Fields.size() > 0)
				break;
			//����һ����¼����ά�ַ���������
			//ת����һ����¼
			m_pRs->MoveNext(); 
		} 
		m_pRs->Close(); 
		return TRUE;
	}
	catch (_com_error e)//�쳣���� 
	{ 
		AfxMessageBox(e.Description()); 
	} 
	return FALSE;
}

CString CAdoDB::vartostr(_variant_t &var)
{
	CString strValue;	
	switch (var.vt)
	{
	//�ַ���
	case VT_BSTR:
	case VT_LPSTR:
	case VT_LPWSTR:
		strValue = (LPCTSTR)(_bstr_t)var;
		break;
	//BYTE����
	case VT_I1:
	case VT_UI1:
		strValue.Format(_T("%d"), var.bVal);
		break;
	//������
	case VT_I2:
		strValue.Format(_T("%d"), var.iVal);
		break;
	//�޷��Ŷ�����
	case VT_UI2:
		strValue.Format(_T("%d"), var.uiVal);
		break;
	//����
	case VT_INT:
		strValue.Format(_T("%d"), var.intVal);
		break;
	//����
	case VT_I4: 
		strValue.Format(_T("%d"), var.lVal);
		break;
	//������
	case VT_I8: 
		strValue.Format(_T("%d"), var.lVal);
		break;
	//�޷�������
	case VT_UINT:
		strValue.Format(_T("%d"), var.uintVal);
		break;
	//�޷�������
	case VT_UI4: 
		strValue.Format(_T("%d"), var.ulVal);
		break;
	//�޷��ų�����
	case VT_UI8: 
		strValue.Format(_T("%d"), var.ulVal);
		break;
	case VT_VOID:
		strValue.Format(_T("%8x"), var.byref);
		break;
	//������,������λС��
	case VT_R4:
		strValue.Format(_T("%.2f"), var.fltVal);
		break;
	//˫������,������λС��
	case VT_R8:
		strValue.Format(_T("%.3f"), var.dblVal);
		break;
	//��ֵ��,������λС��
	case VT_DECIMAL: 
		strValue.Format(_T("%s"), (LPCTSTR)(_bstr_t)var);
		break;
    //������
	case VT_CY:
		{
			COleCurrency cy = var.cyVal;
			strValue = cy.Format();
		}
		break;
	//blob������
	case VT_BLOB:
	case VT_BLOB_OBJECT:
	case 0x2011:
		strValue = _T("[BLOB]");
		break;
	//������
	case VT_BOOL:		
		strValue = var.boolVal ? "TRUE" : "FALSE";
		break;
	//������
	case VT_DATE: 
		{
			DATE dt = var.date;
			COleDateTime oleDt = COleDateTime(dt); 
			strValue = oleDt.Format(_T("%Y-%m-%d %H:%M:%S"));
		}
		break;
	//NULLֵ
	case VT_NULL:
		strValue = "";
		break;
	case VT_EMPTY:
		strValue = "";
		break;
	case VT_UNKNOWN://δ֪����
	default:
		strValue = "UN_KNOW";
		break;
	}
	return strValue;
}