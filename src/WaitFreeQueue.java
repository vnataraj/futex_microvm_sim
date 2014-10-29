/*
 * WaitFreeQueue.java
 *
 *      This file contains Kogan and Petrank's implementation of wait-free 
 *      queues as seen in their paper:
 *      Wait-Free Queues With Multiple Enqueuers and Dequeuers
 *
 *      This is not our original code!
 */

import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicReference;
import java.util.concurrent.atomic.AtomicReferenceArray;
import java.lang.Thread;

class Node
{
    // Holds elements of the underlying list representation of the WFQueue
    Thread value;
    int enqTid;
    AtomicInteger deqTid;
    AtomicReference<Node> next;

    public Node(Thread val, int etid)
    {
        value = val;
        next = new AtomicReference<Node>(null);
        enqTid = etid;
        deqTid = new AtomicInteger(-1);
    }
}

class OpDesc
{
    // Operation descriptor record for each thread
    long phase;
    boolean pending;
    boolean enqueue;
    Node node;

    public OpDesc(long ph, boolean pend, boolean enq, Node n)
    {
        phase = ph;
        pending = pend;
        enqueue = enq;
        node = n;
    }
}

class WaitFreeQueue
{
    public static final int NUM_THRDS = 1000;   // Dummy values

    AtomicReference<Node> head, tail;
    AtomicReferenceArray<OpDesc> state;

    public WaitFreeQueue()
    {
        Node sentinel = new Node(Thread.currentThread(), -1);
        head = new AtomicReference<Node>(sentinel);
        tail = new AtomicReference<Node>(sentinel);
        state = new AtomicReferenceArray<OpDesc>(NUM_THRDS);

        for (int i = 0; i < state.length(); i++)
            state.set(i, new OpDesc(-1, false, true, null));
    }

    void help(long phase)
    {
        for (int i = 0; i < state.length(); i++) {
            OpDesc desc = state.get(i);
            if (desc.pending && desc.phase <= phase) {
                if (desc.enqueue)
                    help_enq(i, phase);
                else 
                    help_deq(i, phase);
            }
        }
    }

    long maxPhase()
    {
        long maxPhase = -1;
        for (int i = 0; i < state.length(); i++) {
            long phase = state.get(i).phase;
            if (phase > maxPhase)
                maxPhase = phase;
        }
        return maxPhase;
    }

    boolean isStillPending(int tid, long ph)
    {
        return state.get(tid).pending && state.get(tid).phase <= ph;
    }

    void enq(Thread value, int TID)
    {
        long phase = maxPhase() + 1;
        state.set(TID, new OpDesc(phase, true, true, new Node(value, TID)));
        help(phase);
        help_finish_enq();
    }

    void help_enq(int tid, long phase)
    {
        while (isStillPending(tid, phase)) {
            Node last = tail.get();
            Node next = last.next.get();
            if (last == tail.get()) {
                if (next == null && isStillPending(tid, phase) &&
                    last.next.compareAndSet(next, state.get(tid).node)) {
                    
                    help_finish_enq();
                    return;
                }
                else {
                    help_finish_enq();
                }
            }
        }
    }

    void help_finish_enq()
    {
        Node last = tail.get();
        Node next = last.next.get();
        if (next != null) {
            int tid = next.enqTid;
            OpDesc curDesc = state.get(tid);
            if (last == tail.get() && state.get(tid).node == next) {
                OpDesc newDesc = new OpDesc(state.get(tid).phase, false, true, next);
                state.compareAndSet(tid, curDesc, newDesc);
                tail.compareAndSet(last, next);
            }
        }
    }

    Thread deq(int TID) throws EmptyException
    {
        long phase = maxPhase() + 1;
        state.set(TID, new OpDesc(phase, true, false, null));
        help(phase);
        help_finish_deq();
        Node node = state.get(TID).node;
        if (node == null)
            throw new EmptyException();
        return node.next.get().value;
    }

    void help_deq(int tid, long phase)
    {
        while (isStillPending(tid, phase)) {
            Node first = head.get();
            Node last  = tail.get();
            Node next  = first.next.get();
            if (first == head.get()) {
                if (first == last) {
                    if (next == null) {
                        OpDesc curDesc = state.get(tid);
                        if (last == tail.get() && isStillPending(tid, phase)) {
                            OpDesc newDesc = new OpDesc(state.get(tid).phase, false, false, null);
                            state.compareAndSet(tid, curDesc, newDesc);
                        }
                    }
                    else {
                        help_finish_enq();
                    }
                }
                else {
                    OpDesc curDesc = state.get(tid);
                    Node node = curDesc.node;
                    if (!isStillPending(tid, phase)) 
                        break;
                    if (first == head.get() && node != first) {
                        OpDesc newDesc = new OpDesc(state.get(tid).phase, true, false, first);
                        if (!state.compareAndSet(tid, curDesc, newDesc))
                            continue;
                    }
                    first.deqTid.compareAndSet(-1, tid);
                    help_finish_deq();
                }
            }
        }
    }

    void help_finish_deq()
    {
        Node first = head.get();
        Node next  = first.next.get();
        int tid = first.deqTid.get();
        if (tid != -1) {
            OpDesc curDesc = state.get(tid);
            if (first == head.get() && next != null) {
                OpDesc newDesc = new OpDesc(state.get(tid).phase, false, false, state.get(tid).node);
                state.compareAndSet(tid, curDesc, newDesc);
                head.compareAndSet(first, next);
            }
        }
    }
}

class EmptyException extends Exception
{
    void EmptyException()
    {
        System.out.println("We're running on empty cap'n");
    }
}
