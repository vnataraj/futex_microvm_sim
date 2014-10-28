// reference skeleton code for java implementation of ours.futex.c
//
import java.util.*;
public class Futex{
  private long tid;
  public Futex(){
    this.tid=-1;  
  }
  public long whoseLock(){
    return this.tid;
  }
  public void changeOwnership(long tid){
    this.tid=tid;
  }
  public boolean okayToGiveLock(long accesstid, boolean bool){
    if((bool==true) && (this.tid == accesstid)){
      return true;
    }
    else{
      return false;
    }
  }
}
