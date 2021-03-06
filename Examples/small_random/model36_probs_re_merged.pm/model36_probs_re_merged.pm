//Generated by ErrorPro
dtmc
//Element consts
const int cfblk5_Output_1=0;
const int cfblk4_Output_2=1;
const int cfblk3_Output_3=2;
const int cfblk2_Output_4=3;
const int cfblk1_Output_5=4;
const int cfblk5_Update_6=5;
const int cfblk4_Update_7=6;
const int cfblk1_Update_8=7;
const int check_redandancy0=8;
const int cfblk4_Output_2_2=9;
const int cfblk4_Output_2_3=10;
const int compare0=11;
const int stop_system=12;
const int check_redandancy1=13;
const int cfblk3_Output_3_2=14;
const int cfblk3_Output_3_3=15;
const int compare1=16;
const int check_redandancy3=17;
const int cfblk1_Output_5_2=18;
const int cfblk1_Output_5_3=19;
const int compare3=20;
const int check_redandancy4=21;
const int cfblk4_Update_7_2=22;
const int cfblk4_Update_7_3=23;
const int compare4=24;
const int check_redandancy5=25;
const int cfblk1_Update_8_2=26;
const int cfblk1_Update_8_3=27;
const int compare5=28;
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
	cf:[0..29] init cfblk5_Output_1;
	//Element cfblk5_Output_1, df inputs ['y_cfblk4_3602554936', 'x_cfblk5_1_1439675590'], cf transitions ['cfblk4_Output_2']
	[cfblk5_Output_1] cf=cfblk5_Output_1 -> 1.0:(cf'=cfblk4_Output_2);
	//Element cfblk4_Output_2, df inputs ['x_cfblk4_1_148207621', 'y_cfblk1_3485148757'], cf transitions ['check_redandancy0']
	[cfblk4_Output_2] cf=cfblk4_Output_2 -> 1.0:(cf'=check_redandancy0);
	//Element cfblk3_Output_3, df inputs ['y_cfblk4_3602554936', 'x_cfblk1_1_568771009_2_x_cfblk4_1_148207621_2_y_cfblk1_3485148757_2_y_cfblk5_3641690329'], cf transitions ['check_redandancy1']
	[cfblk3_Output_3] cf=cfblk3_Output_3 -> 1.0:(cf'=check_redandancy1);
	//Element cfblk2_Output_4, df inputs ['y_cfblk4_3602554936_2_y_cfblk3_3563419543'], cf transitions ['cfblk1_Output_5']
	[cfblk2_Output_4] cf=cfblk2_Output_4 -> 1.0:(cf'=cfblk1_Output_5);
	//Element cfblk1_Output_5, df inputs ['x_cfblk1_1_568771009', 'y_cfblk4_3602554936_2_y_cfblk3_3563419543'], cf transitions ['check_redandancy3']
	[cfblk1_Output_5] cf=cfblk1_Output_5 -> 1.0:(cf'=check_redandancy3);
	//Element cfblk5_Update_6, df inputs ['y_cfblk4_3602554936', 'x_cfblk5_1_1439675590'], cf transitions ['cfblk4_Update_7']
	[cfblk5_Update_6] cf=cfblk5_Update_6 -> 1.0:(cf'=cfblk4_Update_7);
	//Element cfblk4_Update_7, df inputs ['x_cfblk4_1_148207621', 'y_cfblk1_3485148757'], cf transitions ['check_redandancy4']
	[cfblk4_Update_7] cf=cfblk4_Update_7 -> 1.0:(cf'=check_redandancy4);
	//Element cfblk1_Update_8, df inputs ['x_cfblk1_1_568771009', 'y_cfblk4_3602554936_2_y_cfblk3_3563419543'], cf transitions ['check_redandancy5']
	[cfblk1_Update_8] cf=cfblk1_Update_8 -> 1.0:(cf'=check_redandancy5);
	//Element check_redandancy0, df inputs ['redundancy_model36_cfblk4_0'], cf transitions ['cfblk4_Output_2_2', 'cfblk3_Output_3']
	[check_redandancy0] (cf=check_redandancy0) & (redundancy_model36_cfblk4_0=no) -> (cf'=cfblk3_Output_3);	// <-- 
	[check_redandancy0] (cf=check_redandancy0) & ((redundancy_model36_cfblk4_0=v)|(redundancy_model36_cfblk4_0=c)) -> (cf'=cfblk4_Output_2_2);	// <-- 
	//Element cfblk4_Output_2_2, df inputs ['x_cfblk4_1_148207621', 'y_cfblk1_3485148757'], cf transitions ['compare0']
	[cfblk4_Output_2_2] cf=cfblk4_Output_2_2 -> 1.0:(cf'=compare0);
	//Element cfblk4_Output_2_3, df inputs ['x_cfblk4_1_148207621', 'y_cfblk1_3485148757'], cf transitions ['cfblk3_Output_3']
	[cfblk4_Output_2_3] cf=cfblk4_Output_2_3 -> 1.0:(cf'=cfblk3_Output_3);
	//Element compare0, df inputs ['y_cfblk4_3602554936_2_y_cfblk3_3563419543', 'redundancy_model36_cfblk4_0', 'y_cfblk4_3602554936'], cf transitions ['cfblk4_Output_2_3', 'cfblk3_Output_3', 'stop_system']
	[compare0] (cf=compare0) & (redundancy_model36_cfblk4_0=c) & (y_cfblk4_3602554936=ok) & (y_cfblk4_3602554936_2_y_cfblk3_3563419543=prop_1) -> (cf'=cfblk3_Output_3);	// <-- 
	[compare0] (cf=compare0) & (redundancy_model36_cfblk4_0=c) & ((y_cfblk4_3602554936=error) | (y_cfblk4_3602554936_2_y_cfblk3_3563419543=prop_2)) -> (cf'=stop_system);	// <-- 
	[compare0] (cf=compare0) & (redundancy_model36_cfblk4_0=v) & (y_cfblk4_3602554936=ok) & (y_cfblk4_3602554936_2_y_cfblk3_3563419543=prop_1) -> (cf'=cfblk3_Output_3);	// <-- 
	[compare0] (cf=compare0) & (redundancy_model36_cfblk4_0=v) & ((y_cfblk4_3602554936=error) | (y_cfblk4_3602554936_2_y_cfblk3_3563419543=prop_2)) -> (cf'=cfblk4_Output_2_3);	// <-- 
	//Element stop_system, cf transitions ['cfblk5_Output_1']
	[stop_system] cf=stop_system -> 1.0:(cf'=cfblk5_Output_1);
	//Element check_redandancy1, df inputs ['redundancy_model36_cfblk3_1'], cf transitions ['cfblk3_Output_3_2', 'cfblk2_Output_4']
	[check_redandancy1] (cf=check_redandancy1) & (redundancy_model36_cfblk3_1=no) -> (cf'=cfblk2_Output_4);	// <-- 
	[check_redandancy1] (cf=check_redandancy1) & ((redundancy_model36_cfblk3_1=v)|(redundancy_model36_cfblk3_1=c)) -> (cf'=cfblk3_Output_3_2);	// <-- 
	//Element cfblk3_Output_3_2, df inputs ['y_cfblk4_3602554936', 'x_cfblk1_1_568771009_2_x_cfblk4_1_148207621_2_y_cfblk1_3485148757_2_y_cfblk5_3641690329'], cf transitions ['compare1']
	[cfblk3_Output_3_2] cf=cfblk3_Output_3_2 -> 1.0:(cf'=compare1);
	//Element cfblk3_Output_3_3, df inputs ['y_cfblk4_3602554936', 'x_cfblk1_1_568771009_2_x_cfblk4_1_148207621_2_y_cfblk1_3485148757_2_y_cfblk5_3641690329'], cf transitions ['cfblk2_Output_4']
	[cfblk3_Output_3_3] cf=cfblk3_Output_3_3 -> 1.0:(cf'=cfblk2_Output_4);
	//Element compare1, df inputs ['y_cfblk4_3602554936_2_y_cfblk3_3563419543', 'redundancy_model36_cfblk3_1', 'y_cfblk3_3563419543_2'], cf transitions ['cfblk3_Output_3_3', 'cfblk2_Output_4', 'stop_system']
	[compare1] (cf=compare1) & (redundancy_model36_cfblk3_1=c) & (y_cfblk4_3602554936_2_y_cfblk3_3563419543=prop_1) & (y_cfblk3_3563419543_2=ok) -> (cf'=cfblk2_Output_4);	// <-- 
	[compare1] (cf=compare1) & (redundancy_model36_cfblk3_1=c) & ((y_cfblk4_3602554936_2_y_cfblk3_3563419543=prop_2) | (y_cfblk3_3563419543_2=error)) -> (cf'=stop_system);	// <-- 
	[compare1] (cf=compare1) & (redundancy_model36_cfblk3_1=v) & (y_cfblk4_3602554936_2_y_cfblk3_3563419543=prop_1) & (y_cfblk3_3563419543_2=ok) -> (cf'=cfblk2_Output_4);	// <-- 
	[compare1] (cf=compare1) & (redundancy_model36_cfblk3_1=v) & ((y_cfblk4_3602554936_2_y_cfblk3_3563419543=prop_2) | (y_cfblk3_3563419543_2=error)) -> (cf'=cfblk3_Output_3_3);	// <-- 
	//Element check_redandancy3, df inputs ['redundancy_model36_cfblk1_3'], cf transitions ['cfblk1_Output_5_2', 'cfblk5_Update_6']
	[check_redandancy3] (cf=check_redandancy3) & (redundancy_model36_cfblk1_3=no) -> (cf'=cfblk5_Update_6);	// <-- 
	[check_redandancy3] (cf=check_redandancy3) & ((redundancy_model36_cfblk1_3=v)|(redundancy_model36_cfblk1_3=c)) -> (cf'=cfblk1_Output_5_2);	// <-- 
	//Element cfblk1_Output_5_2, df inputs ['x_cfblk1_1_568771009', 'y_cfblk4_3602554936_2_y_cfblk3_3563419543'], cf transitions ['compare3']
	[cfblk1_Output_5_2] cf=cfblk1_Output_5_2 -> 1.0:(cf'=compare3);
	//Element cfblk1_Output_5_3, df inputs ['x_cfblk1_1_568771009', 'y_cfblk4_3602554936_2_y_cfblk3_3563419543'], cf transitions ['cfblk5_Update_6']
	[cfblk1_Output_5_3] cf=cfblk1_Output_5_3 -> 1.0:(cf'=cfblk5_Update_6);
	//Element compare3, df inputs ['y_cfblk1_3485148757', 'redundancy_model36_cfblk1_3', 'x_cfblk1_1_568771009_2_x_cfblk4_1_148207621_2_y_cfblk1_3485148757_2_y_cfblk5_3641690329'], cf transitions ['cfblk1_Output_5_3', 'cfblk5_Update_6', 'stop_system']
	[compare3] (cf=compare3) & (redundancy_model36_cfblk1_3=c) & (y_cfblk1_3485148757=ok) & (x_cfblk1_1_568771009_2_x_cfblk4_1_148207621_2_y_cfblk1_3485148757_2_y_cfblk5_3641690329=prop_1) -> (cf'=cfblk5_Update_6);	// <-- 
	[compare3] (cf=compare3) & (redundancy_model36_cfblk1_3=c) & ((y_cfblk1_3485148757=error) | (x_cfblk1_1_568771009_2_x_cfblk4_1_148207621_2_y_cfblk1_3485148757_2_y_cfblk5_3641690329=prop_2)) -> (cf'=stop_system);	// <-- 
	[compare3] (cf=compare3) & (redundancy_model36_cfblk1_3=v) & (y_cfblk1_3485148757=ok) & (x_cfblk1_1_568771009_2_x_cfblk4_1_148207621_2_y_cfblk1_3485148757_2_y_cfblk5_3641690329=prop_1) -> (cf'=cfblk5_Update_6);	// <-- 
	[compare3] (cf=compare3) & (redundancy_model36_cfblk1_3=v) & ((y_cfblk1_3485148757=error) | (x_cfblk1_1_568771009_2_x_cfblk4_1_148207621_2_y_cfblk1_3485148757_2_y_cfblk5_3641690329=prop_2)) -> (cf'=cfblk1_Output_5_3);	// <-- 
	//Element check_redandancy4, df inputs ['redundancy_model36_cfblk4_0'], cf transitions ['cfblk4_Update_7_2', 'cfblk1_Update_8']
	[check_redandancy4] (cf=check_redandancy4) & (redundancy_model36_cfblk4_0=no) -> (cf'=cfblk1_Update_8);	// <-- 
	[check_redandancy4] (cf=check_redandancy4) & ((redundancy_model36_cfblk4_0=v)|(redundancy_model36_cfblk4_0=c)) -> (cf'=cfblk4_Update_7_2);	// <-- 
	//Element cfblk4_Update_7_2, df inputs ['x_cfblk4_1_148207621', 'y_cfblk1_3485148757'], cf transitions ['compare4']
	[cfblk4_Update_7_2] cf=cfblk4_Update_7_2 -> 1.0:(cf'=compare4);
	//Element cfblk4_Update_7_3, df inputs ['x_cfblk4_1_148207621', 'y_cfblk1_3485148757'], cf transitions ['cfblk1_Update_8']
	[cfblk4_Update_7_3] cf=cfblk4_Update_7_3 -> 1.0:(cf'=cfblk1_Update_8);
	//Element compare4, df inputs ['x_cfblk4_1_148207621', 'redundancy_model36_cfblk4_0', 'x_cfblk1_1_568771009_2_x_cfblk4_1_148207621_2_y_cfblk1_3485148757_2_y_cfblk5_3641690329'], cf transitions ['cfblk4_Update_7_3', 'cfblk1_Update_8', 'stop_system']
	[compare4] (cf=compare4) & (redundancy_model36_cfblk4_0=c) & (x_cfblk4_1_148207621=ok) & (x_cfblk1_1_568771009_2_x_cfblk4_1_148207621_2_y_cfblk1_3485148757_2_y_cfblk5_3641690329=prop_1) -> (cf'=cfblk1_Update_8);	// <-- 
	[compare4] (cf=compare4) & (redundancy_model36_cfblk4_0=c) & ((x_cfblk4_1_148207621=error) | (x_cfblk1_1_568771009_2_x_cfblk4_1_148207621_2_y_cfblk1_3485148757_2_y_cfblk5_3641690329=prop_2)) -> (cf'=stop_system);	// <-- 
	[compare4] (cf=compare4) & (redundancy_model36_cfblk4_0=v) & (x_cfblk4_1_148207621=ok) & (x_cfblk1_1_568771009_2_x_cfblk4_1_148207621_2_y_cfblk1_3485148757_2_y_cfblk5_3641690329=prop_1) -> (cf'=cfblk1_Update_8);	// <-- 
	[compare4] (cf=compare4) & (redundancy_model36_cfblk4_0=v) & ((x_cfblk4_1_148207621=error) | (x_cfblk1_1_568771009_2_x_cfblk4_1_148207621_2_y_cfblk1_3485148757_2_y_cfblk5_3641690329=prop_2)) -> (cf'=cfblk4_Update_7_3);	// <-- 
	//Element check_redandancy5, df inputs ['redundancy_model36_cfblk1_3'], cf transitions ['cfblk1_Update_8_2', 'cfblk5_Output_1']
	[check_redandancy5] (cf=check_redandancy5) & (redundancy_model36_cfblk1_3=no) -> (cf'=cfblk5_Output_1);	// <-- 
	[check_redandancy5] (cf=check_redandancy5) & ((redundancy_model36_cfblk1_3=v)|(redundancy_model36_cfblk1_3=c)) -> (cf'=cfblk1_Update_8_2);	// <-- 
	//Element cfblk1_Update_8_2, df inputs ['x_cfblk1_1_568771009', 'y_cfblk4_3602554936_2_y_cfblk3_3563419543'], cf transitions ['compare5']
	[cfblk1_Update_8_2] cf=cfblk1_Update_8_2 -> 1.0:(cf'=compare5);
	//Element cfblk1_Update_8_3, df inputs ['x_cfblk1_1_568771009', 'y_cfblk4_3602554936_2_y_cfblk3_3563419543'], cf transitions ['cfblk5_Output_1']
	[cfblk1_Update_8_3] cf=cfblk1_Update_8_3 -> 1.0:(cf'=cfblk5_Output_1);
	//Element compare5, df inputs ['x_cfblk1_1_568771009', 'redundancy_model36_cfblk1_3', 'x_cfblk1_1_568771009_2_x_cfblk4_1_148207621_2_y_cfblk1_3485148757_2_y_cfblk5_3641690329'], cf transitions ['cfblk1_Update_8_3', 'cfblk5_Output_1', 'stop_system']
	[compare5] (cf=compare5) & (redundancy_model36_cfblk1_3=c) & (x_cfblk1_1_568771009=ok) & (x_cfblk1_1_568771009_2_x_cfblk4_1_148207621_2_y_cfblk1_3485148757_2_y_cfblk5_3641690329=prop_1) -> (cf'=cfblk5_Output_1);	// <-- 
	[compare5] (cf=compare5) & (redundancy_model36_cfblk1_3=c) & ((x_cfblk1_1_568771009=error) | (x_cfblk1_1_568771009_2_x_cfblk4_1_148207621_2_y_cfblk1_3485148757_2_y_cfblk5_3641690329=prop_2)) -> (cf'=stop_system);	// <-- 
	[compare5] (cf=compare5) & (redundancy_model36_cfblk1_3=v) & (x_cfblk1_1_568771009=ok) & (x_cfblk1_1_568771009_2_x_cfblk4_1_148207621_2_y_cfblk1_3485148757_2_y_cfblk5_3641690329=prop_1) -> (cf'=cfblk5_Output_1);	// <-- 
	[compare5] (cf=compare5) & (redundancy_model36_cfblk1_3=v) & ((x_cfblk1_1_568771009=error) | (x_cfblk1_1_568771009_2_x_cfblk4_1_148207621_2_y_cfblk1_3485148757_2_y_cfblk5_3641690329=prop_2)) -> (cf'=cfblk1_Update_8_3);	// <-- 
endmodule
//Error propagation commands
module error_propagation
	y_cfblk1_3485148757 : [0 .. 6] init ok;
	y_cfblk4_3602554936 : [0 .. 6] init ok;
	x_cfblk5_1_1439675590 : [0 .. 6] init ok;
	x_cfblk4_1_148207621 : [0 .. 6] init ok;
	x_cfblk1_1_568771009 : [0 .. 6] init ok;
	redundancy_model36_cfblk4_0 : [0 .. 6] init v;
	redundancy_model36_cfblk3_1 : [0 .. 6] init c;
	y_cfblk3_3563419543_2 : [0 .. 6] init ok;
	redundancy_model36_cfblk1_3 : [0 .. 6] init no;
	y_cfblk4_3602554936_2_y_cfblk3_3563419543 : [0 .. 6] init prop_1;
	x_cfblk1_1_568771009_2_x_cfblk4_1_148207621_2_y_cfblk1_3485148757_2_y_cfblk5_3641690329 : [0 .. 6] init prop_1;
	//Element cfblk5_Output_1, df inputs ['y_cfblk4_3602554936', 'x_cfblk5_1_1439675590'], df outputs ['x_cfblk1_1_568771009_2_x_cfblk4_1_148207621_2_y_cfblk1_3485148757_2_y_cfblk5_3641690329']
	[cfblk5_Output_1] (y_cfblk4_3602554936=ok) & (x_cfblk5_1_1439675590=ok) -> 0.98:(x_cfblk1_1_568771009_2_x_cfblk4_1_148207621_2_y_cfblk1_3485148757_2_y_cfblk5_3641690329'=prop_1) + 0.02:(x_cfblk1_1_568771009_2_x_cfblk4_1_148207621_2_y_cfblk1_3485148757_2_y_cfblk5_3641690329'=prop_2);	// <-- 
	[cfblk5_Output_1] (y_cfblk4_3602554936!=ok) | (x_cfblk5_1_1439675590!=ok) -> (x_cfblk1_1_568771009_2_x_cfblk4_1_148207621_2_y_cfblk1_3485148757_2_y_cfblk5_3641690329'=prop_2);	// <-- 
	//Element cfblk4_Output_2, df inputs ['x_cfblk4_1_148207621', 'y_cfblk1_3485148757'], df outputs ['y_cfblk4_3602554936']
	[cfblk4_Output_2] (y_cfblk1_3485148757=ok) & (x_cfblk4_1_148207621=ok) -> 0.98:(y_cfblk4_3602554936'=ok) + 0.02:(y_cfblk4_3602554936'=error);	// <-- 
	[cfblk4_Output_2] (y_cfblk1_3485148757!=ok) | (x_cfblk4_1_148207621!=ok) -> (y_cfblk4_3602554936'=error);	// <-- 
	//Element cfblk3_Output_3, df inputs ['y_cfblk4_3602554936', 'x_cfblk1_1_568771009_2_x_cfblk4_1_148207621_2_y_cfblk1_3485148757_2_y_cfblk5_3641690329'], df outputs ['y_cfblk4_3602554936_2_y_cfblk3_3563419543']
	[cfblk3_Output_3] (y_cfblk4_3602554936=ok) & (x_cfblk1_1_568771009_2_x_cfblk4_1_148207621_2_y_cfblk1_3485148757_2_y_cfblk5_3641690329=prop_1) -> 0.974002239936:(y_cfblk4_3602554936_2_y_cfblk3_3563419543'=prop_1) + 0.025997760064:(y_cfblk4_3602554936_2_y_cfblk3_3563419543'=prop_2);	// <-- 
	[cfblk3_Output_3] (y_cfblk4_3602554936!=ok) | (x_cfblk1_1_568771009_2_x_cfblk4_1_148207621_2_y_cfblk1_3485148757_2_y_cfblk5_3641690329!=prop_1) -> (y_cfblk4_3602554936_2_y_cfblk3_3563419543'=prop_2);	// <-- 
	//Element cfblk1_Output_5, df inputs ['x_cfblk1_1_568771009', 'y_cfblk4_3602554936_2_y_cfblk3_3563419543'], df outputs ['y_cfblk1_3485148757']
	[cfblk1_Output_5] (y_cfblk4_3602554936_2_y_cfblk3_3563419543=prop_1) & (x_cfblk1_1_568771009=ok) -> 0.98:(y_cfblk1_3485148757'=ok) + 0.02:(y_cfblk1_3485148757'=error);	// <-- 
	[cfblk1_Output_5] (y_cfblk4_3602554936_2_y_cfblk3_3563419543!=prop_1) | (x_cfblk1_1_568771009!=ok) -> (y_cfblk1_3485148757'=error);	// <-- 
	//Element cfblk5_Update_6, df inputs ['y_cfblk4_3602554936', 'x_cfblk5_1_1439675590'], df outputs ['x_cfblk5_1_1439675590']
	[cfblk5_Update_6] (y_cfblk4_3602554936=ok) & (x_cfblk5_1_1439675590=ok) -> 0.98:(x_cfblk5_1_1439675590'=ok) + 0.02:(x_cfblk5_1_1439675590'=error);	// <-- 
	[cfblk5_Update_6] (y_cfblk4_3602554936!=ok) | (x_cfblk5_1_1439675590!=ok) -> (x_cfblk5_1_1439675590'=error);	// <-- 
	//Element cfblk4_Update_7, df inputs ['x_cfblk4_1_148207621', 'y_cfblk1_3485148757'], df outputs ['x_cfblk4_1_148207621']
	[cfblk4_Update_7] (y_cfblk1_3485148757=ok) & (x_cfblk4_1_148207621=ok) -> 0.98:(x_cfblk4_1_148207621'=ok) + 0.02:(x_cfblk4_1_148207621'=error);	// <-- 
	[cfblk4_Update_7] (y_cfblk1_3485148757!=ok) | (x_cfblk4_1_148207621!=ok) -> (x_cfblk4_1_148207621'=error);	// <-- 
	//Element cfblk1_Update_8, df inputs ['x_cfblk1_1_568771009', 'y_cfblk4_3602554936_2_y_cfblk3_3563419543'], df outputs ['x_cfblk1_1_568771009']
	[cfblk1_Update_8] (y_cfblk4_3602554936_2_y_cfblk3_3563419543=prop_1) & (x_cfblk1_1_568771009=ok) -> 0.98:(x_cfblk1_1_568771009'=ok) + 0.02:(x_cfblk1_1_568771009'=error);	// <-- 
	[cfblk1_Update_8] (y_cfblk4_3602554936_2_y_cfblk3_3563419543!=prop_1) | (x_cfblk1_1_568771009!=ok) -> (x_cfblk1_1_568771009'=error);	// <-- 
	//Element cfblk4_Output_2_2, df inputs ['x_cfblk4_1_148207621', 'y_cfblk1_3485148757'], df outputs ['y_cfblk4_3602554936_2_y_cfblk3_3563419543']
	[cfblk4_Output_2_2] (y_cfblk1_3485148757=ok) & (x_cfblk4_1_148207621=ok) -> 0.98:(y_cfblk4_3602554936_2_y_cfblk3_3563419543'=prop_1) + 0.02:(y_cfblk4_3602554936_2_y_cfblk3_3563419543'=prop_2);	// <-- 
	[cfblk4_Output_2_2] (y_cfblk1_3485148757!=ok) | (x_cfblk4_1_148207621!=ok) -> (y_cfblk4_3602554936_2_y_cfblk3_3563419543'=prop_2);	// <-- 
	//Element cfblk4_Output_2_3, df inputs ['x_cfblk4_1_148207621', 'y_cfblk1_3485148757'], df outputs ['y_cfblk4_3602554936']
	[cfblk4_Output_2_3] (y_cfblk1_3485148757=ok) & (x_cfblk4_1_148207621=ok) -> 0.98:(y_cfblk4_3602554936'=ok) + 0.02:(y_cfblk4_3602554936'=error);	// <-- 
	[cfblk4_Output_2_3] (y_cfblk1_3485148757!=ok) | (x_cfblk4_1_148207621!=ok) -> (y_cfblk4_3602554936'=error);	// <-- 
	//Element cfblk3_Output_3_2, df inputs ['y_cfblk4_3602554936', 'x_cfblk1_1_568771009_2_x_cfblk4_1_148207621_2_y_cfblk1_3485148757_2_y_cfblk5_3641690329'], df outputs ['y_cfblk3_3563419543_2']
	[cfblk3_Output_3_2] (y_cfblk4_3602554936=ok) & (x_cfblk1_1_568771009_2_x_cfblk4_1_148207621_2_y_cfblk1_3485148757_2_y_cfblk5_3641690329=prop_1) -> 0.974002239936:(y_cfblk3_3563419543_2'=ok) + 0.025997760064:(y_cfblk3_3563419543_2'=error);	// <-- 
	[cfblk3_Output_3_2] (y_cfblk4_3602554936!=ok) | (x_cfblk1_1_568771009_2_x_cfblk4_1_148207621_2_y_cfblk1_3485148757_2_y_cfblk5_3641690329!=prop_1) -> (y_cfblk3_3563419543_2'=error);	// <-- 
	//Element cfblk3_Output_3_3, df inputs ['y_cfblk4_3602554936', 'x_cfblk1_1_568771009_2_x_cfblk4_1_148207621_2_y_cfblk1_3485148757_2_y_cfblk5_3641690329'], df outputs ['y_cfblk4_3602554936_2_y_cfblk3_3563419543']
	[cfblk3_Output_3_3] (y_cfblk4_3602554936=ok) & (x_cfblk1_1_568771009_2_x_cfblk4_1_148207621_2_y_cfblk1_3485148757_2_y_cfblk5_3641690329=prop_1) -> 0.974002239936:(y_cfblk4_3602554936_2_y_cfblk3_3563419543'=prop_1) + 0.025997760064:(y_cfblk4_3602554936_2_y_cfblk3_3563419543'=prop_2);	// <-- 
	[cfblk3_Output_3_3] (y_cfblk4_3602554936!=ok) | (x_cfblk1_1_568771009_2_x_cfblk4_1_148207621_2_y_cfblk1_3485148757_2_y_cfblk5_3641690329!=prop_1) -> (y_cfblk4_3602554936_2_y_cfblk3_3563419543'=prop_2);	// <-- 
	//Element cfblk1_Output_5_2, df inputs ['x_cfblk1_1_568771009', 'y_cfblk4_3602554936_2_y_cfblk3_3563419543'], df outputs ['x_cfblk1_1_568771009_2_x_cfblk4_1_148207621_2_y_cfblk1_3485148757_2_y_cfblk5_3641690329']
	[cfblk1_Output_5_2] (y_cfblk4_3602554936_2_y_cfblk3_3563419543=prop_1) & (x_cfblk1_1_568771009=ok) -> 0.98:(x_cfblk1_1_568771009_2_x_cfblk4_1_148207621_2_y_cfblk1_3485148757_2_y_cfblk5_3641690329'=prop_1) + 0.02:(x_cfblk1_1_568771009_2_x_cfblk4_1_148207621_2_y_cfblk1_3485148757_2_y_cfblk5_3641690329'=prop_2);	// <-- 
	[cfblk1_Output_5_2] (y_cfblk4_3602554936_2_y_cfblk3_3563419543!=prop_1) | (x_cfblk1_1_568771009!=ok) -> (x_cfblk1_1_568771009_2_x_cfblk4_1_148207621_2_y_cfblk1_3485148757_2_y_cfblk5_3641690329'=prop_2);	// <-- 
	//Element cfblk1_Output_5_3, df inputs ['x_cfblk1_1_568771009', 'y_cfblk4_3602554936_2_y_cfblk3_3563419543'], df outputs ['y_cfblk1_3485148757']
	[cfblk1_Output_5_3] (y_cfblk4_3602554936_2_y_cfblk3_3563419543=prop_1) & (x_cfblk1_1_568771009=ok) -> 0.98:(y_cfblk1_3485148757'=ok) + 0.02:(y_cfblk1_3485148757'=error);	// <-- 
	[cfblk1_Output_5_3] (y_cfblk4_3602554936_2_y_cfblk3_3563419543!=prop_1) | (x_cfblk1_1_568771009!=ok) -> (y_cfblk1_3485148757'=error);	// <-- 
	//Element cfblk4_Update_7_2, df inputs ['x_cfblk4_1_148207621', 'y_cfblk1_3485148757'], df outputs ['x_cfblk1_1_568771009_2_x_cfblk4_1_148207621_2_y_cfblk1_3485148757_2_y_cfblk5_3641690329']
	[cfblk4_Update_7_2] (y_cfblk1_3485148757=ok) & (x_cfblk4_1_148207621=ok) -> 0.98:(x_cfblk1_1_568771009_2_x_cfblk4_1_148207621_2_y_cfblk1_3485148757_2_y_cfblk5_3641690329'=prop_1) + 0.02:(x_cfblk1_1_568771009_2_x_cfblk4_1_148207621_2_y_cfblk1_3485148757_2_y_cfblk5_3641690329'=prop_2);	// <-- 
	[cfblk4_Update_7_2] (y_cfblk1_3485148757!=ok) | (x_cfblk4_1_148207621!=ok) -> (x_cfblk1_1_568771009_2_x_cfblk4_1_148207621_2_y_cfblk1_3485148757_2_y_cfblk5_3641690329'=prop_2);	// <-- 
	//Element cfblk4_Update_7_3, df inputs ['x_cfblk4_1_148207621', 'y_cfblk1_3485148757'], df outputs ['x_cfblk4_1_148207621']
	[cfblk4_Update_7_3] (y_cfblk1_3485148757=ok) & (x_cfblk4_1_148207621=ok) -> 0.98:(x_cfblk4_1_148207621'=ok) + 0.02:(x_cfblk4_1_148207621'=error);	// <-- 
	[cfblk4_Update_7_3] (y_cfblk1_3485148757!=ok) | (x_cfblk4_1_148207621!=ok) -> (x_cfblk4_1_148207621'=error);	// <-- 
	//Element cfblk1_Update_8_2, df inputs ['x_cfblk1_1_568771009', 'y_cfblk4_3602554936_2_y_cfblk3_3563419543'], df outputs ['x_cfblk1_1_568771009_2_x_cfblk4_1_148207621_2_y_cfblk1_3485148757_2_y_cfblk5_3641690329']
	[cfblk1_Update_8_2] (y_cfblk4_3602554936_2_y_cfblk3_3563419543=prop_1) & (x_cfblk1_1_568771009=ok) -> 0.98:(x_cfblk1_1_568771009_2_x_cfblk4_1_148207621_2_y_cfblk1_3485148757_2_y_cfblk5_3641690329'=prop_1) + 0.02:(x_cfblk1_1_568771009_2_x_cfblk4_1_148207621_2_y_cfblk1_3485148757_2_y_cfblk5_3641690329'=prop_2);	// <-- 
	[cfblk1_Update_8_2] (y_cfblk4_3602554936_2_y_cfblk3_3563419543!=prop_1) | (x_cfblk1_1_568771009!=ok) -> (x_cfblk1_1_568771009_2_x_cfblk4_1_148207621_2_y_cfblk1_3485148757_2_y_cfblk5_3641690329'=prop_2);	// <-- 
	//Element cfblk1_Update_8_3, df inputs ['x_cfblk1_1_568771009', 'y_cfblk4_3602554936_2_y_cfblk3_3563419543'], df outputs ['x_cfblk1_1_568771009']
	[cfblk1_Update_8_3] (y_cfblk4_3602554936_2_y_cfblk3_3563419543=prop_1) & (x_cfblk1_1_568771009=ok) -> 0.98:(x_cfblk1_1_568771009'=ok) + 0.02:(x_cfblk1_1_568771009'=error);	// <-- 
	[cfblk1_Update_8_3] (y_cfblk4_3602554936_2_y_cfblk3_3563419543!=prop_1) | (x_cfblk1_1_568771009!=ok) -> (x_cfblk1_1_568771009'=error);	// <-- 
endmodule

//Failure formulas
formula failure = x_cfblk1_1_568771009 = error;

rewards "time" true : 1; endrewards
