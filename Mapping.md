  Futex        HashMap                Wait-Free Queue
---------                  --------- --------- ----------
| Futex | ->(hashcode) -> | Node 1  | Node 2  | Node 3  |
---------                  --------- --------- ----------
| Futex | ->(hashcode) -> | Node 1  | Node 2  | Node 3  |
---------                  --------- --------- ----------
...


All objects inherit a hashcode thanks to Java.lang.Object superclass.  An option to override this exists if we so desire.
Futex operations are contained independent of the HashMap implementation.
