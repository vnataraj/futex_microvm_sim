JC = javac
.SUFFIXES: .java .class
	
.java.class:
	$(JC) $*.java

CLASSES = \
					Futex.java \
					FutexHandler.java \
					FileChannelIsNullException.java \
					MemoryMappedFile.java \
					ThreadExample.java \
					UnsafeExample.java \
					WaitFreeQueue.java \

default: classes

classes: $(CLASSES:.java=.class)

clean:
	$(RM) *.class
