
def T_sum(thread_index):
    sys_sched()
    heap.flag[thread_index] = True
    sys_sched()
    heap.turn = 1 - thread_index
    sys_sched()
    while (heap.flag[1-thread_index] == True):
        sys_sched()
        if (heap.turn == 1 - thread_index):
            sys_sched()
            continue
        else:
            break
    sys_sched()
    a = heap.t 
    sys_sched()
    a = a + 1 
    sys_sched()
    heap.t = a 
    sys_sched()
    heap.flag[thread_index] = False
    sys_sched()
    
def main():
    heap.t = 0
    heap.turn = 0
    heap.flag = [False,False]
    sys_spawn(T_sum, 0)
    sys_spawn(T_sum, 1)