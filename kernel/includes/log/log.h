#ifndef __LOG_H__
#define __LOG_H__

/* Default size to 4kb */
#define LOG_INITIAL_SIZE		(1024 * 4)
#define LOG_PREFFERED_SIZE		(1024 * 65)


typedef enum _LogTarget
{
	LogMemory,
	LogConsole,
//	LogFile
} LogTarget_t;

typedef enum _LogLevel
{
	LogLevel1,
	LogLevel2,
	LogLevel3
} LogLevel_t;

/* Log Types */
#define LOG_TYPE_RAW			0x00
#define LOG_TYPE_INFORMATION		0x01
#define LOG_TYPE_DEBUG			0x02
#define LOG_TYPE_FATAL			0x03

/* Colors */
#define LOG_COLOR_INFORMATION		0x2ECC71
#define LOG_COLOR_DEBUG			0x9B59B6
#define LOG_COLOR_ERROR			0xFF392B
#define LOG_COLOR_DEFAULT		0x0

void LogInformation(const char *System, const char *Message, ...);

#endif /* __LOG_H__ */
