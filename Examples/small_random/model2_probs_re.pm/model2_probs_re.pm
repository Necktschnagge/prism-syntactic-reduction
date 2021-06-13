//Generated by ErrorPro
dtmc
//Element consts
const int cfblk1_Output_1=0;
const int cfblk4_Output_2=1;
const int cfblk5_Output_3=2;
const int cfblk3_Output_4=3;
const int cfblk2_Output_5=4;
const int cfblk1_Update_6=5;
const int cfblk5_Update_7=6;
const int check_redandancy0=7;
const int cfblk1_Output_1_2=8;
const int cfblk1_Output_1_3=9;
const int compare0=10;
const int stop_system=11;
const int check_redandancy1=12;
const int cfblk4_Output_2_2=13;
const int cfblk4_Output_2_3=14;
const int compare1=15;
const int check_redandancy2=16;
const int cfblk3_Output_4_2=17;
const int cfblk3_Output_4_3=18;
const int compare2=19;
const int check_redandancy4=20;
const int cfblk1_Update_6_2=21;
const int cfblk1_Update_6_3=22;
const int compare4=23;
//Data values consts
const int v=0;
const int ok=1;
const int no=2;
const int error=3;
const int c=4;
//Control flow commands
module control_flow
	cf:[0..24] init cfblk1_Output_1;
	//Element cfblk1_Output_1, df inputs ['y_cfblk5_223889179', 'x_cfblk1_1_1227828511'], cf transitions ['check_redandancy0']
	[cfblk1_Output_1] cf=cfblk1_Output_1 -> 1.0:(cf'=check_redandancy0);
	//Element cfblk4_Output_2, df inputs ['y_cfblk1_67347607'], cf transitions ['check_redandancy1']
	[cfblk4_Output_2] cf=cfblk4_Output_2 -> 1.0:(cf'=check_redandancy1);
	//Element cfblk5_Output_3, df inputs ['y_cfblk1_67347607', 'x_cfblk5_1_2098733092'], cf transitions ['cfblk3_Output_4']
	[cfblk5_Output_3] cf=cfblk5_Output_3 -> 1.0:(cf'=cfblk3_Output_4);
	//Element cfblk3_Output_4, cf transitions ['check_redandancy2']
	[cfblk3_Output_4] cf=cfblk3_Output_4 -> 1.0:(cf'=check_redandancy2);
	//Element cfblk2_Output_5, df inputs ['y_cfblk3_145618393'], cf transitions ['cfblk1_Update_6']
	[cfblk2_Output_5] cf=cfblk2_Output_5 -> 1.0:(cf'=cfblk1_Update_6);
	//Element cfblk1_Update_6, df inputs ['y_cfblk5_223889179', 'x_cfblk1_1_1227828511'], cf transitions ['check_redandancy4']
	[cfblk1_Update_6] cf=cfblk1_Update_6 -> 1.0:(cf'=check_redandancy4);
	//Element cfblk5_Update_7, df inputs ['y_cfblk1_67347607', 'x_cfblk5_1_2098733092'], cf transitions ['cfblk1_Output_1']
	[cfblk5_Update_7] cf=cfblk5_Update_7 -> 1.0:(cf'=cfblk1_Output_1);
	//Element check_redandancy0, df inputs ['redundancy_model2_cfblk1_0'], cf transitions ['cfblk1_Output_1_2', 'cfblk4_Output_2']
	[check_redandancy0] (cf=check_redandancy0) & (redundancy_model2_cfblk1_0=no) -> (cf'=cfblk4_Output_2);	// <-- 
	[check_redandancy0] (cf=check_redandancy0) & ((redundancy_model2_cfblk1_0=v)|(redundancy_model2_cfblk1_0=c)) -> (cf'=cfblk1_Output_1_2);	// <-- 
	//Element cfblk1_Output_1_2, df inputs ['y_cfblk5_223889179', 'x_cfblk1_1_1227828511'], cf transitions ['compare0']
	[cfblk1_Output_1_2] cf=cfblk1_Output_1_2 -> 1.0:(cf'=compare0);
	//Element cfblk1_Output_1_3, df inputs ['y_cfblk5_223889179', 'x_cfblk1_1_1227828511'], cf transitions ['cfblk4_Output_2']
	[cfblk1_Output_1_3] cf=cfblk1_Output_1_3 -> 1.0:(cf'=cfblk4_Output_2);
	//Element compare0, df inputs ['redundancy_model2_cfblk1_0', 'y_cfblk1_67347607_2', 'y_cfblk1_67347607'], cf transitions ['cfblk1_Output_1_3', 'cfblk4_Output_2', 'stop_system']
	[compare0] (cf=compare0) & (redundancy_model2_cfblk1_0=c) & (y_cfblk1_67347607=ok) & (y_cfblk1_67347607_2=ok) -> (cf'=cfblk4_Output_2);	// <-- 
	[compare0] (cf=compare0) & (redundancy_model2_cfblk1_0=c) & ((y_cfblk1_67347607=error) | (y_cfblk1_67347607_2=error)) -> (cf'=stop_system);	// <-- 
	[compare0] (cf=compare0) & (redundancy_model2_cfblk1_0=v) & (y_cfblk1_67347607=ok) & (y_cfblk1_67347607_2=ok) -> (cf'=cfblk4_Output_2);	// <-- 
	[compare0] (cf=compare0) & (redundancy_model2_cfblk1_0=v) & ((y_cfblk1_67347607=error) | (y_cfblk1_67347607_2=error)) -> (cf'=cfblk1_Output_1_3);	// <-- 
	//Element stop_system, cf transitions ['cfblk1_Output_1']
	[stop_system] cf=stop_system -> 1.0:(cf'=cfblk1_Output_1);
	//Element check_redandancy1, df inputs ['redundancy_model2_cfblk4_1'], cf transitions ['cfblk4_Output_2_2', 'cfblk5_Output_3']
	[check_redandancy1] (cf=check_redandancy1) & (redundancy_model2_cfblk4_1=no) -> (cf'=cfblk5_Output_3);	// <-- 
	[check_redandancy1] (cf=check_redandancy1) & ((redundancy_model2_cfblk4_1=v)|(redundancy_model2_cfblk4_1=c)) -> (cf'=cfblk4_Output_2_2);	// <-- 
	//Element cfblk4_Output_2_2, df inputs ['y_cfblk1_67347607'], cf transitions ['compare1']
	[cfblk4_Output_2_2] cf=cfblk4_Output_2_2 -> 1.0:(cf'=compare1);
	//Element cfblk4_Output_2_3, df inputs ['y_cfblk1_67347607'], cf transitions ['cfblk5_Output_3']
	[cfblk4_Output_2_3] cf=cfblk4_Output_2_3 -> 1.0:(cf'=cfblk5_Output_3);
	//Element compare1, df inputs ['redundancy_model2_cfblk4_1', 'y_cfblk4_184753786_2', 'y_cfblk4_184753786'], cf transitions ['cfblk4_Output_2_3', 'cfblk5_Output_3', 'stop_system']
	[compare1] (cf=compare1) & (redundancy_model2_cfblk4_1=c) & (y_cfblk4_184753786=ok) & (y_cfblk4_184753786_2=ok) -> (cf'=cfblk5_Output_3);	// <-- 
	[compare1] (cf=compare1) & (redundancy_model2_cfblk4_1=c) & ((y_cfblk4_184753786=error) | (y_cfblk4_184753786_2=error)) -> (cf'=stop_system);	// <-- 
	[compare1] (cf=compare1) & (redundancy_model2_cfblk4_1=v) & (y_cfblk4_184753786=ok) & (y_cfblk4_184753786_2=ok) -> (cf'=cfblk5_Output_3);	// <-- 
	[compare1] (cf=compare1) & (redundancy_model2_cfblk4_1=v) & ((y_cfblk4_184753786=error) | (y_cfblk4_184753786_2=error)) -> (cf'=cfblk4_Output_2_3);	// <-- 
	//Element check_redandancy2, df inputs ['redundancy_model2_cfblk3_2'], cf transitions ['cfblk3_Output_4_2', 'cfblk2_Output_5']
	[check_redandancy2] (cf=check_redandancy2) & (redundancy_model2_cfblk3_2=no) -> (cf'=cfblk2_Output_5);	// <-- 
	[check_redandancy2] (cf=check_redandancy2) & ((redundancy_model2_cfblk3_2=v)|(redundancy_model2_cfblk3_2=c)) -> (cf'=cfblk3_Output_4_2);	// <-- 
	//Element cfblk3_Output_4_2, cf transitions ['compare2']
	[cfblk3_Output_4_2] cf=cfblk3_Output_4_2 -> 1.0:(cf'=compare2);
	//Element cfblk3_Output_4_3, cf transitions ['cfblk2_Output_5']
	[cfblk3_Output_4_3] cf=cfblk3_Output_4_3 -> 1.0:(cf'=cfblk2_Output_5);
	//Element compare2, df inputs ['redundancy_model2_cfblk3_2', 'y_cfblk3_145618393_2', 'y_cfblk3_145618393'], cf transitions ['cfblk3_Output_4_3', 'cfblk2_Output_5', 'stop_system']
	[compare2] (cf=compare2) & (redundancy_model2_cfblk3_2=c) & (y_cfblk3_145618393=ok) & (y_cfblk3_145618393_2=ok) -> (cf'=cfblk2_Output_5);	// <-- 
	[compare2] (cf=compare2) & (redundancy_model2_cfblk3_2=c) & ((y_cfblk3_145618393=error) | (y_cfblk3_145618393_2=error)) -> (cf'=stop_system);	// <-- 
	[compare2] (cf=compare2) & (redundancy_model2_cfblk3_2=v) & (y_cfblk3_145618393=ok) & (y_cfblk3_145618393_2=ok) -> (cf'=cfblk2_Output_5);	// <-- 
	[compare2] (cf=compare2) & (redundancy_model2_cfblk3_2=v) & ((y_cfblk3_145618393=error) | (y_cfblk3_145618393_2=error)) -> (cf'=cfblk3_Output_4_3);	// <-- 
	//Element check_redandancy4, df inputs ['redundancy_model2_cfblk1_0'], cf transitions ['cfblk1_Update_6_2', 'cfblk5_Update_7']
	[check_redandancy4] (cf=check_redandancy4) & (redundancy_model2_cfblk1_0=no) -> (cf'=cfblk5_Update_7);	// <-- 
	[check_redandancy4] (cf=check_redandancy4) & ((redundancy_model2_cfblk1_0=v)|(redundancy_model2_cfblk1_0=c)) -> (cf'=cfblk1_Update_6_2);	// <-- 
	//Element cfblk1_Update_6_2, df inputs ['y_cfblk5_223889179', 'x_cfblk1_1_1227828511'], cf transitions ['compare4']
	[cfblk1_Update_6_2] cf=cfblk1_Update_6_2 -> 1.0:(cf'=compare4);
	//Element cfblk1_Update_6_3, df inputs ['y_cfblk5_223889179', 'x_cfblk1_1_1227828511'], cf transitions ['cfblk5_Update_7']
	[cfblk1_Update_6_3] cf=cfblk1_Update_6_3 -> 1.0:(cf'=cfblk5_Update_7);
	//Element compare4, df inputs ['redundancy_model2_cfblk1_0', 'x_cfblk1_1_1227828511_2', 'x_cfblk1_1_1227828511'], cf transitions ['cfblk1_Update_6_3', 'cfblk5_Update_7', 'stop_system']
	[compare4] (cf=compare4) & (redundancy_model2_cfblk1_0=c) & (x_cfblk1_1_1227828511=ok) & (x_cfblk1_1_1227828511_2=ok) -> (cf'=cfblk5_Update_7);	// <-- 
	[compare4] (cf=compare4) & (redundancy_model2_cfblk1_0=c) & ((x_cfblk1_1_1227828511=error) | (x_cfblk1_1_1227828511_2=error)) -> (cf'=stop_system);	// <-- 
	[compare4] (cf=compare4) & (redundancy_model2_cfblk1_0=v) & (x_cfblk1_1_1227828511=ok) & (x_cfblk1_1_1227828511_2=ok) -> (cf'=cfblk5_Update_7);	// <-- 
	[compare4] (cf=compare4) & (redundancy_model2_cfblk1_0=v) & ((x_cfblk1_1_1227828511=error) | (x_cfblk1_1_1227828511_2=error)) -> (cf'=cfblk1_Update_6_3);	// <-- 
endmodule
//Error propagation commands
module error_propagation
	y_cfblk1_67347607 : [0 .. 4] init ok;
	y_cfblk3_145618393 : [0 .. 4] init ok;
	y_cfblk4_184753786 : [0 .. 4] init ok;
	y_cfblk5_223889179 : [0 .. 4] init ok;
	x_cfblk1_1_1227828511 : [0 .. 4] init ok;
	x_cfblk5_1_2098733092 : [0 .. 4] init ok;
	redundancy_model2_cfblk1_0 : [0 .. 4] init v;
	y_cfblk1_67347607_2 : [0 .. 4] init ok;
	redundancy_model2_cfblk4_1 : [0 .. 4] init no;
	y_cfblk4_184753786_2 : [0 .. 4] init ok;
	redundancy_model2_cfblk3_2 : [0 .. 4] init no;
	y_cfblk3_145618393_2 : [0 .. 4] init ok;
	x_cfblk1_1_1227828511_2 : [0 .. 4] init ok;
	//Element cfblk1_Output_1, df inputs ['y_cfblk5_223889179', 'x_cfblk1_1_1227828511'], df outputs ['y_cfblk1_67347607']
	[cfblk1_Output_1] (y_cfblk5_223889179=ok) & (x_cfblk1_1_1227828511=ok) -> 0.98:(y_cfblk1_67347607'=ok) + 0.02:(y_cfblk1_67347607'=error);	// <-- 
	[cfblk1_Output_1] (y_cfblk5_223889179!=ok) | (x_cfblk1_1_1227828511!=ok) -> (y_cfblk1_67347607'=error);	// <-- 
	//Element cfblk4_Output_2, df inputs ['y_cfblk1_67347607'], df outputs ['y_cfblk4_184753786']
	[cfblk4_Output_2] (y_cfblk1_67347607=ok) -> 0.974002239936:(y_cfblk4_184753786'=ok) + 0.025997760064:(y_cfblk4_184753786'=error);	// <-- 
	[cfblk4_Output_2] (y_cfblk1_67347607!=ok) -> (y_cfblk4_184753786'=error);	// <-- 
	//Element cfblk5_Output_3, df inputs ['y_cfblk1_67347607', 'x_cfblk5_1_2098733092'], df outputs ['y_cfblk5_223889179']
	[cfblk5_Output_3] (y_cfblk1_67347607=ok) & (x_cfblk5_1_2098733092=ok) -> 0.98:(y_cfblk5_223889179'=ok) + 0.02:(y_cfblk5_223889179'=error);	// <-- 
	[cfblk5_Output_3] (y_cfblk1_67347607!=ok) | (x_cfblk5_1_2098733092!=ok) -> (y_cfblk5_223889179'=error);	// <-- 
	//Element cfblk3_Output_4, df outputs ['y_cfblk3_145618393']
	[cfblk3_Output_4] (true) -> 1:(y_cfblk3_145618393'=ok);	// <-- 
	//Element cfblk1_Update_6, df inputs ['y_cfblk5_223889179', 'x_cfblk1_1_1227828511'], df outputs ['x_cfblk1_1_1227828511']
	[cfblk1_Update_6] (y_cfblk5_223889179=ok) & (x_cfblk1_1_1227828511=ok) -> 0.98:(x_cfblk1_1_1227828511'=ok) + 0.02:(x_cfblk1_1_1227828511'=error);	// <-- 
	[cfblk1_Update_6] (y_cfblk5_223889179!=ok) | (x_cfblk1_1_1227828511!=ok) -> (x_cfblk1_1_1227828511'=error);	// <-- 
	//Element cfblk5_Update_7, df inputs ['y_cfblk1_67347607', 'x_cfblk5_1_2098733092'], df outputs ['x_cfblk5_1_2098733092']
	[cfblk5_Update_7] (y_cfblk1_67347607=ok) & (x_cfblk5_1_2098733092=ok) -> 0.98:(x_cfblk5_1_2098733092'=ok) + 0.02:(x_cfblk5_1_2098733092'=error);	// <-- 
	[cfblk5_Update_7] (y_cfblk1_67347607!=ok) | (x_cfblk5_1_2098733092!=ok) -> (x_cfblk5_1_2098733092'=error);	// <-- 
	//Element cfblk1_Output_1_2, df inputs ['y_cfblk5_223889179', 'x_cfblk1_1_1227828511'], df outputs ['y_cfblk1_67347607_2']
	[cfblk1_Output_1_2] (y_cfblk5_223889179=ok) & (x_cfblk1_1_1227828511=ok) -> 0.98:(y_cfblk1_67347607_2'=ok) + 0.02:(y_cfblk1_67347607_2'=error);	// <-- 
	[cfblk1_Output_1_2] (y_cfblk5_223889179!=ok) | (x_cfblk1_1_1227828511!=ok) -> (y_cfblk1_67347607_2'=error);	// <-- 
	//Element cfblk1_Output_1_3, df inputs ['y_cfblk5_223889179', 'x_cfblk1_1_1227828511'], df outputs ['y_cfblk1_67347607']
	[cfblk1_Output_1_3] (y_cfblk5_223889179=ok) & (x_cfblk1_1_1227828511=ok) -> 0.98:(y_cfblk1_67347607'=ok) + 0.02:(y_cfblk1_67347607'=error);	// <-- 
	[cfblk1_Output_1_3] (y_cfblk5_223889179!=ok) | (x_cfblk1_1_1227828511!=ok) -> (y_cfblk1_67347607'=error);	// <-- 
	//Element cfblk4_Output_2_2, df inputs ['y_cfblk1_67347607'], df outputs ['y_cfblk4_184753786_2']
	[cfblk4_Output_2_2] (y_cfblk1_67347607=ok) -> 0.974002239936:(y_cfblk4_184753786_2'=ok) + 0.025997760064:(y_cfblk4_184753786_2'=error);	// <-- 
	[cfblk4_Output_2_2] (y_cfblk1_67347607!=ok) -> (y_cfblk4_184753786_2'=error);	// <-- 
	//Element cfblk4_Output_2_3, df inputs ['y_cfblk1_67347607'], df outputs ['y_cfblk4_184753786']
	[cfblk4_Output_2_3] (y_cfblk1_67347607=ok) -> 0.974002239936:(y_cfblk4_184753786'=ok) + 0.025997760064:(y_cfblk4_184753786'=error);	// <-- 
	[cfblk4_Output_2_3] (y_cfblk1_67347607!=ok) -> (y_cfblk4_184753786'=error);	// <-- 
	//Element cfblk3_Output_4_2, df outputs ['y_cfblk3_145618393_2']
	[cfblk3_Output_4_2] (true) -> 1:(y_cfblk3_145618393_2'=ok);	// <-- 
	//Element cfblk3_Output_4_3, df outputs ['y_cfblk3_145618393']
	[cfblk3_Output_4_3] (true) -> 1:(y_cfblk3_145618393'=ok);	// <-- 
	//Element cfblk1_Update_6_2, df inputs ['y_cfblk5_223889179', 'x_cfblk1_1_1227828511'], df outputs ['x_cfblk1_1_1227828511_2']
	[cfblk1_Update_6_2] (y_cfblk5_223889179=ok) & (x_cfblk1_1_1227828511=ok) -> 0.98:(x_cfblk1_1_1227828511_2'=ok) + 0.02:(x_cfblk1_1_1227828511_2'=error);	// <-- 
	[cfblk1_Update_6_2] (y_cfblk5_223889179!=ok) | (x_cfblk1_1_1227828511!=ok) -> (x_cfblk1_1_1227828511_2'=error);	// <-- 
	//Element cfblk1_Update_6_3, df inputs ['y_cfblk5_223889179', 'x_cfblk1_1_1227828511'], df outputs ['x_cfblk1_1_1227828511']
	[cfblk1_Update_6_3] (y_cfblk5_223889179=ok) & (x_cfblk1_1_1227828511=ok) -> 0.98:(x_cfblk1_1_1227828511'=ok) + 0.02:(x_cfblk1_1_1227828511'=error);	// <-- 
	[cfblk1_Update_6_3] (y_cfblk5_223889179!=ok) | (x_cfblk1_1_1227828511!=ok) -> (x_cfblk1_1_1227828511'=error);	// <-- 
endmodule

//Failure formulas
formula failure = x_cfblk5_1_2098733092 = error;

rewards "time" true : 1; endrewards
