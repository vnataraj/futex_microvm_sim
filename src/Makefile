JC = javac
.SUFFIXES: .java .class

.java.class:
	$(JC) $*.java

CLASSES = \
					FutexHandler.java \
					WaitFreeQueue.java \

default: classes

classes: $(CLASSES:.java=.class)

clean:
	$(RM) *.class
