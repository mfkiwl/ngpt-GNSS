#include <iostream>
#include <map>
#include <algorithm>
#include <cstring>
#include <string>
#include <stdexcept>

#include "antex.hpp"

using ngpt::antex;
using ngpt::antenna;

typedef ngpt::pcv_type                     pcv_type;
typedef ngpt::antenna_pcv<ngpt::pcv_type>  pcv_pattern;
typedef std::map<std::string, std::string> str_str_map;
typedef std::ifstream::pos_type            pos_type;
typedef std::pair<ngpt::antenna, pos_type> ant_pos_pair;

/*
pcv_type MIN_ZEN = 0;
pcv_type MAX_ZEN = 90;
pcv_type MIN_AZI = 0;
pcv_type MAX_AZI = 360;
*/

// Print a bunch of details for an antenna pcv pattern, nothing fancy
void
print_pcv_info(const pcv_pattern&, const antenna&,
               pcv_type zen_step, pcv_type azi_step,
               pcv_type zen1, pcv_type zen2,
               pcv_type azi1, pcv_type azi2);
void
print_pcv_info_noazi(const pcv_pattern&, const antenna&,
               pcv_type zen_step, pcv_type zen1, pcv_type zen2);

// Parse command line arguments
int
cmd_parse(int, char* c[], str_str_map&);

// Parse the antenna argument
std::size_t
antenna_parser(str_str_map&, std::vector<antenna>&);

// Parse limits
int
limit_parser(const std::string&, pcv_type&, pcv_type&, pcv_type&);

// help and usage
void help();
void usage();
void epilog();

int main(int argv, char* argc[])
{
    // a dictionary with any default options
    str_str_map arg_dict;
    arg_dict["dzen" ] = std::string( "1.0" );
    arg_dict["dazi" ] = std::string( "1.0" );
    arg_dict["types"] = std::string( "G01" );
    arg_dict["list" ] = std::string( "N"   );
    // arg_dict["zen"  ] = std::string( "0.0/90.0/1.0" );
    // arg_dict["azi"  ] = std::string( "0.0/360.0/1.0" );
    pcv_type zen_start, zen_stop, zen_step;
    pcv_type azi_start, azi_stop, azi_step;
    // default
    zen_start = zen_stop = azi_step = azi_stop = -99999;

    // get cmd arguments into the dictionary
    int status = cmd_parse(argv, argc, arg_dict);
    if ( status > 0 )      // error
    {
        std::cerr << "\n\nWrong cmds. Stop.\n";
        return 1;
    }
    else if ( status < 0 ) // -h or -l
    {
        std::cout << "\n";
        return 0;
    }

    // this may throw in non-debug mode
    auto it = arg_dict.find("antex");
    if ( it == arg_dict.end() )
    {
        std::cerr << "\nMust provide name of antex file.\n";
        return 1;
    }
    antex atx ( it->second.c_str() );

    // maybe the user only wants the antenna list
    if (arg_dict["list"] == "Y")
    {
       std::vector<ant_pos_pair> ants ( atx.get_antenna_list() );
       std::cout << "\nAntennas included in ANTEX file: " << atx.filename();
       for (auto const& i: ants)
       {
           std::cout << "\n" << i.first.to_string();       
       }
       std::cout << "\n";
       return 0;
    }
    
    // declare/construct the antenna vector
    std::vector<antenna> ant_vec;
    if ( antenna_parser(arg_dict, ant_vec) < 1 )
    {
        std::cerr << "\nFailed to read antenna model(s).\n";
        return 1;
    }

    // get the zenith & azimouth axis info
    try
    {
        zen_step = std::stod( arg_dict["dzen"] );
        azi_step = std::stod( arg_dict["dazi"] );
        if (zen_step <= .0 || azi_step <= .0 )
        {
            throw std::invalid_argument("\nFuck off\n");
        }
    }
    catch (std::invalid_argument& e)
    {
        std::cerr << "\nInvalid zen and/or azi step!\n";
        return 1;
    }

    // maybe we have azi axis info ...
    if ( (it = arg_dict.find("azi")) != arg_dict.end() )
    {
        if ( limit_parser(it->second, azi_start, azi_stop, azi_step) )
        {
            std::cerr <<"\nInvalid azimouth range: " << it->second << "\n";
            return 1;
        }
    }

    // maybe we have zen axis info ...
    if ( (it = arg_dict.find("zen")) != arg_dict.end() )
    {
        if ( limit_parser(it->second, zen_start, zen_stop, zen_step) )
        {
            std::cerr <<"\nInvalid zenith range: " << it->second << "\n";
            return 1;
        }
    }

    // compute and write pcv's
    for (const auto& i : ant_vec)
    {
        pcv_pattern pcv ( atx.get_antenna_pattern(i) );
        print_pcv_info(pcv, i, zen_step, azi_step, zen_start, zen_stop, azi_start, azi_stop);
    }

    std::cout << "\n";
    return 0;
}

void
help()
{
    std::cout << "\n"
    "Program atxtr\n"
    "This program will read, interpolate and report GNSS antenna Phase Center\n"
    "Variation (PCV) corrections from an ANTEX file. All output is directed\n"
    "to \'stdout\'\n"
    "References: https://igscb.jpl.nasa.gov/projects/antenna/";
    return;
}

void
usage()
{
    std::cout << "\n"
    "Usage:\n"
    " atxtr -a ANTEX -m \"ANTENN1,ANTENNA2,...\" [ [-dzen 0.5] [dazi 2.0] ]\n"
    "\n"
    " -h or --help\n"
    "\tDisplay (this) help message and exit.\n"
    " -a [ANTEX]\n"
    "\tSpecify the input ANTEX file.\n"
    " -m [\"ANTENNA_1,ANTENNA_2,...\"]\n"
    "\tSpecify the antenna model. Use quotation marks \n"
    "\t(\"\") to correctly parse whitespaces.\n"
    "\tThe antenna model must follow the IGS conventions:\n"
    "\thttps://igscb.jpl.nasa.gov/igscb/station/general/rcvr_ant.tab\n"
    "\tand the ANTEX file format specifications. You can\n"
    "\tspecify a serial number, by typing it directly \n"
    "\tafter the RADOME (as in ANTEX); if the serial is\n"
    "\tnot matched, but the antenna (i.e. MODEL+RADOME)\n"
    "\tis, then this generic antnna will be selected.\n"
    " -l or --list\n"
    "\tList all available antennas recorded in the given ANTEX\n"
    "\tfile and exit.\n"
    " -dzen [ZENITH_STEP]\n"
    "\tSpecify the zenith step in degrees. The interpolation\n"
    "\tWill be performed on the interval [ZEN1, ZEN2] with a\n"
    "\tstep size of ZENITH_STEP degrees. The ZEN1, ZEN2 are\n"
    "\tread off from the ANTEX file.\n"
    "\tDefault value is 1.\n"
    " -dazi [AZIMOUTH_STEP]\n"
    "\tIf the antenna PCV pattern includes azimouth-dependent\n"
    "\tcorrections, then this will set the step size for\n"
    "\tthe azimouth interval. The interpolation will be\n"
    "\tperformed on the interval [0, 360] with a step size of\n"
    "\tAZIMOUTH_STEP degrees.\n"
    "\tDefault value is 1.\n"
    " -azi [from/to/step]\n"
    "\tSpecify the range for the azimouth axis. Azimouth\n"
    "\tgrid will span the interval [from,to) with a step\n"
    "\tsize of step degrees. Default is [0,360,1]. This\n"
    "\twill automatically fall back to [0,0,0] if no\n"
    "\tazimouth-dependent pcv values are available. Note\n"
    "\tthat this option will overwrite the \'-dazi\' option.\n"
    " -zen [from/to/step]\n"
    "\tSpecify the range for the zenith ditance axis. The\n"
    "\tgrid will span the interval [from,to) with a step\n"
    "\tsize of step degrees. Default is [0,90,1]. Note\n"
    "\tthat this option will overwrite the \'-dzen\' option.\n";

    /*std::cout << "\n";
    "Example usage:\n"
    "atxtr -a igs08.atx -m \"TRM41249.00     TZGD,LEIATX1230+GNSS NONE\"";*/
    return;
}

void
epilog()
{
    std::cout << "\n"
    "Copyright 2015 National Technical University of Athens.\n\n"
    "This work is free. You can redistribute it and/or modify it under the\n"
    "terms of the Do What The Fuck You Want To Public License, Version 2,\n"
    "as published by Sam Hocevar. See http://www.wtfpl.net/ for more details.\n"
    "\nSend bugs to: xanthos[AT]mail.ntua.gr, demanast[AT]mail.ntua.gr";
    return;
}

int
cmd_parse(int argv, char* argc[], str_str_map& smap)
{
    if ( argv == 1 )
    {
            help();
            std::cout << "\n";
            usage();
            std::cout << "\n";
            epilog();
            return 1;
    }

    for (int i = 1; i < argv; i++)
    {
        if (   !std::strcmp(argc[i], "-h") 
            || !std::strcmp(argc[i], "--help") )
        {
            help();
            std::cout << "\n";
            usage();
            std::cout << "\n";
            epilog();
            return -1;
        }
        else if ( !std::strcmp(argc[i], "-l")
               || !std::strcmp(argc[i], "--list") )
        {
            smap["list"] = std::string("Y");
        }
        else if ( !std::strcmp(argc[i], "-a") )
        {
            if ( i+1 >= argv ) { return 1; }
            smap["antex"] = std::string( argc[i+1] );
            ++i;
        } 
        else if ( !std::strcmp(argc[i], "-m") )
        {
            if ( i+1 >= argv ) { return 1; }
            smap["antennas"] = std::string( argc[i+1] );
            ++i;
        }
        else if ( !std::strcmp(argc[i], "-o") )
        {
            if ( i+1 >= argv ) { return 1; }
            smap["types"] = std::string( argc[i+1] );
            ++i;
        }
        else if ( !std::strcmp(argc[i], "-dzen") )
        {
            if ( i+1 >= argv ) { return 1; }
            smap["dzen"] = std::string( argc[i+1] );
            ++i;
        }
        else if ( !std::strcmp(argc[i], "-dazi") )
        {
            if ( i+1 >= argv ) { return 1; }
            smap["dazi"] = std::string( argc[i+1] );
            ++i;
        }
        else if ( !std::strcmp(argc[i], "-zen") )
        {
            if ( i+1 >= argv ) { return 1; }
            smap["zen"] = std::string( argc[i+1] );
            ++i;
        }
        else if ( !std::strcmp(argc[i], "-azi") )
        {
            if ( i+1 >= argv ) { return 1; }
            smap["azi"] = std::string( argc[i+1] );
            ++i;
        }
        else
        {
            std::cerr << "\nIrrelevant cmd: " << argc[i];
        }
    }
    return 0;
}

int
limit_parser(const std::string& arg_str, pcv_type& start,
            pcv_type& stop, pcv_type& step)
{
    // tokenize the string using '/' as delimeter
    pcv_type  lary[3] = {-9999, -9999, -9999};
    int j = 0;
    
    std::size_t i = 0;
    std::string::size_type pos = arg_str.find_first_of('/', i);
    try
    {
        while (pos != std::string::npos)
        {
            lary[j] = std::stod( arg_str.substr(i, pos-i) );
            ++j;
            i = pos + 1;
            pos = arg_str.find_first_of('/', i);
        }
        lary[j] = std::stod( arg_str.substr(i, pos-i) );
    }
    catch (std::invalid_argument& e)
    {
        return 1;
    }

    // see that all values and no more are assigned.
    if ( j!=2 ) { return 1; }

    start = lary[0];
    stop  = lary[1];
    step  = lary[2];

    return 0;   
}

std::size_t
antenna_parser(str_str_map& arg_map, std::vector<antenna>& ant_vec)
{
    auto it = arg_map.find("antennas");
    if ( it == arg_map.end() )
    {
        std::cerr << "\nMust provide at least one antenna.\n";
        return -1;
    }
    
    // fuck the strings - dead simple tokenizer bitch
    std::string ant_lst (it->second);
    char* str = new char[ant_lst.size() + 1];
    std::transform( ant_lst.begin(), ant_lst.end(), str, 
                    [](const char c)->char{ return c==','?'\0':c;} );
    *(str+ant_lst.size()) = '\0';

    std::size_t idx = 0;
    std::size_t lng;
    char* tmp = str;
    while ( idx < ant_lst.size() )
    {
        lng = std::strlen( tmp+idx );
        ant_vec.emplace_back( tmp+idx );
        idx += lng + 1;
    }
    
    delete[] str;
    return ant_vec.size();
}

void
print_pcv_info(const pcv_pattern& pcv, const antenna& ant,
               pcv_type zen_step, pcv_type azi_step,
               pcv_type zen1, pcv_type zen2,
               pcv_type azi1, pcv_type azi2)
{
    if ( !pcv.has_azi_pcv() )
    {
        std::cerr << "\n## Antenna: "<< ant.to_string() << " has no"
        " azimouth-dependent PCV corrections; Switching to NOAZI grid.\n";
        print_pcv_info_noazi(pcv, ant, zen_step, zen1, zen2);
        return;
    }

    if ( zen1 < -9000 )
    {
        zen1 = pcv.zen1();
        zen2 = pcv.zen2();   
    }

    if ( azi1 < -9000 )
    {
        azi1 = pcv.azi1();
        azi2 = pcv.azi2();   
    }

    std::cout <<"\nANT: " << ant.to_string();
    std::cout <<"\nZEN: " << zen1 << " " << zen2 << " " << zen_step;
    std::cout <<"\nAZI: " << azi1 << " " << azi2 << " " << azi_step;

    for ( pcv_type zen = zen1; zen < zen2; zen += zen_step )
    {
        for ( pcv_type azi = azi1; azi < azi2; azi += azi_step )
        {
            std::cout << "\n" << pcv.azi_pcv(zen, azi, 0);
        }
    }

    return;
}

void
print_pcv_info_noazi(const pcv_pattern& pcv, const antenna& ant, 
    pcv_type zen_step, pcv_type zen1, pcv_type zen2)
{
    if ( zen1 < -9000 )
    {
        zen1 = pcv.zen1();
        zen2 = pcv.zen2();   
    }

    std::cout <<"\nANT: " << ant.to_string();
    std::cout <<"\nZEN: " << zen1 << " " << zen2 << " " << zen_step;
    std::cout <<"\nAZI: 0 0 0";

    for ( pcv_type zen = zen1; zen < zen2; zen += zen_step )
    {
        std::cout << "\n" << pcv.no_azi_pcv(zen, 0);
    }

    return;
}
