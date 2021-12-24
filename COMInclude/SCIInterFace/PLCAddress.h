#pragma once

#define _USE_OMRON_PLC_
#ifdef _USE_OMRON_PLC_
#define SurfaceCCD_Clear_Flag				15000
#define SurfaceCCD_SwitchBatch_Flag			15004
#define Laser_Running_Flag					15008
#define SurfaceCCD_Running_Flag				15012
#define SurfaceCCD_Exception_Alarm			15016
#define SizeCCD_Clear_Flag					15020
#define SizeCCD_SwitchBatch_Flag			15024
#define SizeCCD_Running_Flag				15028
#define SizeCCD_Exception_Alarm				15032
#define Rewind_EA_Counter					15036
#define CCD_Batch_Num						15040

#define HEARTBEAT		100
#define ALARM			102
#define PLC_STATUS		104
#define TUBU_STATUS		106
#define TUBU_ALARM		108
#define ALARM_RESET		110
#define UPS_ALARM		112

#else

#endif // !_USE_OMRON_PLC_