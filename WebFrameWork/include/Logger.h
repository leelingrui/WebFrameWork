#ifndef LOGGER_H
#define LOGGER_H

#include <exception>
#include <string>
#include <iostream>
#include <ostream>
#include <fstream>
#include <filesystem>
#include <time.h>

namespace logger
{
	class ILoggerProvider;
	using Logger = ILoggerProvider;
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
		EventId(int id, const std::string &message = "");
		const std::string &GetEventName() const;
		int GetId() const;

	private:
		int Id;
		const std::string Name;
	};

	class ILoggerProvider
	{
	public:
		ILoggerProvider();
		ILoggerProvider(std::string &logFilePath, size_t MaxLogSizePerPage = 256);
		virtual ~ILoggerProvider();
		virtual inline void Log(const LogLevel level, const EventId &eventId, const std::exception &exception, const std::string &message);
		virtual inline void Log(const LogLevel level, const EventId &eventId, const std::string &message);
		virtual inline void resetLoggerMaxSize(size_t _size);
		virtual inline void Rotate();
		virtual inline void insertNewOutputStream(std::ostream &output);
		virtual inline bool localFileSystemIsOpen();
		virtual inline void setConsole(bool flag);
		virtual inline void setLogLevel(const LogLevel& level);
		virtual inline void LogDiagnose(const EventId &eventId, const std::string &message);
		virtual inline void LogDiagnose(const EventId &eventId, const std::exception &exception, const std::string &message);
		virtual inline void LogDebug(const EventId &eventId, const std::string &message);
		virtual inline void LogDebug(const EventId &eventId, const std::exception &exception, const std::string &message);
		virtual inline void LogInformation(const EventId &eventId, const std::string &message);
		virtual inline void LogInformation(const EventId &eventId, const std::exception &exception, const std::string &message);
		virtual inline void LogWaring(const EventId &eventId, const std::string &message);
		virtual inline void LogWaring(const EventId &eventId, const std::exception &exception, const std::string &message);
		virtual inline void LogError(const EventId &eventId, const std::string &message);
		virtual inline void LogError(const EventId &eventId, const std::exception &exception, const std::string &message);
		virtual inline void LogFatal(const EventId &eventId, const std::string &message);
		virtual inline void LogFatal(const EventId &eventId, const std::exception &exception, const std::string &message);
		// void LogError();
		// void LogFatal();
	private:
		std::vector<std::ostream *> outputStreamArray;
		std::ofstream *localFileSystem;
		std::filesystem::path LogDir;
		size_t MaxSize;
		bool haveConsole;
		LogLevel currentLogLevel;
	};
}

#endif // !LOGGER_H