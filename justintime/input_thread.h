#ifndef __INPUT_THREAD_H__
#define __INPUT_THREAD_H__

#include <stdint.h>

class CIOStdioConnection;

class CInputThread
{
public:
	CInputThread( uint32_t threadID, CIOStdioConnection *pConnection );

	void RunThread( void );

private:
	typedef bool (*tCmdHandler) ( CInputThread *pThis, vector<string> &args );

	// Command handlers
	std::map<std::string, tCmdHandler> m_cmdMap;

	uint32_t m_threadID;
	CIOStdioConnection *m_pConnection;
};

#endif // __INPUT_THREAD_H__
