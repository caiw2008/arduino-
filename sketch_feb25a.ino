#include <TimerOne.h>  //定时中断2560需要用这个库文件

/*1302RST-51 dat-52 clk-53*/
#include <LCD12864RSPI.h>
#include <Keypad.h>  //键盘库文件
#include <stdio.h>
#include <string.h>
#include <DS1302.h>

/*12864引脚定义*/
LCD12864RSPI LCDA(4, 5, 6);

/* 接口定义
  CE(DS1302 pin5) -> Arduino D5  rst
  IO(DS1302 pin6) -> Arduino D6  dat
  SCLK(DS1302 pin7) -> Arduino D7  clk
*/
uint8_t CE_PIN   = 51;
uint8_t IO_PIN   = 52;
uint8_t SCLK_PIN = 53;


/* 日期变量缓存 */
char buf[50];
char day[10];
/* 串口数据缓存 */
String comdata = "";
int numdata[7] = {0}, j = 0, mark = 0;
/* 创建 DS1302 对象 */
DS1302 rtc(CE_PIN, IO_PIN, SCLK_PIN);


/*键盘用*/
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'4', '8', 'b', ','},
  {'3', '7', 'a', 'e'},
  {'2', '6', '0', 'd'},
  {'1', '5', '9', 'c'}
};
/*键盘定义接口*/
byte rowPins[ROWS] = {21, 20, 19, 18}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {17, 16, 15, 14}; //connect to the column pinouts of the keypad
int c, b, d; //c定义为“→”行数，用与指示选择哪一行。
int  nian, yue, ri, shi, fen, miao, xingqi; //定义年，月，日，时，分，秒，星期用，于设置DS1302时间
int  jp = 0;        //将其他键盘关掉,创建布尔型变量K,用于关闭1-9灯其他键盘。
char customKey ;  //键盘数字赋值给customKey，设置为int变量就可以选择了。接收键盘字符变量。
int zongzifushu;  //用于存放总字符数变量。
int zifushu;//用于存放字符数变量。

/*显示器用的数组*/
char wendu[] = {  0xCE, 0xC2, 0xB6, 0xC8}; //温度
char shidu[] = {  0xCA, 0xAA, 0xB6, 0xC8}; //湿度
char guangzhao[] = {  0xB9, 0xE2, 0xD5, 0xD5 }; //光照
char shezhian[] = {  0xC9, 0xE8, 0xD6, 0xC3, 0xB0, 0xB4};                //设置按
char shedingshijian[] = {  0xC9, 0xE8, 0xB6, 0xA8, 0xCA, 0xB1, 0xBC, 0xE4};               //设定时间
char shezhidingshikaiguan1[] = {0xC9, 0xE8, 0xD6, 0xC3, 0xB6, 0xA8, 0xCA, 0xB1, 0xBF, 0xAA, 0xB9, 0xD8, 0x31, 0x00};            //设置定时开关1
char shezhidingshikaiguan2[] = {0xC9, 0xE8, 0xD6, 0xC3, 0xB6, 0xA8, 0xCA, 0xB1, 0xBF, 0xAA, 0xB9, 0xD8, 0x32, 0x00};            //设置定时开关2
char xuandingan[] = {0xD1, 0xA1, 0xB6, 0xA8, 0xB0, 0xB4};                //选定按
char shezhidingshikaiguan3[] = {0xC9, 0xE8, 0xD6, 0xC3, 0xB6, 0xA8, 0xCA, 0xB1, 0xBF, 0xAA, 0xB9, 0xD8, 0x33, 0x00};            //设置定时开关3
char shezhiwendubaojing[] = {0xC9, 0xE8, 0xD6, 0xC3, 0xCE, 0xC2, 0xB6, 0xC8, 0xB1, 0xA8, 0xBE, 0xAF};             //设置温度报警
char shezhishidubaojing[] = {0xC9, 0xE8, 0xD6, 0xC3, 0xCA, 0xAA, 0xB6, 0xC8, 0xB1, 0xA8, 0xBE, 0xAF};             //设置湿度报警
char shezhiguangzhaobaojing[] = {0xC9, 0xE8, 0xD6, 0xC3, 0xB9, 0xE2, 0xD5, 0xD5, 0xB1, 0xA8, 0xBE, 0xAF};         //设置光照报警
char yangshi[] = {0xD1, 0xF9, 0xCA, 0xBD}; //样式
char baocunfanhuian[] = {0xB1, 0xA3, 0xB4, 0xE6, 0xB7, 0xB5, 0xBB, 0xD8, 0xB0, 0xB4}; //保存返回按
char nianzi[] = {0xC4, 0xEA}; //”年字“内码
char yuezi[] = {0xD4, 0xC2};                  //月
char rizi[] = {0xC8, 0xD5};                  //日
char shizi[] = {0xCA, 0xB1};                  //时
char fenzi[] = {0xB7, 0xD6};                  //分
char miaozi[] = {0xC3, 0xEB};                  //秒


//initialize an instance of class NewKeypad 初始化类NewKeypad的实例
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

void print_time()  //定时中断需要执行的程序
{
  /* 从 DS1302 获取当前时间 */
  Time t = rtc.time();
  /* 将星期从数字转换为名称 */
  memset(day, 0, sizeof(day));
  switch (t.day)
  {
    case 1: strcpy(day, "Sunday"); break;
    case 2: strcpy(day, "Monday"); break;
    case 3: strcpy(day, "Tuesday"); break;
    case 4: strcpy(day, "Wednesday"); break;
    case 5: strcpy(day, "Thursday"); break;
    case 6: strcpy(day, "Friday"); break;
    case 7: strcpy(day, "Saturday"); break;
  }
  /* 将日期代码格式化凑成buf等待输出 */
  // snprintf(buf, sizeof(buf), "%s %04d-%02d-%02d %02d:%02d:%02d", day, t.yr, t.mon, t.date, t.hr, t.min, t.sec);
  snprintf(buf, sizeof(buf), "%02d-%02d %02d:%02d:%02d ", t.mon, t.date, t.hr, t.min, t.sec);
  /* 输出日期到串口 */
  //  Serial.println(buf);  //串口输出日期
  LCDA.clear();  //LCD清零
  LCDA.setCursor(0, 0);
  // LCDA.print(t.yr);
  // LCDA.print("/");
  LCDA.print(t.mon);
  LCDA.setCursor(0, 1);
  LCDA.chinese(yuezi, 2);
  LCDA.print(t.date);
  LCDA.setCursor(0, 3);
  LCDA.chinese(rizi, 2);
  LCDA.print(t.hr);
  LCDA.print(":");
  LCDA.print(t.min);
  LCDA.print(":");
  LCDA.print(t.sec);



  LCDA.setCursor(1, 0);
  LCDA.chinese(wendu, 4);
  LCDA.print(t.sec);

  LCDA.setCursor(1, 4);
  LCDA.chinese(shidu, 4);
  LCDA.print(t.sec);

  LCDA.setCursor(2, 0);
  LCDA.chinese(guangzhao, 4);
  LCDA.print(t.sec);
  LCDA.setCursor(3, 2);
  LCDA.chinese(shezhian, 6);
  LCDA.print ("S11");
  delay(1000);
}

void settime() { //串口设定时间函数
  /* 当串口有数据的时候，将数据拼接到变量comdata */
  while (Serial.available() > 0)
  {
    comdata += char(Serial.read());
    delay(2);
    mark = 1;
  }
  /* 以逗号分隔分解comdata的字符串，分解结果变成转换成数字到numdata[]数组 */
  if (mark == 1)
  {
    Serial.print("You inputed : ");
    Serial.println(comdata);
    for (int i = 0; i < comdata.length() ; i++)
    {
      if (comdata[i] == ',' || comdata[i] == 0x10 || comdata[i] == 0x13)
      {
        j++;
      }
      else
      {
        numdata[j] = numdata[j] * 10 + (comdata[i] - '0');
      }
    }
    /* 将转换好的numdata凑成时间格式，写入DS1302 格式为  年，月，日，时，分，秒，星期，  注意最后的逗号要带*/
    Time t(numdata[0], numdata[1], numdata[2], numdata[3], numdata[4], numdata[5], numdata[6]);
    rtc.time(t);
    mark = 0; j = 0;
    /* 清空 comdata 变量，以便等待下一次输入 */
    comdata = String("");
    /* 清空 numdata */
    for (int i = 0; i < 7 ; i++) numdata[i] = 0;
  }
}
void jiemian1() {
  LCDA.clear();
  LCDA.setCursor(c, 0);  //箭头显示的位置
  LCDA.print("--");
  LCDA.setCursor(0, 1);
  LCDA.chinese(shedingshijian, 8);
  LCDA.setCursor(1, 1);
  LCDA.chinese(shezhidingshikaiguan1, 14);
  LCDA.setCursor(2, 1);
  LCDA.chinese(shezhidingshikaiguan2, 14);
  LCDA.setCursor(3, 0);
  LCDA.print(1);
  LCDA.setCursor(3, 2);
  LCDA.chinese(xuandingan, 6);
  LCDA.print("S15");
}
void jiemian2() {
  LCDA.clear();
  LCDA.setCursor(b, 0);  //箭头显示的位置
  LCDA.print("--");
  LCDA.setCursor(0, 1);
  LCDA.chinese(shezhidingshikaiguan3, 14);
  LCDA.setCursor(1, 1);
  LCDA.chinese(shezhiwendubaojing, 12);
  LCDA.setCursor(2, 1);
  LCDA.chinese(shezhishidubaojing, 12);
  LCDA.setCursor(3, 0);
  LCDA.print(2);
  LCDA.setCursor(3, 2);
  LCDA.chinese(xuandingan, 6);
  LCDA.print("S15");
}
void jiemian3() {
  LCDA.clear();
  LCDA.setCursor(d, 0);  //箭头显示的位置
  LCDA.print("--");
  LCDA.setCursor(0, 1);
  LCDA.chinese(shezhiguangzhaobaojing, 12);
  LCDA.setCursor(3, 0);
  LCDA.print(3);
  LCDA.setCursor(3, 2);
  LCDA.chinese(xuandingan, 6);
  LCDA.print("S15");
}
void jiemian4() { //设定时间操作界面
  LCDA.clear();
  LCDA.setCursor(0, 2);
  LCDA.chinese(shedingshijian, 8); //设定时间

  LCDA.setCursor(1, 1);  //nian变量的位置
  LCDA.print(nian);
  LCDA.setCursor(1, 4);  //yue变量的位置
  LCDA.print(yue);
  LCDA.setCursor(1, 6);  //ri变量的位置
  LCDA.print(ri);
  LCDA.setCursor(2, 1);  //shi变量的位置
  LCDA.print(shi);
  LCDA.setCursor(2, 3);  //shi变量的位置
  LCDA.print(fen);
  LCDA.setCursor(2, 5);  //shi变量的位置
  LCDA.print(miao);


  LCDA.setCursor(1, 3);
  LCDA.chinese(nianzi, 2);
  LCDA.setCursor(1, 5);
  LCDA.chinese(yuezi, 2);
  LCDA.setCursor(1, 7);
  LCDA.chinese(rizi, 2);
  LCDA.setCursor(2, 2);
  LCDA.chinese(shizi, 2);
  LCDA.setCursor(2, 4);
  LCDA.chinese(fenzi, 2);
  LCDA.setCursor(2, 6);
  LCDA.chinese(miaozi, 2);

  LCDA.setCursor(3, 0);

  LCDA.chinese(baocunfanhuian, 10);   //
  LCDA.print("S12");
}

void setup() {
  Serial.begin(9600);
  rtc.write_protect(false);  //写保护否
  rtc.halt(false);  //停 否
  Timer1.initialize(1000000); // set a timer of length 100000 microseconds (or 0.1 sec - or 10Hz => the led will blink 5 times, 5 cycles of on-and-off, per second)
  Timer1.attachInterrupt( print_time );
}

void loop() {
  settime();   //DS1302设置时间
  customKey = customKeypad.getKey();  //键盘数字赋值给customKey，设置为int变量就可以选择了。
  switch (customKey ) {
    case 97: //界面一主界面，显示有时间，温度等信息。
      Serial.println("定时器关闭");  //
      Timer1.stop();
      c = 0; //将归零，使指示箭头“--”定位在第一行。
      zongzifushu = 0;  //总字符计数器清零
      nian = 0; yue = 0; ri = 0; shi = 0; fen = 0; miao = 0;
      jiemian1();
      jp = 1;
      break;
    case 98:   //界面二，设定时间，定时开关1等选择信息界面。
      Timer1.start();
      Timer1.initialize(1000000); // set a timer of length 100000 microseconds (or 0.1 sec - or 10Hz => the led will blink 5 times, 5 cycles of on-and-off, per second)
      Timer1.attachInterrupt( print_time );
      Serial.println("定时器启动");
      jp = 0; //关闭其他键盘
      c = 0;
      b = 0;
      d = 0;
      break;
    case 100: //菜单上翻页。S14翻菜单
      if (jp == 1) {
        c = c + 1;
        if (c < 3)  {
          LCDA.clear();
          jiemian1();
          Serial.println(c);
          Serial.println(b);
          Serial.println(d);
        }
        if (c == 3) {
          b = 0;
          LCDA.clear();
          jiemian2();
          Serial.println(c);
          Serial.println(b);
          Serial.println(d);
        }
        if (c >= 4) {
          b = b + 1;
          LCDA.clear();
          jiemian2();
        }
        if (c == 6) { //第三个界面只有一个选项，所以c只需要等于6就可以做出判断。
          d = 0;
          LCDA.clear();
          jiemian3();
        }
        if (c >= 7) {
          c = 0;
          b = 0;
          d = 0;
          LCDA.clear();
          jiemian1();
        }
      }
      break;
    case 101:  //101是S15
      //打开其他键盘
      while (c == 0 & b == 0 & d == 0 & jp == 1) { //判断的话要用==，单个=是赋值的意思。
        customKey = customKeypad.getKey();
        Serial.println(customKey);
        delay(100);   //刷新率越小键盘越灵敏。但是屏幕看不清除。
        jiemian4();

        if (customKey) { //如果监测到键盘有输入
          zongzifushu ++;
          comdata += customKey;
          Serial.println(comdata);
          switch (zongzifushu ) {  //通过监测键盘输入的字符分别
            case 4:
              nian = comdata.toInt();  //字符串转换为int整形变量。
              comdata = "";
              break;
            case 6:
              yue = comdata.toInt();  //字符串转换为int整形变量。
              comdata = "";
              break;
            case 8:
              ri = comdata.toInt();  //字符串转换为int整形变量。
              comdata = "";
              break;
            case 10:
              shi = comdata.toInt();  //字符串转换为int整形变量。
              comdata = "";
              break;
            case 12:
              fen = comdata.toInt();  //字符串转换为int整形变量。
              comdata = "";
              break;
            case 14:
              miao = comdata.toInt();  //字符串转换为int整形变量。
              comdata = "";
              break;
          }
        }
        if (customKey == 98) //如果监测按到S12就是98，退出循环
        {
          Serial.println(zongzifushu);
          if (zongzifushu == 15) { //如果总字符数不等于14说明没有输入完整，不执行设置字符程序。
            Time t(nian, yue, ri, shi, fen, miao, 3); //设置DS1302时间。
            rtc.time(t);
            zongzifushu = 0;
          }
          zongzifushu = 0;
          Timer1.start();
          Timer1.initialize(1000000); // set a timer of length 100000 microseconds (or 0.1 sec - or 10Hz => the led will blink 5 times, 5 cycles of on-and-off, per second)
          Timer1.attachInterrupt( print_time );
          Serial.println("定时器启动");
          comdata = "";  //清理字符串变量里面的内容，不然下次输入时间出问题。
          jp = 0; //关闭其他键盘
          c = 0;
          b = 0;
          d = 0;
          break;
        }
      }
      break;
  }
}

/*
    case 99:  //菜单下翻页
      c = c - 1;
      LCDA.clear();
      jiemian1();
      if (c < 0) {
        c = 7;
        LCDA.clear();
        jiemian3();
      Serial.println(c);
      Serial.println(b);
      Serial.println(d);
      }
      if (c==6)  {
        b = 2;
        LCDA.clear();
        jiemian2();
      Serial.println(c);
      Serial.println(b);
      Serial.println(d);
         }
      if (c<6)   {
        b = b - 1;
        LCDA.clear();
        jiemian2();
      Serial.println(c);
      Serial.println(b);
      Serial.println(d);
      }
      if (c==3 ) {
        c=2;
        LCDA.clear();
        jiemian1();
      Serial.println(c);
      Serial.println(b);
      Serial.println(d);
      }
      if (c<2 )  {
        LCDA.clear();
        jiemian1();
      Serial.println(c);
      Serial.println(b);
      Serial.println(d);
      }
      break;
*/
