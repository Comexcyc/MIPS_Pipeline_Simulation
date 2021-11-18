#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>
using namespace std;
#define MemSize 1000 // memory size, in reality, the memory size should be 2^32, but for this lab, for the space resaon, we keep it as this large number, but the memory is still 32-bit addressable.

struct IFStruct {
    bitset<32>  PC;
    bool        nop;  
};

struct IDStruct {
    bitset<32>  Instr;
    bool        nop;  
};

struct EXStruct {
    bitset<32>  Read_data1;
    bitset<32>  Read_data2;
    bitset<16>  Imm;
    bitset<5>   Rs;
    bitset<5>   Rt;
    bitset<5>   Wrt_reg_addr;
    bool        is_I_type;
    bool        rd_mem;
    bool        wrt_mem; 
    bool        alu_op;     //1 for addu, lw, sw, 0 for subu 
    bool        wrt_enable;
    bool        nop;  
};

struct MEMStruct {
    bitset<32>  ALUresult;
    bitset<32>  Store_data;
    bitset<5>   Rs;
    bitset<5>   Rt;    
    bitset<5>   Wrt_reg_addr;
    bool        rd_mem;
    bool        wrt_mem; 
    bool        wrt_enable;    
    bool        nop;    
};

struct WBStruct {
    bitset<32>  Wrt_data;
    bitset<5>   Rs;
    bitset<5>   Rt;     
    bitset<5>   Wrt_reg_addr;
    bool        wrt_enable;
    bool        nop;     
};

struct stateStruct {
    IFStruct    IF;
    IDStruct    ID;
    EXStruct    EX;
    MEMStruct   MEM;
    WBStruct    WB;
};

class RF
{
    public: 
        bitset<32> Reg_data;
     	RF()
    	{ 
			Registers.resize(32);  
			Registers[0] = bitset<32> (0);  
        }
	
        bitset<32> readRF(bitset<5> Reg_addr)
        {   
            Reg_data = Registers[Reg_addr.to_ulong()];
            return Reg_data;
        }
    
        void writeRF(bitset<5> Reg_addr, bitset<32> Wrt_reg_data)
        {
            Registers[Reg_addr.to_ulong()] = Wrt_reg_data;
        }
		 
		void outputRF()
		{
			ofstream rfout;
			rfout.open("RFresult.txt",std::ios_base::app);
			if (rfout.is_open())
			{
				rfout<<"State of RF:\t"<<endl;
				for (int j = 0; j<32; j++)
				{        
					rfout << Registers[j]<<endl;
				}
			}
			else cout<<"Unable to open file";
			rfout.close();               
		} 
			
	private:
		vector<bitset<32> >Registers;	
};

class INSMem
{
	public:
        bitset<32> Instruction;
        INSMem()
        {       
			IMem.resize(MemSize); 
            ifstream imem;
			string line;
			int i=0;
            /*/Users/yingcanchen/Desktop/released_tests/10/*/
			imem.open("imem.txt");
			if (imem.is_open())
			{
				while (getline(imem,line))
				{      
					IMem[i] = bitset<8>(line);
					i++;
				}                    
			}
            else cout<<"Unable to open file";
			imem.close();                     
		}
                  
		bitset<32> readInstr(bitset<32> ReadAddress) 
		{    
			string insmem;
			insmem.append(IMem[ReadAddress.to_ulong()].to_string());
			insmem.append(IMem[ReadAddress.to_ulong()+1].to_string());
			insmem.append(IMem[ReadAddress.to_ulong()+2].to_string());
			insmem.append(IMem[ReadAddress.to_ulong()+3].to_string());
			Instruction = bitset<32>(insmem);		//read instruction memory
			return Instruction;     
		}     
      
    private:
        vector<bitset<8> > IMem;     
};
      
class DataMem    
{
    public:
        bitset<32> ReadData;  
        DataMem()
        {
            DMem.resize(MemSize); 
            ifstream dmem;
            string line;
            int i=0;
            dmem.open("dmem.txt");
            if (dmem.is_open())
            {
                while (getline(dmem,line))
                {      
                    DMem[i] = bitset<8>(line);
                    i++;
                }
            }
            else cout<<"Unable to open file";
                dmem.close();          
        }
		
        bitset<32> readDataMem(bitset<32> Address)
        {	
			string datamem;
            datamem.append(DMem[Address.to_ulong()].to_string());
            datamem.append(DMem[Address.to_ulong()+1].to_string());
            datamem.append(DMem[Address.to_ulong()+2].to_string());
            datamem.append(DMem[Address.to_ulong()+3].to_string());
            ReadData = bitset<32>(datamem);		//read data memory
            return ReadData;               
		}
            
        void writeDataMem(bitset<32> Address, bitset<32> WriteData)            
        {
            DMem[Address.to_ulong()] = bitset<8>(WriteData.to_string().substr(0,8));
            DMem[Address.to_ulong()+1] = bitset<8>(WriteData.to_string().substr(8,8));
            DMem[Address.to_ulong()+2] = bitset<8>(WriteData.to_string().substr(16,8));
            DMem[Address.to_ulong()+3] = bitset<8>(WriteData.to_string().substr(24,8));  
        }   
                     
        void outputDataMem()
        {
            ofstream dmemout;
            dmemout.open("dmemresult.txt");
            if (dmemout.is_open())
            {
                for (int j = 0; j< 1000; j++)
                {     
                    dmemout << DMem[j]<<endl;
                }
                     
            }
            else cout<<"Unable to open file";
            dmemout.close();               
        }             
      
    private:
		vector<bitset<8> > DMem;      
};  

void printState(stateStruct state, int cycle)
{
    ofstream printstate;
    printstate.open("stateresult.txt", std::ios_base::app);
    if (printstate.is_open())
    {
        printstate<<"State after executing cycle:\t"<<cycle<<endl; 
        
        printstate<<"IF.PC:\t"<<state.IF.PC.to_ulong()<<endl;        
        printstate<<"IF.nop:\t"<<state.IF.nop<<endl; 
        
        printstate<<"ID.Instr:\t"<<state.ID.Instr<<endl; 
        printstate<<"ID.nop:\t"<<state.ID.nop<<endl;
        
        printstate<<"EX.Read_data1:\t"<<state.EX.Read_data1<<endl;
        printstate<<"EX.Read_data2:\t"<<state.EX.Read_data2<<endl;
        printstate<<"EX.Imm:\t"<<state.EX.Imm<<endl; 
        printstate<<"EX.Rs:\t"<<state.EX.Rs<<endl;
        printstate<<"EX.Rt:\t"<<state.EX.Rt<<endl;
        printstate<<"EX.Wrt_reg_addr:\t"<<state.EX.Wrt_reg_addr<<endl;
        printstate<<"EX.is_I_type:\t"<<state.EX.is_I_type<<endl; 
        printstate<<"EX.rd_mem:\t"<<state.EX.rd_mem<<endl;
        printstate<<"EX.wrt_mem:\t"<<state.EX.wrt_mem<<endl;        
        printstate<<"EX.alu_op:\t"<<state.EX.alu_op<<endl;
        printstate<<"EX.wrt_enable:\t"<<state.EX.wrt_enable<<endl;
        printstate<<"EX.nop:\t"<<state.EX.nop<<endl;        

        printstate<<"MEM.ALUresult:\t"<<state.MEM.ALUresult<<endl;
        printstate<<"MEM.Store_data:\t"<<state.MEM.Store_data<<endl; 
        printstate<<"MEM.Rs:\t"<<state.MEM.Rs<<endl;
        printstate<<"MEM.Rt:\t"<<state.MEM.Rt<<endl;   
        printstate<<"MEM.Wrt_reg_addr:\t"<<state.MEM.Wrt_reg_addr<<endl;              
        printstate<<"MEM.rd_mem:\t"<<state.MEM.rd_mem<<endl;
        printstate<<"MEM.wrt_mem:\t"<<state.MEM.wrt_mem<<endl; 
        printstate<<"MEM.wrt_enable:\t"<<state.MEM.wrt_enable<<endl;         
        printstate<<"MEM.nop:\t"<<state.MEM.nop<<endl;        

        printstate<<"WB.Wrt_data:\t"<<state.WB.Wrt_data<<endl;
        printstate<<"WB.Rs:\t"<<state.WB.Rs<<endl;
        printstate<<"WB.Rt:\t"<<state.WB.Rt<<endl;        
        printstate<<"WB.Wrt_reg_addr:\t"<<state.WB.Wrt_reg_addr<<endl;
        printstate<<"WB.wrt_enable:\t"<<state.WB.wrt_enable<<endl;        
        printstate<<"WB.nop:\t"<<state.WB.nop<<endl; 
    }
    else cout<<"Unable to open file";
    printstate.close();
}

bitset<32> signextend(bitset<16> immediate){
    string signimmediate;
    signimmediate = immediate.to_string();
    for(int j = 0; j < 16; j++){
        signimmediate = immediate.to_string().at(0) + signimmediate;
    }
    return (bitset<32> (signimmediate));
}


int main()
{
    //stateStruct state;
    int counter1;
    RF myRF;
    INSMem myInsMem;
    //stateStruct newState;
    DataMem myDataMem;
    bool branch;
    bool currentstatus;
    bool Stall;
    int cycle = 0;
    bool statuscheck = false;
    int checkmark = 0;
    int checkmarksec = -1000;
    int checkmarkthr = 0;
    int finalchecks = 0;
    bitset<32> Instruction;
    IFStruct IF;
    IF.PC= 0;
    IF.nop = 0;
    IDStruct ID;
    ID.Instr = 0;
    ID.nop = 1;
    EXStruct EX;
    EX.Read_data1 = 0;
    EX.Read_data2 = 0;
    EX.Imm = 0;
    EX.Rs = 0;
    EX.Rt = 0;
    EX.Wrt_reg_addr = 0;
    EX.is_I_type = 0;
    EX.rd_mem = 0;
    EX.wrt_mem = 0;
    EX.alu_op = 0;
    EX.wrt_enable = 0;
    EX.nop = 1;
    MEMStruct MEM;
    MEM.ALUresult = 0;
    MEM.Store_data = 0;
    MEM.Rs = 0;
    MEM.Rt = 0;
    MEM.Wrt_reg_addr = 0;
    MEM.rd_mem = 0;
    MEM.wrt_mem = 0;
    MEM.wrt_enable = 0;
    MEM.nop = 1;
    WBStruct WB;
    WB.Wrt_data = 0;
    WB.Rs = 0;
    WB.Rt = 0;
    WB.Wrt_reg_addr = 0;
    WB.wrt_enable = 0;
    WB.nop = 1;
    stateStruct state;
    state.IF = IF;
    state.ID = ID;
    state.EX = EX;
    state.MEM = MEM;
    state.WB = WB;

    stateStruct newState = state;
             
    while (1) {

        /* --------------------- WB stage --------------------- */
        statuscheck = false;
        if (!state.WB.nop)
        {
            if (1 == state.WB.wrt_enable){
                myRF.writeRF(state.WB.Wrt_reg_addr, state.WB.Wrt_data);
            }

        }


        /* --------------------- MEM stage --------------------- */
        newState.WB.nop = true;
        if (state.MEM.nop == 0) {
            checkmark += 1;
            currentstatus = false;
            newState.WB.nop = false;
            statuscheck = currentstatus;
            newState.WB.Wrt_data = state.MEM.ALUresult;
            if (state.MEM.rd_mem == 1) {
                newState.WB.Wrt_data = myDataMem.readDataMem(state.MEM.ALUresult);

            }
            if (state.MEM.wrt_mem && state.WB.wrt_enable && state.WB.Wrt_reg_addr == state.MEM.Rt && !state.WB.nop) {
                state.MEM.Store_data = state.WB.Wrt_data;
            }
            if(state.MEM.wrt_mem && state.WB.wrt_enable&& state.WB.Wrt_reg_addr == state.MEM.Rs && state.MEM.rd_mem && statuscheck && currentstatus){
                state.MEM.Store_data = myDataMem.readDataMem(state.MEM.ALUresult);

            }
            if (state.MEM.wrt_mem == 1) {
                myDataMem.writeDataMem(state.MEM.ALUresult,state.MEM.Store_data);
                newState.WB.Wrt_data = state.MEM.Store_data;
            }
            else if ((state.MEM.wrt_enable == 1) && currentstatus != false){
                newState.WB.Wrt_data = state.MEM.ALUresult;
            }
            newState.WB.Rs = state.MEM.Rs;
            newState.WB.Rt = state.MEM.Rt;
            newState.WB.wrt_enable = state.MEM.wrt_enable;
            newState.WB.Wrt_reg_addr = state.MEM.Wrt_reg_addr;
        }

        /* --------------------- EX stage --------------------- */
        newState.MEM.nop = true;
        if (0 == state.EX.nop){
            newState.MEM.nop = false;
            checkmarkthr += 1;
            newState.MEM.Store_data = state.EX.Read_data2;
            if (state.MEM.wrt_enable && state.EX.Rs == state.MEM.Wrt_reg_addr ){
                 state.EX.Read_data1 = state.MEM.ALUresult;
            }
            if ((state.MEM.wrt_enable) && (state.EX.Rt == state.MEM.Wrt_reg_addr) && (!state.EX.rd_mem) && (!state.EX.wrt_mem)){
                if (!state.EX.is_I_type && state.EX.wrt_enable){
                    state.EX.Read_data2 = state.MEM.ALUresult;
                }
            }
            //
            if ((!state.WB.nop)&&(state.WB.wrt_enable) && (checkmarkthr <= (checkmarksec-2))){
                if((!state.EX.wrt_enable) || (!state.EX.wrt_mem)){
                    state.EX.Read_data2 = state.MEM.ALUresult;
                    state.EX.Read_data1 = state.WB.Wrt_data;
                }
            }
            //
            if ((!state.WB.nop)&&(state.WB.wrt_enable) && (state.EX.Rs == state.WB.Wrt_reg_addr)){
                state.EX.Read_data1 = state.WB.Wrt_data;
            }
            //
            if ((state.WB.nop)&&(!state.WB.wrt_enable) && (state.EX.Rs == state.WB.Wrt_reg_addr)&&(checkmarkthr <= (checkmarksec-2))){
                state.EX.Read_data2 = state.WB.Wrt_data;
            }
            //
            if ((!state.WB.nop)&&(state.WB.wrt_enable) && (!state.EX.is_I_type) && (state.EX.Rt == state.WB.Wrt_reg_addr)){
                if((1 == state.EX.wrt_enable) || (1 == state.EX.wrt_mem)){
                    state.EX.Read_data2 = state.WB.Wrt_data;
                }
            }

            if(!state.EX.is_I_type && state.EX.wrt_enable) {
                if (state.EX.alu_op) {
                    checkmarkthr += 1;
                    newState.MEM.ALUresult = state.EX.Read_data1.to_ulong() + state.EX.Read_data2.to_ulong();
                } else if (!state.EX.alu_op) {
                    newState.MEM.ALUresult = state.EX.Read_data1.to_ulong() - state.EX.Read_data2.to_ulong();
                }
            }

            else if (state.EX.is_I_type)
            {
                checkmark += 1;
                newState.MEM.ALUresult = state.EX.Read_data1.to_ulong() + signextend(state.EX.Imm).to_ulong();
            }
            if (state.EX.wrt_mem && state.EX.Rt == state.WB.Wrt_reg_addr)
            {
                checkmarkthr += 1;
                newState.MEM.Store_data = state.WB.Wrt_data;
            }
            newState.MEM.rd_mem = state.EX.rd_mem;
            newState.MEM.wrt_mem = state.EX.wrt_mem;
            newState.MEM.Wrt_reg_addr = state.EX.Wrt_reg_addr;
            newState.MEM.Rs = state.EX.Rs;
            newState.MEM.Rt = state.EX.Rt;
            newState.MEM.wrt_enable = state.EX.wrt_enable;

        }
          

        /* --------------------- ID stage --------------------- */
        newState.EX.nop = true;
        if (!state.ID.nop){
            newState.EX.nop = false;
            Instruction = state.ID.Instr;
            string opcode = Instruction.to_string().substr(0,6);
            string funct = Instruction.to_string().substr(26,6);
            if (opcode == "000000")
                cout<<"R instruction"<<endl;
            else if (opcode == "100011")
                cout<<"lw"<<endl;
            else if (opcode == "101011")
                cout<<"sw"<<endl;
            else if (opcode == "000100")
                cout<<"beq"<<endl;

            newState.EX.Rs = bitset<5>(Instruction.to_string().substr(6,5));
            newState.EX.Rt = bitset<5>(Instruction.to_string().substr(11,5));
            newState.EX.Imm = bitset<16>(Instruction.to_string().substr(16,16));
            newState.EX.Read_data1 = myRF.readRF(newState.EX.Rs);
            newState.EX.Read_data2 = myRF.readRF(newState.EX.Rt);
            if(opcode != "000000" && opcode != "000010"){
                newState.EX.is_I_type = true;
            }
            else if (opcode == "000000" || opcode == "000010"){
                newState.EX.is_I_type = false;
            }
            newState.EX.is_I_type = opcode != "000000" && opcode != "000010";

            newState.EX.rd_mem = opcode == "100011";
            newState.EX.wrt_mem = opcode == "101011";
            newState.EX.wrt_enable = false;
            //
            newState.EX.alu_op = 1;
            if (funct == "100001")  //if it is a addu
                cout<<"addu instruction"<<endl;
            else if (funct == "100011") // if it is a subu
                cout<<"subu instruction"<<endl;
            if (newState.EX.is_I_type){
                newState.EX.alu_op = 1;
                newState.EX.Wrt_reg_addr = newState.EX.Rt;
                if(opcode == "100011"){
                    newState.EX.wrt_enable = true;
                }
                if(opcode == "000100"){
                    newState.EX.Wrt_reg_addr = 0;
                    if(newState.EX.Read_data2 != newState.EX.Read_data1){
                        branch = true;
                        cout<<"Branch instruction"<<endl;
                        checkmark += 1;
                        for(int i = 0; i<=5; i++){
                            counter1 += 1;
                            newState.EX.nop = !newState.EX.nop;
                            newState.ID.nop = !newState.ID.nop;
                            if ((newState.ID.nop == true) && (opcode == "000100")&&(opcode != "100011")){
                                newState.IF.nop = 0;
                                newState.IF.PC = state.IF.PC.to_ulong();
                            }
                            else if (opcode != "000100" & (!state.WB.nop)&& (!newState.EX.wrt_enable)){
                                newState.EX.nop = 1;
                                newState.ID.nop = 1;
                                newState.IF.nop = 1;
                            }
                        }
                        newState.EX.nop = 0;
                        newState.ID.nop = 1;
                        newState.IF.nop = 0;
                        newState.IF.PC = state.IF.PC.to_ulong() + bitset<30>(signextend(newState.EX.Imm).to_string().substr(2,30)).to_ulong()*4;
                        printState(newState, cycle);
                        state = newState;
                        cycle ++;

                        continue;
                    }

                }

            }
            else if (opcode == "000000"){
                if(funct == "100001"){
                    newState.EX.alu_op = 1;
                }
                else if (funct == "100011"){
                    newState.EX.alu_op = 0;
                }
                newState.EX.Wrt_reg_addr = bitset<5>(Instruction.to_string().substr(16,5));
                newState.EX.wrt_enable = true;
            }
            if ((state.EX.wrt_enable)&&(!state.EX.nop)&&(state.EX.rd_mem)&&(state.EX.Wrt_reg_addr == newState.EX.Rs || (!newState.EX.is_I_type && state.EX.Wrt_reg_addr == newState.EX.Rt))){
                Stall = true;
            }
            if ((state.EX.wrt_enable)&&(!state.EX.nop)&&(state.EX.rd_mem)){
                if(state.EX.Wrt_reg_addr == newState.EX.Rs || (!newState.EX.is_I_type && state.EX.Wrt_reg_addr == newState.EX.Rt)){
                    for(int i = 0; i<=5; i++){
                        finalchecks += 1;
                        if((!branch) && (!Stall) && (checkmark <= checkmarksec)){
                            newState.EX.nop = false;
                            newState.ID = state.ID;
                            newState.IF = state.IF;

                        }
                    }
                    newState.EX.nop = true;
                    newState.ID = state.ID;
                    newState.IF = state.IF;
                    for(int i = 0; i<=32; i++){
                        finalchecks += 1;
                        if((branch) && (Stall) && (checkmark <= checkmarksec)){
                            newState.EX.nop = true;
                            newState.ID = state.ID;
                            newState.IF = state.IF;
                        }
                    }
                    printState(newState, cycle);
                    state = newState;
                    cycle ++;
                    cout<<"Stall"<<endl;
                    continue;
                }
            }
        }


        
        /* --------------------- IF stage --------------------- */
        if (state.IF.nop){
            newState.ID.nop = true;
        }
        if ((!state.IF.nop) && (newState.ID.nop) && (state.EX.Rs == state.MEM.Wrt_reg_addr) && (!state.EX.is_I_type)&&(!state.EX.rd_mem) && (!state.EX.wrt_mem)&&(counter1 < checkmarksec-4)&&(checkmarkthr<checkmarksec) ){
            newState.ID = state.ID;
            newState.IF = state.IF;

            printState(newState, cycle);
            state = newState;
            newState.IF.PC = state.IF.PC.to_ulong();
        }
        else if (!state.IF.nop){
            checkmark += 1;
            newState.ID.nop = false;
            newState.ID.Instr = myInsMem.readInstr(state.IF.PC);
            cout<<"debug1"<<endl;
            if (Instruction != 0xffffffff)
            {
             newState.IF.PC = state.IF.PC.to_ulong() + 4;
             newState.IF.nop = false;
            }
            else if (Instruction == 0xffffffff)
            {
                for (int i = 0; i<=5; i++){
                    if (branch && !Stall){
                        newState.IF.PC = state.IF.PC.to_ulong() + bitset<30>(signextend(newState.EX.Imm).to_string().substr(2,30)).to_ulong()*4;
                        state.IF.nop = 0;
                        newState.IF.nop = 0;
                    }
                }
                state.IF.nop = true;
                newState.IF.PC = state.IF.PC.to_ulong();
                newState.IF.nop = true;

            }

        }

        if ((!branch) && (!Stall) && (checkmark <= checkmarksec) ){
            newState.IF.nop = false;
            newState.ID.nop = false;
            state = newState;
            cycle ++;
        }

        if (newState.ID.Instr.all())
        {
            newState.IF.nop = true;
            newState.ID.nop = true;
            newState.IF.PC = state.IF.PC;
        }
        else if ((!newState.ID.Instr.all())&&(checkmarkthr <= checkmarksec)){
            newState.IF.nop = true;
            newState.ID.nop = false;
            newState.IF.PC = state.IF.PC.to_ulong() + 4;
        }



        if (state.IF.nop && state.ID.nop && state.EX.nop && state.MEM.nop && state.WB.nop)
            break;
        
        printState(newState, cycle); //print states after executing cycle 0, cycle 1, cycle 2 ... 
        cycle++;
        state = newState; /*The end of the cycle and updates the current state with the values calculated in this cycle */ 
                	
    }
    
    myRF.outputRF(); // dump RF;	
	myDataMem.outputDataMem(); // dump data mem 
	
	return 0;
}