#ifndef __WORKER_LOG_H__
#define __WORKER_LOG_H__

class CWorkerLogger
{
public:
	CWorkerLogger();
	~CWorkerLogger();

	bool IsEnabled( void ) const { return m_bEnabled; };
	
	void Disable( void );
	void Enable( void );

	void WriteLogData( uint8_t workerID, const char *pszString, ... );

private:
	std::mutex m_oLogFileMutex;

	bool m_bEnabled;
	FILE *m_pLogFP;
};

#endif // __WORKER_LOG_H__
