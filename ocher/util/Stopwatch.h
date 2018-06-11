#ifndef STOPWATCH_H
#define STOPWATCH_H

#include <chrono>
#include <cstdint>


class Stopwatch {
public:
    Stopwatch(bool autoStart = true)
    {
        if (autoStart)
            start();
    }

    /**
     *  If stopwatch is curently running, elapsed time is since start.
     *  If stopwatch is curently stopped, elapsed time is the previous run (0 if none).
     *  @return Elapsed time, in microseconds.
     */
    uint64_t elapsedUSec()
    {
        if (running())
            return (std::chrono::steady_clock::now() - m_startUSec).count();
        else
            return std::chrono::duration_cast<std::chrono::microseconds>(m_elapsed).count();
    }

    uint64_t lap()
    {
        if (running()) {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = now - m_startUSec;
            m_startUSec = now;
            return std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
        } else {
            start();
            return 0;
        }
    }

    void start()
    {
        m_startUSec = std::chrono::steady_clock::now();
        m_elapsed = {};
    }

    /**
     *  Stops the stopwatch, if it is running.
     *  @return Elapsed time, in microseconds, of the most recent run.
     */
    uint64_t stop()
    {
        if (running()) {
            m_elapsed = std::chrono::steady_clock::now() - m_startUSec;
            m_startUSec = {};
        }
        return std::chrono::duration_cast<std::chrono::microseconds>(m_elapsed).count();
    }

    bool running()
    {
        return m_startUSec.time_since_epoch().count() != 0;
    }

protected:
    std::chrono::time_point<std::chrono::steady_clock> m_startUSec;
    std::chrono::steady_clock::duration m_elapsed;
};

#endif
