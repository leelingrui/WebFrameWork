#include <Logger.h>
#include <cstdarg>

namespace Logger{
	inline int EventId::GetId()
	{
		return Id;
	}


	EventId::EventId(int id, std::string* message)
	{
		Id = id;
		if(message) Name = *message;
	}


	inline EventId::EventId()
	{
		Id = 0;
	}


	inline std::string& EventId::GetEventName()
	{
		return Name;
	}


	inline void ILoggerProvider::LogDiagnose(EventId eventId, std::string message)
	{
		Log(LogLevel::Diagnose, eventId, message);
	}


	inline void ILoggerProvider::LogDiagnose(EventId eventId, std::exception exception, std::string message)
	{
		Log(LogLevel::Diagnose, eventId, exception, message);
	}


	inline void ILoggerProvider::LogDebug(EventId eventId, std::string message)
	{
		Log(LogLevel::Debug, eventId, message);
	}


	inline void ILoggerProvider::LogDebug(EventId eventId, std::exception exception, std::string message)
	{
		Log(LogLevel::Debug, eventId, exception, message);
	}


	inline 	void ILoggerProvider::LogInformation(EventId eventId, std::string message)
	{
		Log(LogLevel::Information, eventId, message);
	}


	inline 	void ILoggerProvider::LogInformation(EventId eventId, std::exception exception, std::string message)
	{
		Log(LogLevel::Information, eventId, exception, message);
	}


	inline void ILoggerProvider::LogWaring(EventId eventId, std::string message)
	{
		Log(LogLevel::Waring, eventId, message);
	}


	inline void ILoggerProvider::LogWaring(EventId eventId, std::exception exception, std::string message)
	{
		Log(LogLevel::Waring, eventId, exception, message);
	}


	inline void ILoggerProvider::LogError(EventId eventId, std::string message)
	{
		Log(LogLevel::Error, eventId, message);

	}


	inline 	void ILoggerProvider::LogError(EventId eventId, std::exception exception, std::string message)
	{
		Log(LogLevel::Error, eventId, exception, message);
	}


	inline void ILoggerProvider::LogFatal(EventId eventId, std::string message)
	{
		Log(LogLevel::Fatal, eventId, message);
	}


	inline void ILoggerProvider::LogFatal(EventId eventId, std::exception exception, std::string message)
	{
		Log(LogLevel::Fatal, eventId, exception, message);
	}


	inline ILoggerProvider::ILoggerProvider()
	{
		MaxSize = 0;
		outputStreamArray.emplace_back(&std::cout);
	}


	inline bool ILoggerProvider::localFileSystemIsOpen()
	{
		return localFileSystem;
	}


	ILoggerProvider::~ILoggerProvider()
	{
		for (int var = 1; var < outputStreamArray.size(); var++)
		{
			delete outputStreamArray[var];
		}
	}


	inline void ILoggerProvider::Rotate()
	{
		char timeformatstring[32];
		time_t time_tick = time(NULL);
		tm times;
		std::string test;
		std::filesystem::path aimpath;
		dynamic_cast<std::ofstream*>(localFileSystem)->close();
		delete localFileSystem;
		localtime_s(&times, &time_tick);
		strftime(timeformatstring, 32, "%F_%I-%M", &times);
		aimpath += LogDir.parent_path();
		aimpath.append(LogDir.stem().c_str());
		aimpath += timeformatstring;
		aimpath += LogDir.extension();
		std::filesystem::create_directories(LogDir.parent_path());
		std::filesystem::rename(LogDir, aimpath);
		std::ofstream* output = new std::ofstream;
		output->open(LogDir, std::ios::app);
		if (output->is_open())
		{
			localFileSystem = output;
			outputStreamArray[1] = localFileSystem;
		}
		else throw std::runtime_error("can not open target file");
	}


	inline void ILoggerProvider::Log(LogLevel level, EventId eventId, std::string message)
	{
		if (currentLogLevel > level) return;
		switch (level)
		{
		case LogLevel::Diagnose:
			for (int var = !haveConsole; var < outputStreamArray.size(); var++)
			{
				*outputStreamArray[var] << "Diagnose [" << eventId.GetId() << "]: " << message << "\n";
			}
			break;
		case LogLevel::Debug:
			for (int var = !haveConsole; var < outputStreamArray.size(); var++)
			{
				*outputStreamArray[var] << "Debug [" << eventId.GetId() << "]: " << message << "\n";
			}
			break;
		case LogLevel::Information:
			for (int var = !haveConsole; var < outputStreamArray.size(); var++)
			{
				*outputStreamArray[var] << "Information [" << eventId.GetId() << "]: " << message << "\n";
			}
			break;
		case LogLevel::Waring:
			for (int var = !haveConsole; var < outputStreamArray.size(); var++)
			{
				*outputStreamArray[var] << "Waring [" << eventId.GetId() << "]: " << message << "\n";
			}
			break;
		case LogLevel::Error:
			for (int var = !haveConsole; var < outputStreamArray.size(); var++)
			{
				*outputStreamArray[var] << "Error [" << eventId.GetId() << "]: " << "---" << message << "\n";
			}
			break;
		case LogLevel::Fatal:
			for (int var = !haveConsole; var < outputStreamArray.size(); var++)
			{
				*outputStreamArray[var] << "Fatal [" << eventId.GetId() << "]: " << message << "\n";
			}
			break;
		case LogLevel::None:
			break;
		default:
			break;
		}
		if (localFileSystem != nullptr && localFileSystem->tellp() / ((long long)1 << 20) > MaxSize)
			Rotate();
	}


	inline void ILoggerProvider::setLogLevel(LogLevel level)
	{
		currentLogLevel = level;
	}


	inline void ILoggerProvider::Log(LogLevel level, EventId eventId, std::exception exception, std::string message)
	{
		if (currentLogLevel > level) return;
		switch (level)
		{
		case LogLevel::Diagnose:
			for (int var = !haveConsole; var < outputStreamArray.size(); var++)
			{
				*outputStreamArray[var] << "Diagnose [" << eventId.GetId() << "]: " << exception.what() << "---" << message << "\n";
			}
			break;
		case LogLevel::Debug:
			for (int var = !haveConsole; var < outputStreamArray.size(); var++)
			{
				*outputStreamArray[var] << "Debug [" << eventId.GetId() << "]: " << exception.what() << "---" << message << "\n";
			}
			break;
		case LogLevel::Information:
			for (int var = !haveConsole; var < outputStreamArray.size(); var++)
			{
				*outputStreamArray[var] << "Information [" << eventId.GetId() << "]: " << exception.what() << "---" << message << "\n";
			}
			break;
		case LogLevel::Waring:
			for (int var = !haveConsole; var < outputStreamArray.size(); var++)
			{
				*outputStreamArray[var] << "Waring [" << eventId.GetId() << "]: " << exception.what() << "---" << message << "\n";
			}
			break;
		case LogLevel::Error:
			for (int var = !haveConsole; var < outputStreamArray.size(); var++)
			{
				*outputStreamArray[var] << "Error [" << eventId.GetId() << "]: " << exception.what() << "---" << message << "\n";
			}
			break;
		case LogLevel::Fatal:
			for (int var = !haveConsole; var < outputStreamArray.size(); var++)
			{
				*outputStreamArray[var] << "Fatal [" << eventId.GetId() << "]: " << exception.what() << "---" << message << "\n";
			}
			break;
		case LogLevel::None:
			break;
		default:
			break;
		}
		if (localFileSystem != nullptr && localFileSystem->tellp() / ((long long)1 << 20) > MaxSize)
			Rotate();
	}


	inline void ILoggerProvider::insertNewOutputStream(std::ostream& output)
	{
		outputStreamArray.emplace_back(&output);
	}


	inline void ILoggerProvider::setConsole(bool flag)
	{
		haveConsole = true;
	}


	ILoggerProvider::ILoggerProvider(std::string& logFilePath, size_t MaxLogSizePerPage)
	{
		MaxSize = 0;
		LogDir = logFilePath;
		std::filesystem::path openpath(LogDir);
		std::ofstream* output = new std::ofstream;
		std::filesystem::create_directories(LogDir.parent_path());
		output->open(openpath, std::ios::app);
		outputStreamArray.emplace_back(&std::cout);
		if (output->is_open())
		{
			localFileSystem = output;
			outputStreamArray.emplace_back(localFileSystem);
		}
	}
}