#include <iostream>
#include <map>
#include "datetime_v2.hpp"
#include "ionex.hpp"

void help();
void usage();
void epilog();

typedef ngpt::datev2<ngpt::milliseconds> epoch;

template<typename T>
struct range {
    T from, to, step;
    range(T f=T(), T t=T(), T step=T())
        : from(f), to(t), step(s)
    {};
    bool empty() const noexcept
    {
        return from == to && (to == step && step == T());
    }
};

typedef std::map<std::string, std::string> str_str_map;

int main(int argv, char* argc[])
{
    // a dictionary with any default options
    str_str_map arg_dict;
    arg_dict["dzen" ] = std::string( "1.0" );
    arg_dict["dazi" ] = std::string( "1.0" );
    arg_dict["types"] = std::string( "G01" );
    arg_dict["list" ] = std::string( "N"   );
    arg_dict["diff" ] = std::string( "N"   );
    
    // axis/epoch limits
    range<float> lat_range;
    range<float> lon_range;
    range<epoch> epoch_range;

    // get cmd arguments into the dictionary
    int status = cmd_parse(argv, argc, arg_dict);
    if ( status > 0 )      // error
    {
        std::cerr << "\n\nWrong cmds. Stop.\n";
        return 1;
    }
    else if ( status < 0 ) // -h
    {
        std::cout << "\n";
        return 0;
    }
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
        else if ( !std::strcmp(argc[i], "-diff") )
        {
            smap["diff"] = std::string("Y");
        }
        else if ( !std::strcmp(argc[i], "-i") )
        {
            if ( i+1 >= argv ) { return 1; }
            smap["ionex"] = std::string( argc[i+1] );
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

void
help()
{
    std::cout << "\n"
    "Program inxtr\n"
    "This program will read IONEX files and interpolate and report TEC maps\n"
    "and/or values for selected regions and time intervals. All output is directed\n"
    "to \'stdout\'\n"
    "References: IONEX: The IONosphere Map EXchange Format Version 1,\n"
    "S. Schaer, W. Gurtner, J. Feltens,\n"
    "https://igscb.jpl.nasa.gov/igscb/data/format/ionex1.pdf";
    return;
}

void
usage()
{
    std::cout << "\n"
    "Usage:\n"
    " inxtr -i IONEX [-start HH:MM:SS [-stop HH:MM:SS [-lat <from/to/step> -lon <from/to/step>] ] ]\n"
    "\n"
    " -h or --help\n"
    "\tDisplay (this) help message and exit.\n"
    " -i [IONEX]\n"
    "\tSpecify the input IONEX file.\n"
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
    "\tthat this option will overwrite the \'-dzen\' option.\n"
    " -diff\n"
    "\tInstead of printing each antenna's pcv corrections,\n"
    "\tprint the diffrences between pcv values. The first\n"
    "\tantenna in the list is considered as \'reference\' and\n"
    "\tfor each antenna in the specified list the respective\n"
    "\tdiscrepancies are computed.";

    std ::cout << "Example usage:\n"
    "atxtr -a igs08.atx -m \"TRM41249.00     TZGD,LEIATX1230+GNSS NONE\"";
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
    "\nSend bugs to: \nxanthos[AT]mail.ntua.gr, \ndemanast[AT]mail.ntua.gr \nvanzach[AT]survey.ntua.gr";
    return;
}
