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
	//1.获取版本信息
	WSADATA wsaData;
	//参数1:协议版本号 WORD 类型 
	//参数2:存储版本信息的结构体的地址
	::WSAStartup(MAKEWORD(2, 2), &wsaData);
	//MAKEWORD是一个宏函数，传入两个字节类型的数据，整合成一个WORD类型的数据
	//MAKEWORD(2,2) 就是产生一个高字节为2，低字节为2的16位的数据
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		//printf("得到的版本号不正确,请求版本失败\n");
		//7 清除协议版本
		::WSACleanup();
		return false;
	}
	//2 创建tcp socket
	SOCKET clientSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (SOCKET_ERROR == clientSocket)//如果返回值为-1
	{
		//printf("创建tcp套接字失败:%d\n", GetLastError());
		//7 清除协议版本
		::WSACleanup();
		return false;
	}
	//3 配置服务器协议地址族 （配置控制信息）
	SOCKADDR_IN serverAddr = { 0 };
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.S_un.S_addr = ::inet_addr(address.c_str()); //将点分十进制转化32位unsigned int
	serverAddr.sin_port = htons(port); //因为我们的主机是小端系统，路由器交换机是大端系统，所以我们发给大端系统的端口号应该小端转大端
	//4 连接服务器
	int r;
	r = ::connect(clientSocket, (sockaddr*)&serverAddr, sizeof serverAddr);
	if (SOCKET_ERROR == r)
	{
		//printf("连接服务器失败\n");
		//6 关闭socket
		::closesocket(clientSocket);
		//7 清除协议版本
		::WSACleanup();
		return false;
	}

	//5 通信
	r = ::send(clientSocket, data.c_str(), data.size(), NULL);
	if (r <= 0)
	{
		//printf("发送失败!\n");

		//6 关闭socket
		::closesocket(clientSocket);
		//7 清除协议版本
		::WSACleanup();
		return false;
	}

	//6 关闭socket
	::closesocket(clientSocket);
	//7 清除协议版本
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

		// 检查是否需要创建目录
		if (tmpDirPath[i] == '\\')
		{
			// 检查目录是否已存在
			if (::_access(tmpDirPath.c_str(), 0) != 0)
			{
				// 创建目录
				int32_t ret = ::_mkdir(tmpDirPath.c_str());
				if (ret != 0)
				{
					// 创建目录失败，退出函数
					return;
				}
			}
		}
	}

	// 创建完整目录路径
	::_mkdir(tmpDirPath.c_str());
}

std::string Bali::getData()
{
	CTime StartTime = CTime::GetCurrentTime();
	CString strStartTime = StartTime.Format("%Y-%m-%d");
	return strStartTime.GetBuffer();
}
