import java.util.*;


public class FutexHandler{
  private Map<Futex, WaitFreeQueue> waitMap;
  private ArrayList<Futex> futexList;

  public FutexHandler(int numFutexes){
    waitMap = new HashMap<Futex, WaitFreeQueue>();
    futexList = new ArrayList<Futex>(numFutexes);
    for(int i = 0; i<numFutexes; i++){
      Futex f = new Futex();
      futexList.add(f);
      waitMap.put(f, new WaitFreeQueue());
    }
  }
  // as per ours.futex.c:
  // futex_wait offers atomicity while remaining entirely within user space.
  // TODO: futex_wait

  // -1 = EWOULDBLOCK
  // -2 = ETIMEDOUT
  // -3 = EINTR
  //
  public Futex getFutexById(int id) throws IndexOutOfBoundsException {
    return this.futexList.get(id);
  }

  public int futex_wait(Futex f){
    int ret = 0;
    if (waitMap.get(f) == null)
      waitMap.put(f, new WaitFreeQueue());
    Thread curr = Thread.currentThread();
    if(f.whoseLock() != curr.getId()){
      waitMap.get(f).enq(curr, (int)curr.getId());
      curr.suspend();
    }
    return 0;
  }
  // also as per ours.futex.c:
  // futex_wake wakes next \val\ processes in wait queue
  // returns count of how many processes awoken
  public int futex_wake(Futex f, int val){
    int count = 0;
    try{
      for (int i = 0;i < val; i++) {
        Thread d = waitMap.get(f).deq((int)Thread.currentThread().getId());
        d.resume();
        count=i;
      }
    }catch (EmptyException e){
      return count;
    }
    return val;
  }

  public static void main(String[] args){
    //test code here
    System.out.println("Hello World!");
    Futex f = new Futex();
    //insert futexes or something
  }

}
