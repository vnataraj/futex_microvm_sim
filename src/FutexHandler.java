import java.util.*;


public class FutexHandler{

  public static Map<Futex, WaitFreeQueue> waitMap;
  // as per ours.futex.c:
  // futex_wait offers atomicity while remaining entirely within user space.
  // TODO: futex_wait

  // -1 = EWOULDBLOCK
  // -2 = ETIMEDOUT
  // -3 = EINTR
  public int futex_wait(Futex f, int reltime){
    int ret = 0;
    if (waitMap.get(f) == null)
      waitMap.put(f, new WaitFreeQueue());
    Thread curr = Thread.currentThread();
    if(f.whoseLock() != curr.getId()){
      ret = -1;
      waitMap.get(f).enq(curr, (int)curr.getId());
      try {
          curr.sleep(reltime);
          ret = -2
      }
      catch (InterruptedException e) {
        ret = -3;
      }
    }
    return ret;
  }
  // also as per ours.futex.c:
  // futex_wake wakes next \val\ processes in wait queue
  // returns count of how many processes awoken
  public int futex_wake(Futex f, int val){
    for (int i = 0;i < val; i++) {
      try {
        Thread d = waitMap.get(f).deq((int)Thread.currentThread().getId());
        d.interrupt();
      }
      catch (EmptyException e){
        return i;
      }
    }
    return val;
  }

  public static void main(String[] args){
    //test code here
    System.out.println("Hello World!");
    waitMap = new HashMap<Futex, WaitFreeQueue>();
    //insert futexes or something
  }

}
