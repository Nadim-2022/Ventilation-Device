//
// Created by iamna on 12/03/2024.
//
switch (key) {
case up:
if(scrollmenu == menu){
tft.selectmenu(0);
} else if(scrollmenu == automode){
tft.displayControlPressureLevel(setPressure);
} else if(scrollmenu == manualmode){
tft.displayControlSpeed(speed);
}
break;
case down:
if(scrollmenu == menu){
tft.selectmenu(1);
} else if(scrollmenu == automode){
tft.displayControlPressureLevel(setPressure);
} else if(scrollmenu == manualmode){
tft.displayControlSpeed(speed);
}
break;
case left:
if(scrollmenu == automode){
setPressure+=2;
if(setPressure > 125){
setPressure = 125;
}
tft.displayControlPressureLevel(setPressure);
} else if(scrollmenu == manualmode){
speed+=1;
if(speed >= 100){
speed = 100;
}
tft.displayControlSpeed(speed);
}
break;
case right:
if(scrollmenu == automode){
setPressure-=2;
if(setPressure < 0){
setPressure = 0;
}
tft.displayControlPressureLevel(setPressure);
} else if(scrollmenu == manualmode){
speed-=1;
if(speed <= 0){
speed = 0;
}
tft.displayControlSpeed(speed);
}
break;
case enter:
if(scrollmenu == menu){
if(tft.getmenu() == 0){
autoMode = false;
tft.fill(0);
tft.displayControlSpeed(speed);
scrollmenu = manualmode;
} else if(tft.getmenu() == 1){
autoMode = true;
tft.fill(0);
tft.displayControlPressureLevel(setPressure);
scrollmenu = automode;
}
} else if(scrollmenu == automode){
tft.fill(0);
tft.autoOrManual();
tft.selectmenu(1);
scrollmenu = menu;
} else if(scrollmenu == manualmode){
tft.fill(0);
tft.autoOrManual();
tft.selectmenu(1);
scrollmenu = menu;
}
break;
default:
break;

}