#include <iostream>
#include <map>
#include <cstring>
#include <algorithm>
#include <limits>
#include <sstream>
#include <iterator>
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
    float start() const { return from;}
    bool less(float f) const { return to >= from ? f<= to : f>= to; }
    void increment(float& f) const { f+=step; }
    bool empty() const noexcept { return from == to; }
    bool validate() const
    {
        if ( step == 0 ) {
            if ( empty() )
                return true;
            else
                return false;
        }
        long npts = static_cast<long>((to-from) / step);
        if ( (npts < 0) || (npts > std::numeric_limits<int>::max()) ) {
            return false;
        }
        long _if = 100*from;
        long _it = 100*to;
        long _is = 100*step;
        if ( (_it-_if)%_is ) {
            return false;
        }
        return true;
    }
};

typedef std::pair<float, float>            _point_;
typedef std::map<std::string, std::string> str_str_map;
typedef ngpt::datev2<ngpt::milliseconds>   epoch;

/// Parse command line arguments.
int
cmd_parse(int, char* [], str_str_map&);

int
resolve_geo_range(std::string, range<float>&);

int
resolve_str_date(std::string, epoch&);

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
    auto sit = arg_dict.find("ionex");
    if ( sit == arg_dict.end() )
    {
        std::cerr << "\nMust provide name of ionex file.\n";
        return 1;
    }
    ngpt::ionex inx ( sit->second.c_str() );

    // set the start date
    if ( (sit = arg_dict.find("start")) == arg_dict.end() ) {
        // not provided; set from ionex file
        epoch_range.from = inx.first_epoch();
    } else {
        if ( resolve_str_date(sit->second, epoch_range.from) ) {
            std::cerr << "\nERROR. Failed to resolve start epoch from string:";
            std::cerr << " \"" << sit->second << "\"\n";
            return 1;
        }
    }
    
    // set the stop date
    if ( (sit = arg_dict.find("stop")) == arg_dict.end() ) {
        // not provided; set from ionex file
        epoch_range.to = inx.last_epoch();
    } else {
        if ( resolve_str_date(sit->second, epoch_range.to) ) {
            std::cerr << "\nERROR. Failed to resolve ending epoch from string:";
            std::cerr << " \"" << sit->second << "\"\n";
            return 1;
        }
    }

    // set the interval
    if ( (sit = arg_dict.find("rate")) == arg_dict.end() ) {
        // not provided; set to 0
        time_step = 0L;
    } else {
        try {
            time_step = std::stol( sit->second );
        } catch (std::invalid_argument& e) {
            std::cerr << "\nERROR. Failed to resolve time interval from:";
            std::cerr << " \"" << sit->second <<"\'.";
            return 1;
        }
        if ( time_step < 0 ) {
            std::cerr << "\nERROR. Invalid time interval (<0).";
            return 1;
        }
    }
   
    // set the latitude range
    if ( (sit = arg_dict.find("lat")) == arg_dict.end() ) {
        auto latt = inx.latitude_grid();
        lat_range.from = std::get<0>(latt);
        lat_range.to   = std::get<1>(latt);
        lat_range.step = std::get<2>(latt);
    } else {
        if ( resolve_geo_range(sit->second, lat_range) ) {
            std::cerr << "\nERROR. Failed to resolve latitude range from:";
            std::cerr << " \"" << sit->second << "\"";
            return 1;
        }
    }
   
    // set the longtitude range
    if ( (sit = arg_dict.find("lon")) == arg_dict.end() ) {
        auto lont = inx.longtitude_grid();
        lon_range.from = std::get<0>(lont);
        lon_range.to   = std::get<1>(lont);
        lon_range.step = std::get<2>(lont);
    } else {
        if ( resolve_geo_range(sit->second, lon_range) ) {
            std::cerr << "\nERROR. Failed to resolve longtitude range from:";
            std::cerr << " \"" << sit->second << "\"";
            return 1;
        }
    }
   
    // set (if specified) the latitude interval
    if ( (sit = arg_dict.find("dlat")) != arg_dict.end() ) {
        try {
            lat_range.step = std::stof(sit->second);
        } catch (std::invalid_argument&) {
            std::cerr << "\nERROR. Failed to resolve latitude step from:";
            std::cerr << " \"" << sit->second << "\"";
            return 1;
        }
    }
   
    // set (if specified) the longtitude interval
    if ( (sit = arg_dict.find("dlon")) != arg_dict.end() ) {
        try {
            lon_range.step = std::stof(sit->second);
        } catch (std::invalid_argument&) {
            std::cerr << "\nERROR. Failed to resolve latitude step from:";
            std::cerr << " \"" << sit->second << "\"";
            return 1;
        }
    }

    // validate the ranges
    if ( !lat_range.validate() ) {
        std::cerr<<"\nWhat the fuck ashole! Invalid latitude range.\n";
        return 1;
    }
    if ( !lon_range.validate() ) {
        std::cerr<<"\nWhat the fuck ashole! Invalid longtitude range.\n";
        return 1;
    }

    // construct the vector of points for which we want the TEC values
    // BEWARE! If lat/lon step = 0, then we are forever looping
    auto prev_lat_step = lat_range.step;
    auto prev_lon_step = lon_range.step;
    if ( lat_range.step == 0 ) {
        if ( lat_range.empty() ) {
            lat_range.step = 1.0f;
        } else {
            std::cerr <<"\nInvalid (zero) latitude step size!.\n";
            return 1;
        }
    }
    if ( lon_range.step == 0 ) {
        if ( lon_range.empty() ) {
            lon_range.step = 1.0f;
        } else {
            std::cerr <<"\nInvalid (zero) longtitude step size!.\n";
            return 1;
        }
    }
    std::vector<_point_> points;
    for (float lat = lat_range.from; lat_range.less(lat); lat_range.increment(lat)) {
        for (float lon = lon_range.from; lon_range.less(lon); lon_range.increment(lon)) {
            points.emplace_back( lon, lat );
        }
    }
    lat_range.step = prev_lat_step;
    lon_range.step = prev_lon_step;
#ifdef DEBUG
    std::cout<<"\n[DEBUG] Interpolating for "<< points.size() <<" points";
#endif

    // let's do this!
    std::vector<epoch> epochs;
    int i_time_step (time_step);
    auto tec_results = inx.interpolate(points, epochs,
                        &epoch_range.from, &epoch_range.to, i_time_step);

    // diff mode !!
    if ( arg_dict["diff"] != "N" ) {
        // a vector of ionex filenames
        std::istringstream iss (arg_dict["diff"]);
        std::vector<std::string> ionexes { std::istream_iterator<std::string>{iss},
                                           std::istream_iterator<std::string>{}
                                         };
        if ( !ionexes.size() ) {
            std::cerr<<"\nERROR. No IONEXes to compute differences provided.\n";
            return 1;
        }

        std::vector<epoch> epochs2;/* (epochs.size()); DON'T FUCKING DO THIS!! */
        epochs2.reserve( epochs.size() );
        for (auto dinx : ionexes) {
            epochs2.clear(); /* FUCKING REMEMBER THIS */
            ngpt::ionex inx2 (dinx.c_str());
            auto tec_results2 = inx2.interpolate(points, epochs2,
                                                 &epoch_range.from,
                                                 &epoch_range.to,
                                                 i_time_step
                                                );
            
            if ( epochs2.size() != epochs.size() ) {
                std::cerr<<"\nERROR. Epoch vectors do not match! Wierd ...\n";
                return 1;
            }

            // print results
            std::cout<<"\nINX: " << inx.filename() << " - " << inx2.filename();
            std::cout<<"\nEPH: " << epoch_range.from.stringify()<<" "<<epoch_range.to.stringify()<<" "<<time_step;
            std::cout<<"\nLAT: " << lat_range.from<<" "<<lat_range.to<<" "<<lat_range.step;
            std::cout<<"\nLON: " << lon_range.from<<" "<<lon_range.to<<" "<<lon_range.step;

            std::size_t epoch_index = 0;
            for (const auto& eph : epochs) {
                std::cout << "\n" << eph.stringify() << "\n";
                auto tec1     = std::begin(tec_results);
                auto tec2     = std::begin(tec_results2);
                auto tec1_end = std::end(tec_results);
                for (; tec1 != tec1_end; ++tec1, ++tec2) {
                    std::cout <<  tec1->operator[](epoch_index) 
                                - tec2->operator[](epoch_index) << " ";
                }
                ++epoch_index;
            }
            std::cout<<"\nEOT";
        }
        return 0;
    }

    // print results (normal i.e. one ionex only)
    std::cout<<"\nINX: " << inx.filename();
    std::cout<<"\nEPH: " << epoch_range.from.stringify()<<" "<<epoch_range.to.stringify()<<" "<<time_step;
    std::cout<<"\nLAT: " << lat_range.from<<" "<<lat_range.to<<" "<<lat_range.step;
    std::cout<<"\nLON: " << lon_range.from<<" "<<lon_range.to<<" "<<lon_range.step;

    std::size_t epoch_index = 0;
    for (const auto& eph : epochs) {
        std::cout << "\n" << eph.stringify() << "\n";
        for (const auto& p : tec_results) {
            std::cout << p[epoch_index] << " ";
        }
        ++epoch_index;
    }
    std::cout<<"\nEOT";

    std::cout<<"\n";
    return 0;
}

// Resolve a lat/lon interval of type "from/to/step"
int
resolve_geo_range(std::string str, range<float>& rng)
{
    // tokenize the string using '/' as delimeter
    float  lary[3] = {-9999, -9999, -9999};
    int j = 0;
    
    std::size_t i = 0;
    std::string::size_type pos = str.find_first_of('/', i);
    try
    {
        while (pos != std::string::npos)
        {
            lary[j] = std::stod( str.substr(i, pos-i) );
            ++j;
            i = pos + 1;
            pos = str.find_first_of('/', i);
        }
        lary[j] = std::stod( str.substr(i, pos-i) );
    }
    catch (std::invalid_argument& e)
    {
        return 1;
    }

    // see that all values and no more are assigned.
    if ( j!=2 ) { return 1; }

    rng.from = lary[0];
    rng.to   = lary[1];
    rng.step = lary[2];

    return 0;   
}

int
cmd_parse(int argv, char* argc[], str_str_map& smap)
{
    if ( argv == 1 ) {
            help();
            std::cout << "\n";
            usage();
            std::cout << "\n";
            epilog();
            return 1;
    }

    for (int i = 1; i < argv; i++) {
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
            if ( i+1 >= argv ) { return 1; }
            smap["diff"] = std::string( argc[i+1] );
            ++i;
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
        else if ( !std::strcmp(argc[i], "-interval") )
        {
            if ( i+1 >= argv ) { return 1; }
            smap["rate"] = std::string( argc[i+1] );
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
        else if ( !std::strcmp(argc[i], "-dlat") )
        {
            if ( i+1 >= argv ) { return 1; }
            smap["dlat"] = std::string( argc[i+1] );
            ++i;
        }
        else if ( !std::strcmp(argc[i], "-dlon") )
        {
            if ( i+1 >= argv ) { return 1; }
            smap["dlon"] = std::string( argc[i+1] );
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

    auto it = std::find(std::begin(str), std::end(str), '/');
    if ( it == std::end(str) ) {
    // only time (2nd case)
        for (long i = std::strtol(p, &end, 10);
             p != end;
             ++p, i = std::strtol(p, &end, 10))
        {
            p = end;
            if (errno == ERANGE || num_tokens++ > 2) { return 1; }
            vec_tokens.emplace_back( i );
        }
        eph = epoch( ngpt::modified_julian_day(0),
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
        eph = epoch ( ngpt::year{(int)vec_tokens[0]},
                      ngpt::month{(int)vec_tokens[1]},
                      ngpt::day_of_month{(int)vec_tokens[2]},
                      ngpt::hours{(int)vec_tokens[3]},
                      ngpt::minutes{(int)vec_tokens[4]},
                      ngpt::milliseconds{vec_tokens[5]*1000L}
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
    " -start [YYYY/MM/DDTHH:MM:SS] or [HH:MM:SS]\n"
    "\tSpecify the first epoch to interpolate. In case only\n"
    "\ta time argument is provided (i.e. \"HH:MM:SS\") it is\n"
    "\tassumed that the day is the (first) day in the\n"
    "\tIONEX file. If not prvided, it is set to the first\n"
    "\tepoch in the file. Note that the time-scale used within\n"
    "\twithi the IONEX files is UT.\n"
    " -stop [YYYY/MM/DDTHH:MM:SS] or [HH:MM:SS]\n"
    "\tSpecify the last epoch to interpolate. In case only\n"
    "\ta time argument is provided (i.e. \"HH:MM:SS\") it is\n"
    "\tassumed that the day is the (last) day in the \n"
    "\tIONEX file.If not prvided, it is set to the last\n"
    "\tepoch in the file. Note that the time-scale used within\n"
    "\twithi the IONEX files is UT.\n"
    " -interval [SECONDS]\n"
    "\tSpecify the time step in integer seconds for the\n"
    "\tinterpolation. If not provided, it is set to\n"
    "\tthe value provided in the IONEX header.\n"
    " -lat [lat1/lat2/dlat]\n"
    "\tThe latitude interval for the interpolation. The\n"
    "\targuments should be decimal degrees (floats).\n"
    "\tIf not provided, it is set to the value provided\n"
    "\tin the IONEX file. Note that you can overide the\n"
    "\t\"dlat\" value via the \"-dlat\" option.\n"
    " -lon [lon1/lon2/dlon]\n"
    "\tThe longtitude interval for the interpolation. The\n"
    "\targuments should be decimal degrees (floats).\n"
    "\tIf not provided, it is set to the value provided\n"
    "\tin the IONEX file. Note that you can overide the\n"
    "\t\"dlon\" value via the \"-dlon\" option.\n"
    " -dlat [LATITUDE STEP]\n"
    "\tSpecify the latitude step in decimal degrees (the\n"
    "\tmax precission is two decimal places). This will\n"
    "\toverride the value of \"-lat\" argument (if provided).\n"
    " -dlon [LONGTITUDE STEP]\n"
    "\tSpecify the longtitude step in decimal degrees (the\n"
    "\tmax precission is two decimal places). This will\n"
    "\toverride the value of \"-lon\" argument (if provided).\n"
    " -diff \"[IONEX1 IONEX2 ...]\"\n"
    "\tIf \"-diff\" is specified, then instead of writing\n"
    "\tTEC values, the program will output TEC differences\n"
    "\t(in TECU) between the \"master\" IONEX file (the one\n"
    "\tpassed with the -i option) and the ones given in the\n"
    "\t-diff argument list. If you want to compare more than\n"
    "\ttwo files, then provide a whitespace-seperated list,\n"
    "\tincluded within quotation marks (\"\").\n";

    std::cout << "\nExample usage:\n"
    "Compute differences in TEC for point (lon=0, lat=0), between the files:\n"
    "igsg0440.03i - codg0440.03i and igsg0440.03i - jplg0440.03i, for all\n"
    "epochs in the (master) file:\n"
    "\tinxtr -i igsg0440.03i -diff \"codg0440.03i jplg0440.03i\" -lat 0/0/0 -lon 0/0/0\n";
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
