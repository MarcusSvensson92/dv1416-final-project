#ifndef TIMER_H
#define TIMER_H

class Timer
{
public:
	Timer(void);

	double getDeltaTime(void) const { return m_deltaTime; }
	bool   isStopped(void)	  const { return m_stopped; }

	void reset(void);
	void start(void);
	void stop(void);
	void tick(void);
private:
	double m_secondsPerCount;
	double m_deltaTime;

	__int64 m_prevTime;
	__int64 m_stopTime;

	bool m_stopped;
};

#endif