/*
  ESP8266WebSocketsWebGUI.h -- ESP8266 WEB GUI library for Arduino IDE

  Copyright (c) 2017 Alex Shavlovsky. All rights reserved.
 
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef WEBSOCKETSMENU_H_
#define WEBSOCKETSMENU_H_

#include <DiscretTimer.h>
#define OUTPUTS_MIN_REFRESH_INTERVAL 1000
#define STRING_OUTPUT_MAX_LENGTH 32

const char HTML_TEMPLATE[] PROGMEM =
"<!DOCTYPE html>\n"
"<html>\n"
"<head>\n"
"<title>ESP menu</title>\n"
"<meta charset=\"utf-8\">\n"
"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
"<style>\n"
"body {margin:0 0;}\n"
"#menu div{padding:8px 0;display:flex;flex-direction:row;flex-wrap:nowrap;align-items:center;border-bottom:1px solid #DDD;font-family:Lucida Sans Unicode;}\n"
"#menu div.top{border-bottom:3px solid #888;}\n"
"#menu p{margin:0 0;word-break:break-all;flex-grow:1;font-size:18px;line-height:24px;}\n"
"#menu input[type=text]{width:100%;font-size:16px;padding:4px 8px;margin:2px 0;display:inline-block;border:1px solid #DDD;border-radius:4px;box-sizing:border-box;}\n"
"#menu input[type=text]:focus{border:1px solid #888;}\n"
"#menu input[type=button]{float:right;background-color:#888;border:none;color:#FFF;text-decoration:none;margin:4px 22px 4px 4px;padding:10px 4px;border-radius:4px;}\n"
"#menu input[type=button]:active{color:#FEB;}\n"
"#menu input[type=button].low{padding:2px 12px;}\n"
"#menu p i{font-style:normal;color:#888;font-size:16px;}\n"
"#menu .top p{text-align:center;font-size:24px;}\n"
"#menu svg{margin:0 16px;height:48px;width:48px;fill:#27AE60;}\n"
"#menu .fldr svg{fill:#3498DB;}\n"
"#menu .fldr:active{background-color:#FEB;}\n"
"#menu .key svg{fill:#888;}\n"
"#menu .key:active{outline:1px solid #DDD;background-color:#FEB;}\n"
"</style>\n"
"</head>\n"
"<body>\n"
"<svg xmlns=\"http://www.w3.org/2000/svg\" style=\"display:none\">\n"
"<symbol id=\"b-hom\" viewBox=\"0 0 24 24\">\n"
"<path d=\"M10 20v-6h4v6h5v-8h3L12 3 2 12h3v8z\"/>\n"
"</symbol>\n"
"<symbol id=\"i-kbd\" viewBox=\"0 0 24 24\">\n"
"<path d=\"M20 5H4c-1.1 0-1.99.9-1.99 2L2 17c0 1.1.9 2 2 2h16c1.1 0 2-.9 2-2V7c0-1.1-.9-2-2-2zm-9 3h2v2h-2V8zm0 3h2v2h-2v-2zM8 8h2v2H8V8zm0 3h2v2H8v-2zm-1 2H5v-2h2v2zm0-3H5V8h2v2zm9 7H8v-2h8v2zm0-4h-2v-2h2v2zm0-3h-2V8h2v2zm3 3h-2v-2h2v2zm0-3h-2V8h2v2z\"/>\n"
"</symbol>\n"
"<symbol id=\"i-fol\" viewBox=\"0 0 24 24\">\n"
"<path d=\"M10 4H4c-1.1 0-1.99.9-1.99 2L2 18c0 1.1.9 2 2 2h16c1.1 0 2-.9 2-2V8c0-1.1-.9-2-2-2h-8l-2-2z\"/>\n"
"</symbol>\n"
"<symbol id=\"i-ent\" viewBox=\"0 0 24 24\">\n"
"<path d=\"M10 6L8.59 7.41 13.17 12l-4.58 4.59L10 18l6-6z\"/>\n"
"</symbol>\n"
"<symbol id=\"b-esc\" viewBox=\"0 0 24 24\">\n"
"<path d=\"M15.41 7.41L14 6l-6 6 6 6 1.41-1.41L10.83 12z\"/>\n"
"</symbol>\n"
"<symbol id=\"i-inp\" viewBox=\"0 0 24 24\">\n"
"<path d=\"M19.43 12.98c.04-.32.07-.64.07-.98s-.03-.66-.07-.98l2.11-1.65c.19-.15.24-.42.12-.64l-2-3.46c-.12-.22-.39-.3-.61-.22l-2.49 1c-.52-.4-1.08-.73-1.69-.98l-.38-2.65C14.46 2.18 14.25 2 14 2h-4c-.25 0-.46.18-.49.42l-.38 2.65c-.61.25-1.17.59-1.69.98l-2.49-1c-.23-.09-.49 0-.61.22l-2 3.46c-.13.22-.07.49.12.64l2.11 1.65c-.04.32-.07.65-.07.98s.03.66.07.98l-2.11 1.65c-.19.15-.24.42-.12.64l2 3.46c.12.22.39.3.61.22l2.49-1c.52.4 1.08.73 1.69.98l.38 2.65c.03.24.24.42.49.42h4c.25 0 .46-.18.49-.42l.38-2.65c.61-.25 1.17-.59 1.69-.98l2.49 1c.23.09.49 0 .61-.22l2-3.46c.12-.22.07-.49-.12-.64l-2.11-1.65zM12 15.5c-1.93 0-3.5-1.57-3.5-3.5s1.57-3.5 3.5-3.5 3.5 1.57 3.5 3.5-1.57 3.5-3.5 3.5z\"/>\n"
"</symbol>\n"
"<symbol id=\"b-add\" viewBox=\"0 0 24 24\">\n"
"<path d=\"m18 4-12 0c-1.1 0-2 0.9-2 2L4 18c0 1.1 0.9 2 2 2l12 0c1.1 0 2-0.9 2-2L20 6c0-1.1-0.9-2-2-2zM17 13l-4 0 0 4-2 0 0-4-4 0 0-2 4 0 0-4 2 0L13 11l4 0z\"/>\n"
"</symbol>\n"
"<symbol id=\"b-sub\" viewBox=\"0 0 24 24\">\n"
"<path d=\"m18 4-12 0c-1.1 0-2 0.9-2 2L4 18c0 1.1 0.9 2 2 2l12 0c1.1 0 2-0.9 2-2L20 6c0-1.1-0.9-2-2-2zM17 13c-10 0 0 0-10 0l0-2 4 0c6 0 0 0 6 0z\"/>\n"
"</symbol>\n"
"<symbol id=\"i-out\" viewBox=\"0 0 24 24\">\n"
"<path d=\"M11 17h2v-6h-2v6zm1-15C6.48 2 2 6.48 2 12s4.48 10 10 10 10-4.48 10-10S17.52 2 12 2zm0 18c-4.41 0-8-3.59-8-8s3.59-8 8-8 8 3.59 8 8-3.59 8-8 8zM11 9h2V7h-2v2z\"/>\n"
"</symbol>\n"
"<symbol id=\"b-1\" viewBox=\"0 0 24 24\">\n"
"<path d=\"M19 3H5c-1.11 0-2 .9-2 2v14c0 1.1.89 2 2 2h14c1.11 0 2-.9 2-2V5c0-1.1-.89-2-2-2zm-9 14l-5-5 1.41-1.41L10 14.17l7.59-7.59L19 8l-9 9z\"/>\n"
"</symbol>\n"
"<symbol id=\"b-0\" viewBox=\"0 0 24 24\">\n"
"<path d=\"M19 5v14H5V5h14m0-2H5c-1.1 0-2 .9-2 2v14c0 1.1.9 2 2 2h14c1.1 0 2-.9 2-2V5c0-1.1-.9-2-2-2z\"/>\n"
"</symbol>\n"
"</svg>\n"
"<div id=\"menu\">\n"
"<div class=\"top\">\n"
"<span><svg class=\"key\" onclick=\"F1('M1')\"><use xlink:href=\"#b-hom\"></use></svg></span>\n"
"<p>Waiting for device response...</p>\n"
"</div>\n"
"</div>\n"
"<script>\n"
"var v_id;\n"
"var v_0=0;\n"
"var v_s=0;\n"
"var v_a=0;\n"
"var v_d=0;\n"
"var v_t=0;\n"
"var connection = new WebSocket('ws://'+location.hostname+':81/',['arduino']);\n"
"connection.onopen=function(){\n"
"connection.send('M1');\n"
"console.log('To   ESP: M1');\n"
"};\n"
"connection.onerror=function(error){\n"
"console.log('Error: ',error);\n"
"};\n"
"connection.onmessage=function(e){\n"
"console.log('From ESP: '+e.data);\n"
"if (e.data.charAt(0)==='P'&&v_t===0){\n"
"arr=e.data.split('=');\n"
"document.getElementById(arr[0]).innerHTML = arr[1];}\n"
"if (e.data.charAt(0)==='D') {\n"
"var tmp1=e.data.substring(0,e.data.indexOf('='));\n"
"var tmp2=e.data.substring(e.data.indexOf('=')+1);\n"
"console.log('t1: '+tmp1);\n"
"console.log('t2: '+tmp2);\n"
"document.getElementById(tmp1).innerHTML=tmp2;\n"
"}\n"
"if (e.data.charAt(0)==='x'){\n"
"document.getElementById(\"menu\").innerHTML = '';}\n"
"if (e.data.charAt(0)==='<'){\n"
"document.getElementById(\"menu\").innerHTML += e.data;}\n"
"}\n"
"function v_print(){\n"
"document.getElementById(v_id).innerHTML = \"<span style=\\\"color:red;\\\">\"+(v_0+v_d*v_s)+\"</span>\";\n"
"}\n"
"function tmr1fn(){\n"
"if (v_t!==0){if (v_t>1) {v_t--;} else {\n"
"v_d=v_d+v_a;\n"
"v_print();\n"
"if (v_d===50) {v_a=v_a*10}\n"
"if (v_d===500) {v_a=v_a*10}\n"
"}}}\n"
"var tmr1id = setInterval(tmr1fn,100);\n"
"function F1(txt) {\n"
"if (txt.charAt(0)==='M') {\n"
"connection.send(txt);console.log('To   ESP: '+txt);\n"
"}\n"
"if (txt.charAt(0)==='B') {\n"
"var tmp='P'+txt.substring(1);\n"
"connection.send(tmp);console.log('To   ESP: '+tmp);\n"
"}\n"
"if (txt.charAt(0)==='E') {\n"
"arr=txt.split('=');\n"
"v_id='P'+arr[0].substring(1);\n"
"var v_str=document.getElementById(v_id).innerHTML;\n"
"if (v_str.charAt(0)==='<') {return;}\n"
"v_0=Number(v_str);\n"
"v_s=Number(arr[1]);\n"
"v_a=1;\n"
"v_d=v_a;\n"
"v_print();\n"
"v_t=10;}\n"
"if (txt==='R'&&v_t!==0) {\n"
"v_t=0;\n"
"v_print();\n"
"var tmp=v_id+\"=\"+(v_0+v_d*v_s);\n"
"connection.send(tmp);console.log('To   ESP: '+tmp);\n"
"}}\n"
"</script>\n"
"</body>\n"
"</html>\n";
// end of const char HTML_TEMPLATE[] PROGMEM =

const char HTML_MFOLDER[] PROGMEM =
"<div class=\"fldr\" onclick=\"F1('M%d')\">\n"
"<span><svg><use xlink:href=\"#i-fol\"></use></svg></span>\n"
"<p>%s</p>\n"
"<span><svg><use xlink:href=\"#i-ent\"></use></svg></span>\n"
"</div>\n";
//%d - link to LVL
//%s - folder title

const char HTML_MROOT[] PROGMEM =
"<div class=\"top\">\n"
"<p>%s</p>\n"
"</div>\n";
//%s - root title

const char HTML_MTITLE[] PROGMEM =
"<div class=\"top\">\n"
"<span><svg class=\"key\" onclick=\"F1('M1')\"><use xlink:href=\"#b-hom\"></use></svg></span>\n"
"<p>%s</p>\n"
"<span><svg class=\"key\" onclick=\"F1('M%d')\"><use xlink:href=\"#b-esc\"></use></svg></span>\n"
"</div>\n";
//%s - menu title
//%d - escape to LVL

const char HTML_MPOUTINT[] PROGMEM =
"<div>\n"
"<span><svg><use xlink:href=\"#i-out\"></use></svg></span>\n"
"<p>%s<br><i id=\"P%d\">%d</i><i>%s</i></p>\n"
"</div>\n";
//%s - param name
//%d - param id
//%d - param value
//%s - param dimension

const char HTML_MPOUTSTR[] PROGMEM =
"<div>\n"
"<span><svg><use xlink:href=\"#i-out\"></use></svg></span>\n"
"<p>%s<br><i id=\"P%d\">%s</i></p>\n"
"</div>";
//%s - param name
//%d - param id
//%s - param str

const char HTML_MPINPINT[] PROGMEM = 
"<div>\n"
"<span><svg><use xlink:href=\"#i-inp\"></use></svg></span>\n"
"<p>%s<br><i id=\"P%d\">%d</i><i>%s</i></p>\n"
"<span><svg class=\"key\" onmousedown=\"F1('E%d=-1')\" onmouseup=\"F1('R')\" onmouseout=\"F1('R')\">\n"
"<use xlink:href=\"#b-sub\"></use></svg></span>\n"
"<span><svg class=\"key\" onmousedown=\"F1('E%d=1')\" onmouseup=\"F1('R')\" onmouseout=\"F1('R')\">\n"
"<use xlink:href=\"#b-add\"></use></svg></span>\n"
"</div>\n";
//%s - param name
//%d - param id
//%d - param value
//%s - param dimension
//%d - param id
//%d - param id

const char HTML_MPINPBOOL[] PROGMEM =
"<div id=\"D%d\">\n"
"<span><svg><use xlink:href=\"#i-inp\"></use></svg></span>\n"
"<p>%s<br><i>%s</i></p>\n"
"<span><svg class=\"key\" onclick=\"F1('B%d=%d')\">\n"
"<use xlink:href=\"#b-%d\"></use></svg></span>\n"
"</div>\n";

const char HTML_MPINPBOOL_HANDLER[] PROGMEM =
"D%d=<span><svg><use xlink:href=\"#i-inp\"></use></svg></span>\n"
"<p>%s<br><i>%s</i></p>\n"
"<span><svg class=\"key\" onclick=\"F1('B%d=%d')\">\n"
"<use xlink:href=\"#b-%d\"></use></svg></span>\n";
//%d - param id
//%s - param name
//%s - param description
//%d - param id
//%d - param value    (1:0)
//%d - !(param value) (0:1)

const char HTML_MPINPSTR[] PROGMEM = 
"<div>\n"
"<span><svg><use xlink:href=\"#i-kbd\"></use></svg></span>\n"
"<p>%s<input type=\"button\" value=\"save\" onclick=\"F1('B%d='+document.getElementById('T%d').value)\"><br>\n"
"<i id=\"D%d\">%s</i><br><input size=\"10\" id=\"T%d\" type=\"text\" value=\"%s\"></p>\n"
"</div>\n";
//%s - param name
//%d - param id
//%s - param val
//%d - param id
//%s - param val
//%d - param id
//%d - param id

const char HTML_MBTN[] PROGMEM = 
"<div>\n"
"<span><svg><use xlink:href=\"#i-kbd\"></use></svg></span>\n"
"<p>%s<input type=\"button\" class=\"low\" value=\"ok\" onclick=\"F1('B%d=1')\"></p>\n"
"</div>\n";
//%s - param name
//%d - param id

class MenuItem {
public:
    MenuItem(uint8_t Lvl) : itemIndex(itemCount), itemLevel(Lvl) {
        itemCount++;
    };
    // If the USER travels through the menu
    // the CONTROLLER requests the full VIEW refresh if the menu level is changed
    // return the full HTML code to the VIEW
    virtual void getHTMLCode(uint8_t clientSlot,char* bf,size_t bfsz) {
        bf[0] = '\0';
    };
    // If the USER interacts with the menu's item
    // makes changes in the MODEL and return refresh message to the VIEW
    // userInput - message from the CONTROLLER
    virtual void getResponseMessageToUserInput(String userInput,char* bf,size_t bfsz) {
        bf[0] = '\0';
    };
    // If the MODEL is changed
    // return refresh message to the VIEW
    // MODEL - publisher
    // VIEW - subscriber
    virtual void getRefreshMessage(char* bf,size_t bfsz) {
        bf[0] = '\0';
    };
    virtual void getEEPROMPointer(uint16_t &offset,uint8_t &siz) {
        siz=0;
    }
    virtual uint8_t getValToEEPROM(uint16_t bytePos) {
        return 0;
    }
    virtual void setValFromEEPROM(uint16_t bytePos,uint8_t byteVal) {};
    uint8_t getLevel() {
        return itemLevel;
    }
    static uint8_t clientLevel[];
    static uint16_t eepromOffset;
protected:
    uint8_t itemIndex;
    uint8_t itemLevel;
private:
    static uint8_t itemCount;
};
uint16_t MenuItem::eepromOffset = 0;             
uint8_t MenuItem::itemCount = 0;                //number of items
uint8_t MenuItem::clientLevel[5] = {0, 0, 0, 0, 0}; //client's menu level / 0 = client's slot empty


class MenuFolder: public MenuItem {
public:
    MenuFolder(uint8_t Lvl, uint8_t ToLvl, const char* FName) :
        MenuItem(Lvl), linkLevel(ToLvl), folderName(FName) {};
    void getHTMLCode(uint8_t ClientN,char* bf,size_t bfsz) {
        if(clientLevel[ClientN] == itemLevel) snprintf_P(bf, bfsz, HTML_MFOLDER, linkLevel, folderName);
        else if(clientLevel[ClientN] == linkLevel) {
            if(linkLevel == 1) snprintf_P(bf, bfsz, HTML_MROOT, folderName);
            else snprintf_P(bf, bfsz, HTML_MTITLE, folderName, itemLevel);
        } else bf[0] = '\0';
    };
protected:
    uint8_t linkLevel;
    const char* folderName;
};

class MenuOutputInteger: public MenuItem {
    DiscretTimer* refreshTimer = new DiscretTimer(OUTPUTS_MIN_REFRESH_INTERVAL);
    std::function<int32_t(void)> geterF;
    const char* MPName;
    int32_t MPVal;
    const char* MPDimens;
    bool inited;
    bool UpdateVal(bool force) {
        if((!inited&&force)||refreshTimer->onSample()) {
            int32_t preVal=MPVal;
            MPVal = geterF();
	    inited=true;
	    return (MPVal!=preVal);
        } else return false;
    };
public:
    MenuOutputInteger(uint8_t Lvl, const char* PName, const char* PDimens, std::function<int32_t(void)> getF, uint32_t UpdatePeriod) :
        MenuItem(Lvl), MPName(PName), MPDimens(PDimens), geterF(getF) {
        refreshTimer->setPeriod(UpdatePeriod);
	inited=false;
    };
    void getHTMLCode(uint8_t ClientN,char* bf,size_t bfsz) {
        if(clientLevel[ClientN] == itemLevel) {
	    UpdateVal(true);
            snprintf_P(bf, bfsz, HTML_MPOUTINT, MPName, itemIndex, MPVal, MPDimens);
        } else bf[0] = '\0';
    };
    void getRefreshMessage(char* bf,size_t bfsz) {
        bf[0] = '\0';
        for(uint8_t ClientN=0; ClientN<5; ClientN++)
            if((clientLevel[ClientN] == itemLevel)&&(UpdateVal(false))) {
                snprintf(bf, bfsz, "P%d=%d", itemIndex, MPVal);
                break;
            }
    };
};

class MenuOutputString: public MenuItem {
    DiscretTimer* refreshTimer = new DiscretTimer(OUTPUTS_MIN_REFRESH_INTERVAL);
    std::function<void(char *,size_t)> geterF;
    const char* MPName;
    char MPStr[STRING_OUTPUT_MAX_LENGTH];
    bool inited;
    bool UpdateVal(bool force) {
        if((!inited&&force)||refreshTimer->onSample()) {
            char preStr[STRING_OUTPUT_MAX_LENGTH];
            strcpy(preStr,MPStr);
            geterF(&MPStr[0],sizeof(MPStr));
	    inited=true;
            return (strcmp(preStr,MPStr)!=0);
        } else return false;
    };
public:
    MenuOutputString(uint8_t Lvl, const char* PName, std::function<void(char*,size_t)> getF, uint32_t UpdatePeriod) :
        MenuItem(Lvl), MPName(PName), geterF(getF) {
        refreshTimer->setPeriod(UpdatePeriod);
	inited=false;
    };
    void getHTMLCode(uint8_t ClientN,char* bf,size_t bfsz) {
        if(clientLevel[ClientN] == itemLevel) {
            UpdateVal(true);
            snprintf_P(bf, bfsz, HTML_MPOUTSTR, MPName, itemIndex, MPStr);
        } else bf[0] = '\0';
    };
    void getRefreshMessage(char* bf,size_t bfsz) {
        bf[0] = '\0';
        for(uint8_t ClientN=0; ClientN<5; ClientN++)
            if((clientLevel[ClientN] == itemLevel) && (this->UpdateVal(false))) {
                snprintf(bf,bfsz,"P%d=%s", itemIndex, MPStr);
                break;
            }
    };
};

class MenuInputInteger: public MenuItem {
public:
    MenuInputInteger(uint8_t Lvl, const char* PName, int16_t PVal, const char* PDimens, int16_t PValMin, int16_t PValMax, int16_t *&ptrInt) :
        MenuItem(Lvl), MPName(PName), MPVal(PVal), MPDimens(PDimens), MPValMin(PValMin), MPValMax(PValMax), EEPROMadr(eepromOffset) {
        eepromOffset+=2;
        ptrInt=&MPVal;
    };
    void getHTMLCode(uint8_t ClientN,char* bf,size_t bfsz) {
        if(clientLevel[ClientN] == itemLevel)
            snprintf_P(bf, bfsz, HTML_MPINPINT, MPName, itemIndex, MPVal, MPDimens, itemIndex, itemIndex);
        else bf[0] = '\0';
    };
    void getResponseMessageToUserInput(String userInput,char* bf,size_t bfsz) {
        int NewVal = userInput.toInt();
        if(NewVal > MPValMax) NewVal = MPValMax;
        if(NewVal < MPValMin) NewVal = MPValMin;
        MPVal = NewVal;
        snprintf(bf, bfsz, "P%d=%d", itemIndex, MPVal);
    };
    void getEEPROMPointer(uint16_t &offset,uint8_t &siz) {
        offset=EEPROMadr;
        siz=2;
    }
    uint8_t getValToEEPROM(uint16_t bytePos) {
        return *((uint8_t*)&MPVal+bytePos);
    }
    void setValFromEEPROM(uint16_t bytePos,uint8_t byteVal) {
        *((uint8_t*)&MPVal+bytePos)=byteVal;
    }
private:
    const char* MPName;
    int16_t MPVal;
    const char* MPDimens;
    int16_t MPValMin;
    int16_t MPValMax;
    uint16_t EEPROMadr;
};

class MenuInputBoolean: public MenuItem {
public:
    MenuInputBoolean(uint8_t Lvl, const char* PName, int8_t PVal, const char* PDesc0, const char* PDesc1, uint8_t *&ptrBool) :
        MenuItem(Lvl), MPName(PName), MPVal(PVal), MPDesc0(PDesc0), MPDesc1(PDesc1), EEPROMadr(eepromOffset) {
        eepromOffset+=1;
        ptrBool=&MPVal;
    };
    void getHTMLCode(uint8_t ClientN,char* bf,size_t bfsz) {
        if(clientLevel[ClientN] == itemLevel)
            snprintf_P(bf, bfsz, HTML_MPINPBOOL, itemIndex, MPName, (MPVal==0) ? MPDesc0 : MPDesc1, itemIndex, (MPVal==0) ? 1 : 0, (MPVal==0) ? 0 : 1);
        else bf[0] = '\0';
    };
    void getResponseMessageToUserInput(String userInput,char* bf,size_t bfsz) {
        int NewVal = userInput.toInt();
        if((NewVal == 0)||(NewVal == 1)) {
            MPVal = NewVal;
            snprintf_P(bf, bfsz, HTML_MPINPBOOL_HANDLER, itemIndex, MPName, (MPVal==0) ? MPDesc0 : MPDesc1, itemIndex, (MPVal==0) ? 1 : 0, (MPVal==0) ? 0 : 1);
        } else bf[0] = '\0';
    };
    void getEEPROMPointer(uint16_t &offset,uint8_t &siz) {
        offset=EEPROMadr;
        siz=1;
    }
    uint8_t getValToEEPROM(uint16_t bytePos) {
        return MPVal;
    }
    void setValFromEEPROM(uint16_t bytePos,uint8_t byteVal) {
        MPVal=byteVal;
    }
private:
    const char* MPName;
    const char* MPDesc0;
    const char* MPDesc1;
    uint8_t MPVal;
    uint16_t EEPROMadr;
};



class MenuInputString: public MenuItem {
public:
    MenuInputString(uint8_t Lvl, const char* PName, const char* DefVal, char*& ptrChar) :
        MenuItem(Lvl), MPName(PName), EEPROMadr(eepromOffset) {
        ptrChar=&MPStr[0];
        setVal(DefVal);
        eepromOffset+=STRING_OUTPUT_MAX_LENGTH;
    };
    void getHTMLCode(uint8_t ClientN,char* bf,size_t bfsz) {
        if(clientLevel[ClientN] == itemLevel) {
            snprintf_P(bf, bfsz, HTML_MPINPSTR, MPName, itemIndex, itemIndex, itemIndex, MPStr, itemIndex, MPStr);
        } else bf[0] = '\0';
    };
    void getResponseMessageToUserInput(String userInput,char* bf,size_t bfsz) {
        userInput.toCharArray(MPStr, sizeof(MPStr));
        snprintf(bf, bfsz, "D%d=%s", itemIndex, MPStr);
    };
    void setVal(const char* PStr) {
        snprintf(MPStr, sizeof(MPStr), PStr);
    };
    void getEEPROMPointer(uint16_t &offset,uint8_t &siz) {
        offset=EEPROMadr;
        siz=STRING_OUTPUT_MAX_LENGTH;
    }
    uint8_t getValToEEPROM(uint16_t bytePos) {
        return *((uint8_t*)&MPStr+bytePos);
    }
    void setValFromEEPROM(uint16_t bytePos,uint8_t byteVal) {
        *((uint8_t*)&MPStr+bytePos)=byteVal;
    }
private:
    const char* MPName;
    char MPStr[STRING_OUTPUT_MAX_LENGTH];
    uint16_t EEPROMadr;
};

class EEPROMuint16: public MenuItem {
public:
    EEPROMuint16(uint16_t PVal, uint16_t *&ptrInt) :
        MenuItem(0), MPVal(PVal), EEPROMadr(eepromOffset) {
        eepromOffset+=2;
        ptrInt=&MPVal;
    };
    void getEEPROMPointer(uint16_t &offset,uint8_t &siz) {
        offset=EEPROMadr;
        siz=2;
    }
    uint8_t getValToEEPROM(uint16_t bytePos) {
        return *((uint8_t*)&MPVal+bytePos);
    }
    void setValFromEEPROM(uint16_t bytePos,uint8_t byteVal) {
        *((uint8_t*)&MPVal+bytePos)=byteVal;
    }
private:
    uint16_t MPVal;
    uint16_t EEPROMadr;
};


class MenuButton: public MenuItem {
public:
    MenuButton(uint8_t Lvl, const char* PName, std::function<void(void)> callF):
        MenuItem(Lvl), MPName(PName), callbackF(callF) {};
    void getHTMLCode(uint8_t ClientN,char* bf,size_t bfsz) {
        if(clientLevel[ClientN] == itemLevel) {
            snprintf_P(bf, bfsz, HTML_MBTN, MPName, itemIndex);
        } else bf[0] = '\0';
    };
    void getResponseMessageToUserInput(String userInput,char* bf,size_t bfsz) {
        if (userInput.toInt()==1) callbackF();
        bf[0] = '\0';
    };
private:
    const char* MPName;
    std::function<void(void)> callbackF;
};


#endif //WEBSOCKETSMENU_H_
