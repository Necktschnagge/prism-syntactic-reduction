//Generated by ErrorPro
dtmc
//Element consts
const int cfblk1_Output_1=0;
const int cfblk2_Output_2=1;
const int cfblk4_Output_3=2;
const int cfblk5_Output_4=3;
const int cfblk3_Output_5=4;
const int cfblk1_Update_6=5;
const int cfblk2_Update_7=6;
const int check_redandancy0=7;
const int cfblk1_Output_1_2=8;
const int cfblk1_Output_1_3=9;
const int compare0=10;
const int stop_system=11;
const int check_redandancy1=12;
const int cfblk2_Output_2_2=13;
const int cfblk2_Output_2_3=14;
const int compare1=15;
const int check_redandancy2=16;
const int cfblk4_Output_3_2=17;
const int cfblk4_Output_3_3=18;
const int compare2=19;
const int check_redandancy3=20;
const int cfblk3_Output_5_2=21;
const int cfblk3_Output_5_3=22;
const int compare3=23;
const int check_redandancy4=24;
const int cfblk1_Update_6_2=25;
const int cfblk1_Update_6_3=26;
const int compare4=27;
const int check_redandancy5=28;
const int cfblk2_Update_7_2=29;
const int cfblk2_Update_7_3=30;
const int compare5=31;
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
	cf:[0..32] init cfblk1_Output_1;
	//Element cfblk1_Output_1, df inputs ['y_cfblk4_3988126096', 'x_cfblk1_1_407717404'], cf transitions ['check_redandancy0']
	[cfblk1_Output_1] cf=cfblk1_Output_1 -> 1.0:(cf'=check_redandancy0);
	//Element cfblk2_Output_2, df inputs ['y_cfblk3_3948990703', 'x_cfblk2_1_1699185373'], cf transitions ['check_redandancy1']
	[cfblk2_Output_2] cf=cfblk2_Output_2 -> 1.0:(cf'=check_redandancy1);
	//Element cfblk4_Output_3, df inputs ['y_cfblk5_4027261489_y_cfblk1_3870719917'], cf transitions ['check_redandancy2']
	[cfblk4_Output_3] cf=cfblk4_Output_3 -> 1.0:(cf'=check_redandancy2);
	//Element cfblk5_Output_4, df inputs ['y_cfblk4_3988126096', 'x_cfblk2_1_1699185373_2_x_cfblk1_1_407717404_2_y_cfblk1_3870719917_2_y_cfblk2_3909855310'], cf transitions ['cfblk3_Output_5']
	[cfblk5_Output_4] cf=cfblk5_Output_4 -> 1.0:(cf'=cfblk3_Output_5);
	//Element cfblk3_Output_5, df inputs ['x_cfblk2_1_1699185373_2_x_cfblk1_1_407717404_2_y_cfblk1_3870719917_2_y_cfblk2_3909855310', 'y_cfblk5_4027261489_y_cfblk1_3870719917'], cf transitions ['check_redandancy3']
	[cfblk3_Output_5] cf=cfblk3_Output_5 -> 1.0:(cf'=check_redandancy3);
	//Element cfblk1_Update_6, df inputs ['y_cfblk4_3988126096', 'x_cfblk1_1_407717404'], cf transitions ['check_redandancy4']
	[cfblk1_Update_6] cf=cfblk1_Update_6 -> 1.0:(cf'=check_redandancy4);
	//Element cfblk2_Update_7, df inputs ['y_cfblk3_3948990703', 'x_cfblk2_1_1699185373'], cf transitions ['check_redandancy5']
	[cfblk2_Update_7] cf=cfblk2_Update_7 -> 1.0:(cf'=check_redandancy5);
	//Element check_redandancy0, df inputs ['redundancy_model93_cfblk1_0'], cf transitions ['cfblk1_Output_1_2', 'cfblk2_Output_2']
	[check_redandancy0] (cf=check_redandancy0) & (redundancy_model93_cfblk1_0=no) -> (cf'=cfblk2_Output_2);	// <-- 
	[check_redandancy0] (cf=check_redandancy0) & ((redundancy_model93_cfblk1_0=v)|(redundancy_model93_cfblk1_0=c)) -> (cf'=cfblk1_Output_1_2);	// <-- 
	//Element cfblk1_Output_1_2, df inputs ['y_cfblk4_3988126096', 'x_cfblk1_1_407717404'], cf transitions ['compare0']
	[cfblk1_Output_1_2] cf=cfblk1_Output_1_2 -> 1.0:(cf'=compare0);
	//Element cfblk1_Output_1_3, df inputs ['y_cfblk4_3988126096', 'x_cfblk1_1_407717404'], cf transitions ['cfblk2_Output_2']
	[cfblk1_Output_1_3] cf=cfblk1_Output_1_3 -> 1.0:(cf'=cfblk2_Output_2);
	//Element compare0, df inputs ['redundancy_model93_cfblk1_0', 'y_cfblk5_4027261489_y_cfblk1_3870719917', 'x_cfblk2_1_1699185373_2_x_cfblk1_1_407717404_2_y_cfblk1_3870719917_2_y_cfblk2_3909855310'], cf transitions ['cfblk1_Output_1_3', 'cfblk2_Output_2', 'stop_system']
	[compare0] (cf=compare0) & (redundancy_model93_cfblk1_0=c) & (y_cfblk5_4027261489_y_cfblk1_3870719917=prop_1) & (x_cfblk2_1_1699185373_2_x_cfblk1_1_407717404_2_y_cfblk1_3870719917_2_y_cfblk2_3909855310=prop_1) -> (cf'=cfblk2_Output_2);	// <-- 
	[compare0] (cf=compare0) & (redundancy_model93_cfblk1_0=c) & ((y_cfblk5_4027261489_y_cfblk1_3870719917=prop_2) | (x_cfblk2_1_1699185373_2_x_cfblk1_1_407717404_2_y_cfblk1_3870719917_2_y_cfblk2_3909855310=prop_2)) -> (cf'=stop_system);	// <-- 
	[compare0] (cf=compare0) & (redundancy_model93_cfblk1_0=v) & (y_cfblk5_4027261489_y_cfblk1_3870719917=prop_1) & (x_cfblk2_1_1699185373_2_x_cfblk1_1_407717404_2_y_cfblk1_3870719917_2_y_cfblk2_3909855310=prop_1) -> (cf'=cfblk2_Output_2);	// <-- 
	[compare0] (cf=compare0) & (redundancy_model93_cfblk1_0=v) & ((y_cfblk5_4027261489_y_cfblk1_3870719917=prop_2) | (x_cfblk2_1_1699185373_2_x_cfblk1_1_407717404_2_y_cfblk1_3870719917_2_y_cfblk2_3909855310=prop_2)) -> (cf'=cfblk1_Output_1_3);	// <-- 
	//Element stop_system, cf transitions ['cfblk1_Output_1']
	[stop_system] cf=stop_system -> 1.0:(cf'=cfblk1_Output_1);
	//Element check_redandancy1, df inputs ['redundancy_model93_cfblk2_1'], cf transitions ['cfblk2_Output_2_2', 'cfblk4_Output_3']
	[check_redandancy1] (cf=check_redandancy1) & (redundancy_model93_cfblk2_1=no) -> (cf'=cfblk4_Output_3);	// <-- 
	[check_redandancy1] (cf=check_redandancy1) & ((redundancy_model93_cfblk2_1=v)|(redundancy_model93_cfblk2_1=c)) -> (cf'=cfblk2_Output_2_2);	// <-- 
	//Element cfblk2_Output_2_2, df inputs ['y_cfblk3_3948990703', 'x_cfblk2_1_1699185373'], cf transitions ['compare1']
	[cfblk2_Output_2_2] cf=cfblk2_Output_2_2 -> 1.0:(cf'=compare1);
	//Element cfblk2_Output_2_3, df inputs ['y_cfblk3_3948990703', 'x_cfblk2_1_1699185373'], cf transitions ['cfblk4_Output_3']
	[cfblk2_Output_2_3] cf=cfblk2_Output_2_3 -> 1.0:(cf'=cfblk4_Output_3);
	//Element compare1, df inputs ['redundancy_model93_cfblk2_1', 'y_cfblk3_3948990703_2_y_cfblk4_3988126096_2_y_cfblk2_3909855310_2', 'x_cfblk2_1_1699185373_2_x_cfblk1_1_407717404_2_y_cfblk1_3870719917_2_y_cfblk2_3909855310'], cf transitions ['cfblk2_Output_2_3', 'cfblk4_Output_3', 'stop_system']
	[compare1] (cf=compare1) & (redundancy_model93_cfblk2_1=c) & (x_cfblk2_1_1699185373_2_x_cfblk1_1_407717404_2_y_cfblk1_3870719917_2_y_cfblk2_3909855310=prop_1) & (y_cfblk3_3948990703_2_y_cfblk4_3988126096_2_y_cfblk2_3909855310_2=prop_1) -> (cf'=cfblk4_Output_3);	// <-- 
	[compare1] (cf=compare1) & (redundancy_model93_cfblk2_1=c) & ((x_cfblk2_1_1699185373_2_x_cfblk1_1_407717404_2_y_cfblk1_3870719917_2_y_cfblk2_3909855310=prop_2) | (y_cfblk3_3948990703_2_y_cfblk4_3988126096_2_y_cfblk2_3909855310_2=prop_2)) -> (cf'=stop_system);	// <-- 
	[compare1] (cf=compare1) & (redundancy_model93_cfblk2_1=v) & (x_cfblk2_1_1699185373_2_x_cfblk1_1_407717404_2_y_cfblk1_3870719917_2_y_cfblk2_3909855310=prop_1) & (y_cfblk3_3948990703_2_y_cfblk4_3988126096_2_y_cfblk2_3909855310_2=prop_1) -> (cf'=cfblk4_Output_3);	// <-- 
	[compare1] (cf=compare1) & (redundancy_model93_cfblk2_1=v) & ((x_cfblk2_1_1699185373_2_x_cfblk1_1_407717404_2_y_cfblk1_3870719917_2_y_cfblk2_3909855310=prop_2) | (y_cfblk3_3948990703_2_y_cfblk4_3988126096_2_y_cfblk2_3909855310_2=prop_2)) -> (cf'=cfblk2_Output_2_3);	// <-- 
	//Element check_redandancy2, df inputs ['redundancy_model93_cfblk4_2'], cf transitions ['cfblk4_Output_3_2', 'cfblk5_Output_4']
	[check_redandancy2] (cf=check_redandancy2) & (redundancy_model93_cfblk4_2=no) -> (cf'=cfblk5_Output_4);	// <-- 
	[check_redandancy2] (cf=check_redandancy2) & ((redundancy_model93_cfblk4_2=v)|(redundancy_model93_cfblk4_2=c)) -> (cf'=cfblk4_Output_3_2);	// <-- 
	//Element cfblk4_Output_3_2, df inputs ['y_cfblk5_4027261489_y_cfblk1_3870719917'], cf transitions ['compare2']
	[cfblk4_Output_3_2] cf=cfblk4_Output_3_2 -> 1.0:(cf'=compare2);
	//Element cfblk4_Output_3_3, df inputs ['y_cfblk5_4027261489_y_cfblk1_3870719917'], cf transitions ['cfblk5_Output_4']
	[cfblk4_Output_3_3] cf=cfblk4_Output_3_3 -> 1.0:(cf'=cfblk5_Output_4);
	//Element compare2, df inputs ['y_cfblk3_3948990703_2_y_cfblk4_3988126096_2_y_cfblk2_3909855310_2', 'redundancy_model93_cfblk4_2', 'y_cfblk4_3988126096'], cf transitions ['cfblk4_Output_3_3', 'cfblk5_Output_4', 'stop_system']
	[compare2] (cf=compare2) & (redundancy_model93_cfblk4_2=c) & (y_cfblk4_3988126096=ok) & (y_cfblk3_3948990703_2_y_cfblk4_3988126096_2_y_cfblk2_3909855310_2=prop_1) -> (cf'=cfblk5_Output_4);	// <-- 
	[compare2] (cf=compare2) & (redundancy_model93_cfblk4_2=c) & ((y_cfblk4_3988126096=error) | (y_cfblk3_3948990703_2_y_cfblk4_3988126096_2_y_cfblk2_3909855310_2=prop_2)) -> (cf'=stop_system);	// <-- 
	[compare2] (cf=compare2) & (redundancy_model93_cfblk4_2=v) & (y_cfblk4_3988126096=ok) & (y_cfblk3_3948990703_2_y_cfblk4_3988126096_2_y_cfblk2_3909855310_2=prop_1) -> (cf'=cfblk5_Output_4);	// <-- 
	[compare2] (cf=compare2) & (redundancy_model93_cfblk4_2=v) & ((y_cfblk4_3988126096=error) | (y_cfblk3_3948990703_2_y_cfblk4_3988126096_2_y_cfblk2_3909855310_2=prop_2)) -> (cf'=cfblk4_Output_3_3);	// <-- 
	//Element check_redandancy3, df inputs ['redundancy_model93_cfblk3_3'], cf transitions ['cfblk3_Output_5_2', 'cfblk1_Update_6']
	[check_redandancy3] (cf=check_redandancy3) & (redundancy_model93_cfblk3_3=no) -> (cf'=cfblk1_Update_6);	// <-- 
	[check_redandancy3] (cf=check_redandancy3) & ((redundancy_model93_cfblk3_3=v)|(redundancy_model93_cfblk3_3=c)) -> (cf'=cfblk3_Output_5_2);	// <-- 
	//Element cfblk3_Output_5_2, df inputs ['x_cfblk2_1_1699185373_2_x_cfblk1_1_407717404_2_y_cfblk1_3870719917_2_y_cfblk2_3909855310', 'y_cfblk5_4027261489_y_cfblk1_3870719917'], cf transitions ['compare3']
	[cfblk3_Output_5_2] cf=cfblk3_Output_5_2 -> 1.0:(cf'=compare3);
	//Element cfblk3_Output_5_3, df inputs ['x_cfblk2_1_1699185373_2_x_cfblk1_1_407717404_2_y_cfblk1_3870719917_2_y_cfblk2_3909855310', 'y_cfblk5_4027261489_y_cfblk1_3870719917'], cf transitions ['cfblk1_Update_6']
	[cfblk3_Output_5_3] cf=cfblk3_Output_5_3 -> 1.0:(cf'=cfblk1_Update_6);
	//Element compare3, df inputs ['y_cfblk3_3948990703_2_y_cfblk4_3988126096_2_y_cfblk2_3909855310_2', 'y_cfblk3_3948990703', 'redundancy_model93_cfblk3_3'], cf transitions ['cfblk3_Output_5_3', 'cfblk1_Update_6', 'stop_system']
	[compare3] (cf=compare3) & (redundancy_model93_cfblk3_3=c) & (y_cfblk3_3948990703=ok) & (y_cfblk3_3948990703_2_y_cfblk4_3988126096_2_y_cfblk2_3909855310_2=prop_1) -> (cf'=cfblk1_Update_6);	// <-- 
	[compare3] (cf=compare3) & (redundancy_model93_cfblk3_3=c) & ((y_cfblk3_3948990703=error) | (y_cfblk3_3948990703_2_y_cfblk4_3988126096_2_y_cfblk2_3909855310_2=prop_2)) -> (cf'=stop_system);	// <-- 
	[compare3] (cf=compare3) & (redundancy_model93_cfblk3_3=v) & (y_cfblk3_3948990703=ok) & (y_cfblk3_3948990703_2_y_cfblk4_3988126096_2_y_cfblk2_3909855310_2=prop_1) -> (cf'=cfblk1_Update_6);	// <-- 
	[compare3] (cf=compare3) & (redundancy_model93_cfblk3_3=v) & ((y_cfblk3_3948990703=error) | (y_cfblk3_3948990703_2_y_cfblk4_3988126096_2_y_cfblk2_3909855310_2=prop_2)) -> (cf'=cfblk3_Output_5_3);	// <-- 
	//Element check_redandancy4, df inputs ['redundancy_model93_cfblk1_0'], cf transitions ['cfblk1_Update_6_2', 'cfblk2_Update_7']
	[check_redandancy4] (cf=check_redandancy4) & (redundancy_model93_cfblk1_0=no) -> (cf'=cfblk2_Update_7);	// <-- 
	[check_redandancy4] (cf=check_redandancy4) & ((redundancy_model93_cfblk1_0=v)|(redundancy_model93_cfblk1_0=c)) -> (cf'=cfblk1_Update_6_2);	// <-- 
	//Element cfblk1_Update_6_2, df inputs ['y_cfblk4_3988126096', 'x_cfblk1_1_407717404'], cf transitions ['compare4']
	[cfblk1_Update_6_2] cf=cfblk1_Update_6_2 -> 1.0:(cf'=compare4);
	//Element cfblk1_Update_6_3, df inputs ['y_cfblk4_3988126096', 'x_cfblk1_1_407717404'], cf transitions ['cfblk2_Update_7']
	[cfblk1_Update_6_3] cf=cfblk1_Update_6_3 -> 1.0:(cf'=cfblk2_Update_7);
	//Element compare4, df inputs ['x_cfblk2_1_1699185373_2_x_cfblk1_1_407717404_2_y_cfblk1_3870719917_2_y_cfblk2_3909855310', 'redundancy_model93_cfblk1_0', 'x_cfblk1_1_407717404'], cf transitions ['cfblk1_Update_6_3', 'cfblk2_Update_7', 'stop_system']
	[compare4] (cf=compare4) & (redundancy_model93_cfblk1_0=c) & (x_cfblk1_1_407717404=ok) & (x_cfblk2_1_1699185373_2_x_cfblk1_1_407717404_2_y_cfblk1_3870719917_2_y_cfblk2_3909855310=prop_1) -> (cf'=cfblk2_Update_7);	// <-- 
	[compare4] (cf=compare4) & (redundancy_model93_cfblk1_0=c) & ((x_cfblk1_1_407717404=error) | (x_cfblk2_1_1699185373_2_x_cfblk1_1_407717404_2_y_cfblk1_3870719917_2_y_cfblk2_3909855310=prop_2)) -> (cf'=stop_system);	// <-- 
	[compare4] (cf=compare4) & (redundancy_model93_cfblk1_0=v) & (x_cfblk1_1_407717404=ok) & (x_cfblk2_1_1699185373_2_x_cfblk1_1_407717404_2_y_cfblk1_3870719917_2_y_cfblk2_3909855310=prop_1) -> (cf'=cfblk2_Update_7);	// <-- 
	[compare4] (cf=compare4) & (redundancy_model93_cfblk1_0=v) & ((x_cfblk1_1_407717404=error) | (x_cfblk2_1_1699185373_2_x_cfblk1_1_407717404_2_y_cfblk1_3870719917_2_y_cfblk2_3909855310=prop_2)) -> (cf'=cfblk1_Update_6_3);	// <-- 
	//Element check_redandancy5, df inputs ['redundancy_model93_cfblk2_1'], cf transitions ['cfblk2_Update_7_2', 'cfblk1_Output_1']
	[check_redandancy5] (cf=check_redandancy5) & (redundancy_model93_cfblk2_1=no) -> (cf'=cfblk1_Output_1);	// <-- 
	[check_redandancy5] (cf=check_redandancy5) & ((redundancy_model93_cfblk2_1=v)|(redundancy_model93_cfblk2_1=c)) -> (cf'=cfblk2_Update_7_2);	// <-- 
	//Element cfblk2_Update_7_2, df inputs ['y_cfblk3_3948990703', 'x_cfblk2_1_1699185373'], cf transitions ['compare5']
	[cfblk2_Update_7_2] cf=cfblk2_Update_7_2 -> 1.0:(cf'=compare5);
	//Element cfblk2_Update_7_3, df inputs ['y_cfblk3_3948990703', 'x_cfblk2_1_1699185373'], cf transitions ['cfblk1_Output_1']
	[cfblk2_Update_7_3] cf=cfblk2_Update_7_3 -> 1.0:(cf'=cfblk1_Output_1);
	//Element compare5, df inputs ['redundancy_model93_cfblk2_1', 'x_cfblk2_1_1699185373_2_x_cfblk1_1_407717404_2_y_cfblk1_3870719917_2_y_cfblk2_3909855310', 'x_cfblk2_1_1699185373'], cf transitions ['cfblk2_Update_7_3', 'cfblk1_Output_1', 'stop_system']
	[compare5] (cf=compare5) & (redundancy_model93_cfblk2_1=c) & (x_cfblk2_1_1699185373=ok) & (x_cfblk2_1_1699185373_2_x_cfblk1_1_407717404_2_y_cfblk1_3870719917_2_y_cfblk2_3909855310=prop_1) -> (cf'=cfblk1_Output_1);	// <-- 
	[compare5] (cf=compare5) & (redundancy_model93_cfblk2_1=c) & ((x_cfblk2_1_1699185373=error) | (x_cfblk2_1_1699185373_2_x_cfblk1_1_407717404_2_y_cfblk1_3870719917_2_y_cfblk2_3909855310=prop_2)) -> (cf'=stop_system);	// <-- 
	[compare5] (cf=compare5) & (redundancy_model93_cfblk2_1=v) & (x_cfblk2_1_1699185373=ok) & (x_cfblk2_1_1699185373_2_x_cfblk1_1_407717404_2_y_cfblk1_3870719917_2_y_cfblk2_3909855310=prop_1) -> (cf'=cfblk1_Output_1);	// <-- 
	[compare5] (cf=compare5) & (redundancy_model93_cfblk2_1=v) & ((x_cfblk2_1_1699185373=error) | (x_cfblk2_1_1699185373_2_x_cfblk1_1_407717404_2_y_cfblk1_3870719917_2_y_cfblk2_3909855310=prop_2)) -> (cf'=cfblk2_Update_7_3);	// <-- 
endmodule
//Error propagation commands
module error_propagation
	y_cfblk3_3948990703 : [0 .. 6] init ok;
	y_cfblk4_3988126096 : [0 .. 6] init ok;
	x_cfblk1_1_407717404 : [0 .. 6] init ok;
	x_cfblk2_1_1699185373 : [0 .. 6] init ok;
	redundancy_model93_cfblk1_0 : [0 .. 6] init c;
	redundancy_model93_cfblk2_1 : [0 .. 6] init no;
	redundancy_model93_cfblk4_2 : [0 .. 6] init c;
	redundancy_model93_cfblk3_3 : [0 .. 6] init c;
	y_cfblk5_4027261489_y_cfblk1_3870719917 : [0 .. 6] init prop_1;
	y_cfblk3_3948990703_2_y_cfblk4_3988126096_2_y_cfblk2_3909855310_2 : [0 .. 6] init prop_1;
	x_cfblk2_1_1699185373_2_x_cfblk1_1_407717404_2_y_cfblk1_3870719917_2_y_cfblk2_3909855310 : [0 .. 6] init prop_1;
	//Element cfblk1_Output_1, df inputs ['y_cfblk4_3988126096', 'x_cfblk1_1_407717404'], df outputs ['y_cfblk5_4027261489_y_cfblk1_3870719917']
	[cfblk1_Output_1] (y_cfblk4_3988126096=ok) & (x_cfblk1_1_407717404=ok) -> 0.98:(y_cfblk5_4027261489_y_cfblk1_3870719917'=prop_1) + 0.02:(y_cfblk5_4027261489_y_cfblk1_3870719917'=prop_2);	// <-- 
	[cfblk1_Output_1] (y_cfblk4_3988126096!=ok) | (x_cfblk1_1_407717404!=ok) -> (y_cfblk5_4027261489_y_cfblk1_3870719917'=prop_2);	// <-- 
	//Element cfblk2_Output_2, df inputs ['y_cfblk3_3948990703', 'x_cfblk2_1_1699185373'], df outputs ['x_cfblk2_1_1699185373_2_x_cfblk1_1_407717404_2_y_cfblk1_3870719917_2_y_cfblk2_3909855310']
	[cfblk2_Output_2] (y_cfblk3_3948990703=ok) & (x_cfblk2_1_1699185373=ok) -> 0.98:(x_cfblk2_1_1699185373_2_x_cfblk1_1_407717404_2_y_cfblk1_3870719917_2_y_cfblk2_3909855310'=prop_1) + 0.02:(x_cfblk2_1_1699185373_2_x_cfblk1_1_407717404_2_y_cfblk1_3870719917_2_y_cfblk2_3909855310'=prop_2);	// <-- 
	[cfblk2_Output_2] (y_cfblk3_3948990703!=ok) | (x_cfblk2_1_1699185373!=ok) -> (x_cfblk2_1_1699185373_2_x_cfblk1_1_407717404_2_y_cfblk1_3870719917_2_y_cfblk2_3909855310'=prop_2);	// <-- 
	//Element cfblk4_Output_3, df inputs ['y_cfblk5_4027261489_y_cfblk1_3870719917'], df outputs ['y_cfblk4_3988126096']
	[cfblk4_Output_3] (y_cfblk5_4027261489_y_cfblk1_3870719917=prop_1) -> 0.969003039904:(y_cfblk4_3988126096'=ok) + 0.030996960095999997:(y_cfblk4_3988126096'=error);	// <-- 
	[cfblk4_Output_3] (y_cfblk5_4027261489_y_cfblk1_3870719917!=prop_1) -> (y_cfblk4_3988126096'=error);	// <-- 
	//Element cfblk5_Output_4, df inputs ['y_cfblk4_3988126096', 'x_cfblk2_1_1699185373_2_x_cfblk1_1_407717404_2_y_cfblk1_3870719917_2_y_cfblk2_3909855310'], df outputs ['y_cfblk5_4027261489_y_cfblk1_3870719917']
	[cfblk5_Output_4] (x_cfblk2_1_1699185373_2_x_cfblk1_1_407717404_2_y_cfblk1_3870719917_2_y_cfblk2_3909855310=prop_1) & (y_cfblk4_3988126096=ok) -> 0.98:(y_cfblk5_4027261489_y_cfblk1_3870719917'=prop_1) + 0.02:(y_cfblk5_4027261489_y_cfblk1_3870719917'=prop_2);	// <-- 
	[cfblk5_Output_4] (x_cfblk2_1_1699185373_2_x_cfblk1_1_407717404_2_y_cfblk1_3870719917_2_y_cfblk2_3909855310!=prop_1) | (y_cfblk4_3988126096!=ok) -> (y_cfblk5_4027261489_y_cfblk1_3870719917'=prop_2);	// <-- 
	//Element cfblk3_Output_5, df inputs ['x_cfblk2_1_1699185373_2_x_cfblk1_1_407717404_2_y_cfblk1_3870719917_2_y_cfblk2_3909855310', 'y_cfblk5_4027261489_y_cfblk1_3870719917'], df outputs ['y_cfblk3_3948990703']
	[cfblk3_Output_5] (x_cfblk2_1_1699185373_2_x_cfblk1_1_407717404_2_y_cfblk1_3870719917_2_y_cfblk2_3909855310=prop_1) & (y_cfblk5_4027261489_y_cfblk1_3870719917=prop_1) -> 0.974002239936:(y_cfblk3_3948990703'=ok) + 0.025997760064:(y_cfblk3_3948990703'=error);	// <-- 
	[cfblk3_Output_5] (x_cfblk2_1_1699185373_2_x_cfblk1_1_407717404_2_y_cfblk1_3870719917_2_y_cfblk2_3909855310!=prop_1) | (y_cfblk5_4027261489_y_cfblk1_3870719917!=prop_1) -> (y_cfblk3_3948990703'=error);	// <-- 
	//Element cfblk1_Update_6, df inputs ['y_cfblk4_3988126096', 'x_cfblk1_1_407717404'], df outputs ['x_cfblk1_1_407717404']
	[cfblk1_Update_6] (y_cfblk4_3988126096=ok) & (x_cfblk1_1_407717404=ok) -> 0.98:(x_cfblk1_1_407717404'=ok) + 0.02:(x_cfblk1_1_407717404'=error);	// <-- 
	[cfblk1_Update_6] (y_cfblk4_3988126096!=ok) | (x_cfblk1_1_407717404!=ok) -> (x_cfblk1_1_407717404'=error);	// <-- 
	//Element cfblk2_Update_7, df inputs ['y_cfblk3_3948990703', 'x_cfblk2_1_1699185373'], df outputs ['x_cfblk2_1_1699185373']
	[cfblk2_Update_7] (y_cfblk3_3948990703=ok) & (x_cfblk2_1_1699185373=ok) -> 0.98:(x_cfblk2_1_1699185373'=ok) + 0.02:(x_cfblk2_1_1699185373'=error);	// <-- 
	[cfblk2_Update_7] (y_cfblk3_3948990703!=ok) | (x_cfblk2_1_1699185373!=ok) -> (x_cfblk2_1_1699185373'=error);	// <-- 
	//Element cfblk1_Output_1_2, df inputs ['y_cfblk4_3988126096', 'x_cfblk1_1_407717404'], df outputs ['x_cfblk2_1_1699185373_2_x_cfblk1_1_407717404_2_y_cfblk1_3870719917_2_y_cfblk2_3909855310']
	[cfblk1_Output_1_2] (y_cfblk4_3988126096=ok) & (x_cfblk1_1_407717404=ok) -> 0.98:(x_cfblk2_1_1699185373_2_x_cfblk1_1_407717404_2_y_cfblk1_3870719917_2_y_cfblk2_3909855310'=prop_1) + 0.02:(x_cfblk2_1_1699185373_2_x_cfblk1_1_407717404_2_y_cfblk1_3870719917_2_y_cfblk2_3909855310'=prop_2);	// <-- 
	[cfblk1_Output_1_2] (y_cfblk4_3988126096!=ok) | (x_cfblk1_1_407717404!=ok) -> (x_cfblk2_1_1699185373_2_x_cfblk1_1_407717404_2_y_cfblk1_3870719917_2_y_cfblk2_3909855310'=prop_2);	// <-- 
	//Element cfblk1_Output_1_3, df inputs ['y_cfblk4_3988126096', 'x_cfblk1_1_407717404'], df outputs ['y_cfblk5_4027261489_y_cfblk1_3870719917']
	[cfblk1_Output_1_3] (y_cfblk4_3988126096=ok) & (x_cfblk1_1_407717404=ok) -> 0.98:(y_cfblk5_4027261489_y_cfblk1_3870719917'=prop_1) + 0.02:(y_cfblk5_4027261489_y_cfblk1_3870719917'=prop_2);	// <-- 
	[cfblk1_Output_1_3] (y_cfblk4_3988126096!=ok) | (x_cfblk1_1_407717404!=ok) -> (y_cfblk5_4027261489_y_cfblk1_3870719917'=prop_2);	// <-- 
	//Element cfblk2_Output_2_2, df inputs ['y_cfblk3_3948990703', 'x_cfblk2_1_1699185373'], df outputs ['y_cfblk3_3948990703_2_y_cfblk4_3988126096_2_y_cfblk2_3909855310_2']
	[cfblk2_Output_2_2] (y_cfblk3_3948990703=ok) & (x_cfblk2_1_1699185373=ok) -> 0.98:(y_cfblk3_3948990703_2_y_cfblk4_3988126096_2_y_cfblk2_3909855310_2'=prop_1) + 0.02:(y_cfblk3_3948990703_2_y_cfblk4_3988126096_2_y_cfblk2_3909855310_2'=prop_2);	// <-- 
	[cfblk2_Output_2_2] (y_cfblk3_3948990703!=ok) | (x_cfblk2_1_1699185373!=ok) -> (y_cfblk3_3948990703_2_y_cfblk4_3988126096_2_y_cfblk2_3909855310_2'=prop_2);	// <-- 
	//Element cfblk2_Output_2_3, df inputs ['y_cfblk3_3948990703', 'x_cfblk2_1_1699185373'], df outputs ['x_cfblk2_1_1699185373_2_x_cfblk1_1_407717404_2_y_cfblk1_3870719917_2_y_cfblk2_3909855310']
	[cfblk2_Output_2_3] (y_cfblk3_3948990703=ok) & (x_cfblk2_1_1699185373=ok) -> 0.98:(x_cfblk2_1_1699185373_2_x_cfblk1_1_407717404_2_y_cfblk1_3870719917_2_y_cfblk2_3909855310'=prop_1) + 0.02:(x_cfblk2_1_1699185373_2_x_cfblk1_1_407717404_2_y_cfblk1_3870719917_2_y_cfblk2_3909855310'=prop_2);	// <-- 
	[cfblk2_Output_2_3] (y_cfblk3_3948990703!=ok) | (x_cfblk2_1_1699185373!=ok) -> (x_cfblk2_1_1699185373_2_x_cfblk1_1_407717404_2_y_cfblk1_3870719917_2_y_cfblk2_3909855310'=prop_2);	// <-- 
	//Element cfblk4_Output_3_2, df inputs ['y_cfblk5_4027261489_y_cfblk1_3870719917'], df outputs ['y_cfblk3_3948990703_2_y_cfblk4_3988126096_2_y_cfblk2_3909855310_2']
	[cfblk4_Output_3_2] (y_cfblk5_4027261489_y_cfblk1_3870719917=prop_1) -> 0.969003039904:(y_cfblk3_3948990703_2_y_cfblk4_3988126096_2_y_cfblk2_3909855310_2'=prop_1) + 0.030996960095999997:(y_cfblk3_3948990703_2_y_cfblk4_3988126096_2_y_cfblk2_3909855310_2'=prop_2);	// <-- 
	[cfblk4_Output_3_2] (y_cfblk5_4027261489_y_cfblk1_3870719917!=prop_1) -> (y_cfblk3_3948990703_2_y_cfblk4_3988126096_2_y_cfblk2_3909855310_2'=prop_2);	// <-- 
	//Element cfblk4_Output_3_3, df inputs ['y_cfblk5_4027261489_y_cfblk1_3870719917'], df outputs ['y_cfblk4_3988126096']
	[cfblk4_Output_3_3] (y_cfblk5_4027261489_y_cfblk1_3870719917=prop_1) -> 0.969003039904:(y_cfblk4_3988126096'=ok) + 0.030996960095999997:(y_cfblk4_3988126096'=error);	// <-- 
	[cfblk4_Output_3_3] (y_cfblk5_4027261489_y_cfblk1_3870719917!=prop_1) -> (y_cfblk4_3988126096'=error);	// <-- 
	//Element cfblk3_Output_5_2, df inputs ['x_cfblk2_1_1699185373_2_x_cfblk1_1_407717404_2_y_cfblk1_3870719917_2_y_cfblk2_3909855310', 'y_cfblk5_4027261489_y_cfblk1_3870719917'], df outputs ['y_cfblk3_3948990703_2_y_cfblk4_3988126096_2_y_cfblk2_3909855310_2']
	[cfblk3_Output_5_2] (x_cfblk2_1_1699185373_2_x_cfblk1_1_407717404_2_y_cfblk1_3870719917_2_y_cfblk2_3909855310=prop_1) & (y_cfblk5_4027261489_y_cfblk1_3870719917=prop_1) -> 0.974002239936:(y_cfblk3_3948990703_2_y_cfblk4_3988126096_2_y_cfblk2_3909855310_2'=prop_1) + 0.025997760064:(y_cfblk3_3948990703_2_y_cfblk4_3988126096_2_y_cfblk2_3909855310_2'=prop_2);	// <-- 
	[cfblk3_Output_5_2] (x_cfblk2_1_1699185373_2_x_cfblk1_1_407717404_2_y_cfblk1_3870719917_2_y_cfblk2_3909855310!=prop_1) | (y_cfblk5_4027261489_y_cfblk1_3870719917!=prop_1) -> (y_cfblk3_3948990703_2_y_cfblk4_3988126096_2_y_cfblk2_3909855310_2'=prop_2);	// <-- 
	//Element cfblk3_Output_5_3, df inputs ['x_cfblk2_1_1699185373_2_x_cfblk1_1_407717404_2_y_cfblk1_3870719917_2_y_cfblk2_3909855310', 'y_cfblk5_4027261489_y_cfblk1_3870719917'], df outputs ['y_cfblk3_3948990703']
	[cfblk3_Output_5_3] (x_cfblk2_1_1699185373_2_x_cfblk1_1_407717404_2_y_cfblk1_3870719917_2_y_cfblk2_3909855310=prop_1) & (y_cfblk5_4027261489_y_cfblk1_3870719917=prop_1) -> 0.974002239936:(y_cfblk3_3948990703'=ok) + 0.025997760064:(y_cfblk3_3948990703'=error);	// <-- 
	[cfblk3_Output_5_3] (x_cfblk2_1_1699185373_2_x_cfblk1_1_407717404_2_y_cfblk1_3870719917_2_y_cfblk2_3909855310!=prop_1) | (y_cfblk5_4027261489_y_cfblk1_3870719917!=prop_1) -> (y_cfblk3_3948990703'=error);	// <-- 
	//Element cfblk1_Update_6_2, df inputs ['y_cfblk4_3988126096', 'x_cfblk1_1_407717404'], df outputs ['x_cfblk2_1_1699185373_2_x_cfblk1_1_407717404_2_y_cfblk1_3870719917_2_y_cfblk2_3909855310']
	[cfblk1_Update_6_2] (y_cfblk4_3988126096=ok) & (x_cfblk1_1_407717404=ok) -> 0.98:(x_cfblk2_1_1699185373_2_x_cfblk1_1_407717404_2_y_cfblk1_3870719917_2_y_cfblk2_3909855310'=prop_1) + 0.02:(x_cfblk2_1_1699185373_2_x_cfblk1_1_407717404_2_y_cfblk1_3870719917_2_y_cfblk2_3909855310'=prop_2);	// <-- 
	[cfblk1_Update_6_2] (y_cfblk4_3988126096!=ok) | (x_cfblk1_1_407717404!=ok) -> (x_cfblk2_1_1699185373_2_x_cfblk1_1_407717404_2_y_cfblk1_3870719917_2_y_cfblk2_3909855310'=prop_2);	// <-- 
	//Element cfblk1_Update_6_3, df inputs ['y_cfblk4_3988126096', 'x_cfblk1_1_407717404'], df outputs ['x_cfblk1_1_407717404']
	[cfblk1_Update_6_3] (y_cfblk4_3988126096=ok) & (x_cfblk1_1_407717404=ok) -> 0.98:(x_cfblk1_1_407717404'=ok) + 0.02:(x_cfblk1_1_407717404'=error);	// <-- 
	[cfblk1_Update_6_3] (y_cfblk4_3988126096!=ok) | (x_cfblk1_1_407717404!=ok) -> (x_cfblk1_1_407717404'=error);	// <-- 
	//Element cfblk2_Update_7_2, df inputs ['y_cfblk3_3948990703', 'x_cfblk2_1_1699185373'], df outputs ['x_cfblk2_1_1699185373_2_x_cfblk1_1_407717404_2_y_cfblk1_3870719917_2_y_cfblk2_3909855310']
	[cfblk2_Update_7_2] (y_cfblk3_3948990703=ok) & (x_cfblk2_1_1699185373=ok) -> 0.98:(x_cfblk2_1_1699185373_2_x_cfblk1_1_407717404_2_y_cfblk1_3870719917_2_y_cfblk2_3909855310'=prop_1) + 0.02:(x_cfblk2_1_1699185373_2_x_cfblk1_1_407717404_2_y_cfblk1_3870719917_2_y_cfblk2_3909855310'=prop_2);	// <-- 
	[cfblk2_Update_7_2] (y_cfblk3_3948990703!=ok) | (x_cfblk2_1_1699185373!=ok) -> (x_cfblk2_1_1699185373_2_x_cfblk1_1_407717404_2_y_cfblk1_3870719917_2_y_cfblk2_3909855310'=prop_2);	// <-- 
	//Element cfblk2_Update_7_3, df inputs ['y_cfblk3_3948990703', 'x_cfblk2_1_1699185373'], df outputs ['x_cfblk2_1_1699185373']
	[cfblk2_Update_7_3] (y_cfblk3_3948990703=ok) & (x_cfblk2_1_1699185373=ok) -> 0.98:(x_cfblk2_1_1699185373'=ok) + 0.02:(x_cfblk2_1_1699185373'=error);	// <-- 
	[cfblk2_Update_7_3] (y_cfblk3_3948990703!=ok) | (x_cfblk2_1_1699185373!=ok) -> (x_cfblk2_1_1699185373'=error);	// <-- 
endmodule

//Failure formulas
formula failure = x_cfblk2_1_1699185373 = error;

rewards "time" true : 1; endrewards
