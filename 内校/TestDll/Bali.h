#pragma once

#include <string>

class Bali final
{
	Bali() = delete;

public:
	static void config(const std::string& address, uint16_t port, const std::string& logPath, bool changeEndian = false);
	static bool send(
		const std::string& sn,
		const std::string& fixture_id,
		const std::string& head_id,
		double value1, double value2, double value3,
		double fillEmpty, double fillDegrass, double degrassEmpty
	);
	static void release();

private:
	static std::string createMessage(
		const std::string& sn,
		const std::string& fixture_id,
		const std::string& head_id,
		double value1, double value2, double value3,
		double fillEmpty, double fillDegrass, double degrassEmpty
	);
	static bool sendMessage(const std::string& address, uint16_t port, const std::string& data);
	static void saveLog(const std::string& path, const std::string& message);

	static void createPath(const std::string& path);
	static std::string getData();

	static std::string address;
	static uint16_t port;
	static std::string logPath;
};