// 模拟模式
double gas_mode = 0; // 气体模拟模式标志

// 球体属性
double nballs = 8;       // 球的数量，最大值大约是100个
double ball_radius = 8;  // 球的半径，最大值大约是 WALL_WIDTH 的 0.5倍
double ball_speed = 0;   // 球的速度，单位：像素/秒（最大速度约为半径 * 200）
double ball_mass = 0.2;  // 球的质量，最小值约为活塞质量的 0.01倍
double ball_gravity = 0; // 球的重力，若为零，则模拟气体

// 测试标志和初始速度
double is_testing = 0;                // 测试模式标志
double ball0_speed = 10;              // 球 0 的初始速度，替代随机生成的速度
double ball0_vel_angle = 0;           // 球 0 的初始速度角度，单位：度
double ball1_speed = 10;              // 球 1 的初始速度
double ball1_vel_angle = -180;        // 球 1 的初始速度角度，单位：度

// 活塞属性
double piston_mass = 10.0;              // 活塞的质量
double piston_gravity = 10.0;          // 活塞的重力，若为零，活塞停止运动

// 底部属性
double bottom_temp_min = 0.0;            // 底部的最小温度
double bottom_temp_max = 100.0;         // 底部的最大温度
