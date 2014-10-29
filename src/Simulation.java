import java.lang.Thread;
import java.util.Scanner;

public class Simulation extends Thread{

  private int val;
  private FutexHandler f;
  public Simulation(int val, FutexHandler f){
    this.val=val;
    this.f=f;
  }
  public void run(){
    Futex fut = this.f.getFutexById(1);
    if(fut.whoseLock()==-1){
      fut.changeOwnership(Thread.currentThread().getId());
    } else if(this.f.futex_wait(fut)==0){
      System.out.println("This is val: "+val);
    }
  }

  public static void main(String[] args){
    FutexHandler f = new FutexHandler(3);
    Scanner s = new Scanner(System.in);
    Simulation sa = new Simulation(1,f);
    Simulation sb = new Simulation(2,f);
    Simulation sc = new Simulation(3,f);
    sa.start();
    sb.start();
    sc.start();
    while(true){
      if(s.hasNext()){
        f.futex_wake(f.getFutexById(1), 3);
        break;
      }
    }
    System.out.println("finished!");
  }
}
