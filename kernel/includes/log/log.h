#ifndef __LOG_H__
#define __LOG_H__


// Place to log.
typedef enum _LogTarget
{
	LogMemory,
	LogConsole
} LogTarget_t;


// Log Types
#define LOG_TYPE_RAW			0x00
#define LOG_TYPE_INFORMATION		0x01
#define LOG_TYPE_DEBUG			0x02
#define LOG_TYPE_FATAL			0x03

// Colors for different log
#define LOG_COLOR_INFORMATION		0x2ECC71
#define LOG_COLOR_DEBUG			0x9B59B6
#define LOG_COLOR_ERROR			0xFF392B
#define LOG_COLOR_DEFAULT		0x0


// Prototype (Declarations)
void LogInternalPrint(int LogType, const char *Header, const char *Message);
void LogInformation(const char *System, const char *Message, ...);

#endif /* __LOG_H__ */
