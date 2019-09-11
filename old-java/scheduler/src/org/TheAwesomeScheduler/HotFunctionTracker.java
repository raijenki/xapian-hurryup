package org.TheAwesomeScheduler;
import java.util.concurrent.atomic.AtomicLong;
import java.util.concurrent.atomic.AtomicInteger;

public class HotFunctionTracker
{
    private static final int EVENT_RING_SIZE = 5000;

    public static class Event
    {
	/// The thread id this event is associated with.
	/// A value of zero specifies the tail of the queue.
        public AtomicLong tid = new AtomicLong();

	/// A zero value means leaving a hot function.
	/// A non-zero value means entering a hot function, and
	/// the value is the time the event happened.
        public long time;
    };

    // This will all work out as a single-consumer multi-producer 
    // queue built as a ring-buffer (thus lock-free).
    //
    // The producers are the worker threads, the consumer
    // is the scheduler.
    static private Event[] eventRing = new Event[EVENT_RING_SIZE];
    static private AtomicInteger eventWritePos = new AtomicInteger();
    static private int eventReadPos;  //< Single-consumer cursor

    static {
	for(int i = 0; i < EVENT_RING_SIZE; ++i)
	    eventRing[i] = new Event();
    };

    /// Registers that the flow of control is entering a hot function
    /// in the specified thread id.
    public static void sendEnter(long tid)
    {
        sendEvent(tid, System.currentTimeMillis());
    }

    /// Registers that the flow of control is leaving a hot function
    /// in the specified thread id.
    public static void sendLeave(long tid)
    {
        sendEvent(tid, 0);
    }

    /// Consumes a event from the queue, if any.
    ///
    /// This is not thread-safe, thus only one consumer thread is allowed.
    ///
    /// \returns The event or `null` if there are no events to be consumed.
    public static Event recvEvent()
    {
	int idx = eventReadPos;
	if(eventRing[idx].tid.get() != 0)
	{
		// Copy the event from the queue to some local variable.
		Event evt = new Event();
		evt.tid.set(eventRing[idx].tid.get());
		evt.time = eventRing[idx].time;

		// Set the ring position as free and advance the consumer.
		eventRing[idx].tid.set(0);
		eventReadPos = (eventReadPos + 1) % EVENT_RING_SIZE;

		return evt;
	}
	return null;
    }

    /// Produces a event to the queue.
    private static void sendEvent(long tid, long time)
    {
        assert tid != 0;
        int idx = eventWritePos.getAndIncrement() % EVENT_RING_SIZE;

	if(eventRing[idx].tid.get() != 0)
		throw new RuntimeException("Ring Buffer Overflow");

	// Safe to set non-atomically.
	// We basically "hold" a lock on this index at this time.
        eventRing[idx].time = time;

	// This should be the last statement.
	// It sets the buffer position as valid.
        eventRing[idx].tid.set(tid);
    }
};
