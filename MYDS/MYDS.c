#include <GL/glut.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

void Display(void);
void Reshape(int,int);
void line(int,int,int,int);
void Timer(int);
void PrintText(int ,int ,char*);


double DegToRad(double );
double SolarZenithAngle(double ,double ,double ,double );
double ShadowLength(double ,double );
double ShadowDirection(int ,int ,int ,double);
double DoubleHour(int,int,int);
void Needle(double ,double ,double ,double ,double ,double );
void NeedleShadow(double ,double ,double ,double ,double ,double );
double TimeToRad(int ,int ,int );
double SolarAzimuth(double ,double ,double ,double ,double );
double HourAngle(double ,double ,double ,double );
double DmsToDeg(int ,int ,int );
double AskDaysFromNewYearsDay(int ,int );
double SunDeclination(double ,double );
double AskOmega(int );
double EquationOfTime(double ,double );
void DrawCompass2(double ,double ,double ,double ,double ,int ,int ,int,int);





// 月ごとの1日の元日からの日数
int daysFromNewYearsDay[12]=
{0,31,59,90,120,151,181,212,243,274,304,334};




int main(int argc,char **argv)
{
    glutInit(&argc,argv);//glut用の引数を先に取っちゃう
    glutInitWindowSize(530,250);
    glutCreateWindow("MYDS");
    glutDisplayFunc(Display);   //関数のポインタ  画面の書き換えが発生したときにDisplayを呼ぶ
    glutReshapeFunc(Reshape);   //関数のポインタ  画面の拡大縮小が発生したときにReshapeを呼ぶ
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    glClearColor(1.0,1.0,1.0,1.0);
    glutTimerFunc(500,Timer,0);

    
    glutMainLoop();//ずっとここをループして見張っている (returnに行かない)

    return(0);
}

//
// ディスプレイ用の関数
//
void Display(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    // glutReshapeWindow(530,250);
    
    time_t tt;
    struct tm *ts;
    int timeDifference = 0;    // 時差
    int monthDifference = 0;    // 月ずらし


    time(&tt);
    ts = localtime(&tt);
    int year = ts->tm_year + 1900;
    int month = ts->tm_mon + 1 + monthDifference;
    int day = ts->tm_mday;
    int day_of_week = ts->tm_wday;
    int hour = ts->tm_hour + timeDifference; // +9
    int minits = ts->tm_min;
    int seconds = ts->tm_sec;
    char *Youbi;

    int xc,yc;              //中心の座標

    // 時間の調整
    if(hour < 0)
    {
        hour = 24 + hour;
    }else if(24 <= hour)
    {
        hour = hour - 24;
    }
    // 月の調整
    if(month < 0)
    {
        month = 12 + month;
    }else if(13 <= month)
    {
        month = month - 12;
    }
    
    double currentLatitude,currentLongitude;// 緯度:latitude  経度:longitude
    double hourAngle;                       // 時角
    double solarZenithAngle;                // 太陽高度
    double solarAzimuth;                    // 太陽の方位角
    double shadowLength,shadowDirection;    // 影の長さと影の方向
    double shadowx,shadowy;                 // 影の座標
    double pointax,pointay,pointbx,pointby; // 針の後ろの座標
    double latitude,longitude;              // 緯度，経度（ラジアン）
    double centralStandardTimeAngle;        // 中央標準時の経度
    double equationOfTime;                  // 均時差
    double doubleHour;                      // 現在時刻(double)
    double omega;                           // 太陽赤緯と均時差の計算に使う(ω)
    double daysFromNewYearsDay;             // 元日からの日数 太陽赤緯と均時差の計算に使う(J)
    double sunDeclination;                  // 太陽赤緯(delta)

    switch(day_of_week){
        case 0:
            Youbi = "日";
            break;
        case 1:
            Youbi = "月";
            break;
        case 2:
            Youbi = "火";
            break;
        case 3:
            Youbi = "水";
            break;
        case 4:
            Youbi = "木";
            break;
        case 5:
            Youbi = "金";
            break;
        case 6:
            Youbi = "土";
            break;
        default:
            Youbi = "？";
    }


    // 中心の座標
    xc = glutGet(GLUT_WINDOW_WIDTH)/2;
    yc = (glutGet(GLUT_WINDOW_HEIGHT)*3)/5;     //glutGet(GLUT_WINDOW_HEIGHT)/2;
    
    // 緯度,経度設定  長野高専J棟　36.6778809,138.2326128
    currentLatitude = 36.6778809;
    currentLongitude = 138.2326128;
    centralStandardTimeAngle = 135;
    latitude = DegToRad(currentLatitude);
    longitude = DegToRad(currentLongitude);

    // 時間(double)
    doubleHour = DoubleHour(hour,minits,seconds);
    // 太陽赤緯
    daysFromNewYearsDay = AskDaysFromNewYearsDay(month,day);
    omega = AskOmega(year);
    sunDeclination = SunDeclination(omega,daysFromNewYearsDay);
    // 均時差
    equationOfTime = EquationOfTime(omega,daysFromNewYearsDay);
    // 時角
    hourAngle = HourAngle(doubleHour,currentLongitude,centralStandardTimeAngle,equationOfTime);

    // 色を変える
    glColor3ub(255,0,255);
    
    
    // 針の高さを計算 針の角度が緯度になるように
    double rod = (glutGet(GLUT_WINDOW_HEIGHT) * 3/10) * tan(latitude);
    // 影の長さと角度表示
    solarZenithAngle = SolarZenithAngle(latitude,longitude,sunDeclination,hourAngle);
    solarAzimuth = SolarAzimuth( latitude, longitude,sunDeclination,solarZenithAngle, hourAngle);
    shadowLength = ShadowLength(rod,solarZenithAngle);
    shadowDirection = solarAzimuth - M_PI;
    shadowx = xc + shadowLength * cos(shadowDirection);
    shadowy = yc -  shadowLength * sin(shadowDirection);
    
    
    printf("\n\n%d年%d月%d日(%s) %d時%d分%d秒\n",year,month,day,Youbi,hour,minits,seconds);
    printf ("今の時間\t= %f\n太陽赤緯\t= %f\n均時差    \t= %f\n時角      \t= %f\n太陽の方位角\t= %f\n太陽高度\t= %f\n",doubleHour,sunDeclination,equationOfTime,hourAngle,solarAzimuth,solarZenithAngle);      ///たしかめ消す
    printf("影の長さ\t= %.5f\n影の向き\t= %.5f\n",shadowLength,shadowDirection);


    // 針の描写
    // 針の後ろ側の座標を決める（黄金比）
    pointax = xc + ((glutGet(GLUT_WINDOW_HEIGHT)*3)/10) * (1/sqrt(((1+sqrt(5)) * (1+sqrt(5)))+1));
    pointay = ((glutGet(GLUT_WINDOW_HEIGHT)*9)/10);
    pointbx = xc - ((glutGet(GLUT_WINDOW_HEIGHT)*3)/10) * (1/sqrt(((1+sqrt(5)) * (1+sqrt(5)))+1));
    pointby = ((glutGet(GLUT_WINDOW_HEIGHT)*9)/10);


    // 影の描写
    // 影を描く
    if ( solarZenithAngle > 0)
    {
        NeedleShadow(shadowx, shadowy, xc, yc, xc, pointay);
    }

    // 羅針盤（日時計のメモリ）を描く
    DrawCompass2( latitude, currentLongitude, centralStandardTimeAngle, sunDeclination, equationOfTime, xc, pointby , yc,rod);
    
    // 針を描く
    Needle(xc,yc,pointax,pointay,pointbx,pointby);


    glFlush();
    glutSwapBuffers();
}

//
// 文字を表示する
//
void PrintText(int x,int y,char *s)
{
    glRasterPos2i(x,y);
    for(int i=0;i < strlen(s);i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10,s[i]);  // GLUT_BITMAP_8_BY_13
    }
}

//
// ウィンドウのサイズ変更が発生したときに座標系を再設定する関数
//
void Reshape(int w,int h)
{
    //printf("ウィンドウの幅と高さ =%d x %d\n",w,h);//確認用
    glViewport(0,0,w,h);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluOrtho2D(0,w,0,h);
    glScaled(1,-1,1);
    glTranslated(0,-h,0);
}

//
// 線を引く
//
void line(int x0,int y0,int x1,int y1)
{
    glBegin(GL_LINES);
    glVertex2i(x0,y0);
    glVertex2i(x1,y1);
    glEnd();
}

///
/// タイマーの設定
///
void Timer(int value)
{
    glutPostRedisplay();
    glutTimerFunc(500,Timer,0);
}

///
/// ここから日時計の計算
/// 度をラジアンに直す
double DegToRad(double deg)
{
    return deg * (M_PI/180);
}

///
/// 度分秒を度に直す
///
double DmsToDeg(int degrees,int minutes,int seconds)
{
    return (double)degrees +(double)minutes/60 + (double)seconds/3600;
}

///
/// 時角計算
/// 中央標準時:time 現在地の経度(度):currentLongitude 中央標準時の経度:centralStandardTimeAngle 均時差:equationOfTime
double HourAngle(double time,double currentLongitude,double centralStandardTimeAngle,double equationOfTime)
{
    double T;
    double hourAngle;

    T = time + (currentLongitude - centralStandardTimeAngle)/15  + equationOfTime;
    hourAngle = 15 * T - 180;

    return DegToRad(hourAngle);
}


///
/// 太陽高度計算
/// 緯度:latitude  経度:longitude  視赤緯:delta 時角:hourAngle
double SolarZenithAngle(double latitude,double longitude,double delta,double hourAngle)
{
    double h;   // 太陽の高度
    
    h = asin((sin(latitude) * sin (delta)) + (cos(latitude) * cos(delta) * cos(hourAngle)));

    return h;
}

///
/// 太陽の方位角を計算
/// 緯度:latitude  経度:longitude 視赤緯:delta 太陽高度: solarZenithAngle 時角:hourAngle
double SolarAzimuth(double latitude,double longitude,double delta,double solarZenithAngle,double hourAngle)
{
    double solarAzimuth1,solarAzimuth2;     // 太陽の方位角
    
    solarAzimuth1 = cos(delta) * sin(hourAngle) / cos(solarZenithAngle);
    solarAzimuth2 = (sin(solarZenithAngle)*sin(latitude)-sin(delta))/cos(solarZenithAngle)/cos(latitude);
    double solarAzimuth = atan2(solarAzimuth1,solarAzimuth2) + M_PI;

    return M_PI/2 - solarAzimuth;
}

///
/// 影の長さを計算
///　物体の長さ:rod 太陽高度:solarZenithAngle
double ShadowLength(double rod ,double solarZenithAngle)
{
    return rod * (1 / tan(solarZenithAngle));
}

///
/// 今の時間の影の向き（ラジアン）を計算
/// 時間:hour時minits分seconds秒 緯度:latitude
double ShadowDirection(int hour,int minits,int seconds,double latitude)
{
    double shadowDirection;                     // 影の角度
    double thetat = (((3.0/2.0) * M_PI ) - TimeToRad( hour, minits, seconds) );
    shadowDirection = atan(tan(thetat)/sin(latitude));
    if((0 <= hour)&&(hour<12))                  // 1日で一周するように午前中の値を変換する
    {
        shadowDirection = (M_PI + shadowDirection);
    }
    return shadowDirection;
}

///
/// 時間(小数点)を計算
/// 
double DoubleHour(int hour ,int minits ,int seconds)
{
    return (double)hour + ((double)minits/60) + ((double)seconds/3600);
}

//
// 羅針盤の針の影を描く
//
void NeedleShadow(double shadowx,double shadowy,double shadowRootx,double shadowRooty,double pointbx,double pointby)
{
    glColor4ub(170,170,170,200);

    glBegin(GL_TRIANGLES);
    glVertex2i(shadowRootx,shadowRooty);
    glVertex2i(shadowx,shadowy);
    glVertex2i(pointbx,pointby);
    glEnd();
}

//
// 羅針盤の針を描く
//
void Needle(double shadowRootx,double shadowRooty,double pointax,double pointay,double pointbx,double pointby)
{
    // glColor3ub(170,170,170);
    glColor3ub(84,83,92);
    glBegin(GL_TRIANGLES);
    glVertex2i(shadowRootx,shadowRooty);
    glVertex2i(pointax,pointay);
    glVertex2i(pointbx,pointby);
    glEnd();
}



///
/// 羅針盤（日時計のメモリ）を描く
/// 緯度:latitude 針の根本の座標:shadowRootx,shadowRooty 
/// 中央標準時:time 現在地の経度(度):currentLongitude 中央標準時の経度:centralStandardTimeAngle 視赤緯:delta 均時差:equationOfTime
void DrawCompass2(double latitude,double currentLongitude,double centralStandardTimeAngle,double delta,double equationOfTime,int shadowRootx,int shadowRooty ,int needleTopY,int rot)
{
    double hourAngle;                       // 時角
    double solarZenithAngle;                // 太陽高度
    double solarAzimuth;                    // 太陽の方位角
    double shadowDirection;                 // 影の方位角
    double shadowx,shadowy;                 // 影の座標
    double shadowLength;                    // 影の長さ
    double shadowDirectionRoot;             // 根元からの影の向き
    char s[10];                             // 時間表示用変数
    char east[5] = "East";
    char west[5] = "West";
    char south[6] = "South";
    char north[6] = "North";
    int scaleLengh = glutGet(GLUT_WINDOW_HEIGHT)*4/5; 


    // 方角を描く(緑の線)
    // 色を変える
    glColor3ub(56, 102, 93);
    line(shadowRootx-scaleLengh,shadowRooty,shadowRootx+scaleLengh,shadowRooty);
    line(shadowRootx,glutGet(GLUT_WINDOW_HEIGHT)/20,shadowRootx,shadowRooty+glutGet(GLUT_WINDOW_HEIGHT)/20);
    PrintText(shadowRootx-scaleLengh - 34,shadowRooty,west);
    PrintText(shadowRootx-10,glutGet(GLUT_WINDOW_HEIGHT)/20 - 2 ,north);
    PrintText(shadowRootx+scaleLengh + 2,shadowRooty,east);
    PrintText(shadowRootx-10,shadowRooty+glutGet(GLUT_WINDOW_HEIGHT)/20 + 7,south);


    for (int i = 0;i<24;i++)
    {
        // 時角を求める
        hourAngle = HourAngle( (double)i, currentLongitude, centralStandardTimeAngle, equationOfTime);

        // 高度を求める
        solarZenithAngle = SolarZenithAngle( latitude, currentLongitude, delta, hourAngle);

        // 太陽の方位角を求める
        solarAzimuth = SolarAzimuth( latitude, currentLongitude, delta, solarZenithAngle, hourAngle);
        shadowDirection = solarAzimuth - M_PI;

        // 影の長さを求める
        shadowLength = ShadowLength(rot,solarZenithAngle);

        // 太陽の高度が0以上なら線を引く(赤の線)
        if(solarZenithAngle > 0)
        {
            shadowx = shadowRootx + shadowLength * cos(shadowDirection);
            shadowy = needleTopY - shadowLength * sin(shadowDirection);

            shadowDirectionRoot = atan2(shadowx -shadowRootx,shadowy-shadowRooty) - M_PI/2;
            shadowx = shadowRootx + scaleLengh*cos(shadowDirectionRoot);
            shadowy = shadowRooty - scaleLengh*sin(shadowDirectionRoot);
            // 色を変える
            glColor3ub(187, 102, 119);
            // 線を引く
            line(shadowx,shadowy,shadowRootx,shadowRooty);
            // 時間を表示  
            sprintf(s,"%2d:00",i);
            PrintText(shadowx,shadowy,s);
        }
    }
}

//
// 時間 hour時minits分seconds秒をラジアンに変換
//
double TimeToRad(int hour,int minits,int seconds)
{
        return ( M_PI * (seconds + 60 * minits + 3600 * hour))/43200;    
}

///
/// 元日からの日数+0.5日を求める
/// 月:month 日:day
double AskDaysFromNewYearsDay(int month,int day)
{
    return ((double)(daysFromNewYearsDay[month - 1 ] + day))+0.5;
} 

///
/// うるう年か判別してomegaを求める 
///
double AskOmega(int year)
{
    double leepYear;

    if (year % 400 == 0) 
    {
        leepYear = 366;
    } else if (year % 100 == 0) {
        leepYear = 365;
    } else if (year % 4 == 0) {
        leepYear = 366;
    } else {
        leepYear = 365;
    }
    
    return (2*M_PI)/leepYear;
}

///
/// 太陽赤緯(太陽光線と地球の赤道面との角度)の計算  
/// J:元日からの日数+0.5日
double SunDeclination(double omega,double J)
{
    double delta;

    delta =   0.33281 - 22.984*cos(omega * J) - 0.34990*cos(2*omega * J) - 0.13980*cos(3*omega * J)
            + 3.7872*sin(omega * J) + 0.03250*sin(2*omega * J) + 0.07187*sin(3*omega * J);

    return DegToRad(delta);
}

///
/// 均時差(天球上を一定な速さで動くと考えた平均太陽と、実際の太陽との移動の差)の計算
///
double EquationOfTime(double omega,double J)
{
    double equationOfTime;
    equationOfTime =  0.0072*cos(omega * J) - 0.0528*cos(2*omega * J) - 0.0012*cos(3*omega * J)
                    - 0.1229*sin(omega * J) - 0.1565*sin(2*omega * J) - 0.0041*sin(3*omega * J);

    return equationOfTime;
}