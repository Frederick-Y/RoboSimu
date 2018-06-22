
#include "ai.h"

#include <cmath>

#include <iostream>

using namespace std;

static double cos_d(double theta) { return cos(theta / 180 * 3.14159); }
static double sin_d(double theta) { return sin(theta / 180 * 3.14159); }
static double tan_d(double theta) { return tan(theta / 180 * 3.14159); }
static double atan2_d(double y, double x) { return (abs(x)<1e-4 && abs(y)<1e-4) ? 0.0 : 180 / 3.14159*atan2(y, x); }
static double atan2_d(Point<double> target, Point<double> cur) { return (cur.getDistance(target) < 1e-4) ? 0.0 : 180 / 3.14159*atan2(target.y - cur.y, target.x - cur.x); }
static double asin_d(double s) { return 180 / 3.14159*asin(s); }
static double minus_angle_d(double t1, double t2) {
    double u = fmod(t1 - t2 + 360, 360);
    if (u > 180) u -= 360;
    return u;
}

// 基本测试AI --- --- --- --- 

// 只会转圈的AI
PlayerControl circle_ai(const Info info) {
    // circle run
    return PlayerControl{ 50, 30, 0, Attack1 };
}

// 跑点AI --- --- --- --- 

// 双方选手的关键点
Point<double> target_coors[2][4] = {
    {
        Point<double>(200, 75),  //SPD
        Point<double>(200, 225), //DEF
        Point<double>(20, 20),   //HP
        Point<double>(20, 280),  //MP
    },{
        Point<double>(200, 75),   //SPD
        Point<double>(200, 225), //DEF
        Point<double>(380, 280), //HP
        Point<double>(380, 20),  //MP
    } };
const int CNT_TARGET = 4;

// 给出前往指定地点的指令
PlayerControl rush_target(const Info info, Point<double> target_coor) {
    // 已经到了
    if (target_coor.getDistance(info.coor) < RADIUS_PROP) {
        return PlayerControl{ -75, 75, 0, NoAction };
    }

    TSpeed max_speed = 50;
    if (info.spd_status) max_speed = 75;
    else if (info.frz_status) max_speed = 25;

    TAngle target_angle = atan2_d(target_coor.y - info.coor.y, target_coor.x - info.coor.x);
    TAngle delta_angle = minus_angle_d(target_angle, info.car_angle);

    TAngle delta_attack_angle = 0;

    TAngle max_roate_angle = (max_speed / FREQ) / RADIUS_CAR / 3.14159 * 180;
    TSpeed ss = delta_angle * RADIUS_CAR * FREQ * 3.14159 / 180;

    if (abs(delta_angle) < max_roate_angle / 2) { // 直接跑
        if (delta_angle > 0)
            return PlayerControl{ max_speed - 2 * ss, max_speed, delta_attack_angle, NoAction };
        else
            return PlayerControl{ max_speed, max_speed - 2 * ss, delta_attack_angle, NoAction };
    }
    else if (abs(delta_angle) > max_roate_angle) { // 全速转
        if (delta_angle > 0)  // 左转
            return PlayerControl{ -max_speed, max_speed, delta_attack_angle, NoAction };
        else  // 右转
            return PlayerControl{ max_speed, -max_speed, delta_attack_angle, NoAction };
    }
    else { // 指定角度转
        return PlayerControl{ -ss, ss, delta_attack_angle, NoAction };
    }

}

// 循环跑点的AI
PlayerControl rush_ai(const Info info) {

    static int target_id = 0;

    Point<double> target_coor = target_coors[info.id][target_id];
    if (target_coor.getDistance(info.coor) < RADIUS_PROP) {
        ++target_id; target_id %= CNT_TARGET;
        target_coor = target_coors[info.id][target_id];
    }

    return rush_target(info, target_coor);

}


// Main ---------- ---------- ---------- ---------- ----------

PlayerControl player_ai(const Info info) {
    return rush_ai(info);
    //return circle_ai(info);
}
