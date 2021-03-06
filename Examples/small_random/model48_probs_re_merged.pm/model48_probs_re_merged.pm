//Generated by ErrorPro
dtmc
//Element consts
const int cfblk5_Output_1=0;
const int cfblk2_Output_2=1;
const int cfblk3_Output_3=2;
const int cfblk1_Output_4=3;
const int cfblk4_Output_5=4;
const int cfblk5_Update_6=5;
const int cfblk1_Update_7=6;
const int check_redandancy2=7;
const int cfblk1_Output_4_2=8;
const int cfblk1_Output_4_3=9;
const int compare2=10;
const int stop_system=11;
const int check_redandancy4=12;
const int cfblk1_Update_7_2=13;
const int cfblk1_Update_7_3=14;
const int compare4=15;
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
	cf:[0..16] init cfblk5_Output_1;
	//Element cfblk5_Output_1, df inputs ['x_cfblk5_1_3723443230', 'y_cfblk1_3554353837'], cf transitions ['cfblk2_Output_2']
	[cfblk5_Output_1] cf=cfblk5_Output_1 -> 1.0:(cf'=cfblk2_Output_2);
	//Element cfblk2_Output_2, df inputs ['y_cfblk5_3710895409'], cf transitions ['cfblk3_Output_3']
	[cfblk2_Output_2] cf=cfblk2_Output_2 -> 1.0:(cf'=cfblk3_Output_3);
	//Element cfblk3_Output_3, df inputs ['y_cfblk5_3710895409'], cf transitions ['cfblk1_Output_4']
	[cfblk3_Output_3] cf=cfblk3_Output_3 -> 1.0:(cf'=cfblk1_Output_4);
	//Element cfblk1_Output_4, df inputs ['x_cfblk1_1_2852538649', 'y_cfblk5_3710895409'], cf transitions ['check_redandancy2']
	[cfblk1_Output_4] cf=cfblk1_Output_4 -> 1.0:(cf'=check_redandancy2);
	//Element cfblk4_Output_5, df inputs ['y_cfblk1_3554353837'], cf transitions ['cfblk5_Update_6']
	[cfblk4_Output_5] cf=cfblk4_Output_5 -> 1.0:(cf'=cfblk5_Update_6);
	//Element cfblk5_Update_6, df inputs ['x_cfblk5_1_3723443230', 'y_cfblk1_3554353837'], cf transitions ['cfblk1_Update_7']
	[cfblk5_Update_6] cf=cfblk5_Update_6 -> 1.0:(cf'=cfblk1_Update_7);
	//Element cfblk1_Update_7, df inputs ['x_cfblk1_1_2852538649', 'y_cfblk5_3710895409'], cf transitions ['check_redandancy4']
	[cfblk1_Update_7] cf=cfblk1_Update_7 -> 1.0:(cf'=check_redandancy4);
	//Element check_redandancy2, df inputs ['redundancy_model48_cfblk1_2'], cf transitions ['cfblk1_Output_4_2', 'cfblk4_Output_5']
	[check_redandancy2] (cf=check_redandancy2) & (redundancy_model48_cfblk1_2=no) -> (cf'=cfblk4_Output_5);	// <-- 
	[check_redandancy2] (cf=check_redandancy2) & ((redundancy_model48_cfblk1_2=v)|(redundancy_model48_cfblk1_2=c)) -> (cf'=cfblk1_Output_4_2);	// <-- 
	//Element cfblk1_Output_4_2, df inputs ['x_cfblk1_1_2852538649', 'y_cfblk5_3710895409'], cf transitions ['compare2']
	[cfblk1_Output_4_2] cf=cfblk1_Output_4_2 -> 1.0:(cf'=compare2);
	//Element cfblk1_Output_4_3, df inputs ['x_cfblk1_1_2852538649', 'y_cfblk5_3710895409'], cf transitions ['cfblk4_Output_5']
	[cfblk1_Output_4_3] cf=cfblk1_Output_4_3 -> 1.0:(cf'=cfblk4_Output_5);
	//Element compare2, df inputs ['x_cfblk1_1_2852538649_2_y_cfblk1_3554353837_2', 'y_cfblk1_3554353837', 'redundancy_model48_cfblk1_2'], cf transitions ['cfblk1_Output_4_3', 'cfblk4_Output_5', 'stop_system']
	[compare2] (cf=compare2) & (redundancy_model48_cfblk1_2=c) & (y_cfblk1_3554353837=ok) & (x_cfblk1_1_2852538649_2_y_cfblk1_3554353837_2=prop_1) -> (cf'=cfblk4_Output_5);	// <-- 
	[compare2] (cf=compare2) & (redundancy_model48_cfblk1_2=c) & ((y_cfblk1_3554353837=error) | (x_cfblk1_1_2852538649_2_y_cfblk1_3554353837_2=prop_2)) -> (cf'=stop_system);	// <-- 
	[compare2] (cf=compare2) & (redundancy_model48_cfblk1_2=v) & (y_cfblk1_3554353837=ok) & (x_cfblk1_1_2852538649_2_y_cfblk1_3554353837_2=prop_1) -> (cf'=cfblk4_Output_5);	// <-- 
	[compare2] (cf=compare2) & (redundancy_model48_cfblk1_2=v) & ((y_cfblk1_3554353837=error) | (x_cfblk1_1_2852538649_2_y_cfblk1_3554353837_2=prop_2)) -> (cf'=cfblk1_Output_4_3);	// <-- 
	//Element stop_system, cf transitions ['cfblk5_Output_1']
	[stop_system] cf=stop_system -> 1.0:(cf'=cfblk5_Output_1);
	//Element check_redandancy4, df inputs ['redundancy_model48_cfblk1_2'], cf transitions ['cfblk1_Update_7_2', 'cfblk5_Output_1']
	[check_redandancy4] (cf=check_redandancy4) & (redundancy_model48_cfblk1_2=no) -> (cf'=cfblk5_Output_1);	// <-- 
	[check_redandancy4] (cf=check_redandancy4) & ((redundancy_model48_cfblk1_2=v)|(redundancy_model48_cfblk1_2=c)) -> (cf'=cfblk1_Update_7_2);	// <-- 
	//Element cfblk1_Update_7_2, df inputs ['x_cfblk1_1_2852538649', 'y_cfblk5_3710895409'], cf transitions ['compare4']
	[cfblk1_Update_7_2] cf=cfblk1_Update_7_2 -> 1.0:(cf'=compare4);
	//Element cfblk1_Update_7_3, df inputs ['x_cfblk1_1_2852538649', 'y_cfblk5_3710895409'], cf transitions ['cfblk5_Output_1']
	[cfblk1_Update_7_3] cf=cfblk1_Update_7_3 -> 1.0:(cf'=cfblk5_Output_1);
	//Element compare4, df inputs ['x_cfblk1_1_2852538649', 'x_cfblk1_1_2852538649_2_y_cfblk1_3554353837_2', 'redundancy_model48_cfblk1_2'], cf transitions ['cfblk1_Update_7_3', 'cfblk5_Output_1', 'stop_system']
	[compare4] (cf=compare4) & (redundancy_model48_cfblk1_2=c) & (x_cfblk1_1_2852538649=ok) & (x_cfblk1_1_2852538649_2_y_cfblk1_3554353837_2=prop_1) -> (cf'=cfblk5_Output_1);	// <-- 
	[compare4] (cf=compare4) & (redundancy_model48_cfblk1_2=c) & ((x_cfblk1_1_2852538649=error) | (x_cfblk1_1_2852538649_2_y_cfblk1_3554353837_2=prop_2)) -> (cf'=stop_system);	// <-- 
	[compare4] (cf=compare4) & (redundancy_model48_cfblk1_2=v) & (x_cfblk1_1_2852538649=ok) & (x_cfblk1_1_2852538649_2_y_cfblk1_3554353837_2=prop_1) -> (cf'=cfblk5_Output_1);	// <-- 
	[compare4] (cf=compare4) & (redundancy_model48_cfblk1_2=v) & ((x_cfblk1_1_2852538649=error) | (x_cfblk1_1_2852538649_2_y_cfblk1_3554353837_2=prop_2)) -> (cf'=cfblk1_Update_7_3);	// <-- 
endmodule
//Error propagation commands
module error_propagation
	y_cfblk1_3554353837 : [0 .. 6] init ok;
	y_cfblk5_3710895409 : [0 .. 6] init ok;
	x_cfblk5_1_3723443230 : [0 .. 6] init ok;
	x_cfblk1_1_2852538649 : [0 .. 6] init ok;
	redundancy_model48_cfblk1_2 : [0 .. 6] init v;
	x_cfblk1_1_2852538649_2_y_cfblk1_3554353837_2 : [0 .. 6] init prop_1;
	//Element cfblk5_Output_1, df inputs ['x_cfblk5_1_3723443230', 'y_cfblk1_3554353837'], df outputs ['y_cfblk5_3710895409']
	[cfblk5_Output_1] (y_cfblk1_3554353837=ok) & (x_cfblk5_1_3723443230=ok) -> 0.98:(y_cfblk5_3710895409'=ok) + 0.02:(y_cfblk5_3710895409'=error);	// <-- 
	[cfblk5_Output_1] (y_cfblk1_3554353837!=ok) | (x_cfblk5_1_3723443230!=ok) -> (y_cfblk5_3710895409'=error);	// <-- 
	//Element cfblk1_Output_4, df inputs ['x_cfblk1_1_2852538649', 'y_cfblk5_3710895409'], df outputs ['y_cfblk1_3554353837']
	[cfblk1_Output_4] (y_cfblk5_3710895409=ok) & (x_cfblk1_1_2852538649=ok) -> 0.98:(y_cfblk1_3554353837'=ok) + 0.02:(y_cfblk1_3554353837'=error);	// <-- 
	[cfblk1_Output_4] (y_cfblk5_3710895409!=ok) | (x_cfblk1_1_2852538649!=ok) -> (y_cfblk1_3554353837'=error);	// <-- 
	//Element cfblk5_Update_6, df inputs ['x_cfblk5_1_3723443230', 'y_cfblk1_3554353837'], df outputs ['x_cfblk5_1_3723443230']
	[cfblk5_Update_6] (y_cfblk1_3554353837=ok) & (x_cfblk5_1_3723443230=ok) -> 0.98:(x_cfblk5_1_3723443230'=ok) + 0.02:(x_cfblk5_1_3723443230'=error);	// <-- 
	[cfblk5_Update_6] (y_cfblk1_3554353837!=ok) | (x_cfblk5_1_3723443230!=ok) -> (x_cfblk5_1_3723443230'=error);	// <-- 
	//Element cfblk1_Update_7, df inputs ['x_cfblk1_1_2852538649', 'y_cfblk5_3710895409'], df outputs ['x_cfblk1_1_2852538649']
	[cfblk1_Update_7] (y_cfblk5_3710895409=ok) & (x_cfblk1_1_2852538649=ok) -> 0.98:(x_cfblk1_1_2852538649'=ok) + 0.02:(x_cfblk1_1_2852538649'=error);	// <-- 
	[cfblk1_Update_7] (y_cfblk5_3710895409!=ok) | (x_cfblk1_1_2852538649!=ok) -> (x_cfblk1_1_2852538649'=error);	// <-- 
	//Element cfblk1_Output_4_2, df inputs ['x_cfblk1_1_2852538649', 'y_cfblk5_3710895409'], df outputs ['x_cfblk1_1_2852538649_2_y_cfblk1_3554353837_2']
	[cfblk1_Output_4_2] (y_cfblk5_3710895409=ok) & (x_cfblk1_1_2852538649=ok) -> 0.98:(x_cfblk1_1_2852538649_2_y_cfblk1_3554353837_2'=prop_1) + 0.02:(x_cfblk1_1_2852538649_2_y_cfblk1_3554353837_2'=prop_2);	// <-- 
	[cfblk1_Output_4_2] (y_cfblk5_3710895409!=ok) | (x_cfblk1_1_2852538649!=ok) -> (x_cfblk1_1_2852538649_2_y_cfblk1_3554353837_2'=prop_2);	// <-- 
	//Element cfblk1_Output_4_3, df inputs ['x_cfblk1_1_2852538649', 'y_cfblk5_3710895409'], df outputs ['y_cfblk1_3554353837']
	[cfblk1_Output_4_3] (y_cfblk5_3710895409=ok) & (x_cfblk1_1_2852538649=ok) -> 0.98:(y_cfblk1_3554353837'=ok) + 0.02:(y_cfblk1_3554353837'=error);	// <-- 
	[cfblk1_Output_4_3] (y_cfblk5_3710895409!=ok) | (x_cfblk1_1_2852538649!=ok) -> (y_cfblk1_3554353837'=error);	// <-- 
	//Element cfblk1_Update_7_2, df inputs ['x_cfblk1_1_2852538649', 'y_cfblk5_3710895409'], df outputs ['x_cfblk1_1_2852538649_2_y_cfblk1_3554353837_2']
	[cfblk1_Update_7_2] (y_cfblk5_3710895409=ok) & (x_cfblk1_1_2852538649=ok) -> 0.98:(x_cfblk1_1_2852538649_2_y_cfblk1_3554353837_2'=prop_1) + 0.02:(x_cfblk1_1_2852538649_2_y_cfblk1_3554353837_2'=prop_2);	// <-- 
	[cfblk1_Update_7_2] (y_cfblk5_3710895409!=ok) | (x_cfblk1_1_2852538649!=ok) -> (x_cfblk1_1_2852538649_2_y_cfblk1_3554353837_2'=prop_2);	// <-- 
	//Element cfblk1_Update_7_3, df inputs ['x_cfblk1_1_2852538649', 'y_cfblk5_3710895409'], df outputs ['x_cfblk1_1_2852538649']
	[cfblk1_Update_7_3] (y_cfblk5_3710895409=ok) & (x_cfblk1_1_2852538649=ok) -> 0.98:(x_cfblk1_1_2852538649'=ok) + 0.02:(x_cfblk1_1_2852538649'=error);	// <-- 
	[cfblk1_Update_7_3] (y_cfblk5_3710895409!=ok) | (x_cfblk1_1_2852538649!=ok) -> (x_cfblk1_1_2852538649'=error);	// <-- 
endmodule

//Failure formulas
formula failure = x_cfblk1_1_2852538649 = error;

rewards "time" true : 1; endrewards
