import java.util.*;
import java.io.*;
import java.nio.channels.FileChannel;
public class MemoryMappedFile{
  private String fname;
  private FileChannel fd;
  private RandomAccessFile raf;
  public MemoryMappedFile(String fname){
    this.fname=fname;
    this.fd = null;
    this.raf = null;
    this.mapMemory(fname);
  }

  public FileChannel getFileChannel() throws FileChannelIsNullException {
    if(this.fd==null){
      throw new FileChannelIsNullException("Could not open the filechannel!");
    }
    return fd;
  }

  public void closeFileChannel() throws FileChannelIsNullException {
    if(this.fd==null){
      throw new FileChannelIsNullException("Could not open the filechannel!");
    }
    else{
      try{
        this.fd.close();
      } catch(IOException ie){
        System.out.println(ie.getMessage());
      }
    }
    return;
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
