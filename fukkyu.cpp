#include<iostream>
#include<fstream>

    int main(){

        std::string fuckyou = "fuck you! ";
        std::string fuckyoufile = fuckyou + ".txt";
        for (int i = 0; i < 1; --i){
            std::ofstream fuck_you(fuckyoufile);
            fuck_you << fuckyou;
            fuckyou = fuckyou + fuckyou;
            fuckyoufile = "." + fuckyou + ".txt";
        }
    return 0;
    }




