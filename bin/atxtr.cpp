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

// print a bunch of details for an antenna pcv pattern, nothing fancy
void
print_pcv_info(const pcv_pattern&, const antenna&,
               pcv_type zen_step, pcv_type azi_step);

int
cmd_parse(int, char* c[], str_str_map&);

std::size_t
antenna_parser(str_str_map&, std::vector<antenna>&);

int main(int argv, char* argc[])
{
    // a dictionary with any default options
    str_str_map arg_dict;
    arg_dict["dzen" ] = std::string( "1.0" );
    arg_dict["dazi" ] = std::string( "1.0" );
    arg_dict["types"] = std::string( "G01" );

    // get comd arguments into the dictionary
    if ( cmd_parse(argv, argc, arg_dict) )
    {
        std::cerr << "Wrong cmds. Stop.\n";
        return 1;
    }

    // this may throw in non-debug mode
    auto it = arg_dict.find("antex");
    if ( it == arg_dict.end() )
    {
        std::cerr << "\nMust provide name of antex file.\n";
        return 1;
    }
    antex atx ( it->second.c_str() );
    
    // declare/construct the antenna vector
    std::vector<antenna> ant_vec;
    if ( antenna_parser(arg_dict, ant_vec) < 1 )
    {
        std::cerr << "\nFailed to read antenna model(s).\n";
        return 1;
    }

    // get the pcv patterns
    std::vector<pcv_pattern> pcv_vec;
    for (const auto& i : ant_vec)
    {
        pcv_vec.emplace_back( atx.get_antenna_pattern(i) );
    }

    // print the correction pattern
    std::size_t idx = 0;
    pcv_type zen_step, azi_step;
    try
    {
        zen_step = std::stod( arg_dict["dzen"] );
        azi_step = std::stod( arg_dict["dazi"] );
        if (zen_step <= .0 || azi_step <= .0 )
        {
            throw std::invalid_argument("Fuck off");
        }
    }
    catch (std::invalid_argument& e)
    {
        std::cerr << "\nInvalid zen and/or azi step!";
        return 1;
    }

    for (const auto& i : pcv_vec)
    {
        print_pcv_info(i, ant_vec[idx], zen_step, azi_step);
    }

    std::cout << "\n";
    return 0;
}

int
cmd_parse(int argv, char* argc[], str_str_map& smap)
{
    for (int i = 1; i < argv; i++)
    {
        if ( !std::strcmp(argc[i], "-a") )
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
        else
        {
            std::cerr << "\nIrrelevant cmd: " << argc[i];
        }
    }
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
    
    // fuck the strings
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
               pcv_type zen_step, pcv_type azi_step)
{
    std::cout <<"\nANT: " << ant.to_string();
    std::cout <<"\nZEN: " << pcv.zen1() << " " << pcv.zen2() << " " << zen_step;
    std::cout <<"\nAZI: " << pcv.azi1() << " " << pcv.azi2() << " " << azi_step;

    for ( pcv_type zen = .1; zen < 90.0 ; zen += zen_step )
    {
        for ( pcv_type azi = .1; azi < 360.0 ; azi += azi_step )
        {
            std::cout << "\n" << pcv.azi_pcv(zen, azi, 0);
        }
    }
    return;
}
