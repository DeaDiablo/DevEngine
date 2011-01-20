#include <Kernel/DevTimer.h>

using namespace dev;

Timer::Timer() :
  _numProc(1),
  _thread(0)
{
  init();
}

Timer::~Timer()
{
}

Timer* Timer::GetTimer()
{
  static Timer timer;
  return &timer;
}

void Timer::init()
{
  DWORD_PTR procMask;
  DWORD_PTR sysMask;
  GetProcessAffinityMask(GetCurrentProcess(), &procMask, &sysMask);

  if (procMask == 0)
     procMask = 1;

  _numProc = 1;
  while((procMask & _numProc) == 0)
  {
    procMask <<= 1;
  }

  _numProc = procMask;
 
  _thread = GetCurrentThread();
 
  DWORD_PTR oldMask = SetThreadAffinityMask(_thread, _numProc);

  LARGE_INTEGER _frequency;

  if (QueryPerformanceFrequency(&_frequency) &&
      QueryPerformanceCounter(&_counter))
  {
    _freq         = (double)_frequency.QuadPart;
    _startTime[0] = (double)_counter.QuadPart;
    
    for(byte i = 1; i < 255; i++)
    {
      _startTime[i] = _startTime[0];
    }

    memcpy(_tickTime, _startTime, 255);
    memcpy(_deltaTime, _startTime, 255);
  }
  else
  {
    _freq = 0;
  }

  SetThreadAffinityMask(_thread, oldMask);
}

void Timer::Reset(byte numTimer)
{
  DWORD_PTR oldMask = SetThreadAffinityMask(_thread, _numProc);
  QueryPerformanceCounter(&_counter);
  SetThreadAffinityMask(_thread, oldMask);
  _startTime[numTimer] = (double)_counter.QuadPart;
  _tickTime[numTimer]  = _startTime[numTimer];
  _deltaTime[numTimer] = _deltaTime[numTimer];
}

double Timer::GetTimeNS(byte numTimer)
{
  return getTime(numTimer, 1000000000);
}

double Timer::GetTimeMkS(byte numTimer)
{
  return getTime(numTimer, 1000000);
}

double Timer::GetTimeMS(byte numTimer)
{
  return getTime(numTimer, 1000);
}

double Timer::GetTimeS(byte numTimer)
{
  return getTime(numTimer, 1);
}

double Timer::GetDeltaTimeNS(byte numTimer)
{
  if (!_freq)
    return 0;

  _prevTickTime[numTimer] = _tickTime[numTimer];
  _tickTime[numTimer] = getTime(numTimer, 1000000000);

  return (_tickTime[numTimer] - _prevTickTime[numTimer]);
}

double Timer::GetDeltaTimeMkS(byte numTimer)
{
  if (!_freq)
    return 0;

  _prevTickTime[numTimer] = _tickTime[numTimer];
  _tickTime[numTimer] = getTime(numTimer, 1000000);

  return (_tickTime[numTimer] - _prevTickTime[numTimer]);
}

double Timer::GetDeltaTimeMS(byte numTimer)
{
  if (!_freq)
    return 0;

  _prevTickTime[numTimer] = _tickTime[numTimer];
  _tickTime[numTimer] = getTime(numTimer, 1000);

  return (_tickTime[numTimer] - _prevTickTime[numTimer]);
}

double Timer::GetDeltaTimeS(byte numTimer)
{
  if (!_freq)
    return 0;

  _prevTickTime[numTimer] = _tickTime[numTimer];
  _tickTime[numTimer] = getTime(numTimer, 1);

  return (_tickTime[numTimer] - _prevTickTime[numTimer]);
}

bool Timer::GetEventTimeNS(byte numTimer, double factor)
{
  if (!_freq)
    return 0;

  _deltaTime[numTimer] = getTime(numTimer, 1000000000);

  if (_deltaTime[numTimer] - _prevDeltaTime[numTimer] < factor)
    return false;

  _prevDeltaTime[numTimer] = _deltaTime[numTimer];

  return true;
}

bool Timer::GetEventTimeMkS(byte numTimer, double factor)
{
  if (!_freq)
    return 0;

  _deltaTime[numTimer] = getTime(numTimer, 1000000);

  if (_deltaTime[numTimer] - _prevDeltaTime[numTimer] < factor)
    return false;

  _prevDeltaTime[numTimer] = _deltaTime[numTimer];

  return true;
}

bool Timer::GetEventTimeMS(byte numTimer, double factor)
{
  if (!_freq)
    return 0;

  _deltaTime[numTimer] = getTime(numTimer, 1000);

  if (_deltaTime[numTimer] - _prevDeltaTime[numTimer] < factor)
    return false;

  _prevDeltaTime[numTimer] = _deltaTime[numTimer];

  return true;
}

bool Timer::GetEventTimeS(byte numTimer, double factor)
{
  if (!_freq)
    return 0;

  _deltaTime[numTimer] = getTime(numTimer, 1);

  if (_deltaTime[numTimer] - _prevDeltaTime[numTimer] < factor)
    return false;

  _prevDeltaTime[numTimer] = _deltaTime[numTimer];

  return true;
}

double Timer::getTime(byte numTimer, double factor)
{
  if (!_freq)
    return 0;

  DWORD_PTR oldMask = SetThreadAffinityMask(_thread, _numProc);
  QueryPerformanceCounter(&_counter);
  SetThreadAffinityMask(_thread, oldMask);

  return factor * double(_counter.QuadPart - _startTime[numTimer]) / _freq;
}
