#include "pch.h"
#include <string>

struct berResults
{
    double tot; //total number of bits
    double err; //error bits (different bits)
    float ber;  //ber calc result
    clock_t t1; //calc start time
    clock_t t2; //calc stop time
};

uint8_t ham_distance(uint8_t n1, uint8_t n2);
void createFile1(const std::string name, const int count, const char value);
berResults ber_calc(std::string fpath1, std::string fpath2);
void printResult(berResults results);

int main(int argc, char* argv[])
{
    std::string fpath1; //path to file 1
    std::string fpath2; //path to file 2
    berResults results;

    openLog("log.log"); //create log.log file for logging purposes
    if (argc != 3) //check the number of arguments
    {
        saveLog("No path to files");
        saveLog("Creating test files. please wait");
        //test 1
        createFile1("test1_file1.bin", 100, 0xFF); //1111 1111
        createFile1("test1_file2.bin", 100, 0xFE); //1111 1110
        //test 2

        createFile1("test2_file1.bin", 100, 0x55);
        createFile1("test2_file2.bin", 100, 0x45);

        //test 3
        createFile1("test3_file1.bin",400000000,0x55); 
        createFile1("test3_file2.bin",400000000,0x50);
        saveLog("creating test files done");
        saveLog("Re-run with correct arguments : ./ber.exe test1_file1.bin test1_file2.bin");
    }
    else 
    {
        fpath1 = argv[1];
        fpath2 = argv[2];

        saveLog("Processing test files");
        results = ber_calc(fpath1, fpath2);
        printResult(results);
    }

    closeLog();
    return 0;
}

uint8_t ham_distance(uint8_t n1, uint8_t n2)
{
    uint8_t x = n1 ^ n2; 
    uint8_t setBits = 0;
    while (x > 0)
    {
        setBits += x & 1;
        x >>= 1;
    }
    return setBits;
}

void createFile1(const std::string name, const int count, const char value)
{
    std::fstream f;
    f.open(name.c_str(), std::ios::binary | std::ios::out);
    for (int i = 0; i < count; i++)
    {
        f.write((char*)&value, 1);
    }
    f.close();
}

berResults ber_calc(std::string fpath1, std::string fpath2)
{
    std::fstream f1, f2; //handlers to files
    berResults results;
    results.t1 = 0;
    results.t2 = 0;
    results.ber = 0;
    results.err = 0;
    results.tot = 0;

    saveLog("Calculating BER...");
    f1.open(fpath1.c_str(), std::ios::binary | std::ios::in);
    f2.open(fpath2.c_str(), std::ios::binary | std::ios::in);
    char a = 0x00;
    char b = 0x00;
    results.t1 = clock();

    while (!f1.eof())
    {
        f1 >> a; 
        f2 >> b; 
        if (!f1.eof()) 
        {
            results.err += ham_distance(a, b); 
            results.tot += 8; 
        }
    }

    results.ber = (float)results.err / results.tot; // foe calculating ber
    results.t2 = clock();
    saveLog("BER calculations finished");
    return results; 
}

void printResult(berResults results)
{
    std::stringstream message;
    message << "Results are: " << std::endl;
    message << "BER: " << results.ber << std::endl;
    message << "TOT: " << results.tot << std::endl;
    message << "ERR: " << results.err << std::endl;
    message << "TIME: " << ((float)results.t2 - results.t1) / CLOCKS_PER_SEC << " sec " << std::endl;
    saveLog(message.str());
}