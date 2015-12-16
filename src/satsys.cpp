#include <stdexcept>
#include "satsys.hpp"

typedef std::map<int, double> idmap;

/// Initialize the static frequency map for GPS.
const idmap
ngpt::SatelliteSystemTraits<ngpt::satellite_system::gps>::frequency_map =
{
    { 1, 1575.42e0 },
    { 2, 1227.60e0 },
    { 5, 1176.45e0 }
};

/// Initialize the static frequency map for GLONASS.
const idmap
ngpt::SatelliteSystemTraits<ngpt::satellite_system::glonass>::frequency_map =
{
    { 1, 1602.000e0 },
    { 2, 1246.000e0 },
    { 3, 1202.025e0 }
};

/// Initialize the static frequency map for GALILEO.
const idmap
ngpt::SatelliteSystemTraits<ngpt::satellite_system::galileo>::frequency_map =
{
    { 1, 1575.420e0 }, /// E1
    { 5, 1176.450e0 }, /// E5a
    { 7, 1207.140e0 }, /// E5b
    { 8, 1191.795e0 }, /// E5(E5a+E5b)
    { 6, 1278.750e0 }  /// E6
};

/// Initialize the static frequency map for SBAS.
const idmap
ngpt::SatelliteSystemTraits<ngpt::satellite_system::sbas>::frequency_map =
{
    { 1, 1575.42e0 },
    { 5, 1176.45e0 }
};

/// Initialize the static frequency map for QZSS.
const idmap
ngpt::SatelliteSystemTraits<ngpt::satellite_system::qzss>::frequency_map =
{
    { 1, 1575.42e0 },
    { 2, 1227.60e0 },
    { 5, 1176.45e0 },
    { 6, 1278.75e0 } /// LEX
};

/// Initialize the static frequency map for BDS.
const idmap
ngpt::SatelliteSystemTraits<ngpt::satellite_system::beidou>::frequency_map =
{
    { 1, 1561.098e0 },
    { 2, 1207.140e0 },
    { 3, 1268.520e0 }
};

/// Initialize the static frequency map for IRNSS.
const idmap
ngpt::SatelliteSystemTraits<ngpt::satellite_system::irnss>::frequency_map =
{
    { 5, 1176.450e0 },
    { 2, 2492.028e0 }, //TODO in \cite rnx303 this is denoted as 'S'
};

/** \details  Given a satellite system enumerator, this function will return
 *            it's identifier (e.g. given SatelliteSystem = GPS, the function
 *            will return 'G'). The identifier are taken from RINEX v3.02
 *
 *  \param[in] s Input Satellite System.
 *  \return      A char, representing the satellite system
 *
 *  \throw       std::runtime_error if no matching satellite system is found.
 */
char ngpt::satsys_identifier( ngpt::satellite_system s )
{
    using ngpt::satellite_system;

    switch ( s )
    {
        case satellite_system::gps :
            return ngpt::SatelliteSystemTraits<satellite_system::gps>::identifier;
        case satellite_system::glonass :
            return ngpt::SatelliteSystemTraits<satellite_system::glonass>::identifier;
        case satellite_system::sbas :
            return ngpt::SatelliteSystemTraits<satellite_system::sbas>::identifier;
        case satellite_system::galileo :
            return ngpt::SatelliteSystemTraits<satellite_system::galileo>::identifier;
        case satellite_system::beidou :
            return ngpt::SatelliteSystemTraits<satellite_system::beidou>::identifier;
        case satellite_system::qzss :
            return ngpt::SatelliteSystemTraits<satellite_system::qzss>::identifier;
        case satellite_system::irnss :
            return ngpt::SatelliteSystemTraits<satellite_system::irnss>::identifier;
        case satellite_system::mixed :
            return ngpt::SatelliteSystemTraits<satellite_system::mixed>::identifier;
        default:
            throw std::runtime_error
                ("ngpt::SatSysIdentifier -> Invalid Satellite System !!");
    }
}

/** \details  Given a char, this function will return the corresponding
 *            satellite system. The identifier are taken from RINEX v3.02
 *
 *  \param[in] c Input char.
 *  \return      The corresponding satellite system
 *
 *  \throw       std::runtime_error if no matching satellite system is found.
 */
ngpt::satellite_system ngpt::char_to_satsys( char c )
{
    using ngpt::satellite_system;

    switch ( c )
    {
        case (ngpt::SatelliteSystemTraits<satellite_system::gps>::identifier) :
            return satellite_system::gps;
        case (ngpt::SatelliteSystemTraits<satellite_system::glonass>::identifier) :
            return satellite_system::glonass;
        case (ngpt::SatelliteSystemTraits<satellite_system::galileo>::identifier) :
            return satellite_system::galileo;
        case (ngpt::SatelliteSystemTraits<satellite_system::sbas>::identifier) :
            return satellite_system::sbas;
        case (ngpt::SatelliteSystemTraits<satellite_system::qzss>::identifier) :
            return satellite_system::qzss;
        case (ngpt::SatelliteSystemTraits<satellite_system::beidou>::identifier) :
            return satellite_system::beidou;
        case (ngpt::SatelliteSystemTraits<satellite_system::irnss>::identifier) :
            return satellite_system::irnss;
        case (ngpt::SatelliteSystemTraits<satellite_system::mixed>::identifier) :
            return satellite_system::mixed;
        default :
            throw std::runtime_error
                ("ngpt::charToSatSys -> Invalid Satellite System Identifier!!");
    }
}

/** \details     Given a frequency band (index) and a satellite system,
 *               return the nominal frequency value.
 *
 *  \param[in]   band The frequency band.
 *  \param[in]   s    The satellite system.
 *  \return      The corresponding satellite system
 *
 *  \throw       std::runtime_error if no matching satellite system or 
 *               satellite system / frequency band pair is not found.
 */
double ngpt::nominal_frequency(int band, ngpt::satellite_system s)
{
    using ngpt::satellite_system;

    try { // std::map.at() might throw !
        switch ( s )
        {
            case satellite_system::gps :
                return ngpt::SatelliteSystemTraits<satellite_system::gps>
                    ::frequency_map.at(band);
            case satellite_system::glonass :
                return ngpt::SatelliteSystemTraits<satellite_system::glonass>
                    ::frequency_map.at(band);
            case satellite_system::sbas :
                return ngpt::SatelliteSystemTraits<satellite_system::sbas>
                    ::frequency_map.at(band);
            case satellite_system::galileo :
                return ngpt::SatelliteSystemTraits<satellite_system::galileo>
                    ::frequency_map.at(band);
            case satellite_system::beidou :
                return ngpt::SatelliteSystemTraits<satellite_system::beidou>
                    ::frequency_map.at(band);
            case satellite_system::qzss :
                return ngpt::SatelliteSystemTraits<satellite_system::qzss>
                    ::frequency_map.at(band);
            case satellite_system::irnss :
                return ngpt::SatelliteSystemTraits<satellite_system::irnss>
                    ::frequency_map.at(band);
            default:
                // std::map.at() will throw an std::out_of_range; let's do the
                // same so we won;t have to catch more than one exception types.
                throw std::out_of_range
                    ("ngpt::nominal_frequency -> Invalid Satellite System!!");
        }
    } catch (std::out_of_range& e) {
        throw std::runtime_error
            ("ngpt::nominal_frequency -> Invalid Satellite System / Frequency Band pair");
    }
}
