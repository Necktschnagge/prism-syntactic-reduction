//Generated by ErrorPro
dtmc
//Element consts
const int cfblk1_Output_1=0;
const int cfblk2_Output_2=1;
const int cfblk5_Output_3=2;
const int cfblk3_Output_4=3;
const int cfblk4_Output_5=4;
const int cfblk1_Update_6=5;
const int cfblk2_Update_7=6;
const int cfblk5_Update_8=7;
const int check_redandancy0=8;
const int cfblk1_Output_1_2=9;
const int cfblk1_Output_1_3=10;
const int compare0=11;
const int stop_system=12;
const int check_redandancy1=13;
const int cfblk2_Output_2_2=14;
const int cfblk2_Output_2_3=15;
const int compare1=16;
const int check_redandancy2=17;
const int cfblk3_Output_4_2=18;
const int cfblk3_Output_4_3=19;
const int compare2=20;
const int check_redandancy3=21;
const int cfblk4_Output_5_2=22;
const int cfblk4_Output_5_3=23;
const int compare3=24;
const int check_redandancy4=25;
const int cfblk1_Update_6_2=26;
const int cfblk1_Update_6_3=27;
const int compare4=28;
const int check_redandancy5=29;
const int cfblk2_Update_7_2=30;
const int cfblk2_Update_7_3=31;
const int compare5=32;
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
	cf:[0..33] init cfblk1_Output_1;
	//Element cfblk1_Output_1, df inputs ['x_cfblk1_1_2363574400', 'y_cfblk2_2797768987'], cf transitions ['check_redandancy0']
	[cfblk1_Output_1] cf=cfblk1_Output_1 -> 1.0:(cf'=check_redandancy0);
	//Element cfblk2_Output_2, df inputs ['y_cfblk4_2876039773', 'x_cfblk2_1_3655042369'], cf transitions ['check_redandancy1']
	[cfblk2_Output_2] cf=cfblk2_Output_2 -> 1.0:(cf'=check_redandancy1);
	//Element cfblk5_Output_3, df inputs ['x_cfblk5_1_3234478981', 'y_cfblk1_2758633594'], cf transitions ['cfblk3_Output_4']
	[cfblk5_Output_3] cf=cfblk5_Output_3 -> 1.0:(cf'=cfblk3_Output_4);
	//Element cfblk3_Output_4, df inputs ['y_cfblk4_2876039773_2_y_cfblk5_2915175166'], cf transitions ['check_redandancy2']
	[cfblk3_Output_4] cf=cfblk3_Output_4 -> 1.0:(cf'=check_redandancy2);
	//Element cfblk4_Output_5, df inputs ['x_cfblk2_1_3655042369_2_x_cfblk1_1_2363574400_2_y_cfblk2_2797768987_2_y_cfblk1_2758633594_2_y_cfblk3_2836904380'], cf transitions ['check_redandancy3']
	[cfblk4_Output_5] cf=cfblk4_Output_5 -> 1.0:(cf'=check_redandancy3);
	//Element cfblk1_Update_6, df inputs ['x_cfblk1_1_2363574400', 'y_cfblk2_2797768987'], cf transitions ['check_redandancy4']
	[cfblk1_Update_6] cf=cfblk1_Update_6 -> 1.0:(cf'=check_redandancy4);
	//Element cfblk2_Update_7, df inputs ['y_cfblk4_2876039773', 'x_cfblk2_1_3655042369'], cf transitions ['check_redandancy5']
	[cfblk2_Update_7] cf=cfblk2_Update_7 -> 1.0:(cf'=check_redandancy5);
	//Element cfblk5_Update_8, df inputs ['x_cfblk5_1_3234478981', 'y_cfblk1_2758633594'], cf transitions ['cfblk1_Output_1']
	[cfblk5_Update_8] cf=cfblk5_Update_8 -> 1.0:(cf'=cfblk1_Output_1);
	//Element check_redandancy0, df inputs ['redundancy_model57_cfblk1_0'], cf transitions ['cfblk1_Output_1_2', 'cfblk2_Output_2']
	[check_redandancy0] (cf=check_redandancy0) & (redundancy_model57_cfblk1_0=no) -> (cf'=cfblk2_Output_2);	// <-- 
	[check_redandancy0] (cf=check_redandancy0) & ((redundancy_model57_cfblk1_0=v)|(redundancy_model57_cfblk1_0=c)) -> (cf'=cfblk1_Output_1_2);	// <-- 
	//Element cfblk1_Output_1_2, df inputs ['x_cfblk1_1_2363574400', 'y_cfblk2_2797768987'], cf transitions ['compare0']
	[cfblk1_Output_1_2] cf=cfblk1_Output_1_2 -> 1.0:(cf'=compare0);
	//Element cfblk1_Output_1_3, df inputs ['x_cfblk1_1_2363574400', 'y_cfblk2_2797768987'], cf transitions ['cfblk2_Output_2']
	[cfblk1_Output_1_3] cf=cfblk1_Output_1_3 -> 1.0:(cf'=cfblk2_Output_2);
	//Element compare0, df inputs ['redundancy_model57_cfblk1_0', 'x_cfblk2_1_3655042369_2_x_cfblk1_1_2363574400_2_y_cfblk2_2797768987_2_y_cfblk1_2758633594_2_y_cfblk3_2836904380', 'y_cfblk1_2758633594'], cf transitions ['cfblk1_Output_1_3', 'cfblk2_Output_2', 'stop_system']
	[compare0] (cf=compare0) & (redundancy_model57_cfblk1_0=c) & (y_cfblk1_2758633594=ok) & (x_cfblk2_1_3655042369_2_x_cfblk1_1_2363574400_2_y_cfblk2_2797768987_2_y_cfblk1_2758633594_2_y_cfblk3_2836904380=prop_1) -> (cf'=cfblk2_Output_2);	// <-- 
	[compare0] (cf=compare0) & (redundancy_model57_cfblk1_0=c) & ((y_cfblk1_2758633594=error) | (x_cfblk2_1_3655042369_2_x_cfblk1_1_2363574400_2_y_cfblk2_2797768987_2_y_cfblk1_2758633594_2_y_cfblk3_2836904380=prop_2)) -> (cf'=stop_system);	// <-- 
	[compare0] (cf=compare0) & (redundancy_model57_cfblk1_0=v) & (y_cfblk1_2758633594=ok) & (x_cfblk2_1_3655042369_2_x_cfblk1_1_2363574400_2_y_cfblk2_2797768987_2_y_cfblk1_2758633594_2_y_cfblk3_2836904380=prop_1) -> (cf'=cfblk2_Output_2);	// <-- 
	[compare0] (cf=compare0) & (redundancy_model57_cfblk1_0=v) & ((y_cfblk1_2758633594=error) | (x_cfblk2_1_3655042369_2_x_cfblk1_1_2363574400_2_y_cfblk2_2797768987_2_y_cfblk1_2758633594_2_y_cfblk3_2836904380=prop_2)) -> (cf'=cfblk1_Output_1_3);	// <-- 
	//Element stop_system, cf transitions ['cfblk1_Output_1']
	[stop_system] cf=stop_system -> 1.0:(cf'=cfblk1_Output_1);
	//Element check_redandancy1, df inputs ['redundancy_model57_cfblk2_1'], cf transitions ['cfblk2_Output_2_2', 'cfblk5_Output_3']
	[check_redandancy1] (cf=check_redandancy1) & (redundancy_model57_cfblk2_1=no) -> (cf'=cfblk5_Output_3);	// <-- 
	[check_redandancy1] (cf=check_redandancy1) & ((redundancy_model57_cfblk2_1=v)|(redundancy_model57_cfblk2_1=c)) -> (cf'=cfblk2_Output_2_2);	// <-- 
	//Element cfblk2_Output_2_2, df inputs ['y_cfblk4_2876039773', 'x_cfblk2_1_3655042369'], cf transitions ['compare1']
	[cfblk2_Output_2_2] cf=cfblk2_Output_2_2 -> 1.0:(cf'=compare1);
	//Element cfblk2_Output_2_3, df inputs ['y_cfblk4_2876039773', 'x_cfblk2_1_3655042369'], cf transitions ['cfblk5_Output_3']
	[cfblk2_Output_2_3] cf=cfblk2_Output_2_3 -> 1.0:(cf'=cfblk5_Output_3);
	//Element compare1, df inputs ['x_cfblk2_1_3655042369_2_x_cfblk1_1_2363574400_2_y_cfblk2_2797768987_2_y_cfblk1_2758633594_2_y_cfblk3_2836904380', 'redundancy_model57_cfblk2_1', 'y_cfblk2_2797768987'], cf transitions ['cfblk2_Output_2_3', 'cfblk5_Output_3', 'stop_system']
	[compare1] (cf=compare1) & (redundancy_model57_cfblk2_1=c) & (y_cfblk2_2797768987=ok) & (x_cfblk2_1_3655042369_2_x_cfblk1_1_2363574400_2_y_cfblk2_2797768987_2_y_cfblk1_2758633594_2_y_cfblk3_2836904380=prop_1) -> (cf'=cfblk5_Output_3);	// <-- 
	[compare1] (cf=compare1) & (redundancy_model57_cfblk2_1=c) & ((y_cfblk2_2797768987=error) | (x_cfblk2_1_3655042369_2_x_cfblk1_1_2363574400_2_y_cfblk2_2797768987_2_y_cfblk1_2758633594_2_y_cfblk3_2836904380=prop_2)) -> (cf'=stop_system);	// <-- 
	[compare1] (cf=compare1) & (redundancy_model57_cfblk2_1=v) & (y_cfblk2_2797768987=ok) & (x_cfblk2_1_3655042369_2_x_cfblk1_1_2363574400_2_y_cfblk2_2797768987_2_y_cfblk1_2758633594_2_y_cfblk3_2836904380=prop_1) -> (cf'=cfblk5_Output_3);	// <-- 
	[compare1] (cf=compare1) & (redundancy_model57_cfblk2_1=v) & ((y_cfblk2_2797768987=error) | (x_cfblk2_1_3655042369_2_x_cfblk1_1_2363574400_2_y_cfblk2_2797768987_2_y_cfblk1_2758633594_2_y_cfblk3_2836904380=prop_2)) -> (cf'=cfblk2_Output_2_3);	// <-- 
	//Element check_redandancy2, df inputs ['redundancy_model57_cfblk3_2'], cf transitions ['cfblk3_Output_4_2', 'cfblk4_Output_5']
	[check_redandancy2] (cf=check_redandancy2) & (redundancy_model57_cfblk3_2=no) -> (cf'=cfblk4_Output_5);	// <-- 
	[check_redandancy2] (cf=check_redandancy2) & ((redundancy_model57_cfblk3_2=v)|(redundancy_model57_cfblk3_2=c)) -> (cf'=cfblk3_Output_4_2);	// <-- 
	//Element cfblk3_Output_4_2, df inputs ['y_cfblk4_2876039773_2_y_cfblk5_2915175166'], cf transitions ['compare2']
	[cfblk3_Output_4_2] cf=cfblk3_Output_4_2 -> 1.0:(cf'=compare2);
	//Element cfblk3_Output_4_3, df inputs ['y_cfblk4_2876039773_2_y_cfblk5_2915175166'], cf transitions ['cfblk4_Output_5']
	[cfblk3_Output_4_3] cf=cfblk3_Output_4_3 -> 1.0:(cf'=cfblk4_Output_5);
	//Element compare2, df inputs ['redundancy_model57_cfblk3_2', 'y_cfblk3_2836904380_2', 'x_cfblk2_1_3655042369_2_x_cfblk1_1_2363574400_2_y_cfblk2_2797768987_2_y_cfblk1_2758633594_2_y_cfblk3_2836904380'], cf transitions ['cfblk3_Output_4_3', 'cfblk4_Output_5', 'stop_system']
	[compare2] (cf=compare2) & (redundancy_model57_cfblk3_2=c) & (x_cfblk2_1_3655042369_2_x_cfblk1_1_2363574400_2_y_cfblk2_2797768987_2_y_cfblk1_2758633594_2_y_cfblk3_2836904380=prop_1) & (y_cfblk3_2836904380_2=ok) -> (cf'=cfblk4_Output_5);	// <-- 
	[compare2] (cf=compare2) & (redundancy_model57_cfblk3_2=c) & ((x_cfblk2_1_3655042369_2_x_cfblk1_1_2363574400_2_y_cfblk2_2797768987_2_y_cfblk1_2758633594_2_y_cfblk3_2836904380=prop_2) | (y_cfblk3_2836904380_2=error)) -> (cf'=stop_system);	// <-- 
	[compare2] (cf=compare2) & (redundancy_model57_cfblk3_2=v) & (x_cfblk2_1_3655042369_2_x_cfblk1_1_2363574400_2_y_cfblk2_2797768987_2_y_cfblk1_2758633594_2_y_cfblk3_2836904380=prop_1) & (y_cfblk3_2836904380_2=ok) -> (cf'=cfblk4_Output_5);	// <-- 
	[compare2] (cf=compare2) & (redundancy_model57_cfblk3_2=v) & ((x_cfblk2_1_3655042369_2_x_cfblk1_1_2363574400_2_y_cfblk2_2797768987_2_y_cfblk1_2758633594_2_y_cfblk3_2836904380=prop_2) | (y_cfblk3_2836904380_2=error)) -> (cf'=cfblk3_Output_4_3);	// <-- 
	//Element check_redandancy3, df inputs ['redundancy_model57_cfblk4_3'], cf transitions ['cfblk4_Output_5_2', 'cfblk1_Update_6']
	[check_redandancy3] (cf=check_redandancy3) & (redundancy_model57_cfblk4_3=no) -> (cf'=cfblk1_Update_6);	// <-- 
	[check_redandancy3] (cf=check_redandancy3) & ((redundancy_model57_cfblk4_3=v)|(redundancy_model57_cfblk4_3=c)) -> (cf'=cfblk4_Output_5_2);	// <-- 
	//Element cfblk4_Output_5_2, df inputs ['x_cfblk2_1_3655042369_2_x_cfblk1_1_2363574400_2_y_cfblk2_2797768987_2_y_cfblk1_2758633594_2_y_cfblk3_2836904380'], cf transitions ['compare3']
	[cfblk4_Output_5_2] cf=cfblk4_Output_5_2 -> 1.0:(cf'=compare3);
	//Element cfblk4_Output_5_3, df inputs ['x_cfblk2_1_3655042369_2_x_cfblk1_1_2363574400_2_y_cfblk2_2797768987_2_y_cfblk1_2758633594_2_y_cfblk3_2836904380'], cf transitions ['cfblk1_Update_6']
	[cfblk4_Output_5_3] cf=cfblk4_Output_5_3 -> 1.0:(cf'=cfblk1_Update_6);
	//Element compare3, df inputs ['y_cfblk4_2876039773_2_y_cfblk5_2915175166', 'redundancy_model57_cfblk4_3', 'y_cfblk4_2876039773'], cf transitions ['cfblk4_Output_5_3', 'cfblk1_Update_6', 'stop_system']
	[compare3] (cf=compare3) & (redundancy_model57_cfblk4_3=c) & (y_cfblk4_2876039773=ok) & (y_cfblk4_2876039773_2_y_cfblk5_2915175166=prop_1) -> (cf'=cfblk1_Update_6);	// <-- 
	[compare3] (cf=compare3) & (redundancy_model57_cfblk4_3=c) & ((y_cfblk4_2876039773=error) | (y_cfblk4_2876039773_2_y_cfblk5_2915175166=prop_2)) -> (cf'=stop_system);	// <-- 
	[compare3] (cf=compare3) & (redundancy_model57_cfblk4_3=v) & (y_cfblk4_2876039773=ok) & (y_cfblk4_2876039773_2_y_cfblk5_2915175166=prop_1) -> (cf'=cfblk1_Update_6);	// <-- 
	[compare3] (cf=compare3) & (redundancy_model57_cfblk4_3=v) & ((y_cfblk4_2876039773=error) | (y_cfblk4_2876039773_2_y_cfblk5_2915175166=prop_2)) -> (cf'=cfblk4_Output_5_3);	// <-- 
	//Element check_redandancy4, df inputs ['redundancy_model57_cfblk1_0'], cf transitions ['cfblk1_Update_6_2', 'cfblk2_Update_7']
	[check_redandancy4] (cf=check_redandancy4) & (redundancy_model57_cfblk1_0=no) -> (cf'=cfblk2_Update_7);	// <-- 
	[check_redandancy4] (cf=check_redandancy4) & ((redundancy_model57_cfblk1_0=v)|(redundancy_model57_cfblk1_0=c)) -> (cf'=cfblk1_Update_6_2);	// <-- 
	//Element cfblk1_Update_6_2, df inputs ['x_cfblk1_1_2363574400', 'y_cfblk2_2797768987'], cf transitions ['compare4']
	[cfblk1_Update_6_2] cf=cfblk1_Update_6_2 -> 1.0:(cf'=compare4);
	//Element cfblk1_Update_6_3, df inputs ['x_cfblk1_1_2363574400', 'y_cfblk2_2797768987'], cf transitions ['cfblk2_Update_7']
	[cfblk1_Update_6_3] cf=cfblk1_Update_6_3 -> 1.0:(cf'=cfblk2_Update_7);
	//Element compare4, df inputs ['redundancy_model57_cfblk1_0', 'x_cfblk2_1_3655042369_2_x_cfblk1_1_2363574400_2_y_cfblk2_2797768987_2_y_cfblk1_2758633594_2_y_cfblk3_2836904380', 'x_cfblk1_1_2363574400'], cf transitions ['cfblk1_Update_6_3', 'cfblk2_Update_7', 'stop_system']
	[compare4] (cf=compare4) & (redundancy_model57_cfblk1_0=c) & (x_cfblk1_1_2363574400=ok) & (x_cfblk2_1_3655042369_2_x_cfblk1_1_2363574400_2_y_cfblk2_2797768987_2_y_cfblk1_2758633594_2_y_cfblk3_2836904380=prop_1) -> (cf'=cfblk2_Update_7);	// <-- 
	[compare4] (cf=compare4) & (redundancy_model57_cfblk1_0=c) & ((x_cfblk1_1_2363574400=error) | (x_cfblk2_1_3655042369_2_x_cfblk1_1_2363574400_2_y_cfblk2_2797768987_2_y_cfblk1_2758633594_2_y_cfblk3_2836904380=prop_2)) -> (cf'=stop_system);	// <-- 
	[compare4] (cf=compare4) & (redundancy_model57_cfblk1_0=v) & (x_cfblk1_1_2363574400=ok) & (x_cfblk2_1_3655042369_2_x_cfblk1_1_2363574400_2_y_cfblk2_2797768987_2_y_cfblk1_2758633594_2_y_cfblk3_2836904380=prop_1) -> (cf'=cfblk2_Update_7);	// <-- 
	[compare4] (cf=compare4) & (redundancy_model57_cfblk1_0=v) & ((x_cfblk1_1_2363574400=error) | (x_cfblk2_1_3655042369_2_x_cfblk1_1_2363574400_2_y_cfblk2_2797768987_2_y_cfblk1_2758633594_2_y_cfblk3_2836904380=prop_2)) -> (cf'=cfblk1_Update_6_3);	// <-- 
	//Element check_redandancy5, df inputs ['redundancy_model57_cfblk2_1'], cf transitions ['cfblk2_Update_7_2', 'cfblk5_Update_8']
	[check_redandancy5] (cf=check_redandancy5) & (redundancy_model57_cfblk2_1=no) -> (cf'=cfblk5_Update_8);	// <-- 
	[check_redandancy5] (cf=check_redandancy5) & ((redundancy_model57_cfblk2_1=v)|(redundancy_model57_cfblk2_1=c)) -> (cf'=cfblk2_Update_7_2);	// <-- 
	//Element cfblk2_Update_7_2, df inputs ['y_cfblk4_2876039773', 'x_cfblk2_1_3655042369'], cf transitions ['compare5']
	[cfblk2_Update_7_2] cf=cfblk2_Update_7_2 -> 1.0:(cf'=compare5);
	//Element cfblk2_Update_7_3, df inputs ['y_cfblk4_2876039773', 'x_cfblk2_1_3655042369'], cf transitions ['cfblk5_Update_8']
	[cfblk2_Update_7_3] cf=cfblk2_Update_7_3 -> 1.0:(cf'=cfblk5_Update_8);
	//Element compare5, df inputs ['x_cfblk2_1_3655042369_2_x_cfblk1_1_2363574400_2_y_cfblk2_2797768987_2_y_cfblk1_2758633594_2_y_cfblk3_2836904380', 'redundancy_model57_cfblk2_1', 'x_cfblk2_1_3655042369'], cf transitions ['cfblk2_Update_7_3', 'cfblk5_Update_8', 'stop_system']
	[compare5] (cf=compare5) & (redundancy_model57_cfblk2_1=c) & (x_cfblk2_1_3655042369=ok) & (x_cfblk2_1_3655042369_2_x_cfblk1_1_2363574400_2_y_cfblk2_2797768987_2_y_cfblk1_2758633594_2_y_cfblk3_2836904380=prop_1) -> (cf'=cfblk5_Update_8);	// <-- 
	[compare5] (cf=compare5) & (redundancy_model57_cfblk2_1=c) & ((x_cfblk2_1_3655042369=error) | (x_cfblk2_1_3655042369_2_x_cfblk1_1_2363574400_2_y_cfblk2_2797768987_2_y_cfblk1_2758633594_2_y_cfblk3_2836904380=prop_2)) -> (cf'=stop_system);	// <-- 
	[compare5] (cf=compare5) & (redundancy_model57_cfblk2_1=v) & (x_cfblk2_1_3655042369=ok) & (x_cfblk2_1_3655042369_2_x_cfblk1_1_2363574400_2_y_cfblk2_2797768987_2_y_cfblk1_2758633594_2_y_cfblk3_2836904380=prop_1) -> (cf'=cfblk5_Update_8);	// <-- 
	[compare5] (cf=compare5) & (redundancy_model57_cfblk2_1=v) & ((x_cfblk2_1_3655042369=error) | (x_cfblk2_1_3655042369_2_x_cfblk1_1_2363574400_2_y_cfblk2_2797768987_2_y_cfblk1_2758633594_2_y_cfblk3_2836904380=prop_2)) -> (cf'=cfblk2_Update_7_3);	// <-- 
endmodule
//Error propagation commands
module error_propagation
	y_cfblk1_2758633594 : [0 .. 6] init ok;
	y_cfblk2_2797768987 : [0 .. 6] init ok;
	y_cfblk4_2876039773 : [0 .. 6] init ok;
	x_cfblk1_1_2363574400 : [0 .. 6] init ok;
	x_cfblk2_1_3655042369 : [0 .. 6] init ok;
	x_cfblk5_1_3234478981 : [0 .. 6] init ok;
	redundancy_model57_cfblk1_0 : [0 .. 6] init no;
	redundancy_model57_cfblk2_1 : [0 .. 6] init no;
	redundancy_model57_cfblk3_2 : [0 .. 6] init no;
	y_cfblk3_2836904380_2 : [0 .. 6] init ok;
	redundancy_model57_cfblk4_3 : [0 .. 6] init no;
	y_cfblk4_2876039773_2_y_cfblk5_2915175166 : [0 .. 6] init prop_1;
	x_cfblk2_1_3655042369_2_x_cfblk1_1_2363574400_2_y_cfblk2_2797768987_2_y_cfblk1_2758633594_2_y_cfblk3_2836904380 : [0 .. 6] init prop_1;
	//Element cfblk1_Output_1, df inputs ['x_cfblk1_1_2363574400', 'y_cfblk2_2797768987'], df outputs ['y_cfblk1_2758633594']
	[cfblk1_Output_1] (y_cfblk2_2797768987=ok) & (x_cfblk1_1_2363574400=ok) -> 0.98:(y_cfblk1_2758633594'=ok) + 0.02:(y_cfblk1_2758633594'=error);	// <-- 
	[cfblk1_Output_1] (y_cfblk2_2797768987!=ok) | (x_cfblk1_1_2363574400!=ok) -> (y_cfblk1_2758633594'=error);	// <-- 
	//Element cfblk2_Output_2, df inputs ['y_cfblk4_2876039773', 'x_cfblk2_1_3655042369'], df outputs ['y_cfblk2_2797768987']
	[cfblk2_Output_2] (y_cfblk4_2876039773=ok) & (x_cfblk2_1_3655042369=ok) -> 0.98:(y_cfblk2_2797768987'=ok) + 0.02:(y_cfblk2_2797768987'=error);	// <-- 
	[cfblk2_Output_2] (y_cfblk4_2876039773!=ok) | (x_cfblk2_1_3655042369!=ok) -> (y_cfblk2_2797768987'=error);	// <-- 
	//Element cfblk5_Output_3, df inputs ['x_cfblk5_1_3234478981', 'y_cfblk1_2758633594'], df outputs ['y_cfblk4_2876039773_2_y_cfblk5_2915175166']
	[cfblk5_Output_3] (y_cfblk1_2758633594=ok) & (x_cfblk5_1_3234478981=ok) -> 0.98:(y_cfblk4_2876039773_2_y_cfblk5_2915175166'=prop_1) + 0.02:(y_cfblk4_2876039773_2_y_cfblk5_2915175166'=prop_2);	// <-- 
	[cfblk5_Output_3] (y_cfblk1_2758633594!=ok) | (x_cfblk5_1_3234478981!=ok) -> (y_cfblk4_2876039773_2_y_cfblk5_2915175166'=prop_2);	// <-- 
	//Element cfblk3_Output_4, df inputs ['y_cfblk4_2876039773_2_y_cfblk5_2915175166'], df outputs ['x_cfblk2_1_3655042369_2_x_cfblk1_1_2363574400_2_y_cfblk2_2797768987_2_y_cfblk1_2758633594_2_y_cfblk3_2836904380']
	[cfblk3_Output_4] (y_cfblk4_2876039773_2_y_cfblk5_2915175166=prop_1) -> 0.969003039904:(x_cfblk2_1_3655042369_2_x_cfblk1_1_2363574400_2_y_cfblk2_2797768987_2_y_cfblk1_2758633594_2_y_cfblk3_2836904380'=prop_1) + 0.030996960095999997:(x_cfblk2_1_3655042369_2_x_cfblk1_1_2363574400_2_y_cfblk2_2797768987_2_y_cfblk1_2758633594_2_y_cfblk3_2836904380'=prop_2);	// <-- 
	[cfblk3_Output_4] (y_cfblk4_2876039773_2_y_cfblk5_2915175166!=prop_1) -> (x_cfblk2_1_3655042369_2_x_cfblk1_1_2363574400_2_y_cfblk2_2797768987_2_y_cfblk1_2758633594_2_y_cfblk3_2836904380'=prop_2);	// <-- 
	//Element cfblk4_Output_5, df inputs ['x_cfblk2_1_3655042369_2_x_cfblk1_1_2363574400_2_y_cfblk2_2797768987_2_y_cfblk1_2758633594_2_y_cfblk3_2836904380'], df outputs ['y_cfblk4_2876039773']
	[cfblk4_Output_5] (x_cfblk2_1_3655042369_2_x_cfblk1_1_2363574400_2_y_cfblk2_2797768987_2_y_cfblk1_2758633594_2_y_cfblk3_2836904380=prop_1) -> 0.969003039904:(y_cfblk4_2876039773'=ok) + 0.030996960095999997:(y_cfblk4_2876039773'=error);	// <-- 
	[cfblk4_Output_5] (x_cfblk2_1_3655042369_2_x_cfblk1_1_2363574400_2_y_cfblk2_2797768987_2_y_cfblk1_2758633594_2_y_cfblk3_2836904380!=prop_1) -> (y_cfblk4_2876039773'=error);	// <-- 
	//Element cfblk1_Update_6, df inputs ['x_cfblk1_1_2363574400', 'y_cfblk2_2797768987'], df outputs ['x_cfblk1_1_2363574400']
	[cfblk1_Update_6] (y_cfblk2_2797768987=ok) & (x_cfblk1_1_2363574400=ok) -> 0.98:(x_cfblk1_1_2363574400'=ok) + 0.02:(x_cfblk1_1_2363574400'=error);	// <-- 
	[cfblk1_Update_6] (y_cfblk2_2797768987!=ok) | (x_cfblk1_1_2363574400!=ok) -> (x_cfblk1_1_2363574400'=error);	// <-- 
	//Element cfblk2_Update_7, df inputs ['y_cfblk4_2876039773', 'x_cfblk2_1_3655042369'], df outputs ['x_cfblk2_1_3655042369']
	[cfblk2_Update_7] (y_cfblk4_2876039773=ok) & (x_cfblk2_1_3655042369=ok) -> 0.98:(x_cfblk2_1_3655042369'=ok) + 0.02:(x_cfblk2_1_3655042369'=error);	// <-- 
	[cfblk2_Update_7] (y_cfblk4_2876039773!=ok) | (x_cfblk2_1_3655042369!=ok) -> (x_cfblk2_1_3655042369'=error);	// <-- 
	//Element cfblk5_Update_8, df inputs ['x_cfblk5_1_3234478981', 'y_cfblk1_2758633594'], df outputs ['x_cfblk5_1_3234478981']
	[cfblk5_Update_8] (y_cfblk1_2758633594=ok) & (x_cfblk5_1_3234478981=ok) -> 0.98:(x_cfblk5_1_3234478981'=ok) + 0.02:(x_cfblk5_1_3234478981'=error);	// <-- 
	[cfblk5_Update_8] (y_cfblk1_2758633594!=ok) | (x_cfblk5_1_3234478981!=ok) -> (x_cfblk5_1_3234478981'=error);	// <-- 
	//Element cfblk1_Output_1_2, df inputs ['x_cfblk1_1_2363574400', 'y_cfblk2_2797768987'], df outputs ['x_cfblk2_1_3655042369_2_x_cfblk1_1_2363574400_2_y_cfblk2_2797768987_2_y_cfblk1_2758633594_2_y_cfblk3_2836904380']
	[cfblk1_Output_1_2] (y_cfblk2_2797768987=ok) & (x_cfblk1_1_2363574400=ok) -> 0.98:(x_cfblk2_1_3655042369_2_x_cfblk1_1_2363574400_2_y_cfblk2_2797768987_2_y_cfblk1_2758633594_2_y_cfblk3_2836904380'=prop_1) + 0.02:(x_cfblk2_1_3655042369_2_x_cfblk1_1_2363574400_2_y_cfblk2_2797768987_2_y_cfblk1_2758633594_2_y_cfblk3_2836904380'=prop_2);	// <-- 
	[cfblk1_Output_1_2] (y_cfblk2_2797768987!=ok) | (x_cfblk1_1_2363574400!=ok) -> (x_cfblk2_1_3655042369_2_x_cfblk1_1_2363574400_2_y_cfblk2_2797768987_2_y_cfblk1_2758633594_2_y_cfblk3_2836904380'=prop_2);	// <-- 
	//Element cfblk1_Output_1_3, df inputs ['x_cfblk1_1_2363574400', 'y_cfblk2_2797768987'], df outputs ['y_cfblk1_2758633594']
	[cfblk1_Output_1_3] (y_cfblk2_2797768987=ok) & (x_cfblk1_1_2363574400=ok) -> 0.98:(y_cfblk1_2758633594'=ok) + 0.02:(y_cfblk1_2758633594'=error);	// <-- 
	[cfblk1_Output_1_3] (y_cfblk2_2797768987!=ok) | (x_cfblk1_1_2363574400!=ok) -> (y_cfblk1_2758633594'=error);	// <-- 
	//Element cfblk2_Output_2_2, df inputs ['y_cfblk4_2876039773', 'x_cfblk2_1_3655042369'], df outputs ['x_cfblk2_1_3655042369_2_x_cfblk1_1_2363574400_2_y_cfblk2_2797768987_2_y_cfblk1_2758633594_2_y_cfblk3_2836904380']
	[cfblk2_Output_2_2] (y_cfblk4_2876039773=ok) & (x_cfblk2_1_3655042369=ok) -> 0.98:(x_cfblk2_1_3655042369_2_x_cfblk1_1_2363574400_2_y_cfblk2_2797768987_2_y_cfblk1_2758633594_2_y_cfblk3_2836904380'=prop_1) + 0.02:(x_cfblk2_1_3655042369_2_x_cfblk1_1_2363574400_2_y_cfblk2_2797768987_2_y_cfblk1_2758633594_2_y_cfblk3_2836904380'=prop_2);	// <-- 
	[cfblk2_Output_2_2] (y_cfblk4_2876039773!=ok) | (x_cfblk2_1_3655042369!=ok) -> (x_cfblk2_1_3655042369_2_x_cfblk1_1_2363574400_2_y_cfblk2_2797768987_2_y_cfblk1_2758633594_2_y_cfblk3_2836904380'=prop_2);	// <-- 
	//Element cfblk2_Output_2_3, df inputs ['y_cfblk4_2876039773', 'x_cfblk2_1_3655042369'], df outputs ['y_cfblk2_2797768987']
	[cfblk2_Output_2_3] (y_cfblk4_2876039773=ok) & (x_cfblk2_1_3655042369=ok) -> 0.98:(y_cfblk2_2797768987'=ok) + 0.02:(y_cfblk2_2797768987'=error);	// <-- 
	[cfblk2_Output_2_3] (y_cfblk4_2876039773!=ok) | (x_cfblk2_1_3655042369!=ok) -> (y_cfblk2_2797768987'=error);	// <-- 
	//Element cfblk3_Output_4_2, df inputs ['y_cfblk4_2876039773_2_y_cfblk5_2915175166'], df outputs ['y_cfblk3_2836904380_2']
	[cfblk3_Output_4_2] (y_cfblk4_2876039773_2_y_cfblk5_2915175166=prop_1) -> 0.969003039904:(y_cfblk3_2836904380_2'=ok) + 0.030996960095999997:(y_cfblk3_2836904380_2'=error);	// <-- 
	[cfblk3_Output_4_2] (y_cfblk4_2876039773_2_y_cfblk5_2915175166!=prop_1) -> (y_cfblk3_2836904380_2'=error);	// <-- 
	//Element cfblk3_Output_4_3, df inputs ['y_cfblk4_2876039773_2_y_cfblk5_2915175166'], df outputs ['x_cfblk2_1_3655042369_2_x_cfblk1_1_2363574400_2_y_cfblk2_2797768987_2_y_cfblk1_2758633594_2_y_cfblk3_2836904380']
	[cfblk3_Output_4_3] (y_cfblk4_2876039773_2_y_cfblk5_2915175166=prop_1) -> 0.969003039904:(x_cfblk2_1_3655042369_2_x_cfblk1_1_2363574400_2_y_cfblk2_2797768987_2_y_cfblk1_2758633594_2_y_cfblk3_2836904380'=prop_1) + 0.030996960095999997:(x_cfblk2_1_3655042369_2_x_cfblk1_1_2363574400_2_y_cfblk2_2797768987_2_y_cfblk1_2758633594_2_y_cfblk3_2836904380'=prop_2);	// <-- 
	[cfblk3_Output_4_3] (y_cfblk4_2876039773_2_y_cfblk5_2915175166!=prop_1) -> (x_cfblk2_1_3655042369_2_x_cfblk1_1_2363574400_2_y_cfblk2_2797768987_2_y_cfblk1_2758633594_2_y_cfblk3_2836904380'=prop_2);	// <-- 
	//Element cfblk4_Output_5_2, df inputs ['x_cfblk2_1_3655042369_2_x_cfblk1_1_2363574400_2_y_cfblk2_2797768987_2_y_cfblk1_2758633594_2_y_cfblk3_2836904380'], df outputs ['y_cfblk4_2876039773_2_y_cfblk5_2915175166']
	[cfblk4_Output_5_2] (x_cfblk2_1_3655042369_2_x_cfblk1_1_2363574400_2_y_cfblk2_2797768987_2_y_cfblk1_2758633594_2_y_cfblk3_2836904380=prop_1) -> 0.969003039904:(y_cfblk4_2876039773_2_y_cfblk5_2915175166'=prop_1) + 0.030996960095999997:(y_cfblk4_2876039773_2_y_cfblk5_2915175166'=prop_2);	// <-- 
	[cfblk4_Output_5_2] (x_cfblk2_1_3655042369_2_x_cfblk1_1_2363574400_2_y_cfblk2_2797768987_2_y_cfblk1_2758633594_2_y_cfblk3_2836904380!=prop_1) -> (y_cfblk4_2876039773_2_y_cfblk5_2915175166'=prop_2);	// <-- 
	//Element cfblk4_Output_5_3, df inputs ['x_cfblk2_1_3655042369_2_x_cfblk1_1_2363574400_2_y_cfblk2_2797768987_2_y_cfblk1_2758633594_2_y_cfblk3_2836904380'], df outputs ['y_cfblk4_2876039773']
	[cfblk4_Output_5_3] (x_cfblk2_1_3655042369_2_x_cfblk1_1_2363574400_2_y_cfblk2_2797768987_2_y_cfblk1_2758633594_2_y_cfblk3_2836904380=prop_1) -> 0.969003039904:(y_cfblk4_2876039773'=ok) + 0.030996960095999997:(y_cfblk4_2876039773'=error);	// <-- 
	[cfblk4_Output_5_3] (x_cfblk2_1_3655042369_2_x_cfblk1_1_2363574400_2_y_cfblk2_2797768987_2_y_cfblk1_2758633594_2_y_cfblk3_2836904380!=prop_1) -> (y_cfblk4_2876039773'=error);	// <-- 
	//Element cfblk1_Update_6_2, df inputs ['x_cfblk1_1_2363574400', 'y_cfblk2_2797768987'], df outputs ['x_cfblk2_1_3655042369_2_x_cfblk1_1_2363574400_2_y_cfblk2_2797768987_2_y_cfblk1_2758633594_2_y_cfblk3_2836904380']
	[cfblk1_Update_6_2] (y_cfblk2_2797768987=ok) & (x_cfblk1_1_2363574400=ok) -> 0.98:(x_cfblk2_1_3655042369_2_x_cfblk1_1_2363574400_2_y_cfblk2_2797768987_2_y_cfblk1_2758633594_2_y_cfblk3_2836904380'=prop_1) + 0.02:(x_cfblk2_1_3655042369_2_x_cfblk1_1_2363574400_2_y_cfblk2_2797768987_2_y_cfblk1_2758633594_2_y_cfblk3_2836904380'=prop_2);	// <-- 
	[cfblk1_Update_6_2] (y_cfblk2_2797768987!=ok) | (x_cfblk1_1_2363574400!=ok) -> (x_cfblk2_1_3655042369_2_x_cfblk1_1_2363574400_2_y_cfblk2_2797768987_2_y_cfblk1_2758633594_2_y_cfblk3_2836904380'=prop_2);	// <-- 
	//Element cfblk1_Update_6_3, df inputs ['x_cfblk1_1_2363574400', 'y_cfblk2_2797768987'], df outputs ['x_cfblk1_1_2363574400']
	[cfblk1_Update_6_3] (y_cfblk2_2797768987=ok) & (x_cfblk1_1_2363574400=ok) -> 0.98:(x_cfblk1_1_2363574400'=ok) + 0.02:(x_cfblk1_1_2363574400'=error);	// <-- 
	[cfblk1_Update_6_3] (y_cfblk2_2797768987!=ok) | (x_cfblk1_1_2363574400!=ok) -> (x_cfblk1_1_2363574400'=error);	// <-- 
	//Element cfblk2_Update_7_2, df inputs ['y_cfblk4_2876039773', 'x_cfblk2_1_3655042369'], df outputs ['x_cfblk2_1_3655042369_2_x_cfblk1_1_2363574400_2_y_cfblk2_2797768987_2_y_cfblk1_2758633594_2_y_cfblk3_2836904380']
	[cfblk2_Update_7_2] (y_cfblk4_2876039773=ok) & (x_cfblk2_1_3655042369=ok) -> 0.98:(x_cfblk2_1_3655042369_2_x_cfblk1_1_2363574400_2_y_cfblk2_2797768987_2_y_cfblk1_2758633594_2_y_cfblk3_2836904380'=prop_1) + 0.02:(x_cfblk2_1_3655042369_2_x_cfblk1_1_2363574400_2_y_cfblk2_2797768987_2_y_cfblk1_2758633594_2_y_cfblk3_2836904380'=prop_2);	// <-- 
	[cfblk2_Update_7_2] (y_cfblk4_2876039773!=ok) | (x_cfblk2_1_3655042369!=ok) -> (x_cfblk2_1_3655042369_2_x_cfblk1_1_2363574400_2_y_cfblk2_2797768987_2_y_cfblk1_2758633594_2_y_cfblk3_2836904380'=prop_2);	// <-- 
	//Element cfblk2_Update_7_3, df inputs ['y_cfblk4_2876039773', 'x_cfblk2_1_3655042369'], df outputs ['x_cfblk2_1_3655042369']
	[cfblk2_Update_7_3] (y_cfblk4_2876039773=ok) & (x_cfblk2_1_3655042369=ok) -> 0.98:(x_cfblk2_1_3655042369'=ok) + 0.02:(x_cfblk2_1_3655042369'=error);	// <-- 
	[cfblk2_Update_7_3] (y_cfblk4_2876039773!=ok) | (x_cfblk2_1_3655042369!=ok) -> (x_cfblk2_1_3655042369'=error);	// <-- 
endmodule

//Failure formulas
formula failure = x_cfblk5_1_3234478981 = error;

rewards "time" true : 1; endrewards
