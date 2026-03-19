#ifndef referee_protocol_H
#define referee_protocol_H

#include "stdint.h"

/****************************宏定义部分****************************/

#define REFEREE_SOF 0xA5 // 起始字节,协议固定为0xA5
#define Robot_Red 0
#define Robot_Blue 1
#define Communicate_Data_LEN 5 // 自定义交互数据长度，该长度决定了我方发送和他方接收，自定义交互数据协议更改时只需要更改此宏定义即可

#pragma pack(1)

/****************************通信协议格式****************************/

/* 通信协议格式偏移，枚举类型,代替#define声明 */
typedef enum
{
	FRAME_HEADER_Offset = 0,
	CMD_ID_Offset = 5,
	DATA_Offset = 7,
} JudgeFrameOffset_e;

/* 通信协议长度 */
typedef enum
{
	LEN_HEADER = 5, // 帧头长
	LEN_CMDID = 2,	// 命令码长度
	LEN_TAIL = 2,	// 帧尾CRC16

	LEN_CRC8 = 4, // 帧头CRC8校验长度=帧头+数据长+包序号
} JudgeFrameLength_e;

/****************************帧头****************************/
/****************************帧头****************************/

/* 帧头偏移 */
typedef enum
{
	SOF = 0,		 // 起始位
	DATA_LENGTH = 1, // 帧内数据长度,根据这个来获取数据长度
	SEQ = 3,		 // 包序号
	CRC8 = 4		 // CRC8
} FrameHeaderOffset_e;

/* 帧头定义 */
typedef struct
{
	uint8_t SOF;
	uint16_t DataLength;
	uint8_t Seq;
	uint8_t CRC8;
} xFrameHeader;


/****************************cmd_id命令码说明****************************/

/* 命令码ID,用来判断接收的是什么数据 */
typedef enum
{
	ID_game_state = 0x0001,				   // 比赛状态数据
	ID_game_result = 0x0002,			   // 比赛结果数据
	ID_game_robot_survivors = 0x0003,	   // 比赛机器人血量数据
	ID_event_data = 0x0101,				   // 场地事件数据
	ID_game_robot_state = 0x0201,		   // 机器人状态数据
	ID_power_heat_data = 0x0202,		   // 实时功率热量数据
	ID_game_robot_pos = 0x0203,			   // 机器人位置数据
	ID_buff_musk = 0x0204,				   // 机器人增益数据
	ID_robot_hurt = 0x0206,				   // 伤害状态数据
	ID_shoot_data = 0x0207,				   // 实时射击数据
	ID_projectile_allowance = 0x0208,	   // 允许发弹量
	ID_student_interactive = 0x0301,	   // 机器人间交互数据

} CmdID_e;

/* 命令码数据段长,根据官方协议来定义长度，还有自定义数据长度 */
typedef enum
{
	LEN_game_state = 3,		// 0x0001
	LEN_game_result = 1,	// 0x0002
	LEN_game_robot_HP = 32, // 0x0003
	LEN_event_data = 4,		// 0x0101
	// LEN_supply_projectile_action = 4,			 // 0x0102
	// LEN_supply_projectile_booking = 4,			 // 0x0103
	LEN_referee_warning = 3,   // 0x0104
	LEN_dart_status = 3,	   // 0x0105
	LEN_game_robot_state = 13, // 0x0201
	LEN_power_heat_data = 16,  // 0x0202
	LEN_game_robot_pos = 16,   // 0x0203
	LEN_buff_musk = 7,		   // 0x0204
	// LEN_aerial_robot_energy = 2,				 // 0x0205
	LEN_robot_hurt = 1,							   // 0x0206
	LEN_shoot_data = 7,							   // 0x0207
	LEN_projectile_allowance = 6,				   // 0X0208
	LEN_rfid_status = 4,						   // 0x0209
	LEN_dart_client_cmd = 6,					   // 0x020A
	LEN_ground_robot_pos = 40,					   // 0x020B
	LEN_radar_mark_progress = 1,				   // 0x020C
	LEN_sentry_autonomous_decision_sync = 6,	   // 0x020D
	LEN_radar_autonomous_decision_sync = 1,		   // 0x020E
	LEN_receive_data = 127,						   // 0x0301
	LEN_custom_controller_interactive_data = 30,   // 0x0302
	LEN_map_command = 15,						   // 0x0303
	LEN_keyboard_mouse_control_data = 12,		   // 0x0304
	LEN_map_receive_radar_data = 24,			   // 0x0305
	LEN_custom_controller_to_player_data = 8,	   // 0x0306
	LEN_map_receive_sentry_data = 103,			   // 0x0307
	LEN_map_receive_robot_data = 34,			   // 0x0308
	LEN_custom_controller_receive_robot_data = 30, // 0x0309

} JudgeDataLength_e;

/****************************接收数据的详细说明****************************/

/* ID: 0x0001  Byte:  11    比赛状态数据 */
typedef struct
{
	uint8_t game_type : 4;
	uint8_t game_progress : 4;
	uint16_t stage_remain_time;
	uint64_t SyncTimeStamp;
} ext_game_state_t;

/* ID: 0x0002  Byte:  1    比赛结果数据 */
typedef struct
{
	uint8_t winner;
} ext_game_result_t;

// 0x0003机器人血量数据，3Hz
/* ID: 0x0003  Byte:  16    比赛机器人血量数据 */
typedef struct
{
	uint16_t ally_1_robot_HP;
	uint16_t ally_2_robot_HP;
	uint16_t ally_3_robot_HP;
	uint16_t ally_4_robot_HP;
	uint16_t reserved;
	uint16_t ally_7_robot_HP;
	uint16_t ally_outpost_HP;
	uint16_t ally_base_HP;
} ext_game_robot_HP_t;

/* ID: 0x0101  Byte:  4    场地事件数据 */
typedef struct
{
	uint32_t event_data;
} ext_event_data_t;

/*ID: 0x0104 Byte: 4     判罚信息*/
typedef struct
{
	uint8_t level;
	uint8_t offending_robot_id;
	uint8_t count;
} ext_referee_warning_t;

/* ID: 0X0201  Byte: 13    机器人状态数据 */
typedef struct
{
	uint8_t robot_id;
	uint8_t robot_level;
	uint16_t current_HP;
	uint16_t maximum_HP;
	uint16_t shooter_barrel_cooling_value;
	uint16_t shooter_barrel_heat_limit;
	uint16_t chassis_power_limit;
	uint8_t power_management_gimbal_output : 1;
	uint8_t power_management_chassis_output : 1;
	uint8_t power_management_shooter_output : 1;
} ext_game_robot_state_t;

/* ID: 0X0202  Byte: 14    实时功率热量数据 */
typedef struct
{
	uint16_t reserved0;
	uint16_t reserved1;
	float reserved2;
	uint16_t buffer_energy;
	uint16_t shooter_17mm_1_barrel_heat;
	uint16_t shooter_42mm_barrel_heat;
} ext_power_heat_data_t;

/* ID: 0x0203  Byte: 16    机器人位置数据 */
typedef struct
{
	float x;
	float y;
	float angle; // 本机器人测速模块的朝向，单位：度。正北为 0 度
} ext_game_robot_pos_t;

/* ID: 0x0204  Byte:  6    机器人增益数据 */
typedef struct
{
	uint8_t recovery_buff;		// 机器人回血增益（百分比，值为 10 表示每秒恢复血量上限的 10%）
	uint8_t cooling_buff;		// 机器人射击热量冷却倍率（直接值，值为 5 表示 5 倍冷却）
	uint8_t defence_buff;		// 机器人防御增益（百分比，值为 50 表示 50%防御增益）
	uint8_t vulnerability_buff; // 机器人负防御增益（百分比，值为 30 表示-30%防御增益）
	uint16_t attack_buff;		// 机器人攻击增益（百分比，值为 50 表示 50%攻击增益）
	uint8_t remaining_energy;	// 机器人剩余能量值反馈,有点迷
} ext_buff_musk_t;

/* ID: 0x0206  Byte:  1    伤害状态数据 */
typedef struct
{
	uint8_t armor_id : 4;
	uint8_t hurt_type : 4;
} ext_robot_hurt_t;

/* ID: 0x0207  Byte:  7    实时射击数据 */
typedef struct
{
	uint8_t bullet_type; // 弹丸类型
	uint8_t shooter_id;	 // 发射机构 ID
	uint8_t bullet_freq; // 弹丸射速（Hz）
	float bullet_speed;	 // 弹丸初速度（m/s）
} ext_shoot_data_t;

/* ID: 0x0208   Byte:  8     子弹数以及金币数dee*/
typedef struct
{
	uint16_t projectile_allowance_17mm;
	uint16_t projectile_allowance_42mm;
	uint16_t remaining_gold_coin;
	uint16_t projectile_allowance_fortress; // 新加：堡垒增益点提供的储备 17mm 弹丸允许发弹量；该值与机器人是否实际占领堡垒无关
} ext_projectile_allowance_t;

/*ID:0x0209    Byte:  8     rfid进度*/
typedef struct
{
	uint32_t rfid_status;
	uint8_t rfid_status_2;
} ext_rfid_status_t;




/****************************机器人交互数据****************************/

/* 发送的内容数据段最大为 113 检测是否超出大小限制?实际上图形段不会超，数据段最多30个，也不会超*/
/* 交互数据头结构 */
typedef struct
{
	uint16_t data_cmd_id; // 由于存在多个内容 ID，但整个cmd_id 上行频率最大为 10Hz，请合理安排带宽。注意交互部分的上行频率
	uint16_t sender_ID;
	uint16_t receiver_ID;
} ext_student_interactive_header_data_t;

/* 机器人id */
typedef enum
{
	// 红方机器人ID
	RobotID_RHero = 1,
	RobotID_REngineer = 2,
	RobotID_RStandard1 = 3,
	RobotID_RStandard2 = 4,
	RobotID_RStandard3 = 5,
	RobotID_RAerial = 6,
	RobotID_RSentry = 7,
	RobotID_RRadar = 9,
	// 蓝方机器人ID
	RobotID_BHero = 101,
	RobotID_BEngineer = 102,
	RobotID_BStandard1 = 103,
	RobotID_BStandard2 = 104,
	RobotID_BStandard3 = 105,
	RobotID_BAerial = 106,
	RobotID_BSentry = 107,
	RobotID_BRadar = 109,
} Robot_ID_e;






#pragma pack()

#endif
