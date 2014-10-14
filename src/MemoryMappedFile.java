import java.util.*;
import java.io.*;
import java.nio.channels.FileChannel;
import java.nio.MappedByteBuffer;

public class MemoryMappedFile{
  private String fname;
  private FileChannel fd;
  private RandomAccessFile raf;
  private MappedByteBuffer mbb;
  public MemoryMappedFile(String fname){
    this.fname=fname;
    this.fd = null;
    this.raf = null;
    this.mbb = null;
    this.mapMemory(fname);
  }
  public void getMapping(){
    return;
  }
  
  private MappedByteBuffer getByteBuffer() throws FileChannelIsNullException {
    if(this.fd==null){
      throw new FileChannelIsNullException("Could not open the filechannel!");
    }
    int bufSize=4*1024;
    try{
      this.mbb = this.fd.map(FileChannel.MapMode.READ_WRITE, 0, bufSize);
      this.fd.close();
    } catch(IOException ie){
      throw new FileChannelIsNullException(ie.getMessage());
    }
    return this.mbb;
  }

  public String getFname(){
    return this.fname;
  } 
  //opens channel for writing and sets the value to fd
  private void mapMemory(String fname){
    try{
      this.raf = new RandomAccessFile(new File(this.fname), "rws");
      this.fd=raf.getChannel();
    } catch(IOException ie){
      System.out.println(ie.getMessage());
    }
    return;
  }

  public static void main(String[] args){
    //testing only
    System.out.println("Hello, World!");
  }

}
