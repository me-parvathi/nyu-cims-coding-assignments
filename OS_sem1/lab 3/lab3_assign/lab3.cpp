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
using namespace std;

/*

Algorithm                               Based on Physical Frames
FIFO                                    F
Random                                  R
Clock                                   C
Enhanced Second Chance / NRU            E
Aging                                   A
Working Set                             W

*/


// ##########################################################################################################
// ##################################################### CONSTANTS ##########################################
// ##########################################################################################################
const int MAX_FRAMES = 128, MAX_VPAGES = 64, RESET_NRU_COUNTER = 48, TAU = 49;
int num_frames, ofs, num_process=0, process_counter = 0;
bool O = false, P = false, F = false, S = false, myverbose=false;
unsigned long long int instruction_counter = 0;     // instruction counter
unsigned long long int context_switcher_counter = 0;    // total context switches
unsigned long long int exit_counter = 0;      // total process exits
unsigned long long int total_cost = 0;            // total cost
vector<int> randvals;
typedef struct frame_tagname{ 
    int frame_id = -1;
    int process = -1;
    int vpage = -1;
    // only used in the aging and working set algorithms :()
    unsigned int age = 0 ;

 } frame_t;
typedef struct  { 
    unsigned int PRESENT : 1;
    unsigned int REFERENCED : 1; 
    unsigned int MODIFIED : 1; 
    unsigned int WRITE_PROTECT : 1; 
    unsigned int PAGED_OUT : 1; 
    unsigned int FRAME_NO :  7; 
    unsigned int FILE_MAPPED : 1;
    //unsigned int VMA: 6
 } pte_t; // can only be total of 32-bit size and will check on this

typedef struct {
    int start_vpage;
    int end_vpage;
    unsigned int write_protect : 1;
    unsigned int file_mapeed : 1;
    
}vma;

typedef struct { 
    char op;
    int vpage_num;
} instruction;

typedef struct {
    unsigned long maps, unmaps, 
    ins, outs, 
    fins, fouts, 
    zeroes, segv, 
    segprot;
} proc_stat;

frame_t frame_table[MAX_FRAMES];
deque <frame_t*> unallocated_frames;
// ##########################################################################################################
// ########################################## FUNCTION DECLARATION ##########################################
// ##########################################################################################################
vector<string> set_options(int argc, char* argv[]);
void initialise_pager (char* algorithm_option);
void break_options(char* options);
void initialise_frames();
void initialise_randvals(ifstream& rfile);
void read_input(ifstream& ipfile);
bool starts_with_comment(const std::string& line);
void print_input();
void do_context_switch(int pid);
void do_read_write_operation(int vpage_num, char operation);
// void do_write_operation(int vpage_num);
void do_exit_operation(int pid);
bool is_valid(int vpage_num);
int page_fault_handler(int vpage_num, char operation);
void print_page_table();
void print_frame_table();
void print_statistics();
frame_t *get_frame();
int myrandom();
int cost(char op);
// ##########################################################################################################
// ############################################# CLASS DEFINITIONS ##########################################
// ##########################################################################################################
class Process{
    protected:
        int process_id;
        int no_of_vma;
        vector<vma> my_vmas;
        pte_t page_table[MAX_VPAGES] = {0};
        map<int,bool> is_hole_mapping;
    
    public:
        proc_stat _proc_stat = {0};
        Process(int pid, int vma_size){
            this->process_id = pid;
            this->no_of_vma = vma_size;
            for(int i = 0; i<MAX_FRAMES ; ++i){
                is_hole_mapping[i] = true;
            }
        }

        void add_vma(vma curr_vma){
            my_vmas.push_back(curr_vma);
        }

        int get_pid(){
            return this->process_id;
        }

        int get_vma_size(){
            return no_of_vma;
        }

        int curr_vma_Size(){
            return my_vmas.size();
        }
        void update_is_hole(int vpage_num, bool is_whole_){
            is_hole_mapping[vpage_num] = is_whole_ ;
        }
        bool is_valid_vma(int vpage_num){
            return !is_hole_mapping[vpage_num];
        }

        bool is_pte_present(int vpage_num){
             return page_table[vpage_num].PRESENT;
        }

        void set_pte_present(int vpage_num){
            page_table[vpage_num].PRESENT = 1;
        }
        void unset_pte_present(int vpage_num){
            page_table[vpage_num].PRESENT = 0;
        }
        void set_pte_modified(int vpage_num){
            page_table[vpage_num].MODIFIED = 1;
        }
        void unset_pte_modified(int vpage_num){
            page_table[vpage_num].MODIFIED = 0;
        }
        void set_pte_refereneced(int vpage_num){
            page_table[vpage_num].REFERENCED = 1;
            if(myverbose) printf("\n\n@@@@@setting referenced  bit@@@@@ for pid%d vpage %d to %d \n\n", this->process_id, vpage_num, page_table[vpage_num].REFERENCED);
        }
        void unset_pte_refereneced(int vpage_num){
            page_table[vpage_num].REFERENCED = 0;
        }
        void set_pte_write_protected(int vpage_num){
            page_table[vpage_num].WRITE_PROTECT = 1;
        }
        void set_pte_paged_out(int vpage_num){
            page_table[vpage_num].PAGED_OUT = 1;
            if(myverbose) printf("\n\n@@@@@setting paged out bit@@@@@ for pid%d to %d \n\n", this->process_id, page_table[vpage_num].PAGED_OUT);
        }
        void set_pte_frame_no(int vpage_num, int frame_no){
            page_table[vpage_num].FRAME_NO = frame_no;
            // if(myverbose) printf("\n\n@@@@@setting frame no bits@@@@@ for pid%d to %d \n\n", this->process_id, page_table[vpage_num].FRAME_NO);
        }
        void unset_pte_frame_no(int vpage_num){
            page_table[vpage_num].FRAME_NO = 0;
        }
        int get_pte_modified_bit(int vpage_num){
            return page_table[vpage_num].MODIFIED;
        }
        int get_pte_paged_out_bit(int vpage_num){
            if(myverbose) printf("\n\n@@@@@@@@@@@@@@@getting paged out bit@@@@@ for pid%d which is %d\n\n", this->process_id, page_table[vpage_num].PAGED_OUT);
            return page_table[vpage_num].PAGED_OUT;
        }
        int get_pte_referenced_bit(int vpage_num){
            return page_table[vpage_num].REFERENCED;
        }
        int get_pte_frame_no(int vpage_num){
            // if(myverbose) printf("\n\n@@@@@@@@@@@@@@@getting frame no bit@@@@@ for pid%d which is %d\n\n", this->process_id, page_table[vpage_num].FRAME_NO);
            return page_table[vpage_num].FRAME_NO;
        }

        void reset_pte(int vpage_num){
            page_table[vpage_num].FILE_MAPPED = 0;
            page_table[vpage_num].FRAME_NO = 0;
            page_table[vpage_num].MODIFIED = 0;
            page_table[vpage_num].PAGED_OUT = 0;
            page_table[vpage_num].PRESENT = 0;
            page_table[vpage_num].REFERENCED = 0;
            page_table[vpage_num].WRITE_PROTECT = 0;
            
        }
        bool is_file_mapped(int vpage_num){
            if(page_table[vpage_num].MODIFIED == 1 || page_table[vpage_num].REFERENCED == 1)
            {
                if(myverbose)printf("Returning an already declared file protect bit which is %d\n",page_table[vpage_num].FILE_MAPPED);
                return page_table[vpage_num].FILE_MAPPED;
            }

            for (int i = 0; i < this->no_of_vma; ++i){
                int start = my_vmas[i].start_vpage;
                int end = my_vmas[i].end_vpage;
                if(vpage_num>=start && vpage_num<=end) {
                    page_table[vpage_num].FILE_MAPPED = my_vmas[i].file_mapeed;
                    return my_vmas[i].file_mapeed;
                }
            }
            //ideally it should never reach here but for debugging
            if(myverbose) printf(" $$$$$$$$$$$DEADDDDD$$$$$$$$$$$$$$$$ ");
            return false;
        }

        bool is_write_protected(int vpage_num){

            if(page_table[vpage_num].MODIFIED == 1 || page_table[vpage_num].REFERENCED == 1)
            {
                if(myverbose)printf("Returning an already declared write protect bit which is %d\n",page_table[vpage_num].WRITE_PROTECT);
                return page_table[vpage_num].WRITE_PROTECT;
            }

            for (int i = 0; i < this->no_of_vma; ++i){
                int start = my_vmas[i].start_vpage;
                int end = my_vmas[i].end_vpage;
                if(vpage_num>=start && vpage_num<=end) {
                    page_table[vpage_num].WRITE_PROTECT = my_vmas[i].write_protect;
                    return my_vmas[i].write_protect;
                }
            }
            //ideally it should never reach here but for debugging
            if(myverbose) printf(" $$$$$$$$$$$DEADDDDD$$$$$$$$$$$$$$$$ ");
            return false;
        }

};
Process* active_process = nullptr;
vector<Process*> process_list;
deque<instruction> instruction_list;
class pager{
    
    public:
        virtual frame_t* select_victim_frame() = 0;
        virtual void reset_age(int frame_id) = 0;
        virtual ~pager() {}
};

class FIFO_pager : public pager {
    private:
        int hand_at = 0;
    public:
        FIFO_pager(){
            // printf("\nFIFO pager\n");
        }
        frame_t* select_victim_frame(){
            frame_t* victim_frame = &frame_table[hand_at];
            hand_at = (hand_at + 1) % num_frames;
            return victim_frame;
        }  
        
        void reset_age(int frame_id) {
            // No OP
        }

        ~FIFO_pager(){}
        

};
class Random_pager : public pager {
    private:
        int hand_at;
    public:
        Random_pager(){
            hand_at = 0;
        }
        frame_t* select_victim_frame(){
            hand_at = myrandom();
            frame_t* victim_frame =  &frame_table[hand_at];
            return victim_frame;
        }
        void reset_age(int frame_id) {
            // No OP
        }
        ~Random_pager(){}
};
class Clock_pager : public pager {
    private:
        int hand_at;
    public:
        Clock_pager(){
            // cout << "clock pager initialised";
            hand_at = 0;
        }
        frame_t* select_victim_frame(){
            if(myverbose) cout << "B4 at clock pager hand_at -->" << hand_at <<"  ";
            frame_t* victim_frame =  &frame_table[hand_at];
            int victim_frame_pid  = victim_frame->process;
            int vpage = victim_frame->vpage;
            if(myverbose) cout << "frame_id " << victim_frame->frame_id << " Referenced bit of vpage " << vpage<< " is   --> " << active_process->get_pte_referenced_bit(vpage) <<"\n";
            while(process_list[victim_frame_pid]->get_pte_referenced_bit(vpage)==1){
                process_list[victim_frame_pid]->unset_pte_refereneced(vpage);
                hand_at = (hand_at + 1) % num_frames;
                victim_frame =  &frame_table[hand_at];
                vpage = victim_frame->vpage;
                victim_frame_pid  = victim_frame->process;
            }
            victim_frame =  &frame_table[hand_at];
            hand_at = (hand_at + 1) % num_frames;
             if(myverbose) cout << "AFT at clock pager hand_at -->" << hand_at <<"  ";
             if(myverbose) 
             {
                int temp = frame_table[hand_at].vpage;
                int temp2 = frame_table[hand_at].frame_id;
                cout << "frame_id " << temp2  << "Referenced bit of vpage " <<  temp <<" is -->" << active_process->get_pte_referenced_bit(vpage) <<"\n";
             }
            return victim_frame;
        }

        void reset_age(int frame_id) {
            // No OP
        }

        ~Clock_pager(){}
};
class NRU_pager : public pager {
    private:
        int last_reset;
       
        int hand_at;
    public:
        NRU_pager(){
            hand_at = 0;
            last_reset = 0;
        }
        int get_class(int r, int m){
            return 2*r+m;
        }
        frame_t* select_victim_frame(){
            if(myverbose) printf("Entering victim frame selection\n");
            int class_map[4] = {-1, -1, -1, -1};
            bool reset = false;
            if(instruction_counter - last_reset  >= 48) {
                if(myverbose)printf("RESET set to true at instruction count %d\n",instruction_counter);
                last_reset = instruction_counter;
                reset = true;
            }
             frame_t* victim_frame = nullptr;
            for(int i = 0; i< num_frames ; ++i){
                if(myverbose) printf(" *%i* ", hand_at);
                victim_frame =  &frame_table[hand_at];
                int vpage_num = victim_frame->vpage;
                int curr_pid = victim_frame->process;
                int curr_class = get_class(process_list[curr_pid]->get_pte_referenced_bit(vpage_num), process_list[curr_pid]->get_pte_modified_bit(vpage_num));
                if(curr_class == 0 && !reset){
                    if(myverbose) printf("Class %d\n",curr_class);
                    hand_at = (hand_at + 1) % num_frames;
                    return victim_frame;
                }
                else{
                    if (class_map[curr_class]==-1) class_map[curr_class] = hand_at;
                }
                if(reset){
                    process_list[curr_pid]->unset_pte_refereneced(vpage_num);
                }
                hand_at = (hand_at + 1) % num_frames;
              
            }

            for(int i = 0 ; i < 4 ; ++i){
                if (class_map[i] != -1) {
                    if(myverbose) printf("\nFrom the second for loop Class %d\n",i);
                    int victim_id = class_map[i];
                    victim_frame =  &frame_table[victim_id];
                    hand_at = (victim_id + 1) % num_frames;
                    break;
                }
            }
            
            return victim_frame;

        }

        void reset_age(int frame_id) {
            // No OP
        }

        ~NRU_pager(){}
};
class Aging_pager : public pager {
    private:
        int hand_at;
    public:
        Aging_pager(){
            hand_at = 0;
        }
        frame_t* select_victim_frame(){
            int min_hand = hand_at;
            //loop throught to find min
            //during each loop do a >> with age and 
            //if referenced bit is set then do a MSB = 1 and unset the R bit
            frame_t* victim_frame = &frame_table[min_hand];
            for(int i = 0; i < num_frames ; ++i ){
                frame_t* curr_frame = &frame_table[hand_at];
                int curr_vpage_num = curr_frame->vpage;
                int curr_pid = curr_frame->process;
                curr_frame->age >>= 1;
                if(process_list[curr_pid]->get_pte_referenced_bit(curr_vpage_num)){
                    curr_frame->age |= 0x80000000;
                    process_list[curr_pid]->unset_pte_refereneced(curr_vpage_num);
                }

                if(victim_frame->age > curr_frame->age){
                    min_hand = hand_at;
                    victim_frame = &frame_table[min_hand];
                }
                hand_at = (hand_at + 1)%num_frames;
            }

            hand_at = (min_hand+1)%num_frames;
            return victim_frame;


        }

        void reset_age(int frame_id) {
            // Add OP
            frame_t *frame = &frame_table[frame_id];
            frame->age = 0;
        }
        ~Aging_pager(){}
};

class Set_pager : public pager {
    private:
        int hand_at;
    public:
        Set_pager(){
            hand_at = 0;
        }
        frame_t* select_victim_frame(){

            int oldest_frame_no = hand_at;
            frame_t* victim_frame = &frame_table[oldest_frame_no];
            for(int i = 0; i< num_frames ; ++i ){
                if(myverbose) printf(" *%i*--> ", hand_at);
                frame_t* curr_frame = &frame_table[hand_at];
                int curr_vpage_num = curr_frame->vpage;
                int curr_pid = curr_frame->process;
                if(instruction_counter > (curr_frame->age + TAU) && process_list[curr_pid]->get_pte_referenced_bit(curr_vpage_num)==0){
                    if(myverbose) printf("1 ");
                    oldest_frame_no = hand_at;
                    victim_frame = &frame_table[oldest_frame_no];
                    break;
                }
                
                if(process_list[curr_pid]->get_pte_referenced_bit(curr_vpage_num)){
                    if(myverbose) printf("2 ");
                    curr_frame->age  = instruction_counter;
                    process_list[curr_pid]->unset_pte_refereneced(curr_vpage_num);
                }

                if(curr_frame->age < victim_frame->age) {
                    if(myverbose) printf("3 %d < %d ", curr_frame->age, victim_frame->age);
                    oldest_frame_no = hand_at;
                    victim_frame = &frame_table[oldest_frame_no];
                }
                hand_at = (hand_at + 1) % num_frames;
                
            }
            
            hand_at = (oldest_frame_no + 1) % num_frames;
            return victim_frame;
        }

        void reset_age(int frame_id) {
            // Add OP
            frame_t *frame = &frame_table[frame_id];
            frame->age = instruction_counter;
        }
            
        ~Set_pager(){}
};



pager* current_pager = nullptr;


// ##########################################################################################################
// ############################################### MAIN FUNCTION ############################################
// ##########################################################################################################

int main(int argc, char* argv[]){
    if(myverbose) printf("in main function");
    // read and create objects from input
    vector<string> ipandrfile;
    ipandrfile = set_options(argc, argv);
    if(ipandrfile.size()<2){  printf("\nNot enough args\n"); return 1;}
    string ipfilename = ipandrfile.front();
    string rfilename = ipandrfile.back();
    ifstream input_file(ipfilename);
    ifstream rfile(rfilename);
    
    if(!input_file.is_open()){ if(myverbose)  printf("\ninput file could not be opened\n"); return 1; }
    if(!rfile.is_open()){ if(myverbose)  printf("\nrfile could not be opened\n"); return 1;}

    initialise_randvals(rfile);
    read_input(input_file);
    initialise_frames();
    print_input();
    if(myverbose) printf("\nsize-->%zu\n",instruction_list.size());
    //  printf("NUM PROCESS lalalal CALLING %d\t\n\n\n", num_process);
    int instr_counter = 0;
    while(!instruction_list.empty()){
        instruction_counter++;
        instruction curr_instr = instruction_list.front();
        instruction_list.pop_front();
        char operation = curr_instr.op;
        int vpage_num = curr_instr.vpage_num;
        if(myverbose) cout << "\ninstruction to be executed --> " << curr_instr.op << " " << curr_instr.vpage_num << "\n";
        printf("%d: ==> %c %d\n",instr_counter++, operation, vpage_num);
        // printf("NUM PROCESS lalalal CALLING %d\t\n\n\n", num_process);
        switch (operation)
        {
        case 'c':
            {
                int pid = vpage_num;
                do_context_switch(pid);
            }
            break;
        case 'w':
        case 'r':
            do_read_write_operation(vpage_num, operation);
            break;
        case 'e':
            {
                int pid = vpage_num;
                do_exit_operation(pid);
            }
            break;
        default:
            break;
        }
    }
    if(myverbose) printf("1NTWTWwetewtw \n");
    if(myverbose) printf("NUM PROCESS BEFRE CALLING %d\t\n\n\n", num_process);
    if(P) print_page_table();
    if(myverbose) printf("2NTWTWwetewtw \n");
    if(F) print_frame_table();
    if(S) print_statistics();
    delete active_process;
    delete current_pager;
    return 0;
}


// ##########################################################################################################
// ########################################## FUNCTION DEFINITION ##########################################
// ##########################################################################################################

void do_context_switch(int pid){
    //set global process_id
    //activate the process of which we need to activate the process table
    total_cost += cost('C');
    context_switcher_counter++ ;
    active_process = process_list[pid];
    if(myverbose) printf("active process-->%d\n",process_list[pid]->get_pid());
}

void do_read_write_operation(int vpage_num, char operation){
   if(myverbose) if(num_process != 4) printf("NUM PROCESS lalalal CALLING %d\t\n\n\n", num_process);
   total_cost += cost('R');
   //check the present bit of the pte
    int frame_no = -1;
    if(!active_process->is_pte_present(vpage_num)) 
    {
        frame_no = page_fault_handler(vpage_num, operation);
    }

    if(frame_no !=-1){

        active_process->set_pte_present(vpage_num);
        if(myverbose) printf("Setting pte_present of pid: %d vpage %d to %d\n", 
        active_process->get_pid(),
        vpage_num, active_process->is_pte_present(vpage_num));
        active_process->is_file_mapped(vpage_num);
        active_process->is_write_protected(vpage_num);
        active_process->set_pte_refereneced(vpage_num);
        if(active_process->is_file_mapped(vpage_num)) {
                    total_cost += cost('F');
                    active_process->_proc_stat.fins++ ;
                    printf(" FIN\n");
                }
        else if(active_process->get_pte_paged_out_bit(vpage_num)==1){
                total_cost += cost('I');
                    active_process->_proc_stat.ins++ ;
                    printf(" IN\n");
            }
        else{
            if(myverbose) printf("Setting pte_refenerenced of vpage %d to %d", vpage_num, active_process->get_pte_referenced_bit(vpage_num));
            total_cost += cost('Z');
            active_process->_proc_stat.zeroes++ ;
            printf(" ZERO\n");
        }
        
        total_cost += cost('M');
            active_process->_proc_stat.maps++ ;
            current_pager->reset_age(frame_no);
            printf(" MAP %d\n",frame_no);
    }

    switch (operation)
                {
                case 'r':
                    //nothing to do
                    // active_process->unset_pte_modified(vpage_num);
                    active_process->set_pte_refereneced(vpage_num);
                    break;
                case 'w':
                    active_process->set_pte_refereneced(vpage_num);
                    if(active_process->is_write_protected(vpage_num)) 
                    {  
                        total_cost += cost('s');
                        active_process->_proc_stat.segprot++ ;
                        printf(" SEGPROT\n");
                    }
                    else {
                        active_process->set_pte_modified(vpage_num);
                        if(myverbose)printf("$$$$$$$$$SEtting pte of pid:%d vpage:%d modified bit to %d\n", active_process->get_pid(), vpage_num, active_process->get_pte_modified_bit(vpage_num));
                    }
                    break;
                default:
                    break;
                }

    // Now,frame must be allocated,assigned to the PTE belonging to the vpage of this instructio
    //1. update the present bit of new pte
    //2. if the vma is write protected or file and operation is write - set referenced to 1
    //2.2 else based on read/write do the read/modify bit
    
}

// void do_write_operation(int vpage_num){

// }

void do_exit_operation(int pid){
    if(myverbose) if(num_process != 4) printf("start NUM PROCESS lalalal CALLING %d\t\n\n\n", num_process);
    printf("EXIT current process %d\n", pid);
    Process* exiting_process = process_list[pid];
    total_cost += cost('E');
    exit_counter++ ;
    for(int vpage_num = 0 ; vpage_num < MAX_VPAGES; ++vpage_num){

        if(exiting_process->is_pte_present(vpage_num)){
            printf(" UNMAP %d:%d\n", pid, vpage_num);
            total_cost += cost('U');
            exiting_process->_proc_stat.unmaps++;

            //now onto the frame and frame table
            int frame_no = exiting_process->get_pte_frame_no(vpage_num);
            // printf("\n frame no %d", frame_no);
            frame_t* frame = &frame_table[frame_no];
            frame->age = 0 ;
            frame->process = -1;
            frame->vpage = -1;
            unallocated_frames.push_back(frame);

            if (exiting_process->get_pte_modified_bit(vpage_num) == 1 && exiting_process->is_file_mapped(vpage_num)) {
                printf(" FOUT\n");
                exiting_process->_proc_stat.fouts++;
                total_cost +=cost('f');
            }

        }
        exiting_process->reset_pte(vpage_num);
       if(myverbose) if(num_process != 4) printf("endNUM PROCESS lalalal CALLING %d\t\n\n\n", num_process);
    }
    /*
        On process exit (instruction), you have to traverse the active process’s page table starting from 0..63 and for each valid entry
UNMAP the page and FOUT modified filemapped pages. Note that dirty non-fmapped (anonymous) pages are not written
back (OUT) as the process exits. The used frame has to be returned to the free pool and made available to the get_frame()
function again. The frames then should be used again in the order they were released.
    */

}

 int page_fault_handler(int vpage_num, char operation){
    int frame_no = -1;
    if(myverbose) printf("Entering pagefault handeler \n");
    if(!active_process->is_valid_vma(vpage_num)){
        total_cost += cost('S');
        active_process->_proc_stat.segv++;
        printf(" SEGV\n");
    }
    else{
        if(myverbose) printf("Entering valid place of of PF handler\n");
        frame_t* new_frame = get_frame();
        frame_no = new_frame->frame_id;
        active_process->set_pte_frame_no(vpage_num, frame_no);
        if(myverbose) printf("Frame id --> %d\n", frame_no);
        //old one only needs to be paged out ?
        int old_pid = frame_table[frame_no].process;
        if(old_pid!=-1){
            int old_vpage = frame_table[frame_no].vpage;
            process_list[old_pid]->unset_pte_present(old_vpage);
            process_list[old_pid]->unset_pte_frame_no(old_vpage);
            if(O) printf(" UNMAP %d:%d\n", old_pid, old_vpage);
            total_cost += cost('U');
            process_list[old_pid]->_proc_stat.unmaps++;
            if(O){
                if(process_list[old_pid]->get_pte_modified_bit(old_vpage)){
                if(process_list[old_pid]->is_file_mapped(old_vpage)) {
                    total_cost += cost('f');
                    printf(" FOUT\n");
                    process_list[old_pid]->_proc_stat.fouts++;
                }
                else {
                    process_list[old_pid]->set_pte_paged_out(old_vpage);
                    if(myverbose) printf("setting the paged out bit for pid:%d vpage:%d to %d",
                    process_list[old_pid]->get_pid(),
                    old_vpage,
                    process_list[old_pid]->get_pte_paged_out_bit(old_vpage));
                    total_cost += cost('O');
                    process_list[old_pid]->_proc_stat.outs++;
                    printf(" OUT\n");
                }
                process_list[old_pid]->unset_pte_modified(old_vpage);
                }
            }
        }
        //update frame table with new pte details
        frame_table[frame_no].process = active_process->get_pid();
        frame_table[frame_no].vpage = vpage_num;
    }

    return frame_no;
}

bool is_valid(int vpage_num){
        return active_process->is_valid_vma(vpage_num);
}

void initialise_frames() {
    for (int i = 0; i < num_frames; i++) {
        frame_t *frame = &frame_table[i];
        frame->frame_id = i;
        unallocated_frames.push_back(frame);
    }
}

frame_t* allocate_frame_from_free_list(){
    if(unallocated_frames.empty()) return nullptr;
    frame_t *frame_to_be_allocated = unallocated_frames.front();
    unallocated_frames.pop_front();
    return frame_to_be_allocated;
}

frame_t *get_frame() {
    frame_t *frame = allocate_frame_from_free_list();
    if (frame == nullptr) frame = current_pager->select_victim_frame();
    return frame;
}



void print_page_table(){
    // add logic for printting
    // printf("\n#########page_Table############");
    Process* curr_process = nullptr;
    if(myverbose) printf("NUM PROCESS %d\n\n", num_process);
    for(int i = 0 ; i < process_list.size() ; ++i){
        curr_process = process_list[i];
        if(myverbose) if(curr_process == nullptr) printf("STOP CURR PROCESS IS NULLPTR  at page table %d\n", i);
        printf("PT[%d]:",i);
        for(int vpage_num = 0 ; vpage_num< MAX_VPAGES ; ++vpage_num){
            if(curr_process->is_pte_present(vpage_num)){
                printf(" %d:",vpage_num);
                if(curr_process->get_pte_referenced_bit(vpage_num)) printf("R");
                else printf("-");
                if(curr_process->get_pte_modified_bit(vpage_num)) printf("M");
                else printf("-");
                if(curr_process->get_pte_paged_out_bit(vpage_num)) printf("S");
                else printf("-");
            }
            else{
                if(curr_process->get_pte_paged_out_bit(vpage_num)) printf(" #");
                else printf(" *");
            }
        }
        printf("\n");
    }
    /*
        1. go throough each process
        2. for each page table entry 
            2.1. if present bit is set 
                2.1.1 RMS based on bit value
            2.2 if present bit is not set 
                2.2.1 '#' if paged out bit is set
                2.2.2 '*" if paged out bit is not set
    */
   
}
void print_frame_table(){
    // add logic for printting
    printf("FT:");
    for(int i = 0 ; i < num_frames ; ++i){
        frame_t curr_frame = frame_table[i];
        if(curr_frame.process!=-1) printf(" %d:%d", curr_frame.process, curr_frame.vpage);
        else printf(" *");
    }
    /*
        1.Go through each entry in frame table
        2. For each entry in frame table
            2.1 print the pid:virtual_page
            2.2 if pid=-1 , print '*'

    */
}
void print_statistics(){
    // add logic for printting
    // printf("\n#########Statistics############");
    for(int i = 0 ; i < process_list.size() ; ++i){
        Process* curr_process = process_list[i];
        printf("\nPROC[%d]: U=%lu M=%lu I=%lu O=%lu FI=%lu FO=%lu Z=%lu SV=%lu SP=%lu",
                curr_process->get_pid(),
                curr_process->_proc_stat.unmaps,
                curr_process->_proc_stat.maps,
                curr_process->_proc_stat.ins,
                curr_process->_proc_stat.outs,
                curr_process->_proc_stat.fins,
                curr_process->_proc_stat.fouts,
                curr_process->_proc_stat.zeroes,
                curr_process->_proc_stat.segv,
                curr_process->_proc_stat.segprot);
    }
    printf("\nTOTALCOST %llu %llu %llu %llu %lu\n",
            instruction_counter, context_switcher_counter, exit_counter, total_cost, sizeof(pte_t));
    /*
        Per process output:
            printf("PROC[%d]: U=%lu M=%lu I=%lu O=%lu FI=%lu FO=%lu Z=%lu SV=%lu SP=%lu\n",
            proc->pid,
            pstats->unmaps, pstats->maps, pstats->ins, pstats->outs,
            pstats->fins, pstats->fouts, pstats->zeros,
            pstats->segv, pstats->segprot);
        Summary output:
            printf("TOTALCOST %lu %lu %lu %llu %lu\n",
            inst_count, ctx_switches, process_exits, cost, sizeof(pte_t));

    */
}
vector<string> set_options(int argc, char* argv[])
{
    int c;
    while((c = getopt (argc, argv, "f:a:o:")) != -1){
        switch(c)
        {
        case 'f':
            num_frames = stoi(optarg);
            break;
        case 'a':
            initialise_pager(optarg);
            break;
        case 'o':
            break_options(optarg);
            break;
        default:
            break;
        }

    }
    vector<string> ipandrfile;
    for (int i = optind; i < argc; ++i) {
            ipandrfile.push_back(argv[i]);
        }
    return ipandrfile;

}

template <typename T>
void instantiate_pager()
{
    current_pager = new T();
}

void initialise_pager(char* algorithm_option){
     char pager_option = algorithm_option[0];
     switch (pager_option)
     {
     case 'f':
        instantiate_pager<FIFO_pager>();
        break;
    case 'r':
        instantiate_pager<Random_pager>();
        break;
    case 'c':
        instantiate_pager<Clock_pager>();
        break;
    case 'a':
        // printf("not yet implemented");
        instantiate_pager<Aging_pager>();
        break;
    case 'e':
        // printf("not yet implemented");
        instantiate_pager<NRU_pager>();
        break;
    case 'w':
        // printf("not yet implemented");
        instantiate_pager<Set_pager>();
        break;
     
    default:
        printf("\n\t######Wrong Input############\n\n");
        exit(1);
        break;
    }
}

void break_options(char* options){
    for(int i=0; i< strlen(options); ++i){
        char opt = options[i];
        switch (opt){
            case 'O':
                O = true;
                break;
            case 'P':
                P = true;
                break;
            case 'F':
                F = true;
                break;
            case 'S':
                S = true;
                break;
        }
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

int myrandom() { 
    //declare a global vector randvals, offset
    if(myverbose) cout << "I am heree "<< ofs << "\n";
    if(ofs == randvals.size()-1){ ofs = -1; }
    ofs++;
    if(myverbose)  printf("randvals[%d]=%d",ofs, randvals[ofs]);
    return (randvals[ofs] % num_frames); 
}


void read_input(ifstream& ipfile){
    if(myverbose) cout << "\n read input \n";
    string line;
    bool read_instr = false, read_process = false, read_vma = false;
    while (std::getline(ipfile, line)) {
        stringstream ss(line);
        if (starts_with_comment(line)) {
            continue;
        }
        else if(read_instr){
            char op;
            int num;
            ss >> op >> num ;
            instruction curr_inst = {op, num};
            instruction_list.push_back(curr_inst);
        }
        else if(read_process){
            read_vma =  true;
            read_process = false;
            int vma_size;
            ss >> vma_size;
            if(myverbose) cout<<"process counter -->"<<process_counter << " vma_size---> " << vma_size <<"\n";
            Process* curr_process = new Process(process_counter++, vma_size);
            process_list.push_back(curr_process);
            if(myverbose) cout << "process back " << process_list.back()->get_pid() << "\n";
            // delete curr_process;
        }
        else if(read_vma){
            //process.vmasize == vma_q.size() --> next process
            //if length of process queue is equal to process_id  + 1 then start reading instructions after vma is done
            int start_vpage, end_vpage;
            unsigned int write_protected, file_mapped;
            if(myverbose) cout << "\n$$$$$ get" << process_list.back()->get_vma_size() << " \n";
            if(myverbose) cout << "\n$$$$$ curr" << process_list.back()->curr_vma_Size() << " \n";
            //if(myverbose) cout << line ;
            if(process_list.back()->get_vma_size() > process_list.back()->curr_vma_Size()) {
                ss >> start_vpage >> end_vpage >> write_protected >> file_mapped;
                if(myverbose) cout << "\nvma details" << start_vpage << " " << end_vpage << " " << write_protected << " " << file_mapped << " \n";
                vma curr_vma = {start_vpage, end_vpage, write_protected, file_mapped};  
                process_list.back()->add_vma(curr_vma);
                for(int i=start_vpage; i<=end_vpage; ++i){
                    Process* update_process = process_list.back();
                    update_process->update_is_hole(i,false);
                }
            }
            if (process_list.back()->get_vma_size() == process_list.back()->curr_vma_Size()) {
                read_vma = false;
                if(process_list.size()==num_process) read_instr = true;
                else read_process = true;
            }
        }
        else{
                ss >> num_process;
                if(myverbose) printf("numofprocesses-->%d\n",num_process);
                read_process = true;
            }
            
        }  

        if(myverbose) cout<< "\nyoyoyoyoyoy" << process_list.size() << " " << process_counter;
} 

bool starts_with_comment(const std::string& line) {
    for (char ch : line) {
        if (!std::isspace(ch)) { // Ignore leading spaces
            return ch == '#';   // Return true if the first non-space character is '#'
        }
    }
    return false; // Line is empty or doesn't start with '#'
}

void print_input(){
    if(myverbose){
        cout<<O<<P<<F<<S;
    }
}


int cost(char op){

    /*

    COST

    Here's the cost of each operation presented in a tabular format:

        | **Operation**             | **Instruction Count** |
        |-------------------------  |-----------------------|
        | Read/Write (Load/Store) R | 1                     |
        | Context Switches        C | 130                   |
        | Process Exits           E | 1230                  |
        | Maps                    M | 350                   |
        | Unmaps                  U | 410                   |
        | Ins                     I | 3200                  |
        | Outs                    O | 2750                  |
        | Fins                    F | 2350                  |
        | Fouts                   f | 2800                  |
        | Zeros                   Z | 150                   |
        | Segv                    S | 440                   |
        | Segprot                 s | 410                   |
*/
    int add_cost = 0;
    switch(op){

        case 'R': 
            add_cost =  1;
            break;
        case 'C': 
            add_cost =  130;
            break;
        case 'E': 
            add_cost =  1230;
            break;
        case 'M': 
            add_cost =  350;
            break;
        case 'U': 
            add_cost =  410;
            break;
        case 'I': 
            add_cost =  3200;
            break;
        case 'O': 
            add_cost =  2750;
            break;
        case 'F': 
            add_cost =  2350;
            break;
        case 'f': 
            add_cost =  2800;
            break;
        case 'Z': 
            add_cost =  150;
            break;
        case 'S': 
            add_cost =  440;
            break;
        case 's': 
            add_cost =  410;
            break;
    }
        return add_cost;
}