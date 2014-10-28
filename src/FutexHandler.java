import java.util.*;


public class FutexHandler{

  public static Map<Futex, WaitFreeQueue> waitMap;
  // as per ours.futex.c:
  // futex_wait offers atomicity while remaining entirely within user space.
  // TODO: futex_wait
  public int futex_wait(Futex f, long tid, int val, int timeout){
    int count=0;
    while(!f.okayToGiveLock(tid, true) || timeout != count){
      count++;
    }
    waitMap.get(f).enq(val, (int)tid);
  }
  // also as per ours.futex.c: 
  // futex_wake wakes next \val\ processes in wait queue
  // returns count of how many processes awoken
  public int futex_wake(Futex f, int val, int spin_counts){
    return 0;
  }

  public static void main(String[] args){
    //test code here
    System.out.println("Hello World!");
    waitMap = new HashMap<Futex, WaitFreeQueue>();
    //insert futexes or something
  }

}
