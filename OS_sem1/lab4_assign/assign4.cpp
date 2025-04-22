#include <fstream>
#include <list>
#include <unistd.h>
#include <sstream>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <vector>
#include <deque>
#include<map>
#include<cstring>
#include <climits>
using namespace std;

/* The schedulers that need to be implemented are FIFO (N), SSTF (S), LOOK (L), CLOOK (C), and FLOOK (F) */
typedef struct {
    int time;
    int track;
    int start_time ;
    int end_time ;
    int index;
    
} instruction;
// ##########################################################################################################
// ##################################################### CONSTANTS ##########################################
// ##########################################################################################################
bool show_fqueue= false, show_queue = false, verbose = false, myverbose = false;
int global_time = 0, execution_counter = 0;
int direction = 0;
int total_time = 0, tot_movement = 0, max_waittime = 0;
double io_busy_time = 0, turnaround_time = 0, wait_time = 0;
int head_at = 0; 

// ##########################################################################################################
// ########################################## FUNCTION DECLARATION ##########################################
// ##########################################################################################################
string get_and_set_options(int argc, char* arv[]);
void set_scheduler(char sched_type);
deque<instruction*> read_input(ifstream& inputfile) ;
bool starts_with_comment(const string& line);
void print_stats();
// ##########################################################################################################
// ############################################# CLASS DEFINITIONS ##########################################
// ##########################################################################################################

class Scheduler{
    protected:
        list<instruction*> io_queue;
    public:
        virtual void add_to_ioqueue(instruction* instr) = 0;
        virtual bool if_request_waiting_in_queue() = 0;
        virtual instruction* strategy() = 0;
        virtual void printQ() = 0;
};

class FCFS_scheduler : public Scheduler {

    public:
        void add_to_ioqueue(instruction* instr){
            if(myverbose) printf("adding  to queue with track %d at time %d \n", instr->track, instr->time);
            io_queue.push_back(instr);
        }

        instruction* strategy(){

            instruction* return_instr = io_queue.front();
            if(return_instr->time  > global_time) return_instr = nullptr ;
            else  io_queue.pop_front();
            return return_instr;
        }

        bool if_request_waiting_in_queue(){
            // true if not empty
            return !io_queue.empty();
        }
        void printQ(){
            for(instruction* curr_instr: io_queue){
                printf("%d(%d)-",curr_instr->index,curr_instr->track);
            }
        }

};
class SSTF_scheduler : public Scheduler {

    public:
        void add_to_ioqueue(instruction* instr){
            if(myverbose) printf("adding  to queue with track %d at time %d \n", instr->track, instr->time);
            io_queue.push_back(instr);
        }

        instruction* strategy(){
            if(!if_request_waiting_in_queue()) return nullptr;
            // calculate the shortest seek time 
            int shortest_seek_time  = INT_MAX;
            int at_index = 0, curr_index =0;
            for(instruction* curr_instr : io_queue)
            {

                int seek_time = curr_instr->track - head_at;
                if(curr_instr->time > global_time) break;
                seek_time = seek_time > 0 ? seek_time : -1 * seek_time ;
                if(shortest_seek_time > seek_time){
                    at_index = curr_index;
                    shortest_seek_time = seek_time;
                }
                curr_index++ ;
            }
            if(shortest_seek_time == INT_MAX) return nullptr;
            //at index we have the sst request --> remove and return it
            list<instruction*>::iterator it = io_queue.begin() ;
            advance(it, at_index);
            instruction* removedItem = *it;
            if(myverbose) printf("REMOVED ITEM %d , index %d, track%d\n", removedItem->index, at_index, removedItem->track);
            io_queue.erase(it);
            return removedItem;
        }

        bool if_request_waiting_in_queue(){
            // true if not empty
            return !io_queue.empty();
        }
        void printQ(){
            for(instruction* curr_instr: io_queue){
                printf("%d(%d)-",curr_instr->index,curr_instr->track);
            }
        }
};
 class LOOK_scheduler : public Scheduler {
    /*
        The initial direction of the LOOK algorithms is from 0-tracks to higher tracks. 
        The head is initially positioned at track=0 at time=0. 
        Note that you do not have to know the maxtrack (think SCAN vs. LOOK).
    */
   private:
    int direction = 1;

   public:
        void add_to_ioqueue(instruction* instr){
            if(myverbose) printf("adding  to queue with track %d at time %d \n", instr->track, instr->time);
            io_queue.push_back(instr);
        }

        instruction* strategy(){
            // calculate the shortest seek time 
            if(!if_request_waiting_in_queue()) return nullptr;
            int shortest_seek_time  = INT_MAX;
            int at_index = 0, curr_index =0;
            for(instruction* curr_instr : io_queue)
            {
                if((curr_instr->track - head_at) * direction <0) 
                {
                    curr_index ++;
                    continue;
                }
                int seek_time = curr_instr->track - head_at;
                if(curr_instr->time > global_time) break;
                seek_time = seek_time > 0 ? seek_time : -1 * seek_time ;
                if(shortest_seek_time > seek_time){
                    at_index = curr_index;
                    shortest_seek_time = seek_time;
                }
                curr_index++ ;
            }
            if(shortest_seek_time == INT_MAX) {
                direction = direction * -1;
                at_index = 0;
                curr_index =0;
                for(instruction* curr_instr : io_queue)
                {
                    if((curr_instr->track - head_at) * direction <0) 
                    {
                        curr_index ++;
                        continue;
                    }
                    int seek_time = curr_instr->track - head_at;
                    if(curr_instr->time > global_time) break;
                    seek_time = seek_time > 0 ? seek_time : -1 * seek_time ;
                    if(shortest_seek_time > seek_time){
                        at_index = curr_index;
                        shortest_seek_time = seek_time;
                    }
                    curr_index++ ;
                }
            }
            if(shortest_seek_time == INT_MAX) return nullptr;
            //at index we have the sst request --> remove and return it
            list<instruction*>::iterator it = io_queue.begin() ;
            advance(it, at_index);
            instruction* removedItem = *it;
            if(myverbose) printf("REMOVED ITEM %d , index %d, track%d\n", removedItem->index, at_index, removedItem->track);
            io_queue.erase(it);
            return removedItem;
        }

        bool if_request_waiting_in_queue(){
            // true if not empty
            return !io_queue.empty();
        }
        void printQ(){
            for(instruction* curr_instr: io_queue){
                printf("%d(%d)-",curr_instr->index,curr_instr->track);
            }
        }

};
class CLOOK_scheduler : public Scheduler {
    /*
        The initial direction of the LOOK algorithms is from 0-tracks to higher tracks. 
        The head is initially positioned at track=0 at time=0. 
        Note that you do not have to know the maxtrack (think SCAN vs. LOOK).
    */
    public:
        void add_to_ioqueue(instruction* instr){
            if(myverbose) printf("adding  to queue with track %d at time %d \n", instr->track, instr->time);
            io_queue.push_back(instr);
        }

        instruction* strategy(){
            // calculate the shortest seek time 
            if(!if_request_waiting_in_queue()) return nullptr;
            int shortest_seek_time  = INT_MAX, least_track = INT_MAX;
            int at_index_wrap = 0, at_index_min = 0, curr_index =0;
            for(instruction* curr_instr : io_queue)
            {
                int seek_time = curr_instr->track - head_at;
                if(seek_time >= 0) {
                    if(seek_time < shortest_seek_time) {
                        shortest_seek_time = seek_time;
                        at_index_min = curr_index;
                    }
                }
                else{
                    if(least_track > curr_instr->track){
                        least_track = curr_instr->track;
                        at_index_wrap = curr_index;
                    }
                }
                curr_index++;
            }
            int index = 0;
            if(shortest_seek_time == INT_MAX && least_track == INT_MAX) return nullptr;
            else{

                if(shortest_seek_time != INT_MAX) index = at_index_min;
                else index = at_index_wrap;
            }
            //at index we have the sst request --> remove and return it
            list<instruction*>::iterator it = io_queue.begin() ;
            advance(it, index);
            instruction* removedItem = *it;
            if(myverbose) printf("REMOVED ITEM %d , index %d, track%d\n", removedItem->index, index, removedItem->track);
            io_queue.erase(it);
            return removedItem;
        }

        bool if_request_waiting_in_queue(){
            // true if not empty
            return !io_queue.empty();
        }
        void printQ(){
            for(instruction* curr_instr: io_queue){
                printf("%d(%d)-",curr_instr->index,curr_instr->track);
            }
        }


};
class FLOOK_scheduler : public Scheduler {
    /*
        The initial direction of the LOOK algorithms is from 0-tracks to higher tracks. 
        The head is initially positioned at track=0 at time=0. 
        Note that you do not have to know the maxtrack (think SCAN vs. LOOK).
    */
   private:
        list<instruction*> add_queue;
        int direction = 1;
   public:
        void add_to_ioqueue(instruction* instr){
            if(myverbose) printf("adding  to queue with track %d at time %d \n", instr->track, instr->time);
            add_queue.push_back(instr);
        }

        instruction* strategy(){
            if(myverbose) printf("Entering strategy at %d\n with direction %d", global_time, direction);
            if(io_queue.empty() && !add_queue.empty()) swap();
            // calculate the shortest seek time 
            if(io_queue.empty()) return nullptr;
           int shortest_seek_time  = INT_MAX;
            int at_index = 0, curr_index =0;
            for(instruction* curr_instr : io_queue)
            {
                if((curr_instr->track - head_at) * direction <0) 
                {
                    curr_index ++;
                    continue;
                }
                int seek_time = curr_instr->track - head_at;
                if(curr_instr->time > global_time) break;
                seek_time = seek_time > 0 ? seek_time : -1 * seek_time ;
                if(shortest_seek_time > seek_time){
                    at_index = curr_index;
                    shortest_seek_time = seek_time;
                }
                curr_index++ ;
            }
            if(shortest_seek_time == INT_MAX) {
                if(myverbose) printf("changing direction from %d to %d\n", direction, (direction * -1));
                direction = direction * -1;
                at_index = 0;
                curr_index =0;
                for(instruction* curr_instr : io_queue)
                {
                    if((curr_instr->track - head_at) * direction <0) 
                    {
                        curr_index ++;
                        continue;
                    }
                    int seek_time = curr_instr->track - head_at;
                    if(curr_instr->time > global_time) break;
                    seek_time = seek_time > 0 ? seek_time : -1 * seek_time ;
                    if(shortest_seek_time > seek_time){
                        at_index = curr_index;
                        shortest_seek_time = seek_time;
                    }
                    curr_index++ ;
                }
            }
            if(shortest_seek_time == INT_MAX) {
                if(myverbose) printf("changing direction from %d to %d and returning nllptr\n", direction, (direction * -1));
                direction = direction * -1;
                return nullptr;
            }
            //at index we have the sst request --> remove and return it
            list<instruction*>::iterator it = io_queue.begin() ;
            advance(it, at_index);
            instruction* removedItem = *it;
            if(myverbose) printf("REMOVED ITEM %d , index %d, track%d\n", removedItem->index, at_index, removedItem->track);
            io_queue.erase(it);
            return removedItem;
        }

        void swap(){
            if(myverbose)printf("SWAPPING\n");
            
            list<instruction*> *io_ptr = &io_queue;
            list<instruction*> *ad_ptr = &add_queue;
            list<instruction*> tempQ = *io_ptr;
            *io_ptr = *ad_ptr;
            *ad_ptr = tempQ;
            if(myverbose) printQ();
            if(myverbose) printf("\n");
            if(myverbose)printf("SWAPPING SUCCESS\n");
            direction = 1;
        }

        void printQ(){
            for(instruction* curr_instr: io_queue){
                printf("%d(%d)-",curr_instr->index,curr_instr->track);
            }
        }

        bool if_request_waiting_in_queue(){
            // true if not empty
            return (!io_queue.empty() || !add_queue.empty());
        }


};


Scheduler* curr_scheduler = nullptr;
instruction* active_instruction = nullptr ;
bool is_occupied = false;
vector<instruction*> final_list;
// ##########################################################################################################
// ############################################### MAIN FUNCTION ############################################
// ##########################################################################################################
int main(int argc, char* argv[]){

    string ipfilename = get_and_set_options(argc, argv);
    ifstream input_file(ipfilename);
    if(!input_file.is_open()){ if(myverbose)  printf("\ninput file could not be opened\n"); return 1; }
    deque<instruction*>instruction_list;
    instruction_list = read_input(input_file);
    if(myverbose) printf("instruction list size is %zu\n", instruction_list.size());
    int instruction_counter = 0;
    while(!instruction_list.empty() || curr_scheduler->if_request_waiting_in_queue()){
        
        instruction* curr_instr = instruction_list.front();
        if (curr_instr != nullptr && curr_instr->time == global_time){
            if(myverbose) printf("if statement 1\n");
            if(verbose) printf("%d:\t%d add %d\n", global_time, instruction_counter++, curr_instr->track );
            instruction_list.pop_front();
            curr_scheduler->add_to_ioqueue(curr_instr);
            final_list.push_back(curr_instr) ;
        }

        if(is_occupied)
        {
            if(myverbose) printf("if statement 2\n");
            if(myverbose) printf("global time: %d index:%d time:%d track:%d start_time:%d end_time%d\n",
            global_time, active_instruction->index, active_instruction->time, active_instruction->track, active_instruction->start_time, active_instruction->end_time);
            if(active_instruction->end_time == global_time)
                {   
                    is_occupied = false;
                    if(verbose) printf("%d:\t %d finish %d\n", global_time, active_instruction->index, active_instruction->end_time - active_instruction->start_time);
                    active_instruction = nullptr;
                }
            head_at += direction;
        }

        if(!is_occupied && active_instruction == nullptr){
            if(myverbose) printf("if statement 3 %d\n", global_time);
            if(curr_scheduler->if_request_waiting_in_queue()){
                active_instruction = curr_scheduler->strategy();
                if(active_instruction == nullptr) {
                    global_time ++;
                    continue;
                }
                if(direction == 0 ) direction = 1;
                else if(head_at > active_instruction->track) direction = -1;
                else direction = 1;
                if(verbose) printf("%d:\t%d issue %d %d\n", global_time, active_instruction->index, active_instruction->track, head_at);
                if(myverbose) printf("Setting start_time as %d for index %d \n", global_time, active_instruction->index);
                active_instruction->start_time = global_time;
                int distance  = head_at - active_instruction->track; 
                distance = distance > 0 ? distance : (-1 * distance) ;
                tot_movement += distance;
                io_busy_time += distance ; // io_busy_time
                int end_time = global_time + distance;
                if(myverbose) printf("Calculating end_time %d with global time %d and distance %d for request %d \n",
                end_time,
                global_time,
                distance,
                active_instruction->index);
                active_instruction->end_time = end_time;
                turnaround_time += active_instruction->end_time - active_instruction->time;
                wait_time += active_instruction->start_time - active_instruction->time;
                if(myverbose) printf("End time us %d\n", active_instruction->end_time);
                if(global_time - active_instruction->time > max_waittime) max_waittime = global_time - active_instruction->time;
                if(distance != 0) is_occupied = true;
                else 
                {   
                    
                    active_instruction = nullptr;
                    continue;
                }
            }

        }

       
        // else {
        //     printf("$$$$$$$$$CODE SHOULD NEVER COME HERE$$$$$$$$$$$$$$$$$$");
        // }
    //    if(myverbose) printf("########  %d --> %d \n", global_time, tot_movement);
       global_time++ ;
    // if(!(!instruction_list.empty() || curr_scheduler->if_request_waiting_in_queue())) total_time = active_instruction->end_time;

    }
    print_stats();
}



// ##########################################################################################################
// ########################################## FUNCTION DEFINITION ##########################################
// ##########################################################################################################

template <typename T>
void instantiate_scheduler()
{
    curr_scheduler = new T();
}

void set_scheduler(char sched_type){
        switch(sched_type){
            case 'N':
                instantiate_scheduler<FCFS_scheduler>();
                break;
            case 'S':
                instantiate_scheduler<SSTF_scheduler>();
                break;
            case 'L':
                instantiate_scheduler<LOOK_scheduler>();
                break;
            case 'C':
                instantiate_scheduler<CLOOK_scheduler>();
                break;
            case 'F':
                instantiate_scheduler<FLOOK_scheduler>();
                break;
            default :
                //instantiate_scheduler<FCFS_scheduler>();
                printf("\n Option provided is invalid \n");
                exit(1);
                break;
        }
}

void print_stats(){

        for(int i=0 ; i< final_list.size() ; ++i){
            instruction* curr_intr = final_list[i];
            if(curr_intr->end_time > total_time) total_time = curr_intr->end_time;
            printf("%5d: %5d %5d %5d\n", curr_intr->index, curr_intr->time, curr_intr->start_time, curr_intr->end_time);
        }
        // total_time = final_list.back()->end_time;

        double io_utilization = io_busy_time / total_time;
        double avg_turnaround  = turnaround_time / final_list.size();
        double avg_waittime = wait_time / final_list.size();
        printf("SUM: %d %d %.4lf %.2lf %.2lf %d\n",
        total_time, tot_movement, io_utilization,
        avg_turnaround, avg_waittime, max_waittime);


}

deque<instruction*> read_input(ifstream& inputfile){
    if(myverbose) printf("Entering read input \n");
    string line;
    deque<instruction*> instruction_list;
    int counter = 0;
    while (getline(inputfile, line)) {
    stringstream ss(line);
    if (starts_with_comment(line)) {
        continue;
    }
    else {
        int time, track;
        ss >> time >> track;
        instruction* instruction_read = new instruction();
        instruction_read->index = counter++;
        instruction_read->time = time;
        instruction_read->track = track;
        instruction_list.push_back(instruction_read);
        if(myverbose) printf("pushing instruction into the list \n");
    }
    }

    return instruction_list;
}

bool starts_with_comment(const std::string& line) {
    for (char ch : line) {
        if (!std::isspace(ch)) { // Ignore leading spaces
            return ch == '#';   // Return true if the first non-space character is '#'
        }
    }
    return false; // Line is empty or doesn't start with '#'
}

string get_and_set_options(int argc, char* argv[]){
    int c;
    while((c = getopt (argc, argv, "s:vqf")) != -1){
        switch(c)
        {
        case 's':
            set_scheduler(optarg[0]);
            break;
        case 'v':
            verbose = true;
            break;
        case 'q':
            show_queue = true;
            break;
        case 'f':
            show_fqueue = true;
            break;
        default:
            break;
        }
    }

    string ipfile;
    ipfile = argv[optind];

    return ipfile;
}
