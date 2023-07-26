#include "Logger.h"

namespace SCION_LOGGER {

    Logger::LogTime::LogTime(const std::string& date)
        : day{date.substr(0, 3)}, dayNumber{date.substr(8, 2)}, month{date.substr(4, 3)}
        , year{ date.substr(20, 4)}, time{date.substr(11, 8)}
    {

    }

    std::string Logger::CurrentDateTime()
    {
        auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

        char buf[30];
        ctime_s(buf, sizeof(buf), &time);

        LogTime logTime{ std::string{buf} };
        return std::format("{0}-{1}-{2} {3}", logTime.year, logTime.month, logTime.dayNumber, logTime.time);
    }

    Logger& Logger::GetInstance()
    {
        static Logger instance{};
        return instance;
    }

    void Logger::Init(bool consoleLog, bool retainLogs)
    {
        assert(!m_bInitialized && "Don not call Initialize more than once!");
        
        if (m_bInitialized)
        {
            std::cout << "Logger has already been initialized!" << std::endl;
            return;
        }

        m_bConsoleLog = consoleLog;
        m_bRetainLogs = retainLogs;
        m_bInitialized = true;
    }
}
