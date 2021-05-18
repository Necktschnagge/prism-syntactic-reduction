//Generated by ErrorPro
dtmc
//Element consts
const int cfblk1_Output_1=0;
const int cfblk2_Output_2=1;
const int cfblk5_Output_3=2;
const int cfblk4_Output_4=3;
const int cfblk3_Output_5=4;
const int cfblk1_Update_6=5;
const int check_redandancy0=6;
const int cfblk1_Output_1_2=7;
const int cfblk1_Output_1_3=8;
const int compare0=9;
const int stop_system=10;
const int check_redandancy1=11;
const int cfblk2_Output_2_2=12;
const int cfblk2_Output_2_3=13;
const int compare1=14;
const int check_redandancy2=15;
const int cfblk4_Output_4_2=16;
const int cfblk4_Output_4_3=17;
const int compare2=18;
const int check_redandancy3=19;
const int cfblk3_Output_5_2=20;
const int cfblk3_Output_5_3=21;
const int compare3=22;
const int check_redandancy4=23;
const int cfblk1_Update_6_2=24;
const int cfblk1_Update_6_3=25;
const int compare4=26;
//Data values consts
const int v=0;
const int ok=1;
const int no=2;
const int error=3;
const int c=4;
//Control flow commands
module control_flow
	cf:[0..27] init cfblk1_Output_1;
	//Element cfblk1_Output_1, df inputs ['y_cfblk3_3691943263', 'x_cfblk1_1_515086474'], cf transitions ['check_redandancy0']
	[cfblk1_Output_1] cf=cfblk1_Output_1 -> 1.0:(cf'=check_redandancy0);
	//Element cfblk2_Output_2, cf transitions ['check_redandancy1']
	[cfblk2_Output_2] cf=cfblk2_Output_2 -> 1.0:(cf'=check_redandancy1);
	//Element cfblk5_Output_3, df inputs ['y_cfblk1_3613672477', 'y_cfblk2_3652807870'], cf transitions ['cfblk4_Output_4']
	[cfblk5_Output_3] cf=cfblk5_Output_3 -> 1.0:(cf'=cfblk4_Output_4);
	//Element cfblk4_Output_4, df inputs ['y_cfblk5_3770214049'], cf transitions ['check_redandancy2']
	[cfblk4_Output_4] cf=cfblk4_Output_4 -> 1.0:(cf'=check_redandancy2);
	//Element cfblk3_Output_5, df inputs ['y_cfblk2_3652807870', 'y_cfblk4_3731078656'], cf transitions ['check_redandancy3']
	[cfblk3_Output_5] cf=cfblk3_Output_5 -> 1.0:(cf'=check_redandancy3);
	//Element cfblk1_Update_6, df inputs ['y_cfblk3_3691943263', 'x_cfblk1_1_515086474'], cf transitions ['check_redandancy4']
	[cfblk1_Update_6] cf=cfblk1_Update_6 -> 1.0:(cf'=check_redandancy4);
	//Element check_redandancy0, df inputs ['redundancy_model55_cfblk1_0'], cf transitions ['cfblk1_Output_1_2', 'cfblk2_Output_2']
	[check_redandancy0] (cf=check_redandancy0) & (redundancy_model55_cfblk1_0=no) -> (cf'=cfblk2_Output_2);	// <-- 
	[check_redandancy0] (cf=check_redandancy0) & ((redundancy_model55_cfblk1_0=v)|(redundancy_model55_cfblk1_0=c)) -> (cf'=cfblk1_Output_1_2);	// <-- 
	//Element cfblk1_Output_1_2, df inputs ['y_cfblk3_3691943263', 'x_cfblk1_1_515086474'], cf transitions ['compare0']
	[cfblk1_Output_1_2] cf=cfblk1_Output_1_2 -> 1.0:(cf'=compare0);
	//Element cfblk1_Output_1_3, df inputs ['y_cfblk3_3691943263', 'x_cfblk1_1_515086474'], cf transitions ['cfblk2_Output_2']
	[cfblk1_Output_1_3] cf=cfblk1_Output_1_3 -> 1.0:(cf'=cfblk2_Output_2);
	//Element compare0, df inputs ['redundancy_model55_cfblk1_0', 'y_cfblk1_3613672477_2', 'y_cfblk1_3613672477'], cf transitions ['cfblk1_Output_1_3', 'cfblk2_Output_2', 'stop_system']
	[compare0] (cf=compare0) & (redundancy_model55_cfblk1_0=c) & (y_cfblk1_3613672477=ok) & (y_cfblk1_3613672477_2=ok) -> (cf'=cfblk2_Output_2);	// <-- 
	[compare0] (cf=compare0) & (redundancy_model55_cfblk1_0=c) & ((y_cfblk1_3613672477=error) | (y_cfblk1_3613672477_2=error)) -> (cf'=stop_system);	// <-- 
	[compare0] (cf=compare0) & (redundancy_model55_cfblk1_0=v) & (y_cfblk1_3613672477=ok) & (y_cfblk1_3613672477_2=ok) -> (cf'=cfblk2_Output_2);	// <-- 
	[compare0] (cf=compare0) & (redundancy_model55_cfblk1_0=v) & ((y_cfblk1_3613672477=error) | (y_cfblk1_3613672477_2=error)) -> (cf'=cfblk1_Output_1_3);	// <-- 
	//Element stop_system, cf transitions ['cfblk1_Output_1']
	[stop_system] cf=stop_system -> 1.0:(cf'=cfblk1_Output_1);
	//Element check_redandancy1, df inputs ['redundancy_model55_cfblk2_1'], cf transitions ['cfblk2_Output_2_2', 'cfblk5_Output_3']
	[check_redandancy1] (cf=check_redandancy1) & (redundancy_model55_cfblk2_1=no) -> (cf'=cfblk5_Output_3);	// <-- 
	[check_redandancy1] (cf=check_redandancy1) & ((redundancy_model55_cfblk2_1=v)|(redundancy_model55_cfblk2_1=c)) -> (cf'=cfblk2_Output_2_2);	// <-- 
	//Element cfblk2_Output_2_2, cf transitions ['compare1']
	[cfblk2_Output_2_2] cf=cfblk2_Output_2_2 -> 1.0:(cf'=compare1);
	//Element cfblk2_Output_2_3, cf transitions ['cfblk5_Output_3']
	[cfblk2_Output_2_3] cf=cfblk2_Output_2_3 -> 1.0:(cf'=cfblk5_Output_3);
	//Element compare1, df inputs ['redundancy_model55_cfblk2_1', 'y_cfblk2_3652807870_2', 'y_cfblk2_3652807870'], cf transitions ['cfblk2_Output_2_3', 'cfblk5_Output_3', 'stop_system']
	[compare1] (cf=compare1) & (redundancy_model55_cfblk2_1=c) & (y_cfblk2_3652807870=ok) & (y_cfblk2_3652807870_2=ok) -> (cf'=cfblk5_Output_3);	// <-- 
	[compare1] (cf=compare1) & (redundancy_model55_cfblk2_1=c) & ((y_cfblk2_3652807870=error) | (y_cfblk2_3652807870_2=error)) -> (cf'=stop_system);	// <-- 
	[compare1] (cf=compare1) & (redundancy_model55_cfblk2_1=v) & (y_cfblk2_3652807870=ok) & (y_cfblk2_3652807870_2=ok) -> (cf'=cfblk5_Output_3);	// <-- 
	[compare1] (cf=compare1) & (redundancy_model55_cfblk2_1=v) & ((y_cfblk2_3652807870=error) | (y_cfblk2_3652807870_2=error)) -> (cf'=cfblk2_Output_2_3);	// <-- 
	//Element check_redandancy2, df inputs ['redundancy_model55_cfblk4_2'], cf transitions ['cfblk4_Output_4_2', 'cfblk3_Output_5']
	[check_redandancy2] (cf=check_redandancy2) & (redundancy_model55_cfblk4_2=no) -> (cf'=cfblk3_Output_5);	// <-- 
	[check_redandancy2] (cf=check_redandancy2) & ((redundancy_model55_cfblk4_2=v)|(redundancy_model55_cfblk4_2=c)) -> (cf'=cfblk4_Output_4_2);	// <-- 
	//Element cfblk4_Output_4_2, df inputs ['y_cfblk5_3770214049'], cf transitions ['compare2']
	[cfblk4_Output_4_2] cf=cfblk4_Output_4_2 -> 1.0:(cf'=compare2);
	//Element cfblk4_Output_4_3, df inputs ['y_cfblk5_3770214049'], cf transitions ['cfblk3_Output_5']
	[cfblk4_Output_4_3] cf=cfblk4_Output_4_3 -> 1.0:(cf'=cfblk3_Output_5);
	//Element compare2, df inputs ['redundancy_model55_cfblk4_2', 'y_cfblk4_3731078656_2', 'y_cfblk4_3731078656'], cf transitions ['cfblk4_Output_4_3', 'cfblk3_Output_5', 'stop_system']
	[compare2] (cf=compare2) & (redundancy_model55_cfblk4_2=c) & (y_cfblk4_3731078656=ok) & (y_cfblk4_3731078656_2=ok) -> (cf'=cfblk3_Output_5);	// <-- 
	[compare2] (cf=compare2) & (redundancy_model55_cfblk4_2=c) & ((y_cfblk4_3731078656=error) | (y_cfblk4_3731078656_2=error)) -> (cf'=stop_system);	// <-- 
	[compare2] (cf=compare2) & (redundancy_model55_cfblk4_2=v) & (y_cfblk4_3731078656=ok) & (y_cfblk4_3731078656_2=ok) -> (cf'=cfblk3_Output_5);	// <-- 
	[compare2] (cf=compare2) & (redundancy_model55_cfblk4_2=v) & ((y_cfblk4_3731078656=error) | (y_cfblk4_3731078656_2=error)) -> (cf'=cfblk4_Output_4_3);	// <-- 
	//Element check_redandancy3, df inputs ['redundancy_model55_cfblk3_3'], cf transitions ['cfblk3_Output_5_2', 'cfblk1_Update_6']
	[check_redandancy3] (cf=check_redandancy3) & (redundancy_model55_cfblk3_3=no) -> (cf'=cfblk1_Update_6);	// <-- 
	[check_redandancy3] (cf=check_redandancy3) & ((redundancy_model55_cfblk3_3=v)|(redundancy_model55_cfblk3_3=c)) -> (cf'=cfblk3_Output_5_2);	// <-- 
	//Element cfblk3_Output_5_2, df inputs ['y_cfblk2_3652807870', 'y_cfblk4_3731078656'], cf transitions ['compare3']
	[cfblk3_Output_5_2] cf=cfblk3_Output_5_2 -> 1.0:(cf'=compare3);
	//Element cfblk3_Output_5_3, df inputs ['y_cfblk2_3652807870', 'y_cfblk4_3731078656'], cf transitions ['cfblk1_Update_6']
	[cfblk3_Output_5_3] cf=cfblk3_Output_5_3 -> 1.0:(cf'=cfblk1_Update_6);
	//Element compare3, df inputs ['redundancy_model55_cfblk3_3', 'y_cfblk3_3691943263_2', 'y_cfblk3_3691943263'], cf transitions ['cfblk3_Output_5_3', 'cfblk1_Update_6', 'stop_system']
	[compare3] (cf=compare3) & (redundancy_model55_cfblk3_3=c) & (y_cfblk3_3691943263=ok) & (y_cfblk3_3691943263_2=ok) -> (cf'=cfblk1_Update_6);	// <-- 
	[compare3] (cf=compare3) & (redundancy_model55_cfblk3_3=c) & ((y_cfblk3_3691943263=error) | (y_cfblk3_3691943263_2=error)) -> (cf'=stop_system);	// <-- 
	[compare3] (cf=compare3) & (redundancy_model55_cfblk3_3=v) & (y_cfblk3_3691943263=ok) & (y_cfblk3_3691943263_2=ok) -> (cf'=cfblk1_Update_6);	// <-- 
	[compare3] (cf=compare3) & (redundancy_model55_cfblk3_3=v) & ((y_cfblk3_3691943263=error) | (y_cfblk3_3691943263_2=error)) -> (cf'=cfblk3_Output_5_3);	// <-- 
	//Element check_redandancy4, df inputs ['redundancy_model55_cfblk1_0'], cf transitions ['cfblk1_Update_6_2', 'cfblk1_Output_1']
	[check_redandancy4] (cf=check_redandancy4) & (redundancy_model55_cfblk1_0=no) -> (cf'=cfblk1_Output_1);	// <-- 
	[check_redandancy4] (cf=check_redandancy4) & ((redundancy_model55_cfblk1_0=v)|(redundancy_model55_cfblk1_0=c)) -> (cf'=cfblk1_Update_6_2);	// <-- 
	//Element cfblk1_Update_6_2, df inputs ['y_cfblk3_3691943263', 'x_cfblk1_1_515086474'], cf transitions ['compare4']
	[cfblk1_Update_6_2] cf=cfblk1_Update_6_2 -> 1.0:(cf'=compare4);
	//Element cfblk1_Update_6_3, df inputs ['y_cfblk3_3691943263', 'x_cfblk1_1_515086474'], cf transitions ['cfblk1_Output_1']
	[cfblk1_Update_6_3] cf=cfblk1_Update_6_3 -> 1.0:(cf'=cfblk1_Output_1);
	//Element compare4, df inputs ['redundancy_model55_cfblk1_0', 'x_cfblk1_1_515086474_2', 'x_cfblk1_1_515086474'], cf transitions ['cfblk1_Update_6_3', 'cfblk1_Output_1', 'stop_system']
	[compare4] (cf=compare4) & (redundancy_model55_cfblk1_0=c) & (x_cfblk1_1_515086474=ok) & (x_cfblk1_1_515086474_2=ok) -> (cf'=cfblk1_Output_1);	// <-- 
	[compare4] (cf=compare4) & (redundancy_model55_cfblk1_0=c) & ((x_cfblk1_1_515086474=error) | (x_cfblk1_1_515086474_2=error)) -> (cf'=stop_system);	// <-- 
	[compare4] (cf=compare4) & (redundancy_model55_cfblk1_0=v) & (x_cfblk1_1_515086474=ok) & (x_cfblk1_1_515086474_2=ok) -> (cf'=cfblk1_Output_1);	// <-- 
	[compare4] (cf=compare4) & (redundancy_model55_cfblk1_0=v) & ((x_cfblk1_1_515086474=error) | (x_cfblk1_1_515086474_2=error)) -> (cf'=cfblk1_Update_6_3);	// <-- 
endmodule
//Error propagation commands
module error_propagation
	y_cfblk1_3613672477 : [0 .. 4] init ok;
	y_cfblk2_3652807870 : [0 .. 4] init ok;
	y_cfblk3_3691943263 : [0 .. 4] init ok;
	y_cfblk4_3731078656 : [0 .. 4] init ok;
	y_cfblk5_3770214049 : [0 .. 4] init ok;
	x_cfblk1_1_515086474 : [0 .. 4] init ok;
	redundancy_model55_cfblk1_0 : [0 .. 4] init no;
	y_cfblk1_3613672477_2 : [0 .. 4] init ok;
	redundancy_model55_cfblk2_1 : [0 .. 4] init v;
	y_cfblk2_3652807870_2 : [0 .. 4] init ok;
	redundancy_model55_cfblk4_2 : [0 .. 4] init c;
	y_cfblk4_3731078656_2 : [0 .. 4] init ok;
	redundancy_model55_cfblk3_3 : [0 .. 4] init no;
	y_cfblk3_3691943263_2 : [0 .. 4] init ok;
	x_cfblk1_1_515086474_2 : [0 .. 4] init ok;
	//Element cfblk1_Output_1, df inputs ['y_cfblk3_3691943263', 'x_cfblk1_1_515086474'], df outputs ['y_cfblk1_3613672477']
	[cfblk1_Output_1] (y_cfblk3_3691943263=ok) & (x_cfblk1_1_515086474=ok) -> 0.98:(y_cfblk1_3613672477'=ok) + 0.02:(y_cfblk1_3613672477'=error);	// <-- 
	[cfblk1_Output_1] (y_cfblk3_3691943263!=ok) | (x_cfblk1_1_515086474!=ok) -> (y_cfblk1_3613672477'=error);	// <-- 
	//Element cfblk2_Output_2, df outputs ['y_cfblk2_3652807870']
	[cfblk2_Output_2] (true) -> 1:(y_cfblk2_3652807870'=ok);	// <-- 
	//Element cfblk5_Output_3, df inputs ['y_cfblk1_3613672477', 'y_cfblk2_3652807870'], df outputs ['y_cfblk5_3770214049']
	[cfblk5_Output_3] (y_cfblk1_3613672477=ok) & (y_cfblk2_3652807870=ok) -> 0.98:(y_cfblk5_3770214049'=ok) + 0.02:(y_cfblk5_3770214049'=error);	// <-- 
	[cfblk5_Output_3] (y_cfblk1_3613672477!=ok) | (y_cfblk2_3652807870!=ok) -> (y_cfblk5_3770214049'=error);	// <-- 
	//Element cfblk4_Output_4, df inputs ['y_cfblk5_3770214049'], df outputs ['y_cfblk4_3731078656']
	[cfblk4_Output_4] (y_cfblk5_3770214049=ok) -> 0.969003039904:(y_cfblk4_3731078656'=ok) + 0.030996960095999997:(y_cfblk4_3731078656'=error);	// <-- 
	[cfblk4_Output_4] (y_cfblk5_3770214049!=ok) -> (y_cfblk4_3731078656'=error);	// <-- 
	//Element cfblk3_Output_5, df inputs ['y_cfblk2_3652807870', 'y_cfblk4_3731078656'], df outputs ['y_cfblk3_3691943263']
	[cfblk3_Output_5] (y_cfblk2_3652807870=ok) & (y_cfblk4_3731078656=ok) -> 0.974002239936:(y_cfblk3_3691943263'=ok) + 0.025997760064:(y_cfblk3_3691943263'=error);	// <-- 
	[cfblk3_Output_5] (y_cfblk2_3652807870!=ok) | (y_cfblk4_3731078656!=ok) -> (y_cfblk3_3691943263'=error);	// <-- 
	//Element cfblk1_Update_6, df inputs ['y_cfblk3_3691943263', 'x_cfblk1_1_515086474'], df outputs ['x_cfblk1_1_515086474']
	[cfblk1_Update_6] (y_cfblk3_3691943263=ok) & (x_cfblk1_1_515086474=ok) -> 0.98:(x_cfblk1_1_515086474'=ok) + 0.02:(x_cfblk1_1_515086474'=error);	// <-- 
	[cfblk1_Update_6] (y_cfblk3_3691943263!=ok) | (x_cfblk1_1_515086474!=ok) -> (x_cfblk1_1_515086474'=error);	// <-- 
	//Element cfblk1_Output_1_2, df inputs ['y_cfblk3_3691943263', 'x_cfblk1_1_515086474'], df outputs ['y_cfblk1_3613672477_2']
	[cfblk1_Output_1_2] (y_cfblk3_3691943263=ok) & (x_cfblk1_1_515086474=ok) -> 0.98:(y_cfblk1_3613672477_2'=ok) + 0.02:(y_cfblk1_3613672477_2'=error);	// <-- 
	[cfblk1_Output_1_2] (y_cfblk3_3691943263!=ok) | (x_cfblk1_1_515086474!=ok) -> (y_cfblk1_3613672477_2'=error);	// <-- 
	//Element cfblk1_Output_1_3, df inputs ['y_cfblk3_3691943263', 'x_cfblk1_1_515086474'], df outputs ['y_cfblk1_3613672477']
	[cfblk1_Output_1_3] (y_cfblk3_3691943263=ok) & (x_cfblk1_1_515086474=ok) -> 0.98:(y_cfblk1_3613672477'=ok) + 0.02:(y_cfblk1_3613672477'=error);	// <-- 
	[cfblk1_Output_1_3] (y_cfblk3_3691943263!=ok) | (x_cfblk1_1_515086474!=ok) -> (y_cfblk1_3613672477'=error);	// <-- 
	//Element cfblk2_Output_2_2, df outputs ['y_cfblk2_3652807870_2']
	[cfblk2_Output_2_2] (true) -> 1:(y_cfblk2_3652807870_2'=ok);	// <-- 
	//Element cfblk2_Output_2_3, df outputs ['y_cfblk2_3652807870']
	[cfblk2_Output_2_3] (true) -> 1:(y_cfblk2_3652807870'=ok);	// <-- 
	//Element cfblk4_Output_4_2, df inputs ['y_cfblk5_3770214049'], df outputs ['y_cfblk4_3731078656_2']
	[cfblk4_Output_4_2] (y_cfblk5_3770214049=ok) -> 0.969003039904:(y_cfblk4_3731078656_2'=ok) + 0.030996960095999997:(y_cfblk4_3731078656_2'=error);	// <-- 
	[cfblk4_Output_4_2] (y_cfblk5_3770214049!=ok) -> (y_cfblk4_3731078656_2'=error);	// <-- 
	//Element cfblk4_Output_4_3, df inputs ['y_cfblk5_3770214049'], df outputs ['y_cfblk4_3731078656']
	[cfblk4_Output_4_3] (y_cfblk5_3770214049=ok) -> 0.969003039904:(y_cfblk4_3731078656'=ok) + 0.030996960095999997:(y_cfblk4_3731078656'=error);	// <-- 
	[cfblk4_Output_4_3] (y_cfblk5_3770214049!=ok) -> (y_cfblk4_3731078656'=error);	// <-- 
	//Element cfblk3_Output_5_2, df inputs ['y_cfblk2_3652807870', 'y_cfblk4_3731078656'], df outputs ['y_cfblk3_3691943263_2']
	[cfblk3_Output_5_2] (y_cfblk2_3652807870=ok) & (y_cfblk4_3731078656=ok) -> 0.974002239936:(y_cfblk3_3691943263_2'=ok) + 0.025997760064:(y_cfblk3_3691943263_2'=error);	// <-- 
	[cfblk3_Output_5_2] (y_cfblk2_3652807870!=ok) | (y_cfblk4_3731078656!=ok) -> (y_cfblk3_3691943263_2'=error);	// <-- 
	//Element cfblk3_Output_5_3, df inputs ['y_cfblk2_3652807870', 'y_cfblk4_3731078656'], df outputs ['y_cfblk3_3691943263']
	[cfblk3_Output_5_3] (y_cfblk2_3652807870=ok) & (y_cfblk4_3731078656=ok) -> 0.974002239936:(y_cfblk3_3691943263'=ok) + 0.025997760064:(y_cfblk3_3691943263'=error);	// <-- 
	[cfblk3_Output_5_3] (y_cfblk2_3652807870!=ok) | (y_cfblk4_3731078656!=ok) -> (y_cfblk3_3691943263'=error);	// <-- 
	//Element cfblk1_Update_6_2, df inputs ['y_cfblk3_3691943263', 'x_cfblk1_1_515086474'], df outputs ['x_cfblk1_1_515086474_2']
	[cfblk1_Update_6_2] (y_cfblk3_3691943263=ok) & (x_cfblk1_1_515086474=ok) -> 0.98:(x_cfblk1_1_515086474_2'=ok) + 0.02:(x_cfblk1_1_515086474_2'=error);	// <-- 
	[cfblk1_Update_6_2] (y_cfblk3_3691943263!=ok) | (x_cfblk1_1_515086474!=ok) -> (x_cfblk1_1_515086474_2'=error);	// <-- 
	//Element cfblk1_Update_6_3, df inputs ['y_cfblk3_3691943263', 'x_cfblk1_1_515086474'], df outputs ['x_cfblk1_1_515086474']
	[cfblk1_Update_6_3] (y_cfblk3_3691943263=ok) & (x_cfblk1_1_515086474=ok) -> 0.98:(x_cfblk1_1_515086474'=ok) + 0.02:(x_cfblk1_1_515086474'=error);	// <-- 
	[cfblk1_Update_6_3] (y_cfblk3_3691943263!=ok) | (x_cfblk1_1_515086474!=ok) -> (x_cfblk1_1_515086474'=error);	// <-- 
endmodule

//Failure formulas
formula failure = x_cfblk1_1_515086474 = error;

rewards "time" true : 1; endrewards
