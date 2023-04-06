#include "Bali.h"

#include <cstdio>
#include <fstream>
#include <io.h>
#include <direct.h>
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")
#include <windows.h>
#include <atltime.h>
#include <atlstr.h>

std::string Bali::address;
uint16_t Bali::port = 1111;
std::string Bali::logPath;

void Bali::config(const std::string& address, uint16_t port, const std::string& logPath)
{
	Bali::address = address;
	Bali::port = port;
	Bali::logPath = logPath;

	Bali::createPath(address);
}

bool Bali::send(
	const std::string& sn,
	const std::string& fixture_id,
	const std::string& head_id,
	double value1, double value2, double value3,
	double fillEmpty, double fillDegrass, double degrassEmpty
)
{
	auto mes = Bali::createMessage(
		sn, fixture_id, head_id,
		value1, value2, value3,
		fillEmpty, fillDegrass, degrassEmpty);
	Bali::saveLog(Bali::logPath, mes);
	return Bali::sendMessage(Bali::address, Bali::port, mes);
}

std::string Bali::createMessage(
	const std::string& sn,
	const std::string& fixture_id,
	const std::string& head_id,
	double value1, double value2, double value3,
	double fillEmpty, double fillDegrass, double degrassEmpty
)
{
	return
		"{\n"
		+ sn + "@start\n"
		+ sn + "@dut_pos@" + fixture_id + "@" + head_id + "\n"
		+ sn + "@pdata@value1@" + std::to_string(value1) + "\n"
		+ sn + "@pdata@value2@" + std::to_string(value2) + "\n"
		+ sn + "@pdata@value3@" + std::to_string(value3) + "\n"
		+ sn + "@pdata@fillEmpty@" + std::to_string(fillEmpty) + "\n"
		+ sn + "@pdata@fillDegrass@" + std::to_string(fillDegrass) + "\n"
		+ sn + "@pdata@degrassEmpty@" + std::to_string(degrassEmpty) + "\n"
		+ sn + "@submit\n"
		+ "}\n";
}

bool Bali::sendMessage(const std::string& address, uint16_t port, const std::string& data)
{
	//1.��ȡ�汾��Ϣ
	WSADATA wsaData;
	//����1:Э��汾�� WORD ���� 
	//����2:�洢�汾��Ϣ�Ľṹ��ĵ�ַ
	::WSAStartup(MAKEWORD(2, 2), &wsaData);
	//MAKEWORD��һ���꺯�������������ֽ����͵����ݣ����ϳ�һ��WORD���͵�����
	//MAKEWORD(2,2) ���ǲ���һ�����ֽ�Ϊ2�����ֽ�Ϊ2��16λ������
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		//printf("�õ��İ汾�Ų���ȷ,����汾ʧ��\n");
		//7 ���Э��汾
		::WSACleanup();
		return false;
	}
	//2 ����tcp socket
	SOCKET clientSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (SOCKET_ERROR == clientSocket)//�������ֵΪ-1
	{
		//printf("����tcp�׽���ʧ��:%d\n", GetLastError());
		//7 ���Э��汾
		::WSACleanup();
		return false;
	}
	//3 ���÷�����Э���ַ�� �����ÿ�����Ϣ��
	SOCKADDR_IN serverAddr = { 0 };
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.S_un.S_addr = ::inet_addr(address.c_str()); //�����ʮ����ת��32λunsigned int
	serverAddr.sin_port = htons(port); //��Ϊ���ǵ�������С��ϵͳ��·�����������Ǵ��ϵͳ���������Ƿ������ϵͳ�Ķ˿ں�Ӧ��С��ת���
	//4 ���ӷ�����
	int r;
	r = ::connect(clientSocket, (sockaddr*)&serverAddr, sizeof serverAddr);
	if (SOCKET_ERROR == r)
	{
		//printf("���ӷ�����ʧ��\n");
		//6 �ر�socket
		::closesocket(clientSocket);
		//7 ���Э��汾
		::WSACleanup();
		return false;
	}

	//5 ͨ��
	r = ::send(clientSocket, data.c_str(), data.size(), NULL);
	if (r <= 0)
	{
		//printf("����ʧ��!\n");

		//6 �ر�socket
		::closesocket(clientSocket);
		//7 ���Э��汾
		::WSACleanup();
		return false;
	}

	//6 �ر�socket
	::closesocket(clientSocket);
	//7 ���Э��汾
	::WSACleanup();

	return true;
}

void Bali::saveLog(const std::string& path, const std::string& message)
{
	std::ofstream ofs;
	ofs.open(path + Bali::getData() + ".log");
	ofs << message;
	ofs.close();
}

void Bali::createPath(const std::string& path)
{
	std::string tmpDirPath;
	for (uint32_t i = 0; i < path.size(); ++i)
	{
		tmpDirPath.push_back(path[i]);

		// ����Ƿ���Ҫ����Ŀ¼
		if (tmpDirPath[i] == '\\')
		{
			// ���Ŀ¼�Ƿ��Ѵ���
			if (::_access(tmpDirPath.c_str(), 0) != 0)
			{
				// ����Ŀ¼
				int32_t ret = ::_mkdir(tmpDirPath.c_str());
				if (ret != 0)
				{
					// ����Ŀ¼ʧ�ܣ��˳�����
					return;
				}
			}
		}
	}

	// ��������Ŀ¼·��
	::_mkdir(tmpDirPath.c_str());
}

std::string Bali::getData()
{
	CTime StartTime = CTime::GetCurrentTime();
	CString strStartTime = StartTime.Format("%Y-%m-%d");
	return strStartTime.GetBuffer();
}
