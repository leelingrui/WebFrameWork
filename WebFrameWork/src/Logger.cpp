#include <Logger.h>
#include <cstdarg>

namespace logger {
	int EventId::GetId() const
	{
		return Id;
	}


	EventId::EventId(int id, const std::string& message) : Name(message)
	{
		Id = id;
	}


	EventId::EventId() : Name()
	{
		Id = 0;
	}


	const std::string& EventId::GetEventName() const
	{
		return Name;
	}


	void ILoggerProvider::LogDiagnose(const EventId& eventId, const std::string& message)
	{
		Log(LogLevel::Diagnose, eventId, message);
	}


	void ILoggerProvider::LogDiagnose(const EventId& eventId, const std::exception& exception, const std::string& message)
	{
		Log(LogLevel::Diagnose, eventId, exception, message);
	}


	void ILoggerProvider::LogDebug(const EventId& eventId, const std::string& message)
	{
		Log(LogLevel::Debug, eventId, message);
	}


	void ILoggerProvider::LogDebug(const EventId& eventId, const std::exception& exception, const std::string& message)
	{
		Log(LogLevel::Debug, eventId, exception, message);
	}


	void ILoggerProvider::LogInformation(const EventId& eventId, const std::string& message)
	{
		Log(LogLevel::Information, eventId, message);
	}


	void ILoggerProvider::LogInformation(const EventId& eventId, const std::exception& exception, const std::string& message)
	{
		Log(LogLevel::Information, eventId, exception, message);
	}


	void ILoggerProvider::LogWaring(const EventId& eventId, const std::string& message)
	{
		Log(LogLevel::Waring, eventId, message);
	}


	void ILoggerProvider::LogWaring(const EventId& eventId, const std::exception& exception, const std::string& message)
	{
		Log(LogLevel::Waring, eventId, exception, message);
	}


	void ILoggerProvider::LogError(const EventId& eventId, const std::string& message)
	{
		Log(LogLevel::Error, eventId, message);

	}


	void ILoggerProvider::LogError(const EventId& eventId, const std::exception& exception, const std::string& message)
	{
		Log(LogLevel::Error, eventId, exception, message);
	}


	void ILoggerProvider::LogFatal(const EventId& eventId, const std::string& message)
	{
		Log(LogLevel::Fatal, eventId, message);
	}


	void ILoggerProvider::LogFatal(const EventId& eventId, const std::exception& exception, const std::string& message)
	{
		Log(LogLevel::Fatal, eventId, exception, message);
	}


	bool ILoggerProvider::localFileSystemIsOpen()
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


	void ILoggerProvider::Rotate()
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


	void ILoggerProvider::Log(const LogLevel level, const EventId& eventId, const std::string& message)
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
		if (localFileSystem != nullptr && localFileSystem->tellp() / ((size_t)1 << 20) > MaxSize)
			Rotate();
	}


	void ILoggerProvider::setLogLevel(const LogLevel& level)
	{
		currentLogLevel = level;
	}


	void ILoggerProvider::Log(const LogLevel level, const EventId& eventId, const std::exception& exception, const std::string& message)
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
		if (localFileSystem != nullptr && localFileSystem->tellp() / ((size_t)1 << 20) > MaxSize)
			Rotate();
	}


	void ILoggerProvider::insertNewOutputStream(std::ostream& output)
	{
		outputStreamArray.emplace_back(&output);
	}


	void ILoggerProvider::setConsole(bool flag)
	{
		haveConsole = true;
	}

	void ILoggerProvider::resetLoggerMaxSize(size_t _size)
	{
		MaxSize = _size;
	}

	ILoggerProvider::ILoggerProvider()
	{
		MaxSize = 0;
		currentLogLevel = LogLevel::Information;
		haveConsole = true;
		outputStreamArray.emplace_back(&std::cout);
		localFileSystem = nullptr;
	}

	ILoggerProvider::ILoggerProvider(std::string& logFilePath, size_t MaxLogSizePerPage)
	{
		haveConsole = true;
		MaxSize = MaxLogSizePerPage;
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
		else
		{
			localFileSystem = nullptr;
		}
	}
}