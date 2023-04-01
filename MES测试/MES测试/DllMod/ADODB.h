#ifndef ADO_DB_H
#define ADO_DB_H

//#pragma   warning(disable:4146)
//#import "C:\Program Files\common files\system\ado\msado15.dll" no_namespace rename("EOF","adoEOF")
//#pragma   warning(default:4146)

#pragma warning(disable:4146)
#import "C:/Program Files/Common Files/System/ado/msado15.dll" no_namespace rename("EOF", "adoEOF") rename("DataTypeEnum", "adoDataTypeEnum") rename("FieldAttributeEnum", "adoFielAttributeEnum") rename("EditModeEnum", "adoEditModeEnum")  rename("LockTypeEnum", "adoLockTypeEnum") rename("RecordStatusEnum", "adoRecordStatusEnum") rename("ParameterDirectionEnum", "adoParameterDirectionEnum")

#pragma warning(disable:4786)

#include <vector>
//利用vector数组结构构造一维字符串数组
typedef ::std::vector<CString> CStrs;
//利用vector数组结构构造二维字符串数组
typedef ::std::vector<CStrs> CDStrs;
//利用vector数组结构构造三维字符串数组
typedef ::std::vector<CDStrs> CTStrs;


class CAdoDB
{

public:
	CAdoDB();
	virtual ~CAdoDB();
public:
	//数据库连接
	BOOL Open(LPCTSTR lpszConnect =_T(""), long lOptions = adConnectUnspecified);
	//数据库关闭
	void Close();
	//判断数据库是否处于打开状态
	BOOL IsOpen();	
	//返回数据库连接指针
	_ConnectionPtr GetConnection() {return m_pConn;};
	//返回记录集指针
	_RecordsetPtr GetRecordset() {return m_pRs;};
	//返回连接字符串
	CString GetConString() {return m_strConn;};
	//执行SQL语句
	_RecordsetPtr Execute(LPCTSTR strSQL, long lOptions = adCmdText);
    //_variant_t数据格式的数据转换为字符串格式
	CString vartostr(_variant_t& var);
	//根据SQL语句查询数据库，获取的记录集保存在二维字符串数组中
	BOOL ExecuteQuery(LPCTSTR lpszSQL,CDStrs& Fields);
	//根据SQL语句查询数据库，获取的记录集保存在一维字符串数组中,记录集是一个字段的记录集
	BOOL ExecuteQuery(LPCTSTR lpszSQL,long Index,CStrs& Fields);
	//只查询一条记录
	BOOL ExecuteQuery(LPCTSTR lpszSQL,CStrs& Fields);
	//根据SQL语句查询数据库，获取一个字段值
	BOOL ExecuteQueryValue(LPCTSTR lpszSQL,CString& value);
	//获得记录集数目
	int GetAdoItemCount(CString sql);
	//开始事务模式
	bool BeginTrans();
	//回滚事务模式
	bool RollBackTrans();
	//提交事务
	bool CommitTrans();

protected:
	//连接字符串
	CString			m_strConn;
	//数据库连接指针
	_ConnectionPtr	m_pConn;	
	//记录集指针
	_RecordsetPtr	m_pRs;	
};

#endif