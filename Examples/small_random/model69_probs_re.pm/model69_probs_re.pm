//Generated by ErrorPro
dtmc
//Element consts
const int cfblk2_Output_1=0;
const int cfblk4_Output_2=1;
const int cfblk3_Output_3=2;
const int cfblk5_Output_4=3;
const int cfblk1_Output_5=4;
const int check_redandancy0=5;
const int cfblk2_Output_1_2=6;
const int cfblk2_Output_1_3=7;
const int compare0=8;
const int stop_system=9;
const int check_redandancy1=10;
const int cfblk4_Output_2_2=11;
const int cfblk4_Output_2_3=12;
const int compare1=13;
const int check_redandancy2=14;
const int cfblk3_Output_3_2=15;
const int cfblk3_Output_3_3=16;
const int compare2=17;
//Data values consts
const int v=0;
const int ok=1;
const int no=2;
const int error=3;
const int c=4;
//Control flow commands
module control_flow
	cf:[0..18] init cfblk2_Output_1;
	//Element cfblk2_Output_1, cf transitions ['check_redandancy0']
	[cfblk2_Output_1] cf=cfblk2_Output_1 -> 1.0:(cf'=check_redandancy0);
	//Element cfblk4_Output_2, df inputs ['y_cfblk2_2866974067'], cf transitions ['check_redandancy1']
	[cfblk4_Output_2] cf=cfblk4_Output_2 -> 1.0:(cf'=check_redandancy1);
	//Element cfblk3_Output_3, cf transitions ['check_redandancy2']
	[cfblk3_Output_3] cf=cfblk3_Output_3 -> 1.0:(cf'=check_redandancy2);
	//Element cfblk5_Output_4, df inputs ['y_cfblk3_2906109460', 'y_cfblk4_2945244853'], cf transitions ['cfblk1_Output_5']
	[cfblk5_Output_4] cf=cfblk5_Output_4 -> 1.0:(cf'=cfblk1_Output_5);
	//Element cfblk1_Output_5, df inputs ['y_cfblk5_2984380246'], cf transitions ['cfblk2_Output_1']
	[cfblk1_Output_5] cf=cfblk1_Output_5 -> 1.0:(cf'=cfblk2_Output_1);
	//Element check_redandancy0, df inputs ['redundancy_model69_cfblk2_0'], cf transitions ['cfblk2_Output_1_2', 'cfblk4_Output_2']
	[check_redandancy0] (cf=check_redandancy0) & (redundancy_model69_cfblk2_0=no) -> (cf'=cfblk4_Output_2);	// <-- 
	[check_redandancy0] (cf=check_redandancy0) & ((redundancy_model69_cfblk2_0=v)|(redundancy_model69_cfblk2_0=c)) -> (cf'=cfblk2_Output_1_2);	// <-- 
	//Element cfblk2_Output_1_2, cf transitions ['compare0']
	[cfblk2_Output_1_2] cf=cfblk2_Output_1_2 -> 1.0:(cf'=compare0);
	//Element cfblk2_Output_1_3, cf transitions ['cfblk4_Output_2']
	[cfblk2_Output_1_3] cf=cfblk2_Output_1_3 -> 1.0:(cf'=cfblk4_Output_2);
	//Element compare0, df inputs ['redundancy_model69_cfblk2_0', 'y_cfblk2_2866974067_2', 'y_cfblk2_2866974067'], cf transitions ['cfblk2_Output_1_3', 'cfblk4_Output_2', 'stop_system']
	[compare0] (cf=compare0) & (redundancy_model69_cfblk2_0=c) & (y_cfblk2_2866974067=ok) & (y_cfblk2_2866974067_2=ok) -> (cf'=cfblk4_Output_2);	// <-- 
	[compare0] (cf=compare0) & (redundancy_model69_cfblk2_0=c) & ((y_cfblk2_2866974067=error) | (y_cfblk2_2866974067_2=error)) -> (cf'=stop_system);	// <-- 
	[compare0] (cf=compare0) & (redundancy_model69_cfblk2_0=v) & (y_cfblk2_2866974067=ok) & (y_cfblk2_2866974067_2=ok) -> (cf'=cfblk4_Output_2);	// <-- 
	[compare0] (cf=compare0) & (redundancy_model69_cfblk2_0=v) & ((y_cfblk2_2866974067=error) | (y_cfblk2_2866974067_2=error)) -> (cf'=cfblk2_Output_1_3);	// <-- 
	//Element stop_system, cf transitions ['cfblk2_Output_1']
	[stop_system] cf=stop_system -> 1.0:(cf'=cfblk2_Output_1);
	//Element check_redandancy1, df inputs ['redundancy_model69_cfblk4_1'], cf transitions ['cfblk4_Output_2_2', 'cfblk3_Output_3']
	[check_redandancy1] (cf=check_redandancy1) & (redundancy_model69_cfblk4_1=no) -> (cf'=cfblk3_Output_3);	// <-- 
	[check_redandancy1] (cf=check_redandancy1) & ((redundancy_model69_cfblk4_1=v)|(redundancy_model69_cfblk4_1=c)) -> (cf'=cfblk4_Output_2_2);	// <-- 
	//Element cfblk4_Output_2_2, df inputs ['y_cfblk2_2866974067'], cf transitions ['compare1']
	[cfblk4_Output_2_2] cf=cfblk4_Output_2_2 -> 1.0:(cf'=compare1);
	//Element cfblk4_Output_2_3, df inputs ['y_cfblk2_2866974067'], cf transitions ['cfblk3_Output_3']
	[cfblk4_Output_2_3] cf=cfblk4_Output_2_3 -> 1.0:(cf'=cfblk3_Output_3);
	//Element compare1, df inputs ['redundancy_model69_cfblk4_1', 'y_cfblk4_2945244853_2', 'y_cfblk4_2945244853'], cf transitions ['cfblk4_Output_2_3', 'cfblk3_Output_3', 'stop_system']
	[compare1] (cf=compare1) & (redundancy_model69_cfblk4_1=c) & (y_cfblk4_2945244853=ok) & (y_cfblk4_2945244853_2=ok) -> (cf'=cfblk3_Output_3);	// <-- 
	[compare1] (cf=compare1) & (redundancy_model69_cfblk4_1=c) & ((y_cfblk4_2945244853=error) | (y_cfblk4_2945244853_2=error)) -> (cf'=stop_system);	// <-- 
	[compare1] (cf=compare1) & (redundancy_model69_cfblk4_1=v) & (y_cfblk4_2945244853=ok) & (y_cfblk4_2945244853_2=ok) -> (cf'=cfblk3_Output_3);	// <-- 
	[compare1] (cf=compare1) & (redundancy_model69_cfblk4_1=v) & ((y_cfblk4_2945244853=error) | (y_cfblk4_2945244853_2=error)) -> (cf'=cfblk4_Output_2_3);	// <-- 
	//Element check_redandancy2, df inputs ['redundancy_model69_cfblk3_2'], cf transitions ['cfblk3_Output_3_2', 'cfblk5_Output_4']
	[check_redandancy2] (cf=check_redandancy2) & (redundancy_model69_cfblk3_2=no) -> (cf'=cfblk5_Output_4);	// <-- 
	[check_redandancy2] (cf=check_redandancy2) & ((redundancy_model69_cfblk3_2=v)|(redundancy_model69_cfblk3_2=c)) -> (cf'=cfblk3_Output_3_2);	// <-- 
	//Element cfblk3_Output_3_2, cf transitions ['compare2']
	[cfblk3_Output_3_2] cf=cfblk3_Output_3_2 -> 1.0:(cf'=compare2);
	//Element cfblk3_Output_3_3, cf transitions ['cfblk5_Output_4']
	[cfblk3_Output_3_3] cf=cfblk3_Output_3_3 -> 1.0:(cf'=cfblk5_Output_4);
	//Element compare2, df inputs ['redundancy_model69_cfblk3_2', 'y_cfblk3_2906109460_2', 'y_cfblk3_2906109460'], cf transitions ['cfblk3_Output_3_3', 'cfblk5_Output_4', 'stop_system']
	[compare2] (cf=compare2) & (redundancy_model69_cfblk3_2=c) & (y_cfblk3_2906109460=ok) & (y_cfblk3_2906109460_2=ok) -> (cf'=cfblk5_Output_4);	// <-- 
	[compare2] (cf=compare2) & (redundancy_model69_cfblk3_2=c) & ((y_cfblk3_2906109460=error) | (y_cfblk3_2906109460_2=error)) -> (cf'=stop_system);	// <-- 
	[compare2] (cf=compare2) & (redundancy_model69_cfblk3_2=v) & (y_cfblk3_2906109460=ok) & (y_cfblk3_2906109460_2=ok) -> (cf'=cfblk5_Output_4);	// <-- 
	[compare2] (cf=compare2) & (redundancy_model69_cfblk3_2=v) & ((y_cfblk3_2906109460=error) | (y_cfblk3_2906109460_2=error)) -> (cf'=cfblk3_Output_3_3);	// <-- 
endmodule
//Error propagation commands
module error_propagation
	y_cfblk2_2866974067 : [0 .. 4] init ok;
	y_cfblk3_2906109460 : [0 .. 4] init ok;
	y_cfblk4_2945244853 : [0 .. 4] init ok;
	y_cfblk5_2984380246 : [0 .. 4] init ok;
	redundancy_model69_cfblk2_0 : [0 .. 4] init no;
	y_cfblk2_2866974067_2 : [0 .. 4] init ok;
	redundancy_model69_cfblk4_1 : [0 .. 4] init no;
	y_cfblk4_2945244853_2 : [0 .. 4] init ok;
	redundancy_model69_cfblk3_2 : [0 .. 4] init c;
	y_cfblk3_2906109460_2 : [0 .. 4] init ok;
	//Element cfblk2_Output_1, df outputs ['y_cfblk2_2866974067']
	[cfblk2_Output_1] (true) -> 1:(y_cfblk2_2866974067'=ok);	// <-- 
	//Element cfblk4_Output_2, df inputs ['y_cfblk2_2866974067'], df outputs ['y_cfblk4_2945244853']
	[cfblk4_Output_2] (y_cfblk2_2866974067=ok) -> 0.974002239936:(y_cfblk4_2945244853'=ok) + 0.025997760064:(y_cfblk4_2945244853'=error);	// <-- 
	[cfblk4_Output_2] (y_cfblk2_2866974067!=ok) -> (y_cfblk4_2945244853'=error);	// <-- 
	//Element cfblk3_Output_3, df outputs ['y_cfblk3_2906109460']
	[cfblk3_Output_3] (true) -> 1:(y_cfblk3_2906109460'=ok);	// <-- 
	//Element cfblk5_Output_4, df inputs ['y_cfblk3_2906109460', 'y_cfblk4_2945244853'], df outputs ['y_cfblk5_2984380246']
	[cfblk5_Output_4] (y_cfblk3_2906109460=ok) & (y_cfblk4_2945244853=ok) -> 0.974002239936:(y_cfblk5_2984380246'=ok) + 0.025997760064:(y_cfblk5_2984380246'=error);	// <-- 
	[cfblk5_Output_4] (y_cfblk3_2906109460!=ok) | (y_cfblk4_2945244853!=ok) -> (y_cfblk5_2984380246'=error);	// <-- 
	//Element cfblk2_Output_1_2, df outputs ['y_cfblk2_2866974067_2']
	[cfblk2_Output_1_2] (true) -> 1:(y_cfblk2_2866974067_2'=ok);	// <-- 
	//Element cfblk2_Output_1_3, df outputs ['y_cfblk2_2866974067']
	[cfblk2_Output_1_3] (true) -> 1:(y_cfblk2_2866974067'=ok);	// <-- 
	//Element cfblk4_Output_2_2, df inputs ['y_cfblk2_2866974067'], df outputs ['y_cfblk4_2945244853_2']
	[cfblk4_Output_2_2] (y_cfblk2_2866974067=ok) -> 0.974002239936:(y_cfblk4_2945244853_2'=ok) + 0.025997760064:(y_cfblk4_2945244853_2'=error);	// <-- 
	[cfblk4_Output_2_2] (y_cfblk2_2866974067!=ok) -> (y_cfblk4_2945244853_2'=error);	// <-- 
	//Element cfblk4_Output_2_3, df inputs ['y_cfblk2_2866974067'], df outputs ['y_cfblk4_2945244853']
	[cfblk4_Output_2_3] (y_cfblk2_2866974067=ok) -> 0.974002239936:(y_cfblk4_2945244853'=ok) + 0.025997760064:(y_cfblk4_2945244853'=error);	// <-- 
	[cfblk4_Output_2_3] (y_cfblk2_2866974067!=ok) -> (y_cfblk4_2945244853'=error);	// <-- 
	//Element cfblk3_Output_3_2, df outputs ['y_cfblk3_2906109460_2']
	[cfblk3_Output_3_2] (true) -> 1:(y_cfblk3_2906109460_2'=ok);	// <-- 
	//Element cfblk3_Output_3_3, df outputs ['y_cfblk3_2906109460']
	[cfblk3_Output_3_3] (true) -> 1:(y_cfblk3_2906109460'=ok);	// <-- 
endmodule

//Failure formulas
formula failure = y_cfblk5_2984380246 = error;

rewards "time" true : 1; endrewards
