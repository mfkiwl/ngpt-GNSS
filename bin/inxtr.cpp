#include <iostream>
#include <map>
#include <cstring>
#include <algorithm>
#include "datetime_v2.hpp"
#include "ionex.hpp"

void help();
void usage();
void epilog();

template<typename T>
struct range {
    T from, to, step;
    range(T f=T(), T t=T(), T s=T())
        : from(f), to(t), step(s)
    {};
    bool empty() const noexcept
    {
        return from == to && (to == step && step == T());
    }
};

typedef std::map<std::string, std::string> str_str_map;
typedef ngpt::datev2<ngpt::milliseconds>   epoch;

/// Parse command line arguments.
int
cmd_parse(int, char* [], str_str_map&);

int main(int argv, char* argc[])
{
    // a dictionary with any default options
    str_str_map arg_dict;
    arg_dict["list" ] = std::string( "N" );
    arg_dict["diff" ] = std::string( "N" );
    
    // axis/epoch limits
    range<float> lat_range;
    range<float> lon_range;
    range<epoch> epoch_range;
    long time_step; // in seconds

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
    
    // this may throw in non-debug mode
    auto it = arg_dict.find("ionex");
    if ( it == arg_dict.end() )
    {
        std::cerr << "\nMust provide name of ionex file.\n";
        return 1;
    }
    ngpt::ionex inx ( it->second.c_str() );

    // set the start date
    if ( (auto sit = arg_dict.find("start")) == arg_dict.end() ) {
        // not provided; set from ionex file
        epoch_range.from = inx.first_epoch();
    } else {
        if ( resolve_str_date(sit->second, epoch_range.from) ) {
            std::cerr << "\nERROR. Failed to resolve start epoch from string:";
            std::cerr << " \"" << sit->second << "\"\n";
            return 1;
        }
        // it might be that the user only provided time (not date)
        
    }
    
    // set the stop date
    if ( (auto sit = arg_dict.find("stop")) == arg_dict.end() ) {
        // not provided; set from ionex file
        epoch_range.to = inx.last_epoch();
    } else {
        if ( resolve_str_date(sit->second, epoch_range.to) ) {
            std::cerr << "\nERROR. Failed to resolve start epoch from string:";
            std::cerr << " \"" << sit->second << "\"\n";
            return 1;
        }
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
        else if ( !std::strcmp(argc[i], "-start") )
        {
            if ( i+1 >= argv ) { return 1; }
            smap["start"] = std::string( argc[i+1] );
            ++i;
        }
        else if ( !std::strcmp(argc[i], "-stop") )
        {
            if ( i+1 >= argv ) { return 1; }
            smap["stop"] = std::string( argc[i+1] );
            ++i;
        }
        else if ( !std::strcmp(argc[i], "-lat") )
        {
            if ( i+1 >= argv ) { return 1; }
            smap["lat"] = std::string( argc[i+1] );
            ++i;
        }
        else if ( !std::strcmp(argc[i], "-lon") )
        {
            if ( i+1 >= argv ) { return 1; }
            smap["lon"] = std::string( argc[i+1] );
            ++i;
        }
        else
        {
            std::cerr << "\nIrrelevant cmd: " << argc[i];
        }
    }
    return 0;
}

// Resolve Datetime or Time. This function can resolve a epoch string of type:
// YYYY/MM/DDTHH:MM:SS , or
// HH:MM:SS
// In the second case, the MJDay of the epoch will be set to 0.
int
resolve_str_date(std::string str, epoch& eph)
{
    int num_tokens = 0;
    const char* p = str.c_str();
    char* end;
    std::vector<long> vec_tokens;

    if (auto (it = std::find(std::begin(str), std::end(str), "/"))
    // only time (2nd case)
                == std::end(str) ) {
        for (long i = std::strtol(p, &end, 10);
             p != end;
             ++p, i = std::strtol(p, &end, 10))
        {
            p = end;
            if (errno == ERANGE || num_tokens++ > 2) { return 1; }
            vec_tokens.emplace_back( i );
        }
        eph = epoch( ngpt::year(0),
                     ngpt::month(0),
                     ngpt::day_of_month(0),
                     ngpt::hours((int)vec_tokens[0]),
                     ngpt::minutes((int)vec_tokens[1]),
                     ngpt::milliseconds(vec_tokens[2]*1000L)
                   );
        return 0;
    } else {
    // normal datetime (1st case)
        for (long i = std::strtol(p, &end, 10);
             p != end;
             ++p, i = std::strtol(p, &end, 10))
        {
            p = end;
            if (errno == ERANGE || num_tokens++ > 5) { return 1; }
            vec_tokens.emplace_back( i );
        }
        *eph = epoch( ngpt::year((int)vec_tokens[0]),
                      ngpt::month((int)vec_tokens[1]),
                      ngpt::day_of_month((int)vec_tokens[2]),
                      ngpt::hours((int)vec_tokens[3]),
                      ngpt::minutes((int)vec_tokens[4]),
                      ngpt::milliseconds(vec_tokens[5]*1000L)
                     );
        return 0;
    }
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
