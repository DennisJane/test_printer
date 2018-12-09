#pragma once

#define SET6(d,x,y,z,a,b,c)	{d[0]=x;d[1]=y;d[2]=z;d[3]=a;d[4]=b;d[5]=c;}
#define SET5(s,x,y,z,a,b) {s[0]=x;s[1]=y;s[2]=z;s[3]=a;s[4]=b;}
#define SET4(s,x,y,z,a) {s[0]=x;s[1]=y;s[2]=z;s[3]=a;}
#define SET3(s,x,y,z) {s[0]=x;s[1]=y;s[2]=z;}
#define SET2(s,x,y)   {s[0]=x;s[1]=y;}

#define EQU2(s,d) {s[0]=d[0];s[1]=d[1];}
#define EQU3(s,d) {s[0]=d[0];s[1]=d[1];s[2]=d[2];}
#define EQU4(s,d) {s[0]=d[0];s[1]=d[1];s[2]=d[2];s[3]=d[3];}
#define EQU5(s,d) {s[0]=d[0];s[1]=d[1];s[2]=d[2];s[3]=d[3];s[4]=d[4];}
#define EQU6(s,d) {s[0]=d[0];s[1]=d[1];s[2]=d[2];s[3]=d[3];s[4]=d[4];s[5]=d[5];}

#define PI 3.1415926535898
#define PIF 3.1415926535898f
#define DEG2RAD(x) (PI*(x/180.0))
#define RAD2DEG(x) (180.0*(x/PI))
