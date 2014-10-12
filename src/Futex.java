// reference skeleton code for java implementation of ours.futex.c
//
public class Futex{
  private int pid;
  private String fname;
  private int offset;
  public Futex(int pid, String fname, int offset){
    this.pid=pid;
    this.fname=fname;
    this.offset=offset;
  }
  public void setPid(int newPid){
    this.pid=newPid;
  }
  public void setFname(String newFname){
    this.fname=newFname;
  }
  public void setOffset(int newOffset){
    this.offset=newOffset;
  }
  public int getPid(){
    return this.pid;
  }
  public String getFname(){
    return this.fname;
  }
  public int getOffset(){
    return this.offset;
  }
}
