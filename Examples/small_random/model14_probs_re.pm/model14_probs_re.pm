//Generated by ErrorPro
dtmc
//Element consts
const int cfblk2_Output_1=0;
const int cfblk3_Output_2=1;
const int cfblk1_Output_3=2;
const int cfblk5_Output_4=3;
const int cfblk4_Output_5=4;
const int cfblk2_Update_6=5;
const int cfblk1_Update_7=6;
const int check_redandancy0=7;
const int cfblk2_Output_1_2=8;
const int cfblk2_Output_1_3=9;
const int compare0=10;
const int stop_system=11;
const int check_redandancy2=12;
const int cfblk1_Output_3_2=13;
const int cfblk1_Output_3_3=14;
const int compare2=15;
const int check_redandancy4=16;
const int cfblk2_Update_6_2=17;
const int cfblk2_Update_6_3=18;
const int compare4=19;
const int check_redandancy5=20;
const int cfblk1_Update_7_2=21;
const int cfblk1_Update_7_3=22;
const int compare5=23;
//Data values consts
const int v=0;
const int ok=1;
const int no=2;
const int error=3;
const int c=4;
//Control flow commands
module control_flow
	cf:[0..24] init cfblk2_Output_1;
	//Element cfblk2_Output_1, df inputs ['y_cfblk1_2491699714', 'x_cfblk2_1_3436158919'], cf transitions ['check_redandancy0']
	[cfblk2_Output_1] cf=cfblk2_Output_1 -> 1.0:(cf'=check_redandancy0);
	//Element cfblk3_Output_2, df inputs ['y_cfblk2_2530835107'], cf transitions ['cfblk1_Output_3']
	[cfblk3_Output_2] cf=cfblk3_Output_2 -> 1.0:(cf'=cfblk1_Output_3);
	//Element cfblk1_Output_3, df inputs ['y_cfblk2_2530835107', 'x_cfblk1_1_2144690950'], cf transitions ['check_redandancy2']
	[cfblk1_Output_3] cf=cfblk1_Output_3 -> 1.0:(cf'=check_redandancy2);
	//Element cfblk5_Output_4, df inputs ['y_cfblk1_2491699714'], cf transitions ['cfblk4_Output_5']
	[cfblk5_Output_4] cf=cfblk5_Output_4 -> 1.0:(cf'=cfblk4_Output_5);
	//Element cfblk4_Output_5, df inputs ['y_cfblk5_2648241286'], cf transitions ['cfblk2_Update_6']
	[cfblk4_Output_5] cf=cfblk4_Output_5 -> 1.0:(cf'=cfblk2_Update_6);
	//Element cfblk2_Update_6, df inputs ['y_cfblk1_2491699714', 'x_cfblk2_1_3436158919'], cf transitions ['check_redandancy4']
	[cfblk2_Update_6] cf=cfblk2_Update_6 -> 1.0:(cf'=check_redandancy4);
	//Element cfblk1_Update_7, df inputs ['y_cfblk2_2530835107', 'x_cfblk1_1_2144690950'], cf transitions ['check_redandancy5']
	[cfblk1_Update_7] cf=cfblk1_Update_7 -> 1.0:(cf'=check_redandancy5);
	//Element check_redandancy0, df inputs ['redundancy_model14_cfblk2_0'], cf transitions ['cfblk2_Output_1_2', 'cfblk3_Output_2']
	[check_redandancy0] (cf=check_redandancy0) & (redundancy_model14_cfblk2_0=no) -> (cf'=cfblk3_Output_2);	// <-- 
	[check_redandancy0] (cf=check_redandancy0) & ((redundancy_model14_cfblk2_0=v)|(redundancy_model14_cfblk2_0=c)) -> (cf'=cfblk2_Output_1_2);	// <-- 
	//Element cfblk2_Output_1_2, df inputs ['y_cfblk1_2491699714', 'x_cfblk2_1_3436158919'], cf transitions ['compare0']
	[cfblk2_Output_1_2] cf=cfblk2_Output_1_2 -> 1.0:(cf'=compare0);
	//Element cfblk2_Output_1_3, df inputs ['y_cfblk1_2491699714', 'x_cfblk2_1_3436158919'], cf transitions ['cfblk3_Output_2']
	[cfblk2_Output_1_3] cf=cfblk2_Output_1_3 -> 1.0:(cf'=cfblk3_Output_2);
	//Element compare0, df inputs ['redundancy_model14_cfblk2_0', 'y_cfblk2_2530835107_2', 'y_cfblk2_2530835107'], cf transitions ['cfblk2_Output_1_3', 'cfblk3_Output_2', 'stop_system']
	[compare0] (cf=compare0) & (redundancy_model14_cfblk2_0=c) & (y_cfblk2_2530835107=ok) & (y_cfblk2_2530835107_2=ok) -> (cf'=cfblk3_Output_2);	// <-- 
	[compare0] (cf=compare0) & (redundancy_model14_cfblk2_0=c) & ((y_cfblk2_2530835107=error) | (y_cfblk2_2530835107_2=error)) -> (cf'=stop_system);	// <-- 
	[compare0] (cf=compare0) & (redundancy_model14_cfblk2_0=v) & (y_cfblk2_2530835107=ok) & (y_cfblk2_2530835107_2=ok) -> (cf'=cfblk3_Output_2);	// <-- 
	[compare0] (cf=compare0) & (redundancy_model14_cfblk2_0=v) & ((y_cfblk2_2530835107=error) | (y_cfblk2_2530835107_2=error)) -> (cf'=cfblk2_Output_1_3);	// <-- 
	//Element stop_system, cf transitions ['cfblk2_Output_1']
	[stop_system] cf=stop_system -> 1.0:(cf'=cfblk2_Output_1);
	//Element check_redandancy2, df inputs ['redundancy_model14_cfblk1_2'], cf transitions ['cfblk1_Output_3_2', 'cfblk5_Output_4']
	[check_redandancy2] (cf=check_redandancy2) & (redundancy_model14_cfblk1_2=no) -> (cf'=cfblk5_Output_4);	// <-- 
	[check_redandancy2] (cf=check_redandancy2) & ((redundancy_model14_cfblk1_2=v)|(redundancy_model14_cfblk1_2=c)) -> (cf'=cfblk1_Output_3_2);	// <-- 
	//Element cfblk1_Output_3_2, df inputs ['y_cfblk2_2530835107', 'x_cfblk1_1_2144690950'], cf transitions ['compare2']
	[cfblk1_Output_3_2] cf=cfblk1_Output_3_2 -> 1.0:(cf'=compare2);
	//Element cfblk1_Output_3_3, df inputs ['y_cfblk2_2530835107', 'x_cfblk1_1_2144690950'], cf transitions ['cfblk5_Output_4']
	[cfblk1_Output_3_3] cf=cfblk1_Output_3_3 -> 1.0:(cf'=cfblk5_Output_4);
	//Element compare2, df inputs ['redundancy_model14_cfblk1_2', 'y_cfblk1_2491699714_2', 'y_cfblk1_2491699714'], cf transitions ['cfblk1_Output_3_3', 'cfblk5_Output_4', 'stop_system']
	[compare2] (cf=compare2) & (redundancy_model14_cfblk1_2=c) & (y_cfblk1_2491699714=ok) & (y_cfblk1_2491699714_2=ok) -> (cf'=cfblk5_Output_4);	// <-- 
	[compare2] (cf=compare2) & (redundancy_model14_cfblk1_2=c) & ((y_cfblk1_2491699714=error) | (y_cfblk1_2491699714_2=error)) -> (cf'=stop_system);	// <-- 
	[compare2] (cf=compare2) & (redundancy_model14_cfblk1_2=v) & (y_cfblk1_2491699714=ok) & (y_cfblk1_2491699714_2=ok) -> (cf'=cfblk5_Output_4);	// <-- 
	[compare2] (cf=compare2) & (redundancy_model14_cfblk1_2=v) & ((y_cfblk1_2491699714=error) | (y_cfblk1_2491699714_2=error)) -> (cf'=cfblk1_Output_3_3);	// <-- 
	//Element check_redandancy4, df inputs ['redundancy_model14_cfblk2_0'], cf transitions ['cfblk2_Update_6_2', 'cfblk1_Update_7']
	[check_redandancy4] (cf=check_redandancy4) & (redundancy_model14_cfblk2_0=no) -> (cf'=cfblk1_Update_7);	// <-- 
	[check_redandancy4] (cf=check_redandancy4) & ((redundancy_model14_cfblk2_0=v)|(redundancy_model14_cfblk2_0=c)) -> (cf'=cfblk2_Update_6_2);	// <-- 
	//Element cfblk2_Update_6_2, df inputs ['y_cfblk1_2491699714', 'x_cfblk2_1_3436158919'], cf transitions ['compare4']
	[cfblk2_Update_6_2] cf=cfblk2_Update_6_2 -> 1.0:(cf'=compare4);
	//Element cfblk2_Update_6_3, df inputs ['y_cfblk1_2491699714', 'x_cfblk2_1_3436158919'], cf transitions ['cfblk1_Update_7']
	[cfblk2_Update_6_3] cf=cfblk2_Update_6_3 -> 1.0:(cf'=cfblk1_Update_7);
	//Element compare4, df inputs ['redundancy_model14_cfblk2_0', 'x_cfblk2_1_3436158919_2', 'x_cfblk2_1_3436158919'], cf transitions ['cfblk2_Update_6_3', 'cfblk1_Update_7', 'stop_system']
	[compare4] (cf=compare4) & (redundancy_model14_cfblk2_0=c) & (x_cfblk2_1_3436158919=ok) & (x_cfblk2_1_3436158919_2=ok) -> (cf'=cfblk1_Update_7);	// <-- 
	[compare4] (cf=compare4) & (redundancy_model14_cfblk2_0=c) & ((x_cfblk2_1_3436158919=error) | (x_cfblk2_1_3436158919_2=error)) -> (cf'=stop_system);	// <-- 
	[compare4] (cf=compare4) & (redundancy_model14_cfblk2_0=v) & (x_cfblk2_1_3436158919=ok) & (x_cfblk2_1_3436158919_2=ok) -> (cf'=cfblk1_Update_7);	// <-- 
	[compare4] (cf=compare4) & (redundancy_model14_cfblk2_0=v) & ((x_cfblk2_1_3436158919=error) | (x_cfblk2_1_3436158919_2=error)) -> (cf'=cfblk2_Update_6_3);	// <-- 
	//Element check_redandancy5, df inputs ['redundancy_model14_cfblk1_2'], cf transitions ['cfblk1_Update_7_2', 'cfblk2_Output_1']
	[check_redandancy5] (cf=check_redandancy5) & (redundancy_model14_cfblk1_2=no) -> (cf'=cfblk2_Output_1);	// <-- 
	[check_redandancy5] (cf=check_redandancy5) & ((redundancy_model14_cfblk1_2=v)|(redundancy_model14_cfblk1_2=c)) -> (cf'=cfblk1_Update_7_2);	// <-- 
	//Element cfblk1_Update_7_2, df inputs ['y_cfblk2_2530835107', 'x_cfblk1_1_2144690950'], cf transitions ['compare5']
	[cfblk1_Update_7_2] cf=cfblk1_Update_7_2 -> 1.0:(cf'=compare5);
	//Element cfblk1_Update_7_3, df inputs ['y_cfblk2_2530835107', 'x_cfblk1_1_2144690950'], cf transitions ['cfblk2_Output_1']
	[cfblk1_Update_7_3] cf=cfblk1_Update_7_3 -> 1.0:(cf'=cfblk2_Output_1);
	//Element compare5, df inputs ['redundancy_model14_cfblk1_2', 'x_cfblk1_1_2144690950_2', 'x_cfblk1_1_2144690950'], cf transitions ['cfblk1_Update_7_3', 'cfblk2_Output_1', 'stop_system']
	[compare5] (cf=compare5) & (redundancy_model14_cfblk1_2=c) & (x_cfblk1_1_2144690950=ok) & (x_cfblk1_1_2144690950_2=ok) -> (cf'=cfblk2_Output_1);	// <-- 
	[compare5] (cf=compare5) & (redundancy_model14_cfblk1_2=c) & ((x_cfblk1_1_2144690950=error) | (x_cfblk1_1_2144690950_2=error)) -> (cf'=stop_system);	// <-- 
	[compare5] (cf=compare5) & (redundancy_model14_cfblk1_2=v) & (x_cfblk1_1_2144690950=ok) & (x_cfblk1_1_2144690950_2=ok) -> (cf'=cfblk2_Output_1);	// <-- 
	[compare5] (cf=compare5) & (redundancy_model14_cfblk1_2=v) & ((x_cfblk1_1_2144690950=error) | (x_cfblk1_1_2144690950_2=error)) -> (cf'=cfblk1_Update_7_3);	// <-- 
endmodule
//Error propagation commands
module error_propagation
	y_cfblk1_2491699714 : [0 .. 4] init ok;
	y_cfblk2_2530835107 : [0 .. 4] init ok;
	y_cfblk5_2648241286 : [0 .. 4] init ok;
	x_cfblk2_1_3436158919 : [0 .. 4] init ok;
	x_cfblk1_1_2144690950 : [0 .. 4] init ok;
	redundancy_model14_cfblk2_0 : [0 .. 4] init v;
	y_cfblk2_2530835107_2 : [0 .. 4] init ok;
	redundancy_model14_cfblk1_2 : [0 .. 4] init v;
	y_cfblk1_2491699714_2 : [0 .. 4] init ok;
	x_cfblk2_1_3436158919_2 : [0 .. 4] init ok;
	x_cfblk1_1_2144690950_2 : [0 .. 4] init ok;
	//Element cfblk2_Output_1, df inputs ['y_cfblk1_2491699714', 'x_cfblk2_1_3436158919'], df outputs ['y_cfblk2_2530835107']
	[cfblk2_Output_1] (y_cfblk1_2491699714=ok) & (x_cfblk2_1_3436158919=ok) -> 0.98:(y_cfblk2_2530835107'=ok) + 0.02:(y_cfblk2_2530835107'=error);	// <-- 
	[cfblk2_Output_1] (y_cfblk1_2491699714!=ok) | (x_cfblk2_1_3436158919!=ok) -> (y_cfblk2_2530835107'=error);	// <-- 
	//Element cfblk1_Output_3, df inputs ['y_cfblk2_2530835107', 'x_cfblk1_1_2144690950'], df outputs ['y_cfblk1_2491699714']
	[cfblk1_Output_3] (y_cfblk2_2530835107=ok) & (x_cfblk1_1_2144690950=ok) -> 0.98:(y_cfblk1_2491699714'=ok) + 0.02:(y_cfblk1_2491699714'=error);	// <-- 
	[cfblk1_Output_3] (y_cfblk2_2530835107!=ok) | (x_cfblk1_1_2144690950!=ok) -> (y_cfblk1_2491699714'=error);	// <-- 
	//Element cfblk5_Output_4, df inputs ['y_cfblk1_2491699714'], df outputs ['y_cfblk5_2648241286']
	[cfblk5_Output_4] (y_cfblk1_2491699714=ok) -> 0.974002239936:(y_cfblk5_2648241286'=ok) + 0.025997760064:(y_cfblk5_2648241286'=error);	// <-- 
	[cfblk5_Output_4] (y_cfblk1_2491699714!=ok) -> (y_cfblk5_2648241286'=error);	// <-- 
	//Element cfblk2_Update_6, df inputs ['y_cfblk1_2491699714', 'x_cfblk2_1_3436158919'], df outputs ['x_cfblk2_1_3436158919']
	[cfblk2_Update_6] (y_cfblk1_2491699714=ok) & (x_cfblk2_1_3436158919=ok) -> 0.98:(x_cfblk2_1_3436158919'=ok) + 0.02:(x_cfblk2_1_3436158919'=error);	// <-- 
	[cfblk2_Update_6] (y_cfblk1_2491699714!=ok) | (x_cfblk2_1_3436158919!=ok) -> (x_cfblk2_1_3436158919'=error);	// <-- 
	//Element cfblk1_Update_7, df inputs ['y_cfblk2_2530835107', 'x_cfblk1_1_2144690950'], df outputs ['x_cfblk1_1_2144690950']
	[cfblk1_Update_7] (y_cfblk2_2530835107=ok) & (x_cfblk1_1_2144690950=ok) -> 0.98:(x_cfblk1_1_2144690950'=ok) + 0.02:(x_cfblk1_1_2144690950'=error);	// <-- 
	[cfblk1_Update_7] (y_cfblk2_2530835107!=ok) | (x_cfblk1_1_2144690950!=ok) -> (x_cfblk1_1_2144690950'=error);	// <-- 
	//Element cfblk2_Output_1_2, df inputs ['y_cfblk1_2491699714', 'x_cfblk2_1_3436158919'], df outputs ['y_cfblk2_2530835107_2']
	[cfblk2_Output_1_2] (y_cfblk1_2491699714=ok) & (x_cfblk2_1_3436158919=ok) -> 0.98:(y_cfblk2_2530835107_2'=ok) + 0.02:(y_cfblk2_2530835107_2'=error);	// <-- 
	[cfblk2_Output_1_2] (y_cfblk1_2491699714!=ok) | (x_cfblk2_1_3436158919!=ok) -> (y_cfblk2_2530835107_2'=error);	// <-- 
	//Element cfblk2_Output_1_3, df inputs ['y_cfblk1_2491699714', 'x_cfblk2_1_3436158919'], df outputs ['y_cfblk2_2530835107']
	[cfblk2_Output_1_3] (y_cfblk1_2491699714=ok) & (x_cfblk2_1_3436158919=ok) -> 0.98:(y_cfblk2_2530835107'=ok) + 0.02:(y_cfblk2_2530835107'=error);	// <-- 
	[cfblk2_Output_1_3] (y_cfblk1_2491699714!=ok) | (x_cfblk2_1_3436158919!=ok) -> (y_cfblk2_2530835107'=error);	// <-- 
	//Element cfblk1_Output_3_2, df inputs ['y_cfblk2_2530835107', 'x_cfblk1_1_2144690950'], df outputs ['y_cfblk1_2491699714_2']
	[cfblk1_Output_3_2] (y_cfblk2_2530835107=ok) & (x_cfblk1_1_2144690950=ok) -> 0.98:(y_cfblk1_2491699714_2'=ok) + 0.02:(y_cfblk1_2491699714_2'=error);	// <-- 
	[cfblk1_Output_3_2] (y_cfblk2_2530835107!=ok) | (x_cfblk1_1_2144690950!=ok) -> (y_cfblk1_2491699714_2'=error);	// <-- 
	//Element cfblk1_Output_3_3, df inputs ['y_cfblk2_2530835107', 'x_cfblk1_1_2144690950'], df outputs ['y_cfblk1_2491699714']
	[cfblk1_Output_3_3] (y_cfblk2_2530835107=ok) & (x_cfblk1_1_2144690950=ok) -> 0.98:(y_cfblk1_2491699714'=ok) + 0.02:(y_cfblk1_2491699714'=error);	// <-- 
	[cfblk1_Output_3_3] (y_cfblk2_2530835107!=ok) | (x_cfblk1_1_2144690950!=ok) -> (y_cfblk1_2491699714'=error);	// <-- 
	//Element cfblk2_Update_6_2, df inputs ['y_cfblk1_2491699714', 'x_cfblk2_1_3436158919'], df outputs ['x_cfblk2_1_3436158919_2']
	[cfblk2_Update_6_2] (y_cfblk1_2491699714=ok) & (x_cfblk2_1_3436158919=ok) -> 0.98:(x_cfblk2_1_3436158919_2'=ok) + 0.02:(x_cfblk2_1_3436158919_2'=error);	// <-- 
	[cfblk2_Update_6_2] (y_cfblk1_2491699714!=ok) | (x_cfblk2_1_3436158919!=ok) -> (x_cfblk2_1_3436158919_2'=error);	// <-- 
	//Element cfblk2_Update_6_3, df inputs ['y_cfblk1_2491699714', 'x_cfblk2_1_3436158919'], df outputs ['x_cfblk2_1_3436158919']
	[cfblk2_Update_6_3] (y_cfblk1_2491699714=ok) & (x_cfblk2_1_3436158919=ok) -> 0.98:(x_cfblk2_1_3436158919'=ok) + 0.02:(x_cfblk2_1_3436158919'=error);	// <-- 
	[cfblk2_Update_6_3] (y_cfblk1_2491699714!=ok) | (x_cfblk2_1_3436158919!=ok) -> (x_cfblk2_1_3436158919'=error);	// <-- 
	//Element cfblk1_Update_7_2, df inputs ['y_cfblk2_2530835107', 'x_cfblk1_1_2144690950'], df outputs ['x_cfblk1_1_2144690950_2']
	[cfblk1_Update_7_2] (y_cfblk2_2530835107=ok) & (x_cfblk1_1_2144690950=ok) -> 0.98:(x_cfblk1_1_2144690950_2'=ok) + 0.02:(x_cfblk1_1_2144690950_2'=error);	// <-- 
	[cfblk1_Update_7_2] (y_cfblk2_2530835107!=ok) | (x_cfblk1_1_2144690950!=ok) -> (x_cfblk1_1_2144690950_2'=error);	// <-- 
	//Element cfblk1_Update_7_3, df inputs ['y_cfblk2_2530835107', 'x_cfblk1_1_2144690950'], df outputs ['x_cfblk1_1_2144690950']
	[cfblk1_Update_7_3] (y_cfblk2_2530835107=ok) & (x_cfblk1_1_2144690950=ok) -> 0.98:(x_cfblk1_1_2144690950'=ok) + 0.02:(x_cfblk1_1_2144690950'=error);	// <-- 
	[cfblk1_Update_7_3] (y_cfblk2_2530835107!=ok) | (x_cfblk1_1_2144690950!=ok) -> (x_cfblk1_1_2144690950'=error);	// <-- 
endmodule

//Failure formulas
formula failure = x_cfblk1_1_2144690950 = error;

rewards "time" true : 1; endrewards
