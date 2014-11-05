import java.util.*;
import java.util.concurrent.atomic.AtomicInteger;


public class FutexHandler{

  public static HashMap<AtomicInteger, WaitFreeQueue> waitMap;
  // as per ours.futex.c:
  // futex_wait offers atomicity while remaining entirely within user space.
  // TODO: futex_wait

  // -1 = EWOULDBLOCK
  // -2 = ETIMEDOUT
  // -3 = EINTR
  public int futex_wait(AtomicInteger futex, int val, int reltime){
    int hash = futex.hashCode();
    Thread curr = Thread.currentThread();
    if (waitMap.get(futex) == null){
      waitMap.put(futex, new WaitFreeQueue());
    }
    if (!futex.compareAndSet(val, val)) {
      return -1;
    }
    waitMap.get(futex).enq(curr, (int)curr.getId());
    try {
      Thread.currentThread().sleep(reltime);
      return -2;
    } catch (Exception e) {
      if (waitMap.get(futex).getHead() == curr) {
        return -3;
      }
    }
    return 0;
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
    waitMap = new HashMap<AtomicInteger, WaitFreeQueue>();
    //insert futexes or something
  }

}
