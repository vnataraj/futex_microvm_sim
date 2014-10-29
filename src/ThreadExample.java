import java.lang.Thread;
import java.util.Scanner;

public class ThreadExample extends Thread{
  private int val;
  public ThreadExample(int val){
    this.val=val; 
  }
  public void run(){
      System.out.println("This is the val: "+this.val);
  }

  public int getVal(){
    return this.val;
  }

  public static void main(String[] args){
    ThreadExample ex1 = new ThreadExample(1);
    ThreadExample ex2 = new ThreadExample(2);
    ThreadExample ex3 = new ThreadExample(3);
    WaitFreeQueue q = new WaitFreeQueue();
    ex1.start();
    ex2.start();
    ex3.start();
    q.enq(ex1, (int)ex1.getId());
    q.enq(ex2, (int)ex2.getId());
    q.enq(ex3, (int)ex3.getId());
    Scanner s = new Scanner(System.in);
    System.out.println("Press any character to deq");
    while(true){
      if(s.hasNext()){
        try{
        System.out.println("Thread 1 Decrementing with value: "+q.deq((int)ex1.getId()).getId());
        System.out.println("Thread 2 Decrementing with value: "+q.deq((int)ex2.getId()).getId());
        System.out.println("Thread 3 Decrementing with value: "+q.deq((int)ex3.getId()).getId());
        break;
        } catch(EmptyException ex){
          System.out.println("We're running on empty cap'n!");
        }
      }
    }
  }

}
