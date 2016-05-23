#ifndef __DUMMY_THREAD_H__
#define __DUMMY_THREAD_H__

class CDummyThread
{
public:
	CDummyThread( uint32_t threadID );

	void RunThread( void );

	uint32_t GetThreadID( void ) const { return m_threadID; };

private:
	uint32_t m_threadID;

	uint32_t sts_cpu_clock;
	uint32_t sts_task_clock;

	uint32_t m_curTaskPos;	

	// bool (*m_fpSystemHandler[SYSTEM_STATE_MAX])( CDummyThread * );
};

#endif // __DUMMY_THREAD_H__
