#include <fstream>
#include <list>
#include <unistd.h>
#include <sstream>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <vector>
#include <deque>
#include<cstring>
using namespace std;


// ######################### Global variables ################################### 
vector<int> randvals;
int ofs;
string scheduler_option;
bool ispreprio =false;
bool myverbose = false;
int pid_counter = 0;
int cpu_counter = 0;
struct io_counter_struct {
    bool flag; //true means some proc in running
    int start_time;
    int end_time;
    int time_counter;
};
io_counter_struct io_counter = {
    .flag = false,
    .start_time = 0,
    .end_time = 0,
    .time_counter = 0
};

int maxprio = 4, quantum = 10000;
enum STATE {
    CREATED,
    READY,
    RUNNING,
    BLOCKED,
    PREEMPT
};
bool verbose_op, scheduler_op, event_op;

enum TRANSITION {
    
    TO_READY,
    TO_RUNNING,
    TO_BLOCKED,
    TO_PREEMPT
};
int myrandom(int burst) { 
    //declare a glabl vector randvals, offset
    if(ofs == randvals.size()-1){ ofs = -1; }
    ofs++;
    if(myverbose)  printf("randvals[%d]=%d",ofs, randvals[ofs]);
    return 1 + (randvals[ofs] % burst); 
}
class Process{
    private:
        int arrival_time;
        int total_cpu_time;
        int cpu_burst;
        int cpu_exec_burst;
        int io_burst;
        int io_exec_burst;
        int process_id;
        int time_in_prev_state;
        int timestamp_to_execute_at;
        int timestamp_for_prev_trans;
        int remaining_exec_time;
        int static_priority;
        int dynamic_priority;
        int finishing_time;
        int waiting_time = 0;
        int blocked_time = 0;
        int turnaround_time;
        enum STATE curr_state;
        enum TRANSITION transition_to;

    public:
       
       Process(int at, int tc, int cb, int ib){
        arrival_time = at;
        total_cpu_time = tc;
        cpu_burst = cb;
        io_burst = ib ;
        process_id = pid_counter++;
        time_in_prev_state = 0;
        timestamp_to_execute_at = at ;
        remaining_exec_time = tc ;
        static_priority = myrandom(maxprio);
        dynamic_priority = static_priority - 1;
        timestamp_for_prev_trans = at;
        cpu_exec_burst = 0;
        curr_state = CREATED;
        transition_to = TO_READY;
        finishing_time = 0;
       }
        void set_remaining_exec_time(int new_exec_time){
            remaining_exec_time = new_exec_time;
        }
        void set_process_id(int new_process_id){
            process_id = new_process_id;
        }
        void set_time_in_prev_state(int new_time_in_prev_state){
            time_in_prev_state = new_time_in_prev_state;
        }
        void set_timestamp_to_execute_at(int new_timestamp_to_execute_at){
            timestamp_to_execute_at = new_timestamp_to_execute_at;
        }
        void set_timestamp_for_prev_trans(int new_timestamp_for_prev_trans){
            timestamp_for_prev_trans = new_timestamp_for_prev_trans;
        }
        void set_io_burst(int new_io_burst){
            io_burst = new_io_burst;
        }
        void set_io_exec_burst(int new_io_burst){
            io_exec_burst = new_io_burst;
        }
       void set_cpu_burst(int new_cpu_burst){
            cpu_burst = new_cpu_burst;
        }
        void set_cpu_exec_burst(int new_cpu_burst){
            cpu_exec_burst = new_cpu_burst;
        }
        void set_transition_to(enum TRANSITION fututre_state){
            transition_to = fututre_state;
        }
        void set_current_state(enum STATE present_state){
            curr_state = present_state;
        }
        void set_dynamic_prio(){
            dynamic_priority==-1? dynamic_priority = static_priority - 1 : dynamic_priority -= 1;
        }
        void reset_dynamic_prio(){
            dynamic_priority = static_priority - 1;
        }
       int get_arrival_time(){
            return arrival_time;
        }
        
       int get_remaining_exec_time(){
            return remaining_exec_time;
        }

        int get_total_cpu_time(){
            return total_cpu_time;
        }
        int get_io_burst(){
            return io_burst;
        }
        int get_io_exec_burst(){
            return io_exec_burst;
        }
       int get_cpu_burst(){
            return cpu_burst;
        }
        int get_cpu_exec_burst(){
            return cpu_exec_burst;
        }
        int get_pid(){
            return process_id;
        }
        int get_time_in_prev_state(){
            return time_in_prev_state;
        }
        int get_timestamp_to_execute_at(){
            return timestamp_to_execute_at;
        }
        int get_timestamp_for_prev_trans(){
            return timestamp_for_prev_trans;
        }
        enum TRANSITION get_trans_to()
        {
            return transition_to;
        }
        enum STATE get_curr_state(){
            return curr_state;
        }
        void printEvent(){
            if(myverbose) printf("AT:%d TC:%d C_burst:%d I_burst:%d curr_state: %u trans_state:%u", arrival_time, total_cpu_time, cpu_burst, io_burst, curr_state, transition_to);
        }
        void set_finishing_time(int ft){
            finishing_time = ft;
        }
        void set_turnaround_time(){
            turnaround_time = finishing_time - arrival_time;
        }
        void set_waiting_time(int wait_time){
            waiting_time += wait_time;
        }
        void set_blocked_time(int wait_in_io ){
            blocked_time += wait_in_io;
        }
        int get_finishing_time(){
            return finishing_time;
        }
        int get_turnaround_time(){
            return turnaround_time;
        }
        int get_waiting_time(){
            return waiting_time;
        }
        int get_blocked_time(){
            return blocked_time;
        }
        int get_static_prio(){
            return static_priority;
        }
        int get_dynamic_prio(){
            return dynamic_priority;
        }
};
vector<Process*> procQ; // for keeping infor to print at the end of simulation

class Event{
    private:
        Process* proc;
    public:
    Event(Process* new_proc){
        proc = new_proc;
    }
    Process* get_process(){
        return proc;
    }

    void set_process(Process* new_proc){
        proc = new_proc;
    }
};

class DES{
    private:
    list<Event*> eventQ;
    public:
        Event* getEvent(){
            Event* event_to_be_processed = eventQ.front();
            eventQ.pop_front();
            return event_to_be_processed;
        }
        bool eventQEmpty(){return eventQ.empty();}
        void insert_eq(Event& event){
            int pos=0;
            Process* proc_to_add = event.get_process();
            list<Event*>::iterator it ;
            it = eventQ.begin();
            for(Event* ev : eventQ){
                
                Process* proc = ev->get_process();
                if(proc_to_add->get_timestamp_to_execute_at()<proc->get_timestamp_to_execute_at()){
                    break;
                }
                it++;
                pos++;
            }
            if(myverbose)  printf("\ninserting event at pos: %d",pos);
            eventQ.insert(it,&event);
            // if(myverbose)  printf("%d\t",event.get_arrival_time());
            // Event* temp = eventQ.back();
            // if(myverbose)  printf("%d\t",temp->get_arrival_time());
        }
        int get_next_event_time(){
            if(eventQ.empty())
            {
                return -1;
            }
            Event* next_event = eventQ.front();
            Process* proc = next_event->get_process();
            int next_event_time = proc->get_timestamp_to_execute_at();
            return next_event_time;
        }
        void delete_event(int pid_of_event_to_delete){
            list<Event*>::iterator it;
            it = eventQ.begin();
            if(myverbose)  printf("\nPID: %d going to be deleted",pid_of_event_to_delete);
            for(Event* ev : eventQ){
                if(pid_of_event_to_delete == ev->get_process()->get_pid() ){
                    if(myverbose)  printf("\nDELETING EVENT");
                    it = eventQ.erase(it);
                    delete ev;
                    return;
                }

                it++;
            }
        }
        void printeventqueue(){
            
            for(Event* ev : eventQ){
                if(myverbose)  printf("EVENT %d:%d ",ev->get_process()->get_timestamp_to_execute_at(), ev->get_process()->get_pid());
            }
        }
        
};

class scheduler{
    protected:
        list<Process*> readyQ; 
        vector<deque<Process*>> active_prioQ;
        vector<deque<Process*>> expired_prioQ;
    public:
    virtual void insert_rq(Process& process_to_be_added)=0;
    virtual Process* deque_rq() =0;
    virtual void printname () = 0;
    virtual void printq() = 0;
    virtual ~scheduler(){

    }

};

class FCFS_scheduler : public scheduler{
    public:
    FCFS_scheduler(){
        // cout<<"FCFS\n";
    }
    void insert_rq(Process& process_to_be_added){
        if(myverbose)  printf("\ninserting to RQ : %d",process_to_be_added.get_pid());
        readyQ.push_back(&process_to_be_added);
    }
    Process* deque_rq(){
        if(readyQ.empty()){
            return nullptr;
        }
        Process* front_of_queue = readyQ.front();
        readyQ.pop_front();
        return front_of_queue;
    }
    virtual void printname (){
        printf("\nFCFS");
    }
    void printq(){
        for(Process* proc: readyQ){
            if(myverbose) printf("RQ: %d",proc->get_pid());
        }
    }
    ~FCFS_scheduler(){

    }

};
class SRTF_scheduler : public scheduler{
    public:
    SRTF_scheduler(){
        // cout<<"SRTF\n";
    }
    void insert_rq(Process& event_to_be_added){
        int new_rem_time_to_be_processed = event_to_be_added.get_remaining_exec_time();
        list<Process*>::iterator it;
        it = readyQ.begin();
        for(Process* proc : readyQ){
            if(new_rem_time_to_be_processed < proc->get_remaining_exec_time()){
                break;
            }
            it++;
        }

        readyQ.insert(it, &event_to_be_added);

    }
    Process* deque_rq(){
        if(readyQ.empty()){
            return nullptr;
        }
        Process* front_of_queue = readyQ.front();
        readyQ.pop_front();
        return front_of_queue;
    }
    virtual void printname (){
        printf("\nSRTF");
    }
    void printq(){
        for(Process* proc: readyQ){
            if(myverbose) printf("RQ: %d",proc->get_pid());
        }
    }
    ~SRTF_scheduler(){

    }
};
class LCFS_scheduler : public scheduler{
    public:
    LCFS_scheduler(){
        // cout<<"LCFS\n";
    }
    void insert_rq(Process& process_to_be_added){
        if(myverbose) printf("\ninserting to RQ : %d",process_to_be_added.get_pid());
        readyQ.push_front(&process_to_be_added);

    }
    virtual void printname (){
        printf("\nLCFS");
    }
    Process* deque_rq(){

        if(readyQ.empty()){
            return nullptr;
        }
        Process* front_of_queue = readyQ.front();
        readyQ.pop_front();
        return front_of_queue;
    }

    void printq(){
        for(Process* proc: readyQ){
            if(myverbose) printf("RQ: %d",proc->get_pid());
        }
    }
    ~LCFS_scheduler(){
        
    }
};
class RR_scheduler : public scheduler{
    public:
    RR_scheduler(){
        // cout<<"RR\n";
    }
    void insert_rq(Process& process_to_be_added){
        if(myverbose) printf("\ninserting to RQ : %d",process_to_be_added.get_pid());
        readyQ.push_back(&process_to_be_added);
    }
    Process* deque_rq(){
        if(readyQ.empty()){
            return nullptr;
        }
        Process* front_of_queue = readyQ.front();
        readyQ.pop_front();
        return front_of_queue;
    }
    virtual void printname (){
         printf("\nRR %d", quantum);
    }
    void printq(){
        for(Process* proc: readyQ){
            if(myverbose)  printf("RQ: %d",proc->get_pid());
        }
    }
    ~RR_scheduler(){

    }

};
class PRIO_scheduler : public scheduler{
    public:
    PRIO_scheduler(){
        // cout<<"PRIO\n";
        for(int i =0; i< maxprio ;++i){
            deque<Process*> empty_Q ;
            expired_prioQ.push_back(empty_Q);
            active_prioQ.push_back(empty_Q);
        }
        if(myverbose)  printf("\nSIZEof EQ %zu AQ %zu", expired_prioQ.size(), active_prioQ.size());
    }
    void insert_rq(Process& process_to_be_added){
        if(process_to_be_added.get_dynamic_prio() == -1){
            process_to_be_added.reset_dynamic_prio();
            expired_prioQ[process_to_be_added.get_dynamic_prio()].push_back(&process_to_be_added);
            if(myverbose)  printf("\n inserted to expired");
            return;
        }
        if(myverbose) printf("\nInserting to active");
        active_prioQ[process_to_be_added.get_dynamic_prio()].push_back(&process_to_be_added);
        if(myverbose)  printf("\n inserted succ");
    }
    Process* deque_rq(){
        if(myverbose)  printf("\nREMOVING ");
        Process* proc_to_dequeue = nullptr;
        for(int i = maxprio-1; i>=0; --i){
            
            if(active_prioQ[i].empty()) continue;
            proc_to_dequeue = active_prioQ[i].front();
            if(myverbose)  printf("%d\n",proc_to_dequeue->get_pid());
            active_prioQ[i].pop_front();
            break;
        }

    if(proc_to_dequeue==nullptr){
        vector<std::deque<Process*>>* aq_ptr = &active_prioQ;
        vector<std::deque<Process*>>* eq_ptr = &expired_prioQ;
        vector<std::deque<Process*>> temp = *aq_ptr;
        *aq_ptr = *eq_ptr;
        *eq_ptr = temp;
        for(int i = maxprio-1; i>=0; --i){
            if(active_prioQ[i].empty()) continue;
            if(myverbose)  printf("\nGOTCHAAA");
            proc_to_dequeue = active_prioQ[i].front();
            active_prioQ[i].pop_front();
            break;
        }
    }
    return proc_to_dequeue;
    }
    virtual void printname (){
        printf("\nPRIO %d", quantum);
    }
    void printq(){
        if(myverbose)  printf("\n My AQ");
        int counter = 0;
        for (deque<Process * > dq : active_prioQ){
            if(myverbose)  printf("\n at prio level %d", counter++);
            for (Process* proc : dq){
                proc->printEvent();
            }
        }
        if(myverbose)  printf("\n My EQ");
        counter = 0;
        for (deque<Process * > dq : expired_prioQ){
            if(myverbose)  printf("\n at prio level %d", counter++);
            for (Process* proc : dq){
                proc->printEvent();
            }
        }
    }
    ~PRIO_scheduler(){

    }
};
class PREPRIO_scheduler : public scheduler{
    public:
    public:
    PREPRIO_scheduler(){
        // cout<<"PRIO\n";
        for(int i =0; i< maxprio ;++i){
            deque<Process*> empty_Q ;
            expired_prioQ.push_back(empty_Q);
            active_prioQ.push_back(empty_Q);
        }
        if(myverbose)  printf("\nSIZEof EQ %zu AQ %zu", expired_prioQ.size(), active_prioQ.size());
    }
    void insert_rq(Process& process_to_be_added){
        if(process_to_be_added.get_dynamic_prio() == -1){
            process_to_be_added.reset_dynamic_prio();
            expired_prioQ[process_to_be_added.get_dynamic_prio()].push_back(&process_to_be_added);
            if(myverbose) printf("\n inserted to expired");
            return;
        }
        if(myverbose) printf("\nInserting to active");
        active_prioQ[process_to_be_added.get_dynamic_prio()].push_back(&process_to_be_added);
         if(myverbose) printf("\n inserted succ");
    }
    Process* deque_rq(){
        if(myverbose) printf("\nREMOVING ");
        Process* proc_to_dequeue = nullptr;
        for(int i = maxprio-1; i>=0; --i){
            
            if(active_prioQ[i].empty()) continue;
            proc_to_dequeue = active_prioQ[i].front();
            if(myverbose)  printf("%d\n",proc_to_dequeue->get_pid());
            active_prioQ[i].pop_front();
            break;
        }

    if(proc_to_dequeue==nullptr){
        vector<std::deque<Process*>>* aq_ptr = &active_prioQ;
        vector<std::deque<Process*>>* eq_ptr = &expired_prioQ;
        vector<std::deque<Process*>> temp = *aq_ptr;
        *aq_ptr = *eq_ptr;
        *eq_ptr = temp;
        for(int i = maxprio-1; i>=0; --i){
            if(active_prioQ[i].empty()) continue;
            if(myverbose)  printf("\nGOTCHAAA");
            proc_to_dequeue = active_prioQ[i].front();
            active_prioQ[i].pop_front();
            break;
        }
    }
    return proc_to_dequeue;
    }
    virtual void printname (){
        printf("\nPREPRIO %d", quantum);
    }
    void printq(){
        if(myverbose)  printf("\n My AQ");
        int counter = 0;
        for (deque<Process * > dq : active_prioQ){
            if(myverbose)  printf("\n at prio level %d", counter++);
            for (Process* proc : dq){
                proc->printEvent();
            }
        }
        if(myverbose)  printf("\n My EQ");
        counter = 0;
        for (deque<Process * > dq : expired_prioQ){
            if(myverbose)  printf("\n at prio level %d", counter++);
            for (Process* proc : dq){
                proc->printEvent();
            }
        }
    }
    ~PREPRIO_scheduler(){

    }

};


scheduler* current_scheduler = nullptr;
Process* current_running_process = nullptr;
template <typename T>
void initialise_correct_scheduler(){
    if(myverbose)  printf("ENTERING 2");
    current_scheduler = new T();
}
// ############################################################################################################
// ########################################## function declarations ###########################################
// ############################################################################################################


// int myrandom(int burst) ;
vector<string> read_options(int argc, char* argv[]);
void initialise_randvals(ifstream& rfile);
void intialise_scheduler(const char* ch);
void print_summary();

// ############################################################################################################
// ########################################## main function ###################################################
// ############################################################################################################


int main(int argc, char* argv[]){
    vector<string> ipandrfile = read_options(argc, argv);
    if(ipandrfile.size()<2){if(myverbose)  printf("\nNot enough args\n"); return 1;}
    DES des_;
    string ipfilename = ipandrfile.front();
    string rfilename = ipandrfile.back();
    ifstream input_file(ipfilename);
    ifstream rfile(rfilename);
    
    if(!input_file.is_open()){ if(myverbose)  printf("\ninput file could not be opened\n"); return 1; }
    if(!rfile.is_open()){ if(myverbose)  printf("\nrfile could not be opened\n"); return 1;}

    initialise_randvals(rfile);
    const char* scheduler_var = scheduler_option.c_str();
    if(myverbose)  printf("CHAAR %c why nothing\n", scheduler_var[0]);
    intialise_scheduler(scheduler_var);
    string line, word;
    while(getline(input_file, line))
    {
        stringstream ss(line);
        vector<int> templist;
        while(ss >> word)
        {
            templist.push_back(stoi(word));
            
        }
        int arrival_time = templist[0];
        int total_cpu_time = templist[1];
        int cpu_burst = templist[2];
        int io_burst = templist[3];
        Process* proc =  new Process(arrival_time, total_cpu_time, cpu_burst, io_burst);
        Event* event =  new Event(proc);
        // event->printEvent();
        des_.insert_eq(*event);
        procQ.push_back(proc);
    }
    // starting event simulation
    while(!des_.eventQEmpty()){

        Event* curr_event = des_.getEvent();
        Process* curr_proc = curr_event->get_process();
        bool call_scheduler=false;
        // if(myverbose)  printf("\nNEXT event ");
        // curr_proc->printEvent();
        if(myverbose)  printf("\n"); des_.printeventqueue();
        int curr_time = curr_proc->get_timestamp_to_execute_at();
        // if(current_running_process!=nullptr && current_running_process->get_timestamp_to_execute_at()==curr_time)
        // {
        //     if(myverbose)  printf("\nsetting curr_run_process_to_nullptr at curr_time %d",curr_time);
        //     current_running_process = nullptr;
        // }
        //once you get this event process it here 
        //based on if it is going to ready or running call scheduler
        enum TRANSITION trans_to =  curr_proc->get_trans_to();
        enum STATE curr_state = curr_proc->get_curr_state();
        delete curr_event;
        switch(trans_to){
            case TO_READY:
                if(curr_state==CREATED || curr_state==BLOCKED)
                {
                    if(io_counter.flag==true)
                    {
                        if(io_counter.end_time == curr_time){
                            if(myverbose)  printf("\n\nADDING IO FROM TO %d ==>",io_counter.time_counter);
                            io_counter.time_counter += io_counter.end_time - io_counter.start_time;
                            if(myverbose)  printf("%d",io_counter.time_counter);
                            io_counter.flag = false;
                        }
                    }
                    // if(myverbose)  printf("\n ###case TO_READY#### \n");
                    curr_proc->set_time_in_prev_state(curr_time - curr_proc->get_timestamp_for_prev_trans());
                    curr_proc->set_timestamp_for_prev_trans(curr_time);
                    curr_proc->set_current_state(READY);
                    curr_proc->set_transition_to(TO_RUNNING);
                    // following order needs to be maintatined, calculate time in prev state before
                    // setting the new timestamp_for_prev_transition
                    if(myverbose)  printf("\nDP %d ", curr_proc->get_dynamic_prio());
                    if(myverbose)  printf("\nRQ before insertion: ");
                    current_scheduler->printq();
                    current_scheduler->insert_rq(*curr_proc);
                    if(verbose_op)
                    {
                    printf("\n%d %d %d : %s -> %s", curr_time, curr_proc->get_pid(),curr_proc->get_time_in_prev_state(),"CR/BL", "READY");
                    }
                    if(ispreprio && current_running_process!=nullptr){
                        bool cond1 = current_running_process->get_dynamic_prio() < curr_proc->get_dynamic_prio();
                        bool cond2 = current_running_process->get_timestamp_to_execute_at() > curr_time;
                        if(cond1 && cond2){
                            if(myverbose)  printf("\nPREEMPTING  %d", current_running_process->get_pid());
                            current_running_process->set_time_in_prev_state(curr_time-current_running_process->get_timestamp_for_prev_trans());
                            //curr_proc->set_dynamic_prio(); --> will be changed in TO_PREEMPT
                            int actual_run_time = current_running_process->get_time_in_prev_state();
                            int planned_run_time = actual_run_time + (current_running_process->get_timestamp_to_execute_at() - curr_time);
                            cpu_counter -= planned_run_time ;
                            cpu_counter += actual_run_time;
                            if(verbose_op) printf("\n cpu_counter_down = %d ",cpu_counter);
                            current_running_process->set_transition_to(TO_PREEMPT);
                            current_running_process->set_remaining_exec_time(current_running_process->get_remaining_exec_time()+ planned_run_time - current_running_process->get_time_in_prev_state());
                            current_running_process->set_cpu_exec_burst(current_running_process->get_cpu_exec_burst()+ planned_run_time -current_running_process->get_time_in_prev_state());
                            current_running_process->set_timestamp_to_execute_at(curr_time);
                            if(myverbose)  printf("\nDeleting next event for curr_running_proc");
                            des_.delete_event(current_running_process->get_pid());
                            // current_running_process->set_timestamp_for_prev_trans(curr_time); // causing issue ?
                            Event* ev = new Event(current_running_process) ;
                            des_.insert_eq(*ev);
                            current_running_process = nullptr;
                        }
                    }
                    call_scheduler = true;
                    des_.printeventqueue();
                }
                break;
            case TO_RUNNING:{
                // if(myverbose)  printf("\n####case TO_RUNNING###### \n");
                // if(myverbose)  printf("\nsetting curr_run_process_to %d",curr_event->get_pid());
                current_running_process = curr_proc;
                curr_proc->set_current_state(RUNNING);
                
                if(curr_proc->get_cpu_exec_burst()<=0) { // if process running for 1st time or cpu burst expired, generate new one
                    curr_proc->set_cpu_exec_burst(myrandom(curr_proc->get_cpu_burst())); 
                }
                curr_proc->get_cpu_exec_burst()>quantum?curr_proc->set_transition_to(TO_PREEMPT):curr_proc->set_transition_to(TO_BLOCKED);
                int will_run_for_time = min(quantum, curr_proc->get_cpu_exec_burst());
                if(will_run_for_time >= curr_proc->get_remaining_exec_time()){
                    will_run_for_time = curr_proc->get_remaining_exec_time();
                    curr_proc->set_cpu_exec_burst(will_run_for_time); // could cause issue
                }
               
                //following order needs to be maintatined, calculate time in prev state before
                //setting the new timestamp_for_prev_transition
                curr_proc->set_time_in_prev_state(curr_time - curr_proc->get_timestamp_for_prev_trans());
                curr_proc->set_waiting_time(curr_proc->get_time_in_prev_state());
                curr_proc->set_timestamp_for_prev_trans(curr_time);
                if(verbose_op)
                {
                printf("\n%d %d %d : %s -> %s cpu_burst=%d rem=%d, prio=%d", curr_time, curr_proc->get_pid(),curr_proc->get_time_in_prev_state(),"READY", "RUNNING",curr_proc->get_cpu_exec_burst(), curr_proc->get_remaining_exec_time(), curr_proc->get_dynamic_prio());
                }
                curr_proc->set_remaining_exec_time(curr_proc->get_remaining_exec_time()-will_run_for_time);
                curr_proc->set_cpu_exec_burst(curr_proc->get_cpu_exec_burst()-will_run_for_time);
                if(curr_proc->get_remaining_exec_time() <= 0 )
                    {
                        curr_proc->set_finishing_time(curr_time+will_run_for_time);
                        curr_proc->set_turnaround_time();
                        curr_proc->set_time_in_prev_state(will_run_for_time);
                        curr_proc->set_transition_to(TO_BLOCKED);
                    }
                
                curr_proc->set_timestamp_to_execute_at(curr_time+will_run_for_time);//when it becomes blocked
                Event* new_event =  new Event(curr_proc);//event for when it trans_To_blocked
                des_.insert_eq(*new_event);
                cpu_counter += will_run_for_time;
                if(verbose_op) printf("\n cpu_counter = %d ",cpu_counter);
                break;
            }
            case TO_BLOCKED:{
                // if(myverbose)  printf("\n#####case TO_BLOCKED#### \n");
                if(curr_proc->get_finishing_time()!=0)
                {
                    if(verbose_op) printf("\n%d %d %d : %s", curr_time, curr_proc->get_pid(),curr_proc->get_time_in_prev_state(), "DONE");
                    current_running_process = nullptr;
                    call_scheduler = true;
                    break;
                }
                current_running_process = nullptr;
                curr_proc->set_current_state(BLOCKED);
                curr_proc->set_transition_to(TO_READY);
                int cpu_executed_time = curr_time - curr_proc->get_timestamp_for_prev_trans(); //comes to blocked from running, so time in prev state is essentially how long it was in cpu
                // if(myverbose)  printf("\ncpu executed time %d",cpu_executed_time);
                curr_proc->set_time_in_prev_state(cpu_executed_time);
                //following order needs to be maintatined, calculate time in prev state before
                //setting the new timestamp_for_prev_transition
                curr_proc->set_timestamp_for_prev_trans(curr_time);
                curr_proc->set_io_exec_burst( myrandom(curr_proc->get_io_burst()));
                curr_proc->set_timestamp_to_execute_at(curr_time+curr_proc->get_io_exec_burst());//when it becomes ready
                curr_proc->set_blocked_time(curr_proc->get_io_exec_burst());
                curr_proc->reset_dynamic_prio();
                Event* new_event =  new Event(curr_proc);
                des_.insert_eq(*new_event);
                call_scheduler = true;
                if(verbose_op){
                printf("\nto_bl:%d %d %d : %s -> %s io_burst=%d rem=%d, prio=%d", curr_time, curr_proc->get_pid(),curr_proc->get_time_in_prev_state(),"RUNNING", "BLOCKED", curr_proc->get_io_exec_burst(), curr_proc->get_remaining_exec_time(), curr_proc->get_dynamic_prio());
                // if(myverbose)  printf(" next execution:%d",curr_event->get_timestamp_to_execute_at());
                }
                if(io_counter.flag==true){
                    if(io_counter.end_time < curr_time+curr_proc->get_io_exec_burst()){
                        
                        if(myverbose)  printf("\nchanging endtime from %d ==>", io_counter.end_time);
                        io_counter.end_time = curr_time+curr_proc->get_io_exec_burst();
                        if(myverbose)  printf("%d",io_counter.end_time);
                    }
                }
                else {
                    io_counter.flag = true;
                    io_counter.start_time = curr_time;
                    if(io_counter.end_time < curr_time+curr_proc->get_io_exec_burst()){
                        
                        if(myverbose)  printf("\nchanging endtime from %d ==>", io_counter.end_time);
                        io_counter.end_time = curr_time+curr_proc->get_io_exec_burst();
                        if(myverbose)  printf("%d",io_counter.end_time);
                    }
                    
                }
                
                break;
            
            }
            case TO_PREEMPT:
                current_running_process = nullptr;
                curr_proc->set_current_state(PREEMPT);
                curr_proc->set_transition_to(TO_RUNNING);
                if(myverbose)  printf("changing dp from %d to ", curr_proc->get_dynamic_prio());
                curr_proc->set_dynamic_prio();
                if(myverbose)  printf("%d",curr_proc->get_dynamic_prio());
                current_scheduler->insert_rq(*curr_proc);
                curr_proc->set_time_in_prev_state(curr_time - curr_proc->get_timestamp_for_prev_trans());
                curr_proc->set_timestamp_for_prev_trans(curr_time);
               
                if(verbose_op)
                    {
                    printf("\n%d %d %d : %s -> %s", curr_time, curr_proc->get_pid(),curr_proc->get_time_in_prev_state(),"RUNNING", "READY");
                    }
                call_scheduler =true;
                break;

        }
        
        if(call_scheduler){
            if(des_.get_next_event_time() == curr_time){
                // if(myverbose) printf(" next event to be executed now at %d time not calling scheduler now", curr_time);
                continue;
            }
            // if(myverbose) printf("\n###########calling scheduler#################");
            call_scheduler= false;
            if(current_running_process==nullptr){
                if(myverbose) printf("dequiing from q, current q b4 dq: ");
                current_scheduler->printq();
                current_running_process = current_scheduler->deque_rq();
                if(current_running_process==nullptr){
                    continue;
                }
                if(myverbose) printf("\nCURRENT RUNNING PROCESS PID:%d",current_running_process->get_pid());
                current_running_process->set_timestamp_to_execute_at(curr_time);
                Event* event = new Event(current_running_process);
                des_.insert_eq(*event);
            }
        }
    }
    current_scheduler->printname();
    // if(quantum<10000) printf(" %d",quantum);
    if(myverbose) printf("\npid\t ft\t tt\t bt\t wt\t sp\t");
    for(Process * proc : procQ)
    {
        printf("\n%04d: %4d %4d %4d %4d %1d | %5d %5d %5d %5d",proc->get_pid(),proc->get_arrival_time(), proc->get_total_cpu_time(), proc->get_cpu_burst(), proc->get_io_burst(), proc->get_static_prio(), proc->get_finishing_time(), proc->get_turnaround_time(), proc->get_blocked_time(), proc->get_waiting_time() );
    }
    print_summary();
    delete current_scheduler;
    return 0;
}


// ###########################################################################################
// ################################### function definitions ##################################
// ###########################################################################################

/*
this function should be called only during transition  2 and 3 
 and the initial assignment of static priority
*/
 

void intialise_scheduler(const char* ch){
    if(myverbose) printf("ENTERING");
    char sched = ch[0];
    switch(sched){
        case 'F':
            if(myverbose) printf("for fcfs");
            initialise_correct_scheduler<FCFS_scheduler>();
            break;
        case 'L':
            initialise_correct_scheduler<LCFS_scheduler>();
            break;
        case 'S':
            initialise_correct_scheduler<SRTF_scheduler>();
            break;
        case 'R':
            initialise_correct_scheduler<RR_scheduler>();
            break;
        case 'P':
            initialise_correct_scheduler<PRIO_scheduler>();
            break;
        case 'E':
            initialise_correct_scheduler<PREPRIO_scheduler>();
            ispreprio = true;
            break;

    }
}

void initialise_randvals(ifstream& rfile){
    string st ;
    getline(rfile, st);
    int size = stoi(st);
    ofs = -1;
    while(getline(rfile, st)){
        int num_ = stoi(st);
        randvals.push_back(num_);
    }
    if(size != randvals.size()){if(myverbose) printf("SIZE mismaatch in randvals\n");}
}


vector<string> read_options(int argc, char* argv[]){
    vector<string> ipandrfile;
    if(argc<=2){return ipandrfile;}
    
    int opt;
    while ((opt = getopt(argc, argv, "s:vetph")) != -1) {
        switch (opt) {
            case 's':{
                // ##delete##
                scheduler_option = optarg[0];
                if(myverbose) printf("s %s",scheduler_option.c_str());
                if(strlen(optarg)>1)
                {
                    char does_not_matter;
                    sscanf(optarg, "%c%d:%d", &does_not_matter, &quantum, &maxprio);
                    if(myverbose) printf("\n%cquant:%dmaxprio:%d", does_not_matter, quantum, maxprio);
                }
                
                break;
            }  
            case 'v':
                verbose_op = true;
                if(myverbose) printf("v ");
                break;
            case 't':
                scheduler_op = true;
                if(myverbose) printf("t ");
                break;
            case 'e':
                event_op = true;
                if(myverbose) printf("e ");
                break;
            case 'p':
                if(myverbose) printf("p ");
                break;
            case 'h':
                if(myverbose) printf("h ");
                break;
            case '?':
                if(myverbose) printf("? ");
                break;
        }
    }

    
    for (int i = optind; i < argc; ++i) {
            ipandrfile.push_back(argv[i]);
        }
    return ipandrfile;
}

void print_summary(){
    printf("\nSUM: ");
    double final_time = 0;
    double cpu_util = 0 ;
    double io_util;
    double avg_turnarount_time;
    double avg_wait_time;
    double throughput;
    double wait_time_sum, turnaround_time_sum;
    for (Process* proc : procQ){
        if(final_time < proc->get_finishing_time()){
            final_time = proc->get_finishing_time();
        }
        wait_time_sum += proc->get_waiting_time();
        turnaround_time_sum += proc->get_turnaround_time();
    }

    cpu_util = cpu_counter/final_time;
    cpu_util = cpu_util * 100;
    io_util = io_counter.time_counter / final_time ;
    io_util = io_util * 100;
    avg_wait_time = wait_time_sum / procQ.size();
    avg_turnarount_time = turnaround_time_sum / procQ.size();
    throughput  = (procQ.size()/final_time) * 100;

    printf("%.0f %.2f %.2f %.2f %.2f %.3f", final_time, cpu_util, io_util, avg_turnarount_time, avg_wait_time, throughput );

}
