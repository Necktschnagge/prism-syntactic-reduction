//Generated by ErrorPro
dtmc
//Element consts
const int cfblk2_Output_1=0;
const int cfblk5_Output_2=1;
const int cfblk3_Output_3=2;
const int cfblk1_Output_4=3;
const int cfblk4_Output_5=4;
const int cfblk5_Update_6=5;
const int check_redandancy0=6;
const int cfblk2_Output_1_2=7;
const int cfblk2_Output_1_3=8;
const int compare0=9;
const int stop_system=10;
const int check_redandancy1=11;
const int cfblk3_Output_3_2=12;
const int cfblk3_Output_3_3=13;
const int compare1=14;
const int check_redandancy3=15;
const int cfblk4_Output_5_2=16;
const int cfblk4_Output_5_3=17;
const int compare3=18;
//Data values consts
const int v=0;
const int ok=1;
const int no=2;
const int error=3;
const int c=4;
//Control flow commands
module control_flow
	cf:[0..19] init cfblk2_Output_1;
	//Element cfblk2_Output_1, cf transitions ['check_redandancy0']
	[cfblk2_Output_1] cf=cfblk2_Output_1 -> 1.0:(cf'=check_redandancy0);
	//Element cfblk5_Output_2, df inputs ['y_cfblk4_1951795810', 'x_cfblk5_1_2799199267'], cf transitions ['cfblk3_Output_3']
	[cfblk5_Output_2] cf=cfblk5_Output_2 -> 1.0:(cf'=cfblk3_Output_3);
	//Element cfblk3_Output_3, df inputs ['y_cfblk2_1873525024', 'y_cfblk5_1990931203'], cf transitions ['check_redandancy1']
	[cfblk3_Output_3] cf=cfblk3_Output_3 -> 1.0:(cf'=check_redandancy1);
	//Element cfblk1_Output_4, df inputs ['y_cfblk3_1912660417'], cf transitions ['cfblk4_Output_5']
	[cfblk1_Output_4] cf=cfblk1_Output_4 -> 1.0:(cf'=cfblk4_Output_5);
	//Element cfblk4_Output_5, df inputs ['y_cfblk5_1990931203'], cf transitions ['check_redandancy3']
	[cfblk4_Output_5] cf=cfblk4_Output_5 -> 1.0:(cf'=check_redandancy3);
	//Element cfblk5_Update_6, df inputs ['y_cfblk4_1951795810', 'x_cfblk5_1_2799199267'], cf transitions ['cfblk2_Output_1']
	[cfblk5_Update_6] cf=cfblk5_Update_6 -> 1.0:(cf'=cfblk2_Output_1);
	//Element check_redandancy0, df inputs ['redundancy_model47_cfblk2_0'], cf transitions ['cfblk2_Output_1_2', 'cfblk5_Output_2']
	[check_redandancy0] (cf=check_redandancy0) & (redundancy_model47_cfblk2_0=no) -> (cf'=cfblk5_Output_2);	// <-- 
	[check_redandancy0] (cf=check_redandancy0) & ((redundancy_model47_cfblk2_0=v)|(redundancy_model47_cfblk2_0=c)) -> (cf'=cfblk2_Output_1_2);	// <-- 
	//Element cfblk2_Output_1_2, cf transitions ['compare0']
	[cfblk2_Output_1_2] cf=cfblk2_Output_1_2 -> 1.0:(cf'=compare0);
	//Element cfblk2_Output_1_3, cf transitions ['cfblk5_Output_2']
	[cfblk2_Output_1_3] cf=cfblk2_Output_1_3 -> 1.0:(cf'=cfblk5_Output_2);
	//Element compare0, df inputs ['redundancy_model47_cfblk2_0', 'y_cfblk2_1873525024_2', 'y_cfblk2_1873525024'], cf transitions ['cfblk2_Output_1_3', 'cfblk5_Output_2', 'stop_system']
	[compare0] (cf=compare0) & (redundancy_model47_cfblk2_0=c) & (y_cfblk2_1873525024=ok) & (y_cfblk2_1873525024_2=ok) -> (cf'=cfblk5_Output_2);	// <-- 
	[compare0] (cf=compare0) & (redundancy_model47_cfblk2_0=c) & ((y_cfblk2_1873525024=error) | (y_cfblk2_1873525024_2=error)) -> (cf'=stop_system);	// <-- 
	[compare0] (cf=compare0) & (redundancy_model47_cfblk2_0=v) & (y_cfblk2_1873525024=ok) & (y_cfblk2_1873525024_2=ok) -> (cf'=cfblk5_Output_2);	// <-- 
	[compare0] (cf=compare0) & (redundancy_model47_cfblk2_0=v) & ((y_cfblk2_1873525024=error) | (y_cfblk2_1873525024_2=error)) -> (cf'=cfblk2_Output_1_3);	// <-- 
	//Element stop_system, cf transitions ['cfblk2_Output_1']
	[stop_system] cf=stop_system -> 1.0:(cf'=cfblk2_Output_1);
	//Element check_redandancy1, df inputs ['redundancy_model47_cfblk3_1'], cf transitions ['cfblk3_Output_3_2', 'cfblk1_Output_4']
	[check_redandancy1] (cf=check_redandancy1) & (redundancy_model47_cfblk3_1=no) -> (cf'=cfblk1_Output_4);	// <-- 
	[check_redandancy1] (cf=check_redandancy1) & ((redundancy_model47_cfblk3_1=v)|(redundancy_model47_cfblk3_1=c)) -> (cf'=cfblk3_Output_3_2);	// <-- 
	//Element cfblk3_Output_3_2, df inputs ['y_cfblk2_1873525024', 'y_cfblk5_1990931203'], cf transitions ['compare1']
	[cfblk3_Output_3_2] cf=cfblk3_Output_3_2 -> 1.0:(cf'=compare1);
	//Element cfblk3_Output_3_3, df inputs ['y_cfblk2_1873525024', 'y_cfblk5_1990931203'], cf transitions ['cfblk1_Output_4']
	[cfblk3_Output_3_3] cf=cfblk3_Output_3_3 -> 1.0:(cf'=cfblk1_Output_4);
	//Element compare1, df inputs ['redundancy_model47_cfblk3_1', 'y_cfblk3_1912660417_2', 'y_cfblk3_1912660417'], cf transitions ['cfblk3_Output_3_3', 'cfblk1_Output_4', 'stop_system']
	[compare1] (cf=compare1) & (redundancy_model47_cfblk3_1=c) & (y_cfblk3_1912660417=ok) & (y_cfblk3_1912660417_2=ok) -> (cf'=cfblk1_Output_4);	// <-- 
	[compare1] (cf=compare1) & (redundancy_model47_cfblk3_1=c) & ((y_cfblk3_1912660417=error) | (y_cfblk3_1912660417_2=error)) -> (cf'=stop_system);	// <-- 
	[compare1] (cf=compare1) & (redundancy_model47_cfblk3_1=v) & (y_cfblk3_1912660417=ok) & (y_cfblk3_1912660417_2=ok) -> (cf'=cfblk1_Output_4);	// <-- 
	[compare1] (cf=compare1) & (redundancy_model47_cfblk3_1=v) & ((y_cfblk3_1912660417=error) | (y_cfblk3_1912660417_2=error)) -> (cf'=cfblk3_Output_3_3);	// <-- 
	//Element check_redandancy3, df inputs ['redundancy_model47_cfblk4_3'], cf transitions ['cfblk4_Output_5_2', 'cfblk5_Update_6']
	[check_redandancy3] (cf=check_redandancy3) & (redundancy_model47_cfblk4_3=no) -> (cf'=cfblk5_Update_6);	// <-- 
	[check_redandancy3] (cf=check_redandancy3) & ((redundancy_model47_cfblk4_3=v)|(redundancy_model47_cfblk4_3=c)) -> (cf'=cfblk4_Output_5_2);	// <-- 
	//Element cfblk4_Output_5_2, df inputs ['y_cfblk5_1990931203'], cf transitions ['compare3']
	[cfblk4_Output_5_2] cf=cfblk4_Output_5_2 -> 1.0:(cf'=compare3);
	//Element cfblk4_Output_5_3, df inputs ['y_cfblk5_1990931203'], cf transitions ['cfblk5_Update_6']
	[cfblk4_Output_5_3] cf=cfblk4_Output_5_3 -> 1.0:(cf'=cfblk5_Update_6);
	//Element compare3, df inputs ['redundancy_model47_cfblk4_3', 'y_cfblk4_1951795810_2', 'y_cfblk4_1951795810'], cf transitions ['cfblk4_Output_5_3', 'cfblk5_Update_6', 'stop_system']
	[compare3] (cf=compare3) & (redundancy_model47_cfblk4_3=c) & (y_cfblk4_1951795810=ok) & (y_cfblk4_1951795810_2=ok) -> (cf'=cfblk5_Update_6);	// <-- 
	[compare3] (cf=compare3) & (redundancy_model47_cfblk4_3=c) & ((y_cfblk4_1951795810=error) | (y_cfblk4_1951795810_2=error)) -> (cf'=stop_system);	// <-- 
	[compare3] (cf=compare3) & (redundancy_model47_cfblk4_3=v) & (y_cfblk4_1951795810=ok) & (y_cfblk4_1951795810_2=ok) -> (cf'=cfblk5_Update_6);	// <-- 
	[compare3] (cf=compare3) & (redundancy_model47_cfblk4_3=v) & ((y_cfblk4_1951795810=error) | (y_cfblk4_1951795810_2=error)) -> (cf'=cfblk4_Output_5_3);	// <-- 
endmodule
//Error propagation commands
module error_propagation
	y_cfblk2_1873525024 : [0 .. 4] init ok;
	y_cfblk3_1912660417 : [0 .. 4] init ok;
	y_cfblk4_1951795810 : [0 .. 4] init ok;
	y_cfblk5_1990931203 : [0 .. 4] init ok;
	x_cfblk5_1_2799199267 : [0 .. 4] init ok;
	redundancy_model47_cfblk2_0 : [0 .. 4] init c;
	y_cfblk2_1873525024_2 : [0 .. 4] init ok;
	redundancy_model47_cfblk3_1 : [0 .. 4] init v;
	y_cfblk3_1912660417_2 : [0 .. 4] init ok;
	redundancy_model47_cfblk4_3 : [0 .. 4] init c;
	y_cfblk4_1951795810_2 : [0 .. 4] init ok;
	//Element cfblk2_Output_1, df outputs ['y_cfblk2_1873525024']
	[cfblk2_Output_1] (true) -> 1:(y_cfblk2_1873525024'=ok);	// <-- 
	//Element cfblk5_Output_2, df inputs ['y_cfblk4_1951795810', 'x_cfblk5_1_2799199267'], df outputs ['y_cfblk5_1990931203']
	[cfblk5_Output_2] (y_cfblk4_1951795810=ok) & (x_cfblk5_1_2799199267=ok) -> 0.98:(y_cfblk5_1990931203'=ok) + 0.02:(y_cfblk5_1990931203'=error);	// <-- 
	[cfblk5_Output_2] (y_cfblk4_1951795810!=ok) | (x_cfblk5_1_2799199267!=ok) -> (y_cfblk5_1990931203'=error);	// <-- 
	//Element cfblk3_Output_3, df inputs ['y_cfblk2_1873525024', 'y_cfblk5_1990931203'], df outputs ['y_cfblk3_1912660417']
	[cfblk3_Output_3] (y_cfblk2_1873525024=ok) & (y_cfblk5_1990931203=ok) -> 0.974002239936:(y_cfblk3_1912660417'=ok) + 0.025997760064:(y_cfblk3_1912660417'=error);	// <-- 
	[cfblk3_Output_3] (y_cfblk2_1873525024!=ok) | (y_cfblk5_1990931203!=ok) -> (y_cfblk3_1912660417'=error);	// <-- 
	//Element cfblk4_Output_5, df inputs ['y_cfblk5_1990931203'], df outputs ['y_cfblk4_1951795810']
	[cfblk4_Output_5] (y_cfblk5_1990931203=ok) -> 0.974002239936:(y_cfblk4_1951795810'=ok) + 0.025997760064:(y_cfblk4_1951795810'=error);	// <-- 
	[cfblk4_Output_5] (y_cfblk5_1990931203!=ok) -> (y_cfblk4_1951795810'=error);	// <-- 
	//Element cfblk5_Update_6, df inputs ['y_cfblk4_1951795810', 'x_cfblk5_1_2799199267'], df outputs ['x_cfblk5_1_2799199267']
	[cfblk5_Update_6] (y_cfblk4_1951795810=ok) & (x_cfblk5_1_2799199267=ok) -> 0.98:(x_cfblk5_1_2799199267'=ok) + 0.02:(x_cfblk5_1_2799199267'=error);	// <-- 
	[cfblk5_Update_6] (y_cfblk4_1951795810!=ok) | (x_cfblk5_1_2799199267!=ok) -> (x_cfblk5_1_2799199267'=error);	// <-- 
	//Element cfblk2_Output_1_2, df outputs ['y_cfblk2_1873525024_2']
	[cfblk2_Output_1_2] (true) -> 1:(y_cfblk2_1873525024_2'=ok);	// <-- 
	//Element cfblk2_Output_1_3, df outputs ['y_cfblk2_1873525024']
	[cfblk2_Output_1_3] (true) -> 1:(y_cfblk2_1873525024'=ok);	// <-- 
	//Element cfblk3_Output_3_2, df inputs ['y_cfblk2_1873525024', 'y_cfblk5_1990931203'], df outputs ['y_cfblk3_1912660417_2']
	[cfblk3_Output_3_2] (y_cfblk2_1873525024=ok) & (y_cfblk5_1990931203=ok) -> 0.974002239936:(y_cfblk3_1912660417_2'=ok) + 0.025997760064:(y_cfblk3_1912660417_2'=error);	// <-- 
	[cfblk3_Output_3_2] (y_cfblk2_1873525024!=ok) | (y_cfblk5_1990931203!=ok) -> (y_cfblk3_1912660417_2'=error);	// <-- 
	//Element cfblk3_Output_3_3, df inputs ['y_cfblk2_1873525024', 'y_cfblk5_1990931203'], df outputs ['y_cfblk3_1912660417']
	[cfblk3_Output_3_3] (y_cfblk2_1873525024=ok) & (y_cfblk5_1990931203=ok) -> 0.974002239936:(y_cfblk3_1912660417'=ok) + 0.025997760064:(y_cfblk3_1912660417'=error);	// <-- 
	[cfblk3_Output_3_3] (y_cfblk2_1873525024!=ok) | (y_cfblk5_1990931203!=ok) -> (y_cfblk3_1912660417'=error);	// <-- 
	//Element cfblk4_Output_5_2, df inputs ['y_cfblk5_1990931203'], df outputs ['y_cfblk4_1951795810_2']
	[cfblk4_Output_5_2] (y_cfblk5_1990931203=ok) -> 0.974002239936:(y_cfblk4_1951795810_2'=ok) + 0.025997760064:(y_cfblk4_1951795810_2'=error);	// <-- 
	[cfblk4_Output_5_2] (y_cfblk5_1990931203!=ok) -> (y_cfblk4_1951795810_2'=error);	// <-- 
	//Element cfblk4_Output_5_3, df inputs ['y_cfblk5_1990931203'], df outputs ['y_cfblk4_1951795810']
	[cfblk4_Output_5_3] (y_cfblk5_1990931203=ok) -> 0.974002239936:(y_cfblk4_1951795810'=ok) + 0.025997760064:(y_cfblk4_1951795810'=error);	// <-- 
	[cfblk4_Output_5_3] (y_cfblk5_1990931203!=ok) -> (y_cfblk4_1951795810'=error);	// <-- 
endmodule

//Failure formulas
formula failure = x_cfblk5_1_2799199267 = error;

rewards "time" true : 1; endrewards
