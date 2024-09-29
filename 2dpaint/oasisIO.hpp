#ifndef OASISIO_H
#define OASISIO_H

#include <iostream>
//#include <bitset>
#include <fstream>
#include <vector>

namespace oasisio {

using byte = unsigned char;

#define MAGIC "%SEMI-OASIS\r\n"
#define VERSION "1.0"

#define BINARY 1
#define ASCII 2
#define NAME 3

#define POSITIVE_WHOLE 0
#define NEGATIVE_WHOLE 1
#define POSITIVE_RECIPROCAL 2
#define NEGATIVE_RECIPROCAL 3
#define POSITIVE_RATIO 4
#define NEGATIVE_RATIO 5
#define SINGLE_PRECISION_FLOAT 6
#define DOUBLE_PRECISION_FLOAT 7

#define DELTA_1 0
#define DELTA_2 1
#define DELTA_3 2
#define DELTA_G1 3
#define DELTA_G2 4
#define DELTA_G 5

class Delta {
protected:
    int type;
    int deltaX;
    int deltaY;

public:
    Delta(int t, int dx, int dy=0)
        : type(t)
        , deltaX(dx)
        , deltaY(dy)
    {}

    int getType() const {
        return type;
    }

    int getDeltaX() const {
        return deltaX;
    }

    int getDeltaY() const {
        return deltaY;
    }

};

#define POINT_LIST_0 0
#define POINT_LIST_1 1
#define POINT_LIST_2 2
#define POINT_LIST_3 3
#define POINT_LIST_4 4
#define POINT_LIST_5 5

class PointList {
protected:
    int type;
    std::vector<Delta> list;

public:
    PointList(int t)
        : type(t)
    {}

    int getType() const {
        return type;
    }

    void setType(int t) {
        type = t;
    }

    std::vector<Delta> getList() const {
        return list;
    }

    void addDelta(Delta& delta) {
        list.push_back(delta);
    }

    void addDelta(int t, int dx, int dy=0) {
        Delta next(t, dx, dy);
        list.push_back(next);
    }
};

std::ostream& operator<<(std::ostream& o, const PointList& pl);

class OasisWriter {

public:
    OasisWriter()
    {}

    static void toBytesChar(std::ofstream& f, char c) {
        char out[1] = {c};
        f.write(out, 1);
    }

    static void toBytesReal(std::ofstream& f, int type, float flo) {

        if(type < 0 || type > 7) {
            return;
        }

        char format[1] = {(char)type};
        f.write(format, 1);

        switch(type){
        case POSITIVE_WHOLE:
        case NEGATIVE_WHOLE:
            toBytesUnsigned(f, (unsigned int)flo);
            break;
        case POSITIVE_RECIPROCAL:
        case NEGATIVE_RECIPROCAL:
            toBytesUnsigned(f, (unsigned int)flo);
            break;
        case SINGLE_PRECISION_FLOAT:
            toBytesFloat(f, flo, true);
            break;
        case DOUBLE_PRECISION_FLOAT:
            toBytesFloat(f, flo, false);
            break;
        default:
            throw std::exception("Type does not match input.");
        }

    }

    static void toBytesReal(std::ofstream& f, int type, unsigned int i1, unsigned int i2) {

        if(type < 0 || type > 7) {
            return;
        }

        char format[1] = {(char)type};
        f.write(format, 1);

        switch(type){
        case POSITIVE_RATIO:
        case NEGATIVE_RATIO:
            toBytesUnsigned(f, i1);
            toBytesUnsigned(f, i2);
            break;
        default:
            throw std::exception("Type does not match input.");
        }

    }

    static void toBytesUnsigned(std::ofstream& f, unsigned int u_int) {

        byte out[4] = {0};
        byte mask = 127;

        int i = -1;
        while(u_int > 0) {
            ++i;
            if(i >= 4){
                throw std::exception("Not enough memory.");
            }
            byte next = (u_int & mask) + 128;
            u_int = u_int >> 7;
            out[i] = next;
        }

        if(i >= 0) {
            out[i] = out[i] & mask;
        }

        /*
        int j;
        for(j=0; j<4; ++j) {
            std::cout << std::bitset<8>((int)out[j]).to_string() << " ";
        }
        std::cout << std::endl;
        */

        f.write((char *)out, sizeof(out));

    }

    static void toBytesSigned(std::ofstream& f, signed int s_int) {

        byte out[4] = {0};
        byte mask = 127;
        bool negative = s_int < 0;
        if(negative) {
            s_int = s_int * -1;
        }

        byte first = ((s_int & 63) << 1) + 128;
        s_int = s_int >> 6;
        if(negative) {
            ++first;
        }
        out[0] = first;

        int i = 0;
        while(s_int > 0) {
            ++i;
            if(i >= 4){
                throw std::exception("Not enough memory.");
            }
            byte next = (s_int & mask) + 128;
            s_int = s_int >> 7;
            out[i] = next;
        }

        if(i >= 0) {
            out[i] = out[i] & mask;
        }

        /*
        int j;
        for(j=0; j<4; ++j) {
            std::cout << std::bitset<8>((int)out[j]).to_string() << " ";
        }
        std::cout << std::endl;
        */

        f.write((char *)out, sizeof(out));

    }

    static void toBytesFloat(std::ofstream& f, float fl, bool singlePrec) {

        int size, expoBias, expoPos, expoLen;
        if(singlePrec) {
            size = 4;
            expoBias = 127;
            expoPos = 23;
            expoLen = 8;
        } else {
            size = 8;
            expoBias = 1023;
            expoPos = 52;
            expoLen = 11;
        }

        byte out[8] = {0};
        if(fl < 0) {
            fl = fl * -1;
            out[0] = out[0] + 128;
        }
        long long whole = (long long) floor(fl);
        double decimal = fl - whole;

        //std::cout << "Whole : " << whole << std::endl;
        //std::cout << "Decimal : " << decimal << std::endl;

        int shift = 0;
        while(decimal != 0 && whole < (((long long)1) << (expoPos-1))) {

            whole = whole << 1;
            ++shift;
            decimal *= 2;

            //std::cout << decimal << std::endl;

            if(decimal >= 1) {
                ++whole;
                --decimal;
            }
        }

        int mostSig = mostSigBit(whole);
        int expo = mostSig - shift;

        //std::cout << "Whole    : " << std::bitset<23>(whole).to_string() << std::endl;
        //std::cout << "Most Sig : " << mostSig << std::endl;
        //std::cout << "Exponent : " << expo << std::endl;

        setBit(expoPos, expoLen, expo + expoBias, out, size);
        setBit(expoPos - mostSig, mostSig, whole, out, size);

        /*
        int i;
        for(i=0; i<4; ++i) {
            std::cout << std::bitset<8>((int)out[i]).to_string() << " ";
        }
        std::cout << std::endl;
        */

        f.write((char *)out, size);

    }


    static void setBit(int pos, int len, long long num, byte* out, int size) {

        int index = size - (pos / 8) - 1;
        int bit = pos % 8 ;

        int count = std::min(8-bit, len);

        //std::cout << "Index : " << index << std::endl;
        //std::cout << "Bit   : " << bit << std::endl;
        //std::cout << "Count : " << count << std::endl;

        unsigned int clear_mask = ~((255 >> (8 - count)) << bit);
        unsigned int extract_mask = 255 >> (8 - count);

        //std::cout << "Clear Mask   : " << std::bitset<8>(clear_mask).to_string() << std::endl;
        //std::cout << "Extract Mask : " << std::bitset<8>(extract_mask).to_string() << std::endl;

        unsigned int insert = (num & extract_mask) << bit;

        //std::cout << "Insert       : " << std::bitset<8>(insert).to_string() << std::endl;

        out[index] = out[index] & clear_mask;
        out[index] = out[index] | insert;

        pos += count;
        len -= count;
        num = num >> count;

        if(len > 0) {
            setBit(pos, len, num, out, size);
        }

    }

    static int mostSigBit(long long n) {
        int i = -1;
        while(n > 0) {
            ++i;
            n /= 2;
        }
        return i;
    }

    static void toBytesString(std::ofstream& f, std::string s, int type = BINARY) {

        char* data = s.data();
        char len = (char)s.length();

        /*
        std::cout << "Input :" << std::endl;

        int i;
        for(i=0; i<len+1; ++i) {
            std::cout << data[i] << "\t" << std::bitset<8>((int)data[i]).to_string() << std::endl;
        }
        */

        switch(type) {
        case ASCII:
            if(!printable(data, len)) {
                throw std::exception("Invalid characters.");
            }
            break;
        case NAME:
            if(len == 0) {
                throw std::exception("Empty name.");
            }
            if(!printable(data, len, false)) {
                throw std::exception("Invalid characters.");
            }
            break;
        }

        char length[1] = {len};

        f.write(length, 1);
        f.write(data, len);

    }

    static bool printable(char* data, int len, bool space = true) {
        int i;
        for(i=0; i<len; ++i) {
            if( (data[i] < 33 || data[i] > 126) && (!space || data[i] != ' ') ) {
                return false;
            }
        }
        return true;
    }

    static void toBytesDelta(std::ofstream& f, const Delta& d) {

        switch(d.getType()) {
        case DELTA_1:
            if(d.getDeltaX() != 0 && d.getDeltaY() != 0) {
                throw std::exception("Invalid delta.");
            }
            if(d.getDeltaX() != 0) {
                toBytesSigned(f, d.getDeltaX());
            } else {
                toBytesSigned(f, d.getDeltaY());
            }
            break;

        case DELTA_2:
        {
            if(d.getDeltaX() != 0 && d.getDeltaY() != 0) {
                throw std::exception("Invalid delta.");
            }
            if(d.getDeltaX() != 0) {
                unsigned int delta = abs(d.getDeltaX());
                if(d.getDeltaX() < 0) {
                    delta = (delta << 2) + 2;
                } else {
                    delta = (delta << 2);
                }
                toBytesUnsigned(f, delta);
            } else {
                unsigned int delta = abs(d.getDeltaY());
                if(d.getDeltaY() < 0) {
                    delta = (delta << 2) + 3;
                } else {
                    delta = (delta << 2) + 1;
                }
                toBytesUnsigned(f, delta);
            }
            break;
        }

        case DELTA_3:
        {
            unsigned int delta;
            if(d.getDeltaX() != 0) {
                delta = abs(d.getDeltaX());
            } else {
                delta = abs(d.getDeltaY());
            }
            delta = delta << 3;
            if( d.getDeltaX() == 0 && d.getDeltaY() >= 0 ) {
                delta += 1;
            } else if( d.getDeltaX() < 0 && d.getDeltaY() == 0 ) {
                delta += 2;
            } else if( d.getDeltaX() == 0 && d.getDeltaY() < 0 ) {
                delta += 3;
            } else if( d.getDeltaX() >= 0 && d.getDeltaY() >= 0 ) {
                delta += 4;
            } else if( d.getDeltaX() < 0 && d.getDeltaY() >= 0 ) {
                delta += 5;
            } else if( d.getDeltaX() < 0 && d.getDeltaY() < 0 ) {
                delta += 6;
            } else if( d.getDeltaX() >= 0 && d.getDeltaY() < 0 ) {
                delta += 7;
            }
            toBytesUnsigned(f, delta);
            break;
        }

        case DELTA_G1:
        {
            unsigned int delta;
            if(d.getDeltaX() != 0) {
                delta = abs(d.getDeltaX());
            } else {
                delta = abs(d.getDeltaY());
            }
            delta = delta << 3;
            if( d.getDeltaX() == 0 && d.getDeltaY() >= 0 ) {
                delta += 1;
            } else if( d.getDeltaX() < 0 && d.getDeltaY() == 0 ) {
                delta += 2;
            } else if( d.getDeltaX() == 0 && d.getDeltaY() < 0 ) {
                delta += 3;
            } else if( d.getDeltaX() >= 0 && d.getDeltaY() >= 0 ) {
                delta += 4;
            } else if( d.getDeltaX() < 0 && d.getDeltaY() >= 0 ) {
                delta += 5;
            } else if( d.getDeltaX() < 0 && d.getDeltaY() < 0 ) {
                delta += 6;
            } else if( d.getDeltaX() >= 0 && d.getDeltaY() < 0 ) {
                delta += 7;
            }
            delta = delta << 1;
            toBytesUnsigned(f, delta);
            break;
        }

        case DELTA_G2:
        {
            unsigned int deltaX = abs(d.getDeltaX());
            deltaX = (deltaX << 2) + 1;
            if(d.getDeltaX() < 0) {
                deltaX += 2;
            }
            toBytesUnsigned(f, deltaX);
            toBytesSigned(f, d.getDeltaY());
            break;
        }

        }

    }

    static void toBytesPointList(std::ofstream& f, const PointList& pl) {

        toBytesUnsigned(f, pl.getType());
        toBytesUnsigned(f, (unsigned int)(pl.getList().size()));

        int i;
        for(i=0; i<pl.getList().size(); ++i) {
            toBytesDelta(f, pl.getList()[i]);
        }

    }

};



class OasisReader{

public:

    static unsigned char fromBytesChar(std::ifstream& f) {
        byte bytes[1] = {0};
        f.read((char *)bytes, 1);
        return bytes[0];
    }

    static float fromBytesReal(std::ifstream& f) {

        byte bytes[1] = {0};
        f.read((char *)bytes, 1);

        int type = bytes[0];

        float out = 0;

        switch(type) {

        case POSITIVE_WHOLE:
        {
            out = (float)fromBytesUnsigned(f);
            break;
        }
        case NEGATIVE_WHOLE:
        {
            out = ((float)fromBytesUnsigned(f)) * -1;
            break;
        }
        case POSITIVE_RECIPROCAL:
        {
            out = 1.0f / fromBytesUnsigned(f);
            break;
        }
        case NEGATIVE_RECIPROCAL:
        {
            out = -1.0f / fromBytesUnsigned(f);
            break;
        }
        case POSITIVE_RATIO:
        {
            float top = (float)fromBytesUnsigned(f);
            float bot = (float)fromBytesUnsigned(f);
            out =  top / bot;
            break;
        }
        case NEGATIVE_RATIO:
        {
            float top = (float)fromBytesUnsigned(f);
            float bot = (float)fromBytesUnsigned(f);
            out =  -top / bot;
            break;
        }
        case SINGLE_PRECISION_FLOAT:
        {
            out =  fromBytesFloat(f, true);
            break;
        }
        case DOUBLE_PRECISION_FLOAT:
        {
            out =  fromBytesFloat(f, false);
            break;
        }
        default:
            throw std::exception("Invalid type for real value.");

        }

        return out;
    }

    static unsigned int fromBytesUnsigned(std::ifstream& f) {

        byte bytes[4] = {0};
        f.read((char *)bytes, 4);

        byte mask = 127;
        unsigned int out = 0;

        int i = 0;
        while( (128 & bytes[i]) > 0) {
            ++i;
        }

        for(; i>=0; i--) {
            byte next = bytes[i] & mask;
            out = (out << 7) + next;
        }

        return out;
    }

    static signed int fromBytesSigned(std::ifstream& f) {

        byte bytes[4] = {0};
        f.read((char *)bytes, 4);

        /*
        std::cout << "Output :" << std::endl;
        int j;
        for(j=0; j<4; ++j) {
            std::cout << std::bitset<8>((int)bytes[j]).to_string() << " ";
        }
        std::cout << std::endl;
        */

        byte mask = 127;
        unsigned int out = 0;
        bool negative = (bytes[0] & 1);

        int i = 0;
        while( (128 & bytes[i]) > 0) {
            ++i;
        }

        for(; i>=1; i--) {
            byte next = bytes[i] & mask;
            out = (out << 7) + next;
        }
        byte last = (bytes[0] >> 1) & 63;
        out = (out << 6) + last;
        if(negative) {
            out = out * -1;
        }

        return out;
    }

    static unsigned long long extractBit(int pos, int len, byte* out, int size) {

        int index = size - (pos / 8) - 1;
        int bit = pos % 8 ;

        int count = std::min(8-bit, len);

        //std::cout << "Index : " << index << std::endl;
        //std::cout << "Bit   : " << bit << std::endl;
        //std::cout << "Count : " << count << std::endl;

        unsigned int extract_mask = (255 >> (8 - count)) << bit;

        //std::cout << "Extract Mask : " << std::bitset<8>(extract_mask).to_string() << std::endl;

        unsigned int extract = (out[index] & extract_mask) >> bit;

        //std::cout << "Extract      : " << std::bitset<8>(extract).to_string() << std::endl;

        pos += count;
        len -= count;

        if(len > 0) {
            return (extractBit(pos, len, out, size) << count) + extract;
        } else {
            return extract;
        }

    }

    static float fromBytesFloat(std::ifstream& f, bool singlePrec) {

        byte bytes[8] = {0};
        if(singlePrec) {
            f.read((char *)bytes, 4);

            /*
            int i;
            for(i=0; i<4; ++i) {
                std::cout << std::bitset<8>((int)bytes[i]).to_string() << " ";
            }
            std::cout << std::endl;
            */

        } else {
            f.read((char *)bytes, 8);

            /*
            int i;
            for(i=0; i<8; ++i) {
                std::cout << std::bitset<8>((int)bytes[i]).to_string() << " ";
            }
            std::cout << std::endl;
            */

        }



        int size, expoBias, expoPos, expoLen;
        if(singlePrec) {
            size = 4;
            expoBias = 127;
            expoPos = 23;
            expoLen = 8;
        } else {
            size = 8;
            expoBias = 1023;
            expoPos = 52;
            expoLen = 11;
        }

        int expo = (int)extractBit(expoPos, expoLen, bytes, size) - expoBias;

        //std::cout << "Exponent : " << expo << std::endl;

        long long whole, decimal;
        int places = expoPos-expo;
        if(expo >= 0) {
            whole = extractBit(expoPos-expo, expo, bytes, size) + (((long long)1) << expo);
            decimal = extractBit(0, expoPos-expo, bytes, size);
        } else {
            whole = 0;
            decimal = extractBit(0, expoPos, bytes, size) + (((long long)1) << expoPos);
        }



        double decimal_value = 0;
        int i;
        for(i=0; i<places; ++i) {

            //std::cout << "Places left : " << (places - i) << std::endl;
            //std::cout << std::bitset<23>(decimal).to_string() << std::endl;

            if((decimal & 1) == 1) {
                ++decimal_value;
            }
            if(decimal_value > 0) {
                decimal_value /= 2;
            }

            //std::cout << decimal_value << std::endl;

            decimal = decimal >> 1;

        }

        float out = (float)(whole + decimal_value);

        if(bytes[0] >= 128) {
            out = out * -1;
        }

        return out;

    }

    static bool printable(char* data, int len, bool space = true) {
        int i;
        for(i=0; i<len; ++i) {
            if( (data[i] < 33 || data[i] > 126) && (!space || data[i] != ' ') ) {
                return false;
            }
        }
        return true;
    }

    static std::string fromBytesString(std::ifstream& f, int type = BINARY) {

        char len[1];
        f.read(len, 1);
        unsigned char length = len[0];

        char* data = new char[length+1];
        f.read(data, length);
        data[length] = '\0';

        /*
        std::cout << "Output :" << std::endl;

        int i;
        for(i=0; i<len[0]+1; ++i) {
            std::cout << data[i] << "\t" << std::bitset<8>((int)data[i]).to_string() << std::endl;
        }
        */

        switch(type) {
        case ASCII:
            if(!printable(data, len[0])) {
                throw std::exception("Invalid characters.");
            }
            break;
        case NAME:
            if(len[0] == 0) {
                throw std::exception("Empty name.");
            }
            if(!printable(data, len[0], false)) {
                throw std::exception("Invalid characters.");
            }
            break;
        }

        std::string out(data);
        delete[] data;
        return out;

    }

    static Delta fromBytesDelta(std::ifstream& f, int type) {

        switch(type) {
        case DELTA_1:
        {
            int data = fromBytesSigned(f);
            Delta out(type, data);
            return out;
        }

        case DELTA_2:
        {

            unsigned int data = fromBytesUnsigned(f);
            char dir = data & 3;
            data = data >> 2;

            //std::cout << std::bitset<8>(data).to_string() << std::endl;

            switch(dir) {
            case 0:
            {
                return Delta(type, data);
            }

            case 1:
            {
                Delta out(type, 0, data);
                return out;
            }

            case 2:
            {
                Delta out(type, -(int)data);
                return out;
            }

            case 3:
            {
                Delta out(type, 0, -(int)data);
                return out;
            }

            default:
                throw std::exception("Should be impossible to get this.");

            }
        }

        case DELTA_3:
        {

            unsigned int data = fromBytesUnsigned(f);
            char dir = data & 7;
            data = data >> 3;

            switch(dir) {
            case 0:
            {
                Delta out(type, data);
                return out;
            }

            case 1:
            {
                Delta out(type, 0, data);
                return out;
            }

            case 2:
            {
                Delta out(type, -(int)data);
                return out;
            }

            case 3:
            {
                Delta out(type, 0, -(int)data);
                return out;
            }

            case 4:
            {
                Delta out(type, data, data);
                return out;
            }

            case 5:
            {
                Delta out(type, -(int)data, data);
                return out;
            }

            case 6:
            {
                Delta out(type, -(int)data, -(int)data);
                return out;
            }

            case 7:
            {
                Delta out(type, data, -(int)data);
                return out;
            }

            default:
                throw std::exception("Should be impossible to get this.");
            }
        }

        case DELTA_G:
        case DELTA_G1:
        case DELTA_G2:
        {

            unsigned int data1 = fromBytesUnsigned(f);
            char isG1 = data1 & 1;
            data1 = data1 >> 1;

            if(isG1 == 0) {

                char dir = data1 & 7;
                data1 = data1 >> 3;

                switch(dir) {
                case 0:
                {
                    Delta out(type, data1);
                    return out;
                }

                case 1:
                {
                    Delta out(type, 0, data1);
                    return out;
                }

                case 2:
                {
                    Delta out(type, -(int)data1);
                    return out;
                }

                case 3:
                {
                    Delta out(type, 0, -(int)data1);
                    return out;
                }

                case 4:
                {
                    Delta out(type, data1, data1);
                    return out;
                }

                case 5:
                {
                    Delta out(type, -(int)data1, data1);
                    return out;
                }

                case 6:
                {
                    Delta out(type, -(int)data1, -(int)data1);
                    return out;
                }

                case 7:
                {
                    Delta out(type, data1, -(int)data1);
                    return out;
                }

                default:
                    throw std::exception("Should be impossible to get this.");

                }

            } else {

                int dy = fromBytesSigned(f);
                int dx;
                char dir1 = data1 & 1;
                data1 = data1 >> 1;
                if(dir1 == 0) {
                    dx = data1;
                } else {
                    dx = -(int)data1;
                }

                Delta out(type, dx, dy);
                return out;

            }
        }

        default:
            throw std::exception("Invalid type.");
        }

    }

    static void fromBytesPointList(std::ifstream& f, PointList& pl) {

        unsigned int type = fromBytesUnsigned(f);
        unsigned int length = fromBytesUnsigned(f);

        std::cout << "Type " << type << " Length " << length << std::endl;

        int delta;
        switch(type) {
        case 0:
        case 1:
            delta = DELTA_1;
            break;
        case 2:
            delta = DELTA_2;
            break;
        case 3:
            delta = DELTA_3;
            break;
        case 4:
        case 5:
            delta = DELTA_G;
            break;
        default:
            throw std::exception("Invalid type.");
        }

        pl.setType(type);

        for(unsigned i=0; i<length; ++i) {
            Delta next = fromBytesDelta(f, delta);
            std::cout << "(" << next.getDeltaX() << ", " << next.getDeltaY() << ") ";
            pl.addDelta(next);
        }
        std::cout << std::endl;

    }

};







}




#endif // OASISIO_H

















