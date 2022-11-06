#ifndef LOGGER_H
#define LOGGER_H

#include <exception>
#include <string>
#include <iostream>
#include <ostream>
#include <fstream>
#include <filesystem>
#include <time.h>

namespace Logger
{
	enum class LogLevel
	{
		Diagnose,
		Debug,
		Information,
		Waring,
		Error,
		Fatal, 
		None
	};

	class EventId
	{
	public:
		EventId();
		/// <summary>
		/// Initializes an instance of the EventId struct.
		/// </summary>
		/// <param name="id">The numeric identifier for this event.</param>
		/// <param name="message">The name of this event.</param>
		EventId(int id, std::string* message = nullptr);
		std::string& GetEventName();
		int GetId();
	private:
		int Id;
		std::string Name;
	};

	class ILoggerProvider
	{
	public:
		ILoggerProvider();
		ILoggerProvider(std::string& logFilePath, size_t MaxLogSizePerPage = 256);
		virtual ~ILoggerProvider();
		virtual inline void Log(LogLevel level, EventId eventId, std::exception exception, std::string message);
		virtual inline void Log(LogLevel level, EventId eventId, std::string message);
		virtual inline void Rotate();
		virtual inline void insertNewOutputStream(std::ostream& output);
		virtual bool localFileSystemIsOpen();
		virtual void setConsole(bool flag);
		virtual	void setLogLevel(LogLevel level);
		virtual void LogDiagnose(EventId eventId, std::string message);
		virtual void LogDiagnose(EventId eventId, std::exception exception, std::string message);
		virtual void LogDebug(EventId eventId, std::string message);
		virtual void LogDebug(EventId eventId, std::exception exception, std::string message);
		virtual void LogInformation(EventId eventId, std::string message);
		virtual void LogInformation(EventId eventId, std::exception exception, std::string message);
		virtual void LogWaring(EventId eventId, std::string message);
		virtual void LogWaring(EventId eventId, std::exception exception, std::string message);
		virtual void LogError(EventId eventId, std::string message);
		virtual void LogError(EventId eventId, std::exception exception, std::string message);
		virtual void LogFatal(EventId eventId, std::string message);
		virtual void LogFatal(EventId eventId, std::exception exception, std::string message);
		//void LogError();
		//void LogFatal();
	private:
		std::vector<std::ostream*> outputStreamArray;
		std::ofstream* localFileSystem;
		std::filesystem::path LogDir;
		int MaxSize;
		bool haveConsole;
		LogLevel currentLogLevel;
	};
}


#endif // !LOGGER_H
