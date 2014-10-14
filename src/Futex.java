// reference skeleton code for java implementation of ours.futex.c
//
import java.util.*;
import java.nio.MappedByteBuffer;
public class Futex{
  private int pid;
  private int offset;
  private MappedByteBuffer buf;
  public Futex(int pid, MappedByteBuffer buf, int offset){
    this.pid=pid;
    this.buf=buf;
    this.offset=offset;
  }
  public void setPid(int newPid){
    this.pid=newPid;
  }
  public void setOffset(int newOffset){
    this.offset=newOffset;
  }
  public void setMapBuf(MappedByteBuffer buf){
    this.buf=buf;
  }
  public int getPid(){
    return this.pid;
  }
  public MappedByteBuffer getBuf(){
    return this.buf;
  }
  public int getOffset(){
    return this.offset;
  }
}
