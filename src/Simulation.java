import java.util.*;


public class Simulation{

  // as per ours.futex.c:
  // futex_wait offers atomicity while remaining entirely within user space.
  // TODO: futex_wait
  public int futex_wait(Futex f, int val, int spin_counts){
    return 0;
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
  }

}
