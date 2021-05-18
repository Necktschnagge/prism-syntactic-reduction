//Generated by ErrorPro
dtmc
//Element consts
const int cfblk3_Output_1=0;
const int cfblk1_Output_2=1;
const int cfblk5_Output_3=2;
const int cfblk4_Output_4=3;
const int cfblk2_Output_5=4;
const int check_redandancy0=5;
const int cfblk3_Output_1_2=6;
const int cfblk3_Output_1_3=7;
const int compare0=8;
const int stop_system=9;
const int check_redandancy2=10;
const int cfblk4_Output_4_2=11;
const int cfblk4_Output_4_3=12;
const int compare2=13;
//Data values consts
const int v=0;
const int ok=1;
const int no=2;
const int error=3;
const int c=4;
//Control flow commands
module control_flow
	cf:[0..14] init cfblk3_Output_1;
	//Element cfblk3_Output_1, cf transitions ['check_redandancy0']
	[cfblk3_Output_1] cf=cfblk3_Output_1 -> 1.0:(cf'=check_redandancy0);
	//Element cfblk1_Output_2, df inputs ['y_cfblk3_3504100903'], cf transitions ['cfblk5_Output_3']
	[cfblk1_Output_2] cf=cfblk1_Output_2 -> 1.0:(cf'=cfblk5_Output_3);
	//Element cfblk5_Output_3, df inputs ['y_cfblk3_3504100903'], cf transitions ['cfblk4_Output_4']
	[cfblk5_Output_3] cf=cfblk5_Output_3 -> 1.0:(cf'=cfblk4_Output_4);
	//Element cfblk4_Output_4, df inputs ['y_cfblk5_3582371689'], cf transitions ['check_redandancy2']
	[cfblk4_Output_4] cf=cfblk4_Output_4 -> 1.0:(cf'=check_redandancy2);
	//Element cfblk2_Output_5, df inputs ['y_cfblk4_3543236296'], cf transitions ['cfblk3_Output_1']
	[cfblk2_Output_5] cf=cfblk2_Output_5 -> 1.0:(cf'=cfblk3_Output_1);
	//Element check_redandancy0, df inputs ['redundancy_model29_cfblk3_0'], cf transitions ['cfblk3_Output_1_2', 'cfblk1_Output_2']
	[check_redandancy0] (cf=check_redandancy0) & (redundancy_model29_cfblk3_0=no) -> (cf'=cfblk1_Output_2);	// <-- 
	[check_redandancy0] (cf=check_redandancy0) & ((redundancy_model29_cfblk3_0=v)|(redundancy_model29_cfblk3_0=c)) -> (cf'=cfblk3_Output_1_2);	// <-- 
	//Element cfblk3_Output_1_2, cf transitions ['compare0']
	[cfblk3_Output_1_2] cf=cfblk3_Output_1_2 -> 1.0:(cf'=compare0);
	//Element cfblk3_Output_1_3, cf transitions ['cfblk1_Output_2']
	[cfblk3_Output_1_3] cf=cfblk3_Output_1_3 -> 1.0:(cf'=cfblk1_Output_2);
	//Element compare0, df inputs ['redundancy_model29_cfblk3_0', 'y_cfblk3_3504100903_2', 'y_cfblk3_3504100903'], cf transitions ['cfblk3_Output_1_3', 'cfblk1_Output_2', 'stop_system']
	[compare0] (cf=compare0) & (redundancy_model29_cfblk3_0=c) & (y_cfblk3_3504100903=ok) & (y_cfblk3_3504100903_2=ok) -> (cf'=cfblk1_Output_2);	// <-- 
	[compare0] (cf=compare0) & (redundancy_model29_cfblk3_0=c) & ((y_cfblk3_3504100903=error) | (y_cfblk3_3504100903_2=error)) -> (cf'=stop_system);	// <-- 
	[compare0] (cf=compare0) & (redundancy_model29_cfblk3_0=v) & (y_cfblk3_3504100903=ok) & (y_cfblk3_3504100903_2=ok) -> (cf'=cfblk1_Output_2);	// <-- 
	[compare0] (cf=compare0) & (redundancy_model29_cfblk3_0=v) & ((y_cfblk3_3504100903=error) | (y_cfblk3_3504100903_2=error)) -> (cf'=cfblk3_Output_1_3);	// <-- 
	//Element stop_system, cf transitions ['cfblk3_Output_1']
	[stop_system] cf=stop_system -> 1.0:(cf'=cfblk3_Output_1);
	//Element check_redandancy2, df inputs ['redundancy_model29_cfblk4_2'], cf transitions ['cfblk4_Output_4_2', 'cfblk2_Output_5']
	[check_redandancy2] (cf=check_redandancy2) & (redundancy_model29_cfblk4_2=no) -> (cf'=cfblk2_Output_5);	// <-- 
	[check_redandancy2] (cf=check_redandancy2) & ((redundancy_model29_cfblk4_2=v)|(redundancy_model29_cfblk4_2=c)) -> (cf'=cfblk4_Output_4_2);	// <-- 
	//Element cfblk4_Output_4_2, df inputs ['y_cfblk5_3582371689'], cf transitions ['compare2']
	[cfblk4_Output_4_2] cf=cfblk4_Output_4_2 -> 1.0:(cf'=compare2);
	//Element cfblk4_Output_4_3, df inputs ['y_cfblk5_3582371689'], cf transitions ['cfblk2_Output_5']
	[cfblk4_Output_4_3] cf=cfblk4_Output_4_3 -> 1.0:(cf'=cfblk2_Output_5);
	//Element compare2, df inputs ['redundancy_model29_cfblk4_2', 'y_cfblk4_3543236296_2', 'y_cfblk4_3543236296'], cf transitions ['cfblk4_Output_4_3', 'cfblk2_Output_5', 'stop_system']
	[compare2] (cf=compare2) & (redundancy_model29_cfblk4_2=c) & (y_cfblk4_3543236296=ok) & (y_cfblk4_3543236296_2=ok) -> (cf'=cfblk2_Output_5);	// <-- 
	[compare2] (cf=compare2) & (redundancy_model29_cfblk4_2=c) & ((y_cfblk4_3543236296=error) | (y_cfblk4_3543236296_2=error)) -> (cf'=stop_system);	// <-- 
	[compare2] (cf=compare2) & (redundancy_model29_cfblk4_2=v) & (y_cfblk4_3543236296=ok) & (y_cfblk4_3543236296_2=ok) -> (cf'=cfblk2_Output_5);	// <-- 
	[compare2] (cf=compare2) & (redundancy_model29_cfblk4_2=v) & ((y_cfblk4_3543236296=error) | (y_cfblk4_3543236296_2=error)) -> (cf'=cfblk4_Output_4_3);	// <-- 
endmodule
//Error propagation commands
module error_propagation
	y_cfblk3_3504100903 : [0 .. 4] init ok;
	y_cfblk4_3543236296 : [0 .. 4] init ok;
	y_cfblk5_3582371689 : [0 .. 4] init ok;
	redundancy_model29_cfblk3_0 : [0 .. 4] init c;
	y_cfblk3_3504100903_2 : [0 .. 4] init ok;
	redundancy_model29_cfblk4_2 : [0 .. 4] init c;
	y_cfblk4_3543236296_2 : [0 .. 4] init ok;
	//Element cfblk3_Output_1, df outputs ['y_cfblk3_3504100903']
	[cfblk3_Output_1] (true) -> 1:(y_cfblk3_3504100903'=ok);	// <-- 
	//Element cfblk5_Output_3, df inputs ['y_cfblk3_3504100903'], df outputs ['y_cfblk5_3582371689']
	[cfblk5_Output_3] (y_cfblk3_3504100903=ok) -> 0.969003039904:(y_cfblk5_3582371689'=ok) + 0.030996960095999997:(y_cfblk5_3582371689'=error);	// <-- 
	[cfblk5_Output_3] (y_cfblk3_3504100903!=ok) -> (y_cfblk5_3582371689'=error);	// <-- 
	//Element cfblk4_Output_4, df inputs ['y_cfblk5_3582371689'], df outputs ['y_cfblk4_3543236296']
	[cfblk4_Output_4] (y_cfblk5_3582371689=ok) -> 0.974002239936:(y_cfblk4_3543236296'=ok) + 0.025997760064:(y_cfblk4_3543236296'=error);	// <-- 
	[cfblk4_Output_4] (y_cfblk5_3582371689!=ok) -> (y_cfblk4_3543236296'=error);	// <-- 
	//Element cfblk3_Output_1_2, df outputs ['y_cfblk3_3504100903_2']
	[cfblk3_Output_1_2] (true) -> 1:(y_cfblk3_3504100903_2'=ok);	// <-- 
	//Element cfblk3_Output_1_3, df outputs ['y_cfblk3_3504100903']
	[cfblk3_Output_1_3] (true) -> 1:(y_cfblk3_3504100903'=ok);	// <-- 
	//Element cfblk4_Output_4_2, df inputs ['y_cfblk5_3582371689'], df outputs ['y_cfblk4_3543236296_2']
	[cfblk4_Output_4_2] (y_cfblk5_3582371689=ok) -> 0.974002239936:(y_cfblk4_3543236296_2'=ok) + 0.025997760064:(y_cfblk4_3543236296_2'=error);	// <-- 
	[cfblk4_Output_4_2] (y_cfblk5_3582371689!=ok) -> (y_cfblk4_3543236296_2'=error);	// <-- 
	//Element cfblk4_Output_4_3, df inputs ['y_cfblk5_3582371689'], df outputs ['y_cfblk4_3543236296']
	[cfblk4_Output_4_3] (y_cfblk5_3582371689=ok) -> 0.974002239936:(y_cfblk4_3543236296'=ok) + 0.025997760064:(y_cfblk4_3543236296'=error);	// <-- 
	[cfblk4_Output_4_3] (y_cfblk5_3582371689!=ok) -> (y_cfblk4_3543236296'=error);	// <-- 
endmodule

//Failure formulas
formula failure = y_cfblk5_3582371689 = error;

rewards "time" true : 1; endrewards
