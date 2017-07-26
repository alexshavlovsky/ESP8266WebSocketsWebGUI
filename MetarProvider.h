#ifndef METARPROVIDER_H_
#define METARPROVIDER_H_

const char METAR_URL[] PROGMEM = "http://www.aviationweather.gov/adds/dataserver_current/httpparam?dataSource=metars&requestType=retrieve&format=xml&hoursBeforeNow=3&mostRecent=true&stationString=";

class MetarProvider {
    static String parseXMLtag(int start_pos,const String & buf,const String & tag) {
        int tag_idx = buf.indexOf(tag,start_pos);
        if(tag_idx==-1) return "";
        tag_idx+=tag.length();
        int tag_end = buf.indexOf('<',tag_idx);
        if(tag_end==-1) return "";
        return buf.substring(tag_idx,tag_end);
    }

public:
    const double NOT_SET=1234;

    double temp=NOT_SET;

    void get(String stID) {
        HTTPClient http;
        http.begin(METAR_URL+stID);
        int httpCode = http.GET();
        if(httpCode == HTTP_CODE_OK) {
            String payload = http.getString();
            String tmp = parseXMLtag(0,payload,"<temp_c>");
            if(tmp!="") temp = tmp.toFloat();
        }
        http.end();
    }
};

#endif //METARPROVIDER_H_
