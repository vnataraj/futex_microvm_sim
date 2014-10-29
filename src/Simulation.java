import java.lang.Thread;
import java.util.Scanner;

public class Simulation extends Thread{

  private int val;
  public Simulation(int val){
     this.val=val;
  }
  public void run(FutexHandler f){
    Futex fut = f.getFutexById(1);
    if(fut.whoseLock()==-1){
      fut.changeOwnership(Thread.currentThread().getId());
    } else if(f.futex_wait(fut)==0){
      System.out.println("This is val: "+val);
    }
  }

  public static void main(String[] args){
    FutexHandler f = new FutexHandler(3);
    Simulation sa = new Simulation(1);
    Simulation sb = new Simulation(2);
    Simulation sc = new Simulation(3);
    sa.start();
    sb.start();
    sc.start();
    f.futex_wake(f.getFutexById(1), 3);
  }

}
