  Futex        HashMap                Wait-Free Queue
---------                  --------- --------- ----------
| Futex | ->(hashcode) -> | Node 1  | Node 2  | Node 3  |
---------                  --------- --------- ----------
| Futex | ->(hashcode) -> | Node 1  | Node 2  | Node 3  |
---------                  --------- --------- ----------
...
