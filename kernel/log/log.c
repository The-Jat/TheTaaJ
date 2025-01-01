// Includes
#include <log/log.h>
#include <video/boot_terminal.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

// Globals
LogTarget_t g_logTarget = LogConsole;

// Internal Log Print (common) 
void LogInternalPrint(int LogType, const char *Header, const char *Message)
{
	// Check for target, if it is LogConsole.
	if (g_logTarget == LogConsole) 
	{
		// First handle the header.
		if (LogType != LOG_TYPE_RAW)
		{
			// Select Color on the basis of LogType
			if (LogType == LOG_TYPE_INFORMATION)
				GetBootTerminal()->FgColor = LOG_COLOR_INFORMATION;
			else if (LogType == LOG_TYPE_DEBUG)
				GetBootTerminal()->FgColor = LOG_COLOR_DEBUG;
			else if (LogType == LOG_TYPE_FATAL)
				GetBootTerminal()->FgColor = LOG_COLOR_ERROR;

			// Print to the console.
			printf("[%s] ", Header);
		}

		// Print the Message to the console.
		printf("%s\n", Message);

		// Set console Foreground color to default.
		GetBootTerminal()->FgColor = LOG_COLOR_DEFAULT;
	}
}

/* Raw Log */
void Log(const char *Message, ...)
{
	/* Output Buffer */
	char oBuffer[256];
	va_list ArgList;

	/* Sanitize arguments */
	if (Message == NULL) {
		return;
	}

	/* Memset buffer */
	memset(&oBuffer[0], 0, 256);

	/* Format string */
	va_start(ArgList, Message);
	vsprintf(oBuffer, Message, ArgList);
	va_end(ArgList);

	/* Append newline */
	strcat(oBuffer, "\n");

	/* Print */
	LogInternalPrint(LOG_TYPE_RAW, NULL, oBuffer);
}


// Raw Log
void LogRaw(const char *Message, ...)
{
	/* Output Buffer */
	char oBuffer[256];
	va_list ArgList;

	/* Sanitize arguments */
	if (Message == NULL) {
		return;
	}

	/* Memset buffer */
	memset(&oBuffer[0], 0, 256);

	/* Format string */
	va_start(ArgList, Message);
	vsprintf(oBuffer, Message, ArgList);
	va_end(ArgList);

	/* Print */
	LogInternalPrint(LOG_TYPE_RAW, NULL, oBuffer);
}


// Output debug to log
void LogDebug(const char *System, const char *Message, ...)
{
	/* Output Buffer */
	char oBuffer[256];
	va_list ArgList;

	/* Sanitize arguments */
	if (System == NULL
		|| Message == NULL) {
		return;
	}

	/* Memset buffer */
	memset(&oBuffer[0], 0, 256);

	/* Format string */
	va_start(ArgList, Message);
	vsprintf(oBuffer, Message, ArgList);
	va_end(ArgList);

	/* Print */
	LogInternalPrint(LOG_TYPE_DEBUG, System, oBuffer);
}


// Output Error to log
void LogFatal(const char *System, const char *Message, ...)
{
	/* Output Buffer */
	char oBuffer[256];
	va_list ArgList;

	/* Sanitize arguments */
	if (System == NULL
		|| Message == NULL) {
		return;
	}

	/* Memset buffer */
	memset(&oBuffer[0], 0, 256);

	/* Format string */
	va_start(ArgList, Message);
	vsprintf(oBuffer, Message, ArgList);
	va_end(ArgList);

	/* Print */
	LogInternalPrint(LOG_TYPE_FATAL, System, oBuffer);
}


// Output information to log
void LogInformation(const char *System, const char *Message, ...)
{
	// Output Buffer
	char oBuffer[256];
	va_list ArgList;

	// Sanitize arguments
	if (System == NULL
		|| Message == NULL) {
		return;
	}

	// Memset buffer, i.e reset
	memset(&oBuffer[0], 0, 256);

	// Format string
	va_start(ArgList, Message);
	vsprintf(oBuffer, Message, ArgList);
	va_end(ArgList);

	// Print
	LogInternalPrint(LOG_TYPE_INFORMATION, System, oBuffer);
}
