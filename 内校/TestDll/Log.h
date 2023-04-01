#ifndef LOG_H
#define LOG_H


#include <fstream>
#include <string>
#include <sstream>
#include <ctime>

using namespace std;



class Log
{
public:
    Log();
    ~Log();

    bool Open(string sFileName);
    void Close();
    bool CommonLogInit();
	bool OffsetLogInit();
    void Enable();
    void Disable();
    string GetTimeStr();

	void LogFile(string log);

//	Log mainLog;
	template <class T>
	static void WriteFuncBegin(T x)
	{
		CHAR szPath[MAX_PATH] = {0};
		GetLogFilePath(szPath);
	    ofstream fout(szPath,ios::app);
		fout.seekp(ios::end);
		cout << GetTimeStr() << "   " << x << endl;
	}

    template <typename T> void LogOut(const T& value)
    {
        if (m_bEnabled)
        {
            m_tOLogFile << value;
        }
    }

    template <typename T> void LogOutLn(const T& value)
    {
        if (m_bEnabled)
        {
            m_tOLogFile << value << endl;
        }
    }

    void LogOutLn()
    {
        if (m_bEnabled)
        {
            m_tOLogFile << endl;
        }
    }

    template <typename T> Log& operator<<(const T& value)
    {
        if (m_bEnabled)
        {
            m_tOLogFile << value;
        }
        return (*this);
    }

    Log& operator<<(ostream& (*_Pfn)(ostream&))
    {
        if (m_bEnabled)
        {
            (*_Pfn)(m_tOLogFile);
        }
        return (*this);
    }


private:
    template<typename T> string ValueToStr(T value)
    {
        ostringstream ost;
        ost << value;
        return ost.str();
    }
private:
    ofstream m_tOLogFile;
    bool m_bEnabled;
};

#endif



