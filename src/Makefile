JC = javac
.SUFFIXES: .java .class
	
.java.class:
	$(JC) $*.java

CLASSES = \
					Futex.java \
					Simulation.java \

default: classes

classes: $(CLASSES:.java=.class)

clean:
	$(RM) *.class
