//Generated by ErrorPro
dtmc
//Element consts
const int cfblk2_Output_1=0;
const int cfblk1_Output_2=1;
const int cfblk3_Output_3=2;
const int cfblk4_Output_4=3;
const int cfblk5_Output_5=4;
const int cfblk4_Update_6=5;
const int cfblk5_Update_7=6;
const int check_redandancy0=7;
const int cfblk2_Output_1_2=8;
const int cfblk2_Output_1_3=9;
const int compare0=10;
const int stop_system=11;
const int check_redandancy2=12;
const int cfblk3_Output_3_2=13;
const int cfblk3_Output_3_3=14;
const int compare2=15;
const int check_redandancy3=16;
const int cfblk4_Output_4_2=17;
const int cfblk4_Output_4_3=18;
const int compare3=19;
const int check_redandancy4=20;
const int cfblk4_Update_6_2=21;
const int cfblk4_Update_6_3=22;
const int compare4=23;
//Data values consts
const int v=0;
const int prop_2=1;
const int prop_1=2;
const int ok=3;
const int no=4;
const int error=5;
const int c=6;
//Control flow commands
module control_flow
	cf:[0..24] init cfblk2_Output_1;
	//Element cfblk2_Output_1, cf transitions ['check_redandancy0']
	[cfblk2_Output_1] cf=cfblk2_Output_1 -> 1.0:(cf'=check_redandancy0);
	//Element cfblk1_Output_2, df inputs ['y_cfblk3_54286051_y_cfblk2_15150658_y_cfblk4_93421444'], cf transitions ['cfblk3_Output_3']
	[cfblk1_Output_2] cf=cfblk1_Output_2 -> 1.0:(cf'=cfblk3_Output_3);
	//Element cfblk3_Output_3, cf transitions ['check_redandancy2']
	[cfblk3_Output_3] cf=cfblk3_Output_3 -> 1.0:(cf'=check_redandancy2);
	//Element cfblk4_Output_4, df inputs ['x_cfblk4_1_310919350', 'y_cfblk5_132556837'], cf transitions ['check_redandancy3']
	[cfblk4_Output_4] cf=cfblk4_Output_4 -> 1.0:(cf'=check_redandancy3);
	//Element cfblk5_Output_5, df inputs ['x_cfblk5_1_1602387319', 'y_cfblk3_54286051_y_cfblk2_15150658_y_cfblk4_93421444'], cf transitions ['cfblk4_Update_6']
	[cfblk5_Output_5] cf=cfblk5_Output_5 -> 1.0:(cf'=cfblk4_Update_6);
	//Element cfblk4_Update_6, df inputs ['x_cfblk4_1_310919350', 'y_cfblk5_132556837'], cf transitions ['check_redandancy4']
	[cfblk4_Update_6] cf=cfblk4_Update_6 -> 1.0:(cf'=check_redandancy4);
	//Element cfblk5_Update_7, df inputs ['x_cfblk5_1_1602387319', 'y_cfblk3_54286051_y_cfblk2_15150658_y_cfblk4_93421444'], cf transitions ['cfblk2_Output_1']
	[cfblk5_Update_7] cf=cfblk5_Update_7 -> 1.0:(cf'=cfblk2_Output_1);
	//Element check_redandancy0, df inputs ['redundancy_model22_cfblk2_0'], cf transitions ['cfblk2_Output_1_2', 'cfblk1_Output_2']
	[check_redandancy0] (cf=check_redandancy0) & (redundancy_model22_cfblk2_0=no) -> (cf'=cfblk1_Output_2);	// <-- 
	[check_redandancy0] (cf=check_redandancy0) & ((redundancy_model22_cfblk2_0=v)|(redundancy_model22_cfblk2_0=c)) -> (cf'=cfblk2_Output_1_2);	// <-- 
	//Element cfblk2_Output_1_2, cf transitions ['compare0']
	[cfblk2_Output_1_2] cf=cfblk2_Output_1_2 -> 1.0:(cf'=compare0);
	//Element cfblk2_Output_1_3, cf transitions ['cfblk1_Output_2']
	[cfblk2_Output_1_3] cf=cfblk2_Output_1_3 -> 1.0:(cf'=cfblk1_Output_2);
	//Element compare0, df inputs ['redundancy_model22_cfblk2_0', 'y_cfblk3_54286051_y_cfblk2_15150658_y_cfblk4_93421444', 'x_cfblk4_1_310919350_2_y_cfblk4_93421444_2_y_cfblk3_54286051_2_y_cfblk2_15150658_2'], cf transitions ['cfblk2_Output_1_3', 'cfblk1_Output_2', 'stop_system']
	[compare0] (cf=compare0) & (redundancy_model22_cfblk2_0=c) & (y_cfblk3_54286051_y_cfblk2_15150658_y_cfblk4_93421444=prop_1) & (x_cfblk4_1_310919350_2_y_cfblk4_93421444_2_y_cfblk3_54286051_2_y_cfblk2_15150658_2=prop_1) -> (cf'=cfblk1_Output_2);	// <-- 
	[compare0] (cf=compare0) & (redundancy_model22_cfblk2_0=c) & ((y_cfblk3_54286051_y_cfblk2_15150658_y_cfblk4_93421444=prop_2) | (x_cfblk4_1_310919350_2_y_cfblk4_93421444_2_y_cfblk3_54286051_2_y_cfblk2_15150658_2=prop_2)) -> (cf'=stop_system);	// <-- 
	[compare0] (cf=compare0) & (redundancy_model22_cfblk2_0=v) & (y_cfblk3_54286051_y_cfblk2_15150658_y_cfblk4_93421444=prop_1) & (x_cfblk4_1_310919350_2_y_cfblk4_93421444_2_y_cfblk3_54286051_2_y_cfblk2_15150658_2=prop_1) -> (cf'=cfblk1_Output_2);	// <-- 
	[compare0] (cf=compare0) & (redundancy_model22_cfblk2_0=v) & ((y_cfblk3_54286051_y_cfblk2_15150658_y_cfblk4_93421444=prop_2) | (x_cfblk4_1_310919350_2_y_cfblk4_93421444_2_y_cfblk3_54286051_2_y_cfblk2_15150658_2=prop_2)) -> (cf'=cfblk2_Output_1_3);	// <-- 
	//Element stop_system, cf transitions ['cfblk2_Output_1']
	[stop_system] cf=stop_system -> 1.0:(cf'=cfblk2_Output_1);
	//Element check_redandancy2, df inputs ['redundancy_model22_cfblk3_2'], cf transitions ['cfblk3_Output_3_2', 'cfblk4_Output_4']
	[check_redandancy2] (cf=check_redandancy2) & (redundancy_model22_cfblk3_2=no) -> (cf'=cfblk4_Output_4);	// <-- 
	[check_redandancy2] (cf=check_redandancy2) & ((redundancy_model22_cfblk3_2=v)|(redundancy_model22_cfblk3_2=c)) -> (cf'=cfblk3_Output_3_2);	// <-- 
	//Element cfblk3_Output_3_2, cf transitions ['compare2']
	[cfblk3_Output_3_2] cf=cfblk3_Output_3_2 -> 1.0:(cf'=compare2);
	//Element cfblk3_Output_3_3, cf transitions ['cfblk4_Output_4']
	[cfblk3_Output_3_3] cf=cfblk3_Output_3_3 -> 1.0:(cf'=cfblk4_Output_4);
	//Element compare2, df inputs ['redundancy_model22_cfblk3_2', 'y_cfblk3_54286051_y_cfblk2_15150658_y_cfblk4_93421444', 'x_cfblk4_1_310919350_2_y_cfblk4_93421444_2_y_cfblk3_54286051_2_y_cfblk2_15150658_2'], cf transitions ['cfblk3_Output_3_3', 'cfblk4_Output_4', 'stop_system']
	[compare2] (cf=compare2) & (redundancy_model22_cfblk3_2=c) & (y_cfblk3_54286051_y_cfblk2_15150658_y_cfblk4_93421444=prop_1) & (x_cfblk4_1_310919350_2_y_cfblk4_93421444_2_y_cfblk3_54286051_2_y_cfblk2_15150658_2=prop_1) -> (cf'=cfblk4_Output_4);	// <-- 
	[compare2] (cf=compare2) & (redundancy_model22_cfblk3_2=c) & ((y_cfblk3_54286051_y_cfblk2_15150658_y_cfblk4_93421444=prop_2) | (x_cfblk4_1_310919350_2_y_cfblk4_93421444_2_y_cfblk3_54286051_2_y_cfblk2_15150658_2=prop_2)) -> (cf'=stop_system);	// <-- 
	[compare2] (cf=compare2) & (redundancy_model22_cfblk3_2=v) & (y_cfblk3_54286051_y_cfblk2_15150658_y_cfblk4_93421444=prop_1) & (x_cfblk4_1_310919350_2_y_cfblk4_93421444_2_y_cfblk3_54286051_2_y_cfblk2_15150658_2=prop_1) -> (cf'=cfblk4_Output_4);	// <-- 
	[compare2] (cf=compare2) & (redundancy_model22_cfblk3_2=v) & ((y_cfblk3_54286051_y_cfblk2_15150658_y_cfblk4_93421444=prop_2) | (x_cfblk4_1_310919350_2_y_cfblk4_93421444_2_y_cfblk3_54286051_2_y_cfblk2_15150658_2=prop_2)) -> (cf'=cfblk3_Output_3_3);	// <-- 
	//Element check_redandancy3, df inputs ['redundancy_model22_cfblk4_3'], cf transitions ['cfblk4_Output_4_2', 'cfblk5_Output_5']
	[check_redandancy3] (cf=check_redandancy3) & (redundancy_model22_cfblk4_3=no) -> (cf'=cfblk5_Output_5);	// <-- 
	[check_redandancy3] (cf=check_redandancy3) & ((redundancy_model22_cfblk4_3=v)|(redundancy_model22_cfblk4_3=c)) -> (cf'=cfblk4_Output_4_2);	// <-- 
	//Element cfblk4_Output_4_2, df inputs ['x_cfblk4_1_310919350', 'y_cfblk5_132556837'], cf transitions ['compare3']
	[cfblk4_Output_4_2] cf=cfblk4_Output_4_2 -> 1.0:(cf'=compare3);
	//Element cfblk4_Output_4_3, df inputs ['x_cfblk4_1_310919350', 'y_cfblk5_132556837'], cf transitions ['cfblk5_Output_5']
	[cfblk4_Output_4_3] cf=cfblk4_Output_4_3 -> 1.0:(cf'=cfblk5_Output_5);
	//Element compare3, df inputs ['redundancy_model22_cfblk4_3', 'y_cfblk3_54286051_y_cfblk2_15150658_y_cfblk4_93421444', 'x_cfblk4_1_310919350_2_y_cfblk4_93421444_2_y_cfblk3_54286051_2_y_cfblk2_15150658_2'], cf transitions ['cfblk4_Output_4_3', 'cfblk5_Output_5', 'stop_system']
	[compare3] (cf=compare3) & (redundancy_model22_cfblk4_3=c) & (y_cfblk3_54286051_y_cfblk2_15150658_y_cfblk4_93421444=prop_1) & (x_cfblk4_1_310919350_2_y_cfblk4_93421444_2_y_cfblk3_54286051_2_y_cfblk2_15150658_2=prop_1) -> (cf'=cfblk5_Output_5);	// <-- 
	[compare3] (cf=compare3) & (redundancy_model22_cfblk4_3=c) & ((y_cfblk3_54286051_y_cfblk2_15150658_y_cfblk4_93421444=prop_2) | (x_cfblk4_1_310919350_2_y_cfblk4_93421444_2_y_cfblk3_54286051_2_y_cfblk2_15150658_2=prop_2)) -> (cf'=stop_system);	// <-- 
	[compare3] (cf=compare3) & (redundancy_model22_cfblk4_3=v) & (y_cfblk3_54286051_y_cfblk2_15150658_y_cfblk4_93421444=prop_1) & (x_cfblk4_1_310919350_2_y_cfblk4_93421444_2_y_cfblk3_54286051_2_y_cfblk2_15150658_2=prop_1) -> (cf'=cfblk5_Output_5);	// <-- 
	[compare3] (cf=compare3) & (redundancy_model22_cfblk4_3=v) & ((y_cfblk3_54286051_y_cfblk2_15150658_y_cfblk4_93421444=prop_2) | (x_cfblk4_1_310919350_2_y_cfblk4_93421444_2_y_cfblk3_54286051_2_y_cfblk2_15150658_2=prop_2)) -> (cf'=cfblk4_Output_4_3);	// <-- 
	//Element check_redandancy4, df inputs ['redundancy_model22_cfblk4_3'], cf transitions ['cfblk4_Update_6_2', 'cfblk5_Update_7']
	[check_redandancy4] (cf=check_redandancy4) & (redundancy_model22_cfblk4_3=no) -> (cf'=cfblk5_Update_7);	// <-- 
	[check_redandancy4] (cf=check_redandancy4) & ((redundancy_model22_cfblk4_3=v)|(redundancy_model22_cfblk4_3=c)) -> (cf'=cfblk4_Update_6_2);	// <-- 
	//Element cfblk4_Update_6_2, df inputs ['x_cfblk4_1_310919350', 'y_cfblk5_132556837'], cf transitions ['compare4']
	[cfblk4_Update_6_2] cf=cfblk4_Update_6_2 -> 1.0:(cf'=compare4);
	//Element cfblk4_Update_6_3, df inputs ['x_cfblk4_1_310919350', 'y_cfblk5_132556837'], cf transitions ['cfblk5_Update_7']
	[cfblk4_Update_6_3] cf=cfblk4_Update_6_3 -> 1.0:(cf'=cfblk5_Update_7);
	//Element compare4, df inputs ['x_cfblk4_1_310919350', 'redundancy_model22_cfblk4_3', 'x_cfblk4_1_310919350_2_y_cfblk4_93421444_2_y_cfblk3_54286051_2_y_cfblk2_15150658_2'], cf transitions ['cfblk4_Update_6_3', 'cfblk5_Update_7', 'stop_system']
	[compare4] (cf=compare4) & (redundancy_model22_cfblk4_3=c) & (x_cfblk4_1_310919350=ok) & (x_cfblk4_1_310919350_2_y_cfblk4_93421444_2_y_cfblk3_54286051_2_y_cfblk2_15150658_2=prop_1) -> (cf'=cfblk5_Update_7);	// <-- 
	[compare4] (cf=compare4) & (redundancy_model22_cfblk4_3=c) & ((x_cfblk4_1_310919350=error) | (x_cfblk4_1_310919350_2_y_cfblk4_93421444_2_y_cfblk3_54286051_2_y_cfblk2_15150658_2=prop_2)) -> (cf'=stop_system);	// <-- 
	[compare4] (cf=compare4) & (redundancy_model22_cfblk4_3=v) & (x_cfblk4_1_310919350=ok) & (x_cfblk4_1_310919350_2_y_cfblk4_93421444_2_y_cfblk3_54286051_2_y_cfblk2_15150658_2=prop_1) -> (cf'=cfblk5_Update_7);	// <-- 
	[compare4] (cf=compare4) & (redundancy_model22_cfblk4_3=v) & ((x_cfblk4_1_310919350=error) | (x_cfblk4_1_310919350_2_y_cfblk4_93421444_2_y_cfblk3_54286051_2_y_cfblk2_15150658_2=prop_2)) -> (cf'=cfblk4_Update_6_3);	// <-- 
endmodule
//Error propagation commands
module error_propagation
	y_cfblk5_132556837 : [0 .. 6] init ok;
	x_cfblk4_1_310919350 : [0 .. 6] init ok;
	x_cfblk5_1_1602387319 : [0 .. 6] init ok;
	redundancy_model22_cfblk2_0 : [0 .. 6] init c;
	redundancy_model22_cfblk3_2 : [0 .. 6] init c;
	redundancy_model22_cfblk4_3 : [0 .. 6] init no;
	y_cfblk3_54286051_y_cfblk2_15150658_y_cfblk4_93421444 : [0 .. 6] init prop_1;
	x_cfblk4_1_310919350_2_y_cfblk4_93421444_2_y_cfblk3_54286051_2_y_cfblk2_15150658_2 : [0 .. 6] init prop_1;
	//Element cfblk2_Output_1, df outputs ['y_cfblk3_54286051_y_cfblk2_15150658_y_cfblk4_93421444']
	[cfblk2_Output_1] (true) -> 1:(y_cfblk3_54286051_y_cfblk2_15150658_y_cfblk4_93421444'=prop_1);	// <-- 
	//Element cfblk3_Output_3, df outputs ['y_cfblk3_54286051_y_cfblk2_15150658_y_cfblk4_93421444']
	[cfblk3_Output_3] (true) -> 1:(y_cfblk3_54286051_y_cfblk2_15150658_y_cfblk4_93421444'=prop_1);	// <-- 
	//Element cfblk4_Output_4, df inputs ['x_cfblk4_1_310919350', 'y_cfblk5_132556837'], df outputs ['y_cfblk3_54286051_y_cfblk2_15150658_y_cfblk4_93421444']
	[cfblk4_Output_4] (y_cfblk5_132556837=ok) & (x_cfblk4_1_310919350=ok) -> 0.98:(y_cfblk3_54286051_y_cfblk2_15150658_y_cfblk4_93421444'=prop_1) + 0.02:(y_cfblk3_54286051_y_cfblk2_15150658_y_cfblk4_93421444'=prop_2);	// <-- 
	[cfblk4_Output_4] (y_cfblk5_132556837!=ok) | (x_cfblk4_1_310919350!=ok) -> (y_cfblk3_54286051_y_cfblk2_15150658_y_cfblk4_93421444'=prop_2);	// <-- 
	//Element cfblk5_Output_5, df inputs ['x_cfblk5_1_1602387319', 'y_cfblk3_54286051_y_cfblk2_15150658_y_cfblk4_93421444'], df outputs ['y_cfblk5_132556837']
	[cfblk5_Output_5] (y_cfblk3_54286051_y_cfblk2_15150658_y_cfblk4_93421444=prop_1) & (x_cfblk5_1_1602387319=ok) -> 0.98:(y_cfblk5_132556837'=ok) + 0.02:(y_cfblk5_132556837'=error);	// <-- 
	[cfblk5_Output_5] (y_cfblk3_54286051_y_cfblk2_15150658_y_cfblk4_93421444!=prop_1) | (x_cfblk5_1_1602387319!=ok) -> (y_cfblk5_132556837'=error);	// <-- 
	//Element cfblk4_Update_6, df inputs ['x_cfblk4_1_310919350', 'y_cfblk5_132556837'], df outputs ['x_cfblk4_1_310919350']
	[cfblk4_Update_6] (y_cfblk5_132556837=ok) & (x_cfblk4_1_310919350=ok) -> 0.98:(x_cfblk4_1_310919350'=ok) + 0.02:(x_cfblk4_1_310919350'=error);	// <-- 
	[cfblk4_Update_6] (y_cfblk5_132556837!=ok) | (x_cfblk4_1_310919350!=ok) -> (x_cfblk4_1_310919350'=error);	// <-- 
	//Element cfblk5_Update_7, df inputs ['x_cfblk5_1_1602387319', 'y_cfblk3_54286051_y_cfblk2_15150658_y_cfblk4_93421444'], df outputs ['x_cfblk5_1_1602387319']
	[cfblk5_Update_7] (y_cfblk3_54286051_y_cfblk2_15150658_y_cfblk4_93421444=prop_1) & (x_cfblk5_1_1602387319=ok) -> 0.98:(x_cfblk5_1_1602387319'=ok) + 0.02:(x_cfblk5_1_1602387319'=error);	// <-- 
	[cfblk5_Update_7] (y_cfblk3_54286051_y_cfblk2_15150658_y_cfblk4_93421444!=prop_1) | (x_cfblk5_1_1602387319!=ok) -> (x_cfblk5_1_1602387319'=error);	// <-- 
	//Element cfblk2_Output_1_2, df outputs ['x_cfblk4_1_310919350_2_y_cfblk4_93421444_2_y_cfblk3_54286051_2_y_cfblk2_15150658_2']
	[cfblk2_Output_1_2] (true) -> 1:(x_cfblk4_1_310919350_2_y_cfblk4_93421444_2_y_cfblk3_54286051_2_y_cfblk2_15150658_2'=prop_1);	// <-- 
	//Element cfblk2_Output_1_3, df outputs ['y_cfblk3_54286051_y_cfblk2_15150658_y_cfblk4_93421444']
	[cfblk2_Output_1_3] (true) -> 1:(y_cfblk3_54286051_y_cfblk2_15150658_y_cfblk4_93421444'=prop_1);	// <-- 
	//Element cfblk3_Output_3_2, df outputs ['x_cfblk4_1_310919350_2_y_cfblk4_93421444_2_y_cfblk3_54286051_2_y_cfblk2_15150658_2']
	[cfblk3_Output_3_2] (true) -> 1:(x_cfblk4_1_310919350_2_y_cfblk4_93421444_2_y_cfblk3_54286051_2_y_cfblk2_15150658_2'=prop_1);	// <-- 
	//Element cfblk3_Output_3_3, df outputs ['y_cfblk3_54286051_y_cfblk2_15150658_y_cfblk4_93421444']
	[cfblk3_Output_3_3] (true) -> 1:(y_cfblk3_54286051_y_cfblk2_15150658_y_cfblk4_93421444'=prop_1);	// <-- 
	//Element cfblk4_Output_4_2, df inputs ['x_cfblk4_1_310919350', 'y_cfblk5_132556837'], df outputs ['x_cfblk4_1_310919350_2_y_cfblk4_93421444_2_y_cfblk3_54286051_2_y_cfblk2_15150658_2']
	[cfblk4_Output_4_2] (y_cfblk5_132556837=ok) & (x_cfblk4_1_310919350=ok) -> 0.98:(x_cfblk4_1_310919350_2_y_cfblk4_93421444_2_y_cfblk3_54286051_2_y_cfblk2_15150658_2'=prop_1) + 0.02:(x_cfblk4_1_310919350_2_y_cfblk4_93421444_2_y_cfblk3_54286051_2_y_cfblk2_15150658_2'=prop_2);	// <-- 
	[cfblk4_Output_4_2] (y_cfblk5_132556837!=ok) | (x_cfblk4_1_310919350!=ok) -> (x_cfblk4_1_310919350_2_y_cfblk4_93421444_2_y_cfblk3_54286051_2_y_cfblk2_15150658_2'=prop_2);	// <-- 
	//Element cfblk4_Output_4_3, df inputs ['x_cfblk4_1_310919350', 'y_cfblk5_132556837'], df outputs ['y_cfblk3_54286051_y_cfblk2_15150658_y_cfblk4_93421444']
	[cfblk4_Output_4_3] (y_cfblk5_132556837=ok) & (x_cfblk4_1_310919350=ok) -> 0.98:(y_cfblk3_54286051_y_cfblk2_15150658_y_cfblk4_93421444'=prop_1) + 0.02:(y_cfblk3_54286051_y_cfblk2_15150658_y_cfblk4_93421444'=prop_2);	// <-- 
	[cfblk4_Output_4_3] (y_cfblk5_132556837!=ok) | (x_cfblk4_1_310919350!=ok) -> (y_cfblk3_54286051_y_cfblk2_15150658_y_cfblk4_93421444'=prop_2);	// <-- 
	//Element cfblk4_Update_6_2, df inputs ['x_cfblk4_1_310919350', 'y_cfblk5_132556837'], df outputs ['x_cfblk4_1_310919350_2_y_cfblk4_93421444_2_y_cfblk3_54286051_2_y_cfblk2_15150658_2']
	[cfblk4_Update_6_2] (y_cfblk5_132556837=ok) & (x_cfblk4_1_310919350=ok) -> 0.98:(x_cfblk4_1_310919350_2_y_cfblk4_93421444_2_y_cfblk3_54286051_2_y_cfblk2_15150658_2'=prop_1) + 0.02:(x_cfblk4_1_310919350_2_y_cfblk4_93421444_2_y_cfblk3_54286051_2_y_cfblk2_15150658_2'=prop_2);	// <-- 
	[cfblk4_Update_6_2] (y_cfblk5_132556837!=ok) | (x_cfblk4_1_310919350!=ok) -> (x_cfblk4_1_310919350_2_y_cfblk4_93421444_2_y_cfblk3_54286051_2_y_cfblk2_15150658_2'=prop_2);	// <-- 
	//Element cfblk4_Update_6_3, df inputs ['x_cfblk4_1_310919350', 'y_cfblk5_132556837'], df outputs ['x_cfblk4_1_310919350']
	[cfblk4_Update_6_3] (y_cfblk5_132556837=ok) & (x_cfblk4_1_310919350=ok) -> 0.98:(x_cfblk4_1_310919350'=ok) + 0.02:(x_cfblk4_1_310919350'=error);	// <-- 
	[cfblk4_Update_6_3] (y_cfblk5_132556837!=ok) | (x_cfblk4_1_310919350!=ok) -> (x_cfblk4_1_310919350'=error);	// <-- 
endmodule

//Failure formulas
formula failure = x_cfblk5_1_1602387319 = error;

rewards "time" true : 1; endrewards
