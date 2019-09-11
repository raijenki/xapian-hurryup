import org.TheAwesomeScheduler.HotFunctionTracker;
import org.TheAwesomeScheduler.HotFunctionTracker.Event;
import energymeasure.EnergyReading;

//NOT OPTIMIZED
// TODO remove memory allocations every tick
// TODO remove system call every tick

public class AgentSchedulingThread extends Thread {
    Process matrix[] = new Process[6];

    void setMatrixAffinity() {
        for(int i = 0; i <= 5; i++) {
            int expectedAffinity = (1 << i);
            if(matrix[i] != null && matrix[i].affinity != expectedAffinity)
            {
                EnergyReading.setAffinity((int)matrix[i].tid, 1 << i);
                matrix[i].affinity = expectedAffinity;
            }
        }
    }

    int allocateProcess(long tid) {
        for(int i = 0; i <= 5; i++) {
            if(matrix[i] == null && matrix[i].tid == tid)
                return i;
        }
        for(int i = 0; i <= 5; i++) {
            if(matrix[i] == null)
            {
                matrix[i] = new Process();
                matrix[i].affinity = EnergyReading.getAffinity((int)matrix[i].tid);
                return i;
            }
        }
        return -1; // TODO
    }

    int nextFreeBigCore(long mytimestamp)
    {
        for(int i = 0; i <= 1; i++) {
            if(matrix[i] != null && matrix[i].status == Status.NotInRequest)
                return i;
        }
        for(int i = 0; i <= 1; i++) {
            if(matrix[i] != null && mytimestamp < matrix[i].timestamp)
                return i; 
        }
        return -1;
    }

    public enum Status
    {
        NotInRequest,
        InRequest,
        InHotRequest,
    };

	public static class Process { 
		public long timestamp;
        public long tid;
        public long diff;
	    public Status status = Status.NotInRequest;
        public int affinity;
	}

    public void run() {
    
    // Index 0, 1 = BIG
    // Index 2-5 = LITTLE
        int threshold = 350; // MEAN + 2*STD

        while(true)
        {
            try {
                Thread.sleep(10);
            } catch(InterruptedException e) {
                break;
            }

            while(true)
            {
                Event event = HotFunctionTracker.recvEvent();
                if(event == null)
                    break;
                //System.out.printf("GOT EVENT FOR TID %d ON TIME %d\n", event.tid.get(), event.time);
                
		        long tid = event.tid.get();

                boolean leaveEvent = false;
                for(int i = 0; i <= 5; ++i)
                {
                    if(matrix[i] != null && matrix[i].tid == tid)
                    {
                        matrix[i].status = Status.NotInRequest;
                        leaveEvent = true;
                        break;
                    }
                }

                if(!leaveEvent)
                {
                    int pos = allocateProcess(tid);
                    matrix[pos].tid = tid;
                    matrix[pos].timestamp = event.time;
		            matrix[pos].status = Status.InRequest;       
                }
            }

            long currentTimestamp = System.currentTimeMillis();
            
            for(int i = 0; i <= 5; i++)
            {
                if(matrix[i] != null)
                {
                    matrix[i].diff = currentTimestamp - matrix[i].timestamp;
                    if(matrix[i].diff >= threshold)
                    {
                        matrix[i].status = Status.InHotRequest;
                    }
                }
            }

            for(int i = 2; i <= 5; i++) {
                if(matrix[i] != null && matrix[i].status == Status.InHotRequest)
                {
                    int next = nextFreeBigCore(matrix[i].timestamp);
                    if(next != -1) {
                        Process tmp = matrix[next];
                        matrix[next] = matrix[i];
                        matrix[i] = tmp;
                    }
                }
            }            

            setMatrixAffinity();
            

            
            
        }
    }
};

