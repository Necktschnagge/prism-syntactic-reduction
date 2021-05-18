//Generated by ErrorPro
dtmc
//Element consts
const int cfblk3_Output_1=0;
const int cfblk1_Output_2=1;
const int cfblk5_Output_3=2;
const int cfblk4_Output_4=3;
const int cfblk2_Output_5=4;
const int cfblk3_Update_6=5;
const int cfblk5_Update_7=6;
const int cfblk4_Update_8=7;
const int check_redandancy0=8;
const int cfblk3_Output_1_2=9;
const int cfblk3_Output_1_3=10;
const int compare0=11;
const int stop_system=12;
const int check_redandancy2=13;
const int cfblk4_Output_4_2=14;
const int cfblk4_Output_4_3=15;
const int compare2=16;
const int check_redandancy3=17;
const int cfblk2_Output_5_2=18;
const int cfblk2_Output_5_3=19;
const int compare3=20;
const int check_redandancy4=21;
const int cfblk3_Update_6_2=22;
const int cfblk3_Update_6_3=23;
const int compare4=24;
const int check_redandancy5=25;
const int cfblk4_Update_8_2=26;
const int cfblk4_Update_8_3=27;
const int compare5=28;
//Data values consts
const int v=0;
const int ok=1;
const int no=2;
const int error=3;
const int c=4;
//Control flow commands
module control_flow
	cf:[0..29] init cfblk3_Output_1;
	//Element cfblk3_Output_1, df inputs ['y_cfblk2_2600040187', 'x_cfblk3_1_2716427233'], cf transitions ['check_redandancy0']
	[cfblk3_Output_1] cf=cfblk3_Output_1 -> 1.0:(cf'=check_redandancy0);
	//Element cfblk1_Output_2, df inputs ['y_cfblk3_2639175580'], cf transitions ['cfblk5_Output_3']
	[cfblk1_Output_2] cf=cfblk1_Output_2 -> 1.0:(cf'=cfblk5_Output_3);
	//Element cfblk5_Output_3, df inputs ['y_cfblk4_2678310973', 'x_cfblk5_1_1004395876'], cf transitions ['cfblk4_Output_4']
	[cfblk5_Output_3] cf=cfblk5_Output_3 -> 1.0:(cf'=cfblk4_Output_4);
	//Element cfblk4_Output_4, df inputs ['y_cfblk5_2717446366', 'x_cfblk4_1_4007895202'], cf transitions ['check_redandancy2']
	[cfblk4_Output_4] cf=cfblk4_Output_4 -> 1.0:(cf'=check_redandancy2);
	//Element cfblk2_Output_5, df inputs ['y_cfblk4_2678310973', 'y_cfblk5_2717446366'], cf transitions ['check_redandancy3']
	[cfblk2_Output_5] cf=cfblk2_Output_5 -> 1.0:(cf'=check_redandancy3);
	//Element cfblk3_Update_6, df inputs ['y_cfblk2_2600040187', 'x_cfblk3_1_2716427233'], cf transitions ['check_redandancy4']
	[cfblk3_Update_6] cf=cfblk3_Update_6 -> 1.0:(cf'=check_redandancy4);
	//Element cfblk5_Update_7, df inputs ['y_cfblk4_2678310973', 'x_cfblk5_1_1004395876'], cf transitions ['cfblk4_Update_8']
	[cfblk5_Update_7] cf=cfblk5_Update_7 -> 1.0:(cf'=cfblk4_Update_8);
	//Element cfblk4_Update_8, df inputs ['y_cfblk5_2717446366', 'x_cfblk4_1_4007895202'], cf transitions ['check_redandancy5']
	[cfblk4_Update_8] cf=cfblk4_Update_8 -> 1.0:(cf'=check_redandancy5);
	//Element check_redandancy0, df inputs ['redundancy_model26_cfblk3_0'], cf transitions ['cfblk3_Output_1_2', 'cfblk1_Output_2']
	[check_redandancy0] (cf=check_redandancy0) & (redundancy_model26_cfblk3_0=no) -> (cf'=cfblk1_Output_2);	// <-- 
	[check_redandancy0] (cf=check_redandancy0) & ((redundancy_model26_cfblk3_0=v)|(redundancy_model26_cfblk3_0=c)) -> (cf'=cfblk3_Output_1_2);	// <-- 
	//Element cfblk3_Output_1_2, df inputs ['y_cfblk2_2600040187', 'x_cfblk3_1_2716427233'], cf transitions ['compare0']
	[cfblk3_Output_1_2] cf=cfblk3_Output_1_2 -> 1.0:(cf'=compare0);
	//Element cfblk3_Output_1_3, df inputs ['y_cfblk2_2600040187', 'x_cfblk3_1_2716427233'], cf transitions ['cfblk1_Output_2']
	[cfblk3_Output_1_3] cf=cfblk3_Output_1_3 -> 1.0:(cf'=cfblk1_Output_2);
	//Element compare0, df inputs ['redundancy_model26_cfblk3_0', 'y_cfblk3_2639175580_2', 'y_cfblk3_2639175580'], cf transitions ['cfblk3_Output_1_3', 'cfblk1_Output_2', 'stop_system']
	[compare0] (cf=compare0) & (redundancy_model26_cfblk3_0=c) & (y_cfblk3_2639175580=ok) & (y_cfblk3_2639175580_2=ok) -> (cf'=cfblk1_Output_2);	// <-- 
	[compare0] (cf=compare0) & (redundancy_model26_cfblk3_0=c) & ((y_cfblk3_2639175580=error) | (y_cfblk3_2639175580_2=error)) -> (cf'=stop_system);	// <-- 
	[compare0] (cf=compare0) & (redundancy_model26_cfblk3_0=v) & (y_cfblk3_2639175580=ok) & (y_cfblk3_2639175580_2=ok) -> (cf'=cfblk1_Output_2);	// <-- 
	[compare0] (cf=compare0) & (redundancy_model26_cfblk3_0=v) & ((y_cfblk3_2639175580=error) | (y_cfblk3_2639175580_2=error)) -> (cf'=cfblk3_Output_1_3);	// <-- 
	//Element stop_system, cf transitions ['cfblk3_Output_1']
	[stop_system] cf=stop_system -> 1.0:(cf'=cfblk3_Output_1);
	//Element check_redandancy2, df inputs ['redundancy_model26_cfblk4_2'], cf transitions ['cfblk4_Output_4_2', 'cfblk2_Output_5']
	[check_redandancy2] (cf=check_redandancy2) & (redundancy_model26_cfblk4_2=no) -> (cf'=cfblk2_Output_5);	// <-- 
	[check_redandancy2] (cf=check_redandancy2) & ((redundancy_model26_cfblk4_2=v)|(redundancy_model26_cfblk4_2=c)) -> (cf'=cfblk4_Output_4_2);	// <-- 
	//Element cfblk4_Output_4_2, df inputs ['y_cfblk5_2717446366', 'x_cfblk4_1_4007895202'], cf transitions ['compare2']
	[cfblk4_Output_4_2] cf=cfblk4_Output_4_2 -> 1.0:(cf'=compare2);
	//Element cfblk4_Output_4_3, df inputs ['y_cfblk5_2717446366', 'x_cfblk4_1_4007895202'], cf transitions ['cfblk2_Output_5']
	[cfblk4_Output_4_3] cf=cfblk4_Output_4_3 -> 1.0:(cf'=cfblk2_Output_5);
	//Element compare2, df inputs ['redundancy_model26_cfblk4_2', 'y_cfblk4_2678310973_2', 'y_cfblk4_2678310973'], cf transitions ['cfblk4_Output_4_3', 'cfblk2_Output_5', 'stop_system']
	[compare2] (cf=compare2) & (redundancy_model26_cfblk4_2=c) & (y_cfblk4_2678310973=ok) & (y_cfblk4_2678310973_2=ok) -> (cf'=cfblk2_Output_5);	// <-- 
	[compare2] (cf=compare2) & (redundancy_model26_cfblk4_2=c) & ((y_cfblk4_2678310973=error) | (y_cfblk4_2678310973_2=error)) -> (cf'=stop_system);	// <-- 
	[compare2] (cf=compare2) & (redundancy_model26_cfblk4_2=v) & (y_cfblk4_2678310973=ok) & (y_cfblk4_2678310973_2=ok) -> (cf'=cfblk2_Output_5);	// <-- 
	[compare2] (cf=compare2) & (redundancy_model26_cfblk4_2=v) & ((y_cfblk4_2678310973=error) | (y_cfblk4_2678310973_2=error)) -> (cf'=cfblk4_Output_4_3);	// <-- 
	//Element check_redandancy3, df inputs ['redundancy_model26_cfblk2_3'], cf transitions ['cfblk2_Output_5_2', 'cfblk3_Update_6']
	[check_redandancy3] (cf=check_redandancy3) & (redundancy_model26_cfblk2_3=no) -> (cf'=cfblk3_Update_6);	// <-- 
	[check_redandancy3] (cf=check_redandancy3) & ((redundancy_model26_cfblk2_3=v)|(redundancy_model26_cfblk2_3=c)) -> (cf'=cfblk2_Output_5_2);	// <-- 
	//Element cfblk2_Output_5_2, df inputs ['y_cfblk4_2678310973', 'y_cfblk5_2717446366'], cf transitions ['compare3']
	[cfblk2_Output_5_2] cf=cfblk2_Output_5_2 -> 1.0:(cf'=compare3);
	//Element cfblk2_Output_5_3, df inputs ['y_cfblk4_2678310973', 'y_cfblk5_2717446366'], cf transitions ['cfblk3_Update_6']
	[cfblk2_Output_5_3] cf=cfblk2_Output_5_3 -> 1.0:(cf'=cfblk3_Update_6);
	//Element compare3, df inputs ['redundancy_model26_cfblk2_3', 'y_cfblk2_2600040187_2', 'y_cfblk2_2600040187'], cf transitions ['cfblk2_Output_5_3', 'cfblk3_Update_6', 'stop_system']
	[compare3] (cf=compare3) & (redundancy_model26_cfblk2_3=c) & (y_cfblk2_2600040187=ok) & (y_cfblk2_2600040187_2=ok) -> (cf'=cfblk3_Update_6);	// <-- 
	[compare3] (cf=compare3) & (redundancy_model26_cfblk2_3=c) & ((y_cfblk2_2600040187=error) | (y_cfblk2_2600040187_2=error)) -> (cf'=stop_system);	// <-- 
	[compare3] (cf=compare3) & (redundancy_model26_cfblk2_3=v) & (y_cfblk2_2600040187=ok) & (y_cfblk2_2600040187_2=ok) -> (cf'=cfblk3_Update_6);	// <-- 
	[compare3] (cf=compare3) & (redundancy_model26_cfblk2_3=v) & ((y_cfblk2_2600040187=error) | (y_cfblk2_2600040187_2=error)) -> (cf'=cfblk2_Output_5_3);	// <-- 
	//Element check_redandancy4, df inputs ['redundancy_model26_cfblk3_0'], cf transitions ['cfblk3_Update_6_2', 'cfblk5_Update_7']
	[check_redandancy4] (cf=check_redandancy4) & (redundancy_model26_cfblk3_0=no) -> (cf'=cfblk5_Update_7);	// <-- 
	[check_redandancy4] (cf=check_redandancy4) & ((redundancy_model26_cfblk3_0=v)|(redundancy_model26_cfblk3_0=c)) -> (cf'=cfblk3_Update_6_2);	// <-- 
	//Element cfblk3_Update_6_2, df inputs ['y_cfblk2_2600040187', 'x_cfblk3_1_2716427233'], cf transitions ['compare4']
	[cfblk3_Update_6_2] cf=cfblk3_Update_6_2 -> 1.0:(cf'=compare4);
	//Element cfblk3_Update_6_3, df inputs ['y_cfblk2_2600040187', 'x_cfblk3_1_2716427233'], cf transitions ['cfblk5_Update_7']
	[cfblk3_Update_6_3] cf=cfblk3_Update_6_3 -> 1.0:(cf'=cfblk5_Update_7);
	//Element compare4, df inputs ['redundancy_model26_cfblk3_0', 'x_cfblk3_1_2716427233_2', 'x_cfblk3_1_2716427233'], cf transitions ['cfblk3_Update_6_3', 'cfblk5_Update_7', 'stop_system']
	[compare4] (cf=compare4) & (redundancy_model26_cfblk3_0=c) & (x_cfblk3_1_2716427233=ok) & (x_cfblk3_1_2716427233_2=ok) -> (cf'=cfblk5_Update_7);	// <-- 
	[compare4] (cf=compare4) & (redundancy_model26_cfblk3_0=c) & ((x_cfblk3_1_2716427233=error) | (x_cfblk3_1_2716427233_2=error)) -> (cf'=stop_system);	// <-- 
	[compare4] (cf=compare4) & (redundancy_model26_cfblk3_0=v) & (x_cfblk3_1_2716427233=ok) & (x_cfblk3_1_2716427233_2=ok) -> (cf'=cfblk5_Update_7);	// <-- 
	[compare4] (cf=compare4) & (redundancy_model26_cfblk3_0=v) & ((x_cfblk3_1_2716427233=error) | (x_cfblk3_1_2716427233_2=error)) -> (cf'=cfblk3_Update_6_3);	// <-- 
	//Element check_redandancy5, df inputs ['redundancy_model26_cfblk4_2'], cf transitions ['cfblk4_Update_8_2', 'cfblk3_Output_1']
	[check_redandancy5] (cf=check_redandancy5) & (redundancy_model26_cfblk4_2=no) -> (cf'=cfblk3_Output_1);	// <-- 
	[check_redandancy5] (cf=check_redandancy5) & ((redundancy_model26_cfblk4_2=v)|(redundancy_model26_cfblk4_2=c)) -> (cf'=cfblk4_Update_8_2);	// <-- 
	//Element cfblk4_Update_8_2, df inputs ['y_cfblk5_2717446366', 'x_cfblk4_1_4007895202'], cf transitions ['compare5']
	[cfblk4_Update_8_2] cf=cfblk4_Update_8_2 -> 1.0:(cf'=compare5);
	//Element cfblk4_Update_8_3, df inputs ['y_cfblk5_2717446366', 'x_cfblk4_1_4007895202'], cf transitions ['cfblk3_Output_1']
	[cfblk4_Update_8_3] cf=cfblk4_Update_8_3 -> 1.0:(cf'=cfblk3_Output_1);
	//Element compare5, df inputs ['redundancy_model26_cfblk4_2', 'x_cfblk4_1_4007895202_2', 'x_cfblk4_1_4007895202'], cf transitions ['cfblk4_Update_8_3', 'cfblk3_Output_1', 'stop_system']
	[compare5] (cf=compare5) & (redundancy_model26_cfblk4_2=c) & (x_cfblk4_1_4007895202=ok) & (x_cfblk4_1_4007895202_2=ok) -> (cf'=cfblk3_Output_1);	// <-- 
	[compare5] (cf=compare5) & (redundancy_model26_cfblk4_2=c) & ((x_cfblk4_1_4007895202=error) | (x_cfblk4_1_4007895202_2=error)) -> (cf'=stop_system);	// <-- 
	[compare5] (cf=compare5) & (redundancy_model26_cfblk4_2=v) & (x_cfblk4_1_4007895202=ok) & (x_cfblk4_1_4007895202_2=ok) -> (cf'=cfblk3_Output_1);	// <-- 
	[compare5] (cf=compare5) & (redundancy_model26_cfblk4_2=v) & ((x_cfblk4_1_4007895202=error) | (x_cfblk4_1_4007895202_2=error)) -> (cf'=cfblk4_Update_8_3);	// <-- 
endmodule
//Error propagation commands
module error_propagation
	y_cfblk2_2600040187 : [0 .. 4] init ok;
	y_cfblk3_2639175580 : [0 .. 4] init ok;
	y_cfblk4_2678310973 : [0 .. 4] init ok;
	y_cfblk5_2717446366 : [0 .. 4] init ok;
	x_cfblk3_1_2716427233 : [0 .. 4] init ok;
	x_cfblk5_1_1004395876 : [0 .. 4] init ok;
	x_cfblk4_1_4007895202 : [0 .. 4] init ok;
	redundancy_model26_cfblk3_0 : [0 .. 4] init v;
	y_cfblk3_2639175580_2 : [0 .. 4] init ok;
	redundancy_model26_cfblk4_2 : [0 .. 4] init v;
	y_cfblk4_2678310973_2 : [0 .. 4] init ok;
	redundancy_model26_cfblk2_3 : [0 .. 4] init v;
	y_cfblk2_2600040187_2 : [0 .. 4] init ok;
	x_cfblk3_1_2716427233_2 : [0 .. 4] init ok;
	x_cfblk4_1_4007895202_2 : [0 .. 4] init ok;
	//Element cfblk3_Output_1, df inputs ['y_cfblk2_2600040187', 'x_cfblk3_1_2716427233'], df outputs ['y_cfblk3_2639175580']
	[cfblk3_Output_1] (y_cfblk2_2600040187=ok) & (x_cfblk3_1_2716427233=ok) -> 0.98:(y_cfblk3_2639175580'=ok) + 0.02:(y_cfblk3_2639175580'=error);	// <-- 
	[cfblk3_Output_1] (y_cfblk2_2600040187!=ok) | (x_cfblk3_1_2716427233!=ok) -> (y_cfblk3_2639175580'=error);	// <-- 
	//Element cfblk5_Output_3, df inputs ['y_cfblk4_2678310973', 'x_cfblk5_1_1004395876'], df outputs ['y_cfblk5_2717446366']
	[cfblk5_Output_3] (y_cfblk4_2678310973=ok) & (x_cfblk5_1_1004395876=ok) -> 0.98:(y_cfblk5_2717446366'=ok) + 0.02:(y_cfblk5_2717446366'=error);	// <-- 
	[cfblk5_Output_3] (y_cfblk4_2678310973!=ok) | (x_cfblk5_1_1004395876!=ok) -> (y_cfblk5_2717446366'=error);	// <-- 
	//Element cfblk4_Output_4, df inputs ['y_cfblk5_2717446366', 'x_cfblk4_1_4007895202'], df outputs ['y_cfblk4_2678310973']
	[cfblk4_Output_4] (y_cfblk5_2717446366=ok) & (x_cfblk4_1_4007895202=ok) -> 0.98:(y_cfblk4_2678310973'=ok) + 0.02:(y_cfblk4_2678310973'=error);	// <-- 
	[cfblk4_Output_4] (y_cfblk5_2717446366!=ok) | (x_cfblk4_1_4007895202!=ok) -> (y_cfblk4_2678310973'=error);	// <-- 
	//Element cfblk2_Output_5, df inputs ['y_cfblk4_2678310973', 'y_cfblk5_2717446366'], df outputs ['y_cfblk2_2600040187']
	[cfblk2_Output_5] (y_cfblk4_2678310973=ok) & (y_cfblk5_2717446366=ok) -> 0.974002239936:(y_cfblk2_2600040187'=ok) + 0.025997760064:(y_cfblk2_2600040187'=error);	// <-- 
	[cfblk2_Output_5] (y_cfblk4_2678310973!=ok) | (y_cfblk5_2717446366!=ok) -> (y_cfblk2_2600040187'=error);	// <-- 
	//Element cfblk3_Update_6, df inputs ['y_cfblk2_2600040187', 'x_cfblk3_1_2716427233'], df outputs ['x_cfblk3_1_2716427233']
	[cfblk3_Update_6] (y_cfblk2_2600040187=ok) & (x_cfblk3_1_2716427233=ok) -> 0.98:(x_cfblk3_1_2716427233'=ok) + 0.02:(x_cfblk3_1_2716427233'=error);	// <-- 
	[cfblk3_Update_6] (y_cfblk2_2600040187!=ok) | (x_cfblk3_1_2716427233!=ok) -> (x_cfblk3_1_2716427233'=error);	// <-- 
	//Element cfblk5_Update_7, df inputs ['y_cfblk4_2678310973', 'x_cfblk5_1_1004395876'], df outputs ['x_cfblk5_1_1004395876']
	[cfblk5_Update_7] (y_cfblk4_2678310973=ok) & (x_cfblk5_1_1004395876=ok) -> 0.98:(x_cfblk5_1_1004395876'=ok) + 0.02:(x_cfblk5_1_1004395876'=error);	// <-- 
	[cfblk5_Update_7] (y_cfblk4_2678310973!=ok) | (x_cfblk5_1_1004395876!=ok) -> (x_cfblk5_1_1004395876'=error);	// <-- 
	//Element cfblk4_Update_8, df inputs ['y_cfblk5_2717446366', 'x_cfblk4_1_4007895202'], df outputs ['x_cfblk4_1_4007895202']
	[cfblk4_Update_8] (y_cfblk5_2717446366=ok) & (x_cfblk4_1_4007895202=ok) -> 0.98:(x_cfblk4_1_4007895202'=ok) + 0.02:(x_cfblk4_1_4007895202'=error);	// <-- 
	[cfblk4_Update_8] (y_cfblk5_2717446366!=ok) | (x_cfblk4_1_4007895202!=ok) -> (x_cfblk4_1_4007895202'=error);	// <-- 
	//Element cfblk3_Output_1_2, df inputs ['y_cfblk2_2600040187', 'x_cfblk3_1_2716427233'], df outputs ['y_cfblk3_2639175580_2']
	[cfblk3_Output_1_2] (y_cfblk2_2600040187=ok) & (x_cfblk3_1_2716427233=ok) -> 0.98:(y_cfblk3_2639175580_2'=ok) + 0.02:(y_cfblk3_2639175580_2'=error);	// <-- 
	[cfblk3_Output_1_2] (y_cfblk2_2600040187!=ok) | (x_cfblk3_1_2716427233!=ok) -> (y_cfblk3_2639175580_2'=error);	// <-- 
	//Element cfblk3_Output_1_3, df inputs ['y_cfblk2_2600040187', 'x_cfblk3_1_2716427233'], df outputs ['y_cfblk3_2639175580']
	[cfblk3_Output_1_3] (y_cfblk2_2600040187=ok) & (x_cfblk3_1_2716427233=ok) -> 0.98:(y_cfblk3_2639175580'=ok) + 0.02:(y_cfblk3_2639175580'=error);	// <-- 
	[cfblk3_Output_1_3] (y_cfblk2_2600040187!=ok) | (x_cfblk3_1_2716427233!=ok) -> (y_cfblk3_2639175580'=error);	// <-- 
	//Element cfblk4_Output_4_2, df inputs ['y_cfblk5_2717446366', 'x_cfblk4_1_4007895202'], df outputs ['y_cfblk4_2678310973_2']
	[cfblk4_Output_4_2] (y_cfblk5_2717446366=ok) & (x_cfblk4_1_4007895202=ok) -> 0.98:(y_cfblk4_2678310973_2'=ok) + 0.02:(y_cfblk4_2678310973_2'=error);	// <-- 
	[cfblk4_Output_4_2] (y_cfblk5_2717446366!=ok) | (x_cfblk4_1_4007895202!=ok) -> (y_cfblk4_2678310973_2'=error);	// <-- 
	//Element cfblk4_Output_4_3, df inputs ['y_cfblk5_2717446366', 'x_cfblk4_1_4007895202'], df outputs ['y_cfblk4_2678310973']
	[cfblk4_Output_4_3] (y_cfblk5_2717446366=ok) & (x_cfblk4_1_4007895202=ok) -> 0.98:(y_cfblk4_2678310973'=ok) + 0.02:(y_cfblk4_2678310973'=error);	// <-- 
	[cfblk4_Output_4_3] (y_cfblk5_2717446366!=ok) | (x_cfblk4_1_4007895202!=ok) -> (y_cfblk4_2678310973'=error);	// <-- 
	//Element cfblk2_Output_5_2, df inputs ['y_cfblk4_2678310973', 'y_cfblk5_2717446366'], df outputs ['y_cfblk2_2600040187_2']
	[cfblk2_Output_5_2] (y_cfblk4_2678310973=ok) & (y_cfblk5_2717446366=ok) -> 0.974002239936:(y_cfblk2_2600040187_2'=ok) + 0.025997760064:(y_cfblk2_2600040187_2'=error);	// <-- 
	[cfblk2_Output_5_2] (y_cfblk4_2678310973!=ok) | (y_cfblk5_2717446366!=ok) -> (y_cfblk2_2600040187_2'=error);	// <-- 
	//Element cfblk2_Output_5_3, df inputs ['y_cfblk4_2678310973', 'y_cfblk5_2717446366'], df outputs ['y_cfblk2_2600040187']
	[cfblk2_Output_5_3] (y_cfblk4_2678310973=ok) & (y_cfblk5_2717446366=ok) -> 0.974002239936:(y_cfblk2_2600040187'=ok) + 0.025997760064:(y_cfblk2_2600040187'=error);	// <-- 
	[cfblk2_Output_5_3] (y_cfblk4_2678310973!=ok) | (y_cfblk5_2717446366!=ok) -> (y_cfblk2_2600040187'=error);	// <-- 
	//Element cfblk3_Update_6_2, df inputs ['y_cfblk2_2600040187', 'x_cfblk3_1_2716427233'], df outputs ['x_cfblk3_1_2716427233_2']
	[cfblk3_Update_6_2] (y_cfblk2_2600040187=ok) & (x_cfblk3_1_2716427233=ok) -> 0.98:(x_cfblk3_1_2716427233_2'=ok) + 0.02:(x_cfblk3_1_2716427233_2'=error);	// <-- 
	[cfblk3_Update_6_2] (y_cfblk2_2600040187!=ok) | (x_cfblk3_1_2716427233!=ok) -> (x_cfblk3_1_2716427233_2'=error);	// <-- 
	//Element cfblk3_Update_6_3, df inputs ['y_cfblk2_2600040187', 'x_cfblk3_1_2716427233'], df outputs ['x_cfblk3_1_2716427233']
	[cfblk3_Update_6_3] (y_cfblk2_2600040187=ok) & (x_cfblk3_1_2716427233=ok) -> 0.98:(x_cfblk3_1_2716427233'=ok) + 0.02:(x_cfblk3_1_2716427233'=error);	// <-- 
	[cfblk3_Update_6_3] (y_cfblk2_2600040187!=ok) | (x_cfblk3_1_2716427233!=ok) -> (x_cfblk3_1_2716427233'=error);	// <-- 
	//Element cfblk4_Update_8_2, df inputs ['y_cfblk5_2717446366', 'x_cfblk4_1_4007895202'], df outputs ['x_cfblk4_1_4007895202_2']
	[cfblk4_Update_8_2] (y_cfblk5_2717446366=ok) & (x_cfblk4_1_4007895202=ok) -> 0.98:(x_cfblk4_1_4007895202_2'=ok) + 0.02:(x_cfblk4_1_4007895202_2'=error);	// <-- 
	[cfblk4_Update_8_2] (y_cfblk5_2717446366!=ok) | (x_cfblk4_1_4007895202!=ok) -> (x_cfblk4_1_4007895202_2'=error);	// <-- 
	//Element cfblk4_Update_8_3, df inputs ['y_cfblk5_2717446366', 'x_cfblk4_1_4007895202'], df outputs ['x_cfblk4_1_4007895202']
	[cfblk4_Update_8_3] (y_cfblk5_2717446366=ok) & (x_cfblk4_1_4007895202=ok) -> 0.98:(x_cfblk4_1_4007895202'=ok) + 0.02:(x_cfblk4_1_4007895202'=error);	// <-- 
	[cfblk4_Update_8_3] (y_cfblk5_2717446366!=ok) | (x_cfblk4_1_4007895202!=ok) -> (x_cfblk4_1_4007895202'=error);	// <-- 
endmodule

//Failure formulas
formula failure = x_cfblk4_1_4007895202 = error;

rewards "time" true : 1; endrewards
