#include <stdexcept>
#include "satsys.hpp"

typedef std::map<int, double> idmap;

/// Initialize the static frequency map for GPS.
const idmap
ngpt::SatelliteSystemTraits<ngpt::SATELLITE_SYSTEM::GPS>::frequency_map =
{
  { 1, 1575.42e0 },
  { 2, 1227.60e0 },
  { 5, 1176.45e0 }
};

/// Initialize the static frequency map for GLONASS.
const idmap
ngpt::SatelliteSystemTraits<ngpt::SATELLITE_SYSTEM::GLONASS>::frequency_map =
{
  { 1, 1602.000e0 },
  { 2, 1246.000e0 },
  { 3, 1202.025e0 }
};

/// Initialize the static frequency map for GALILEO.
const idmap
ngpt::SatelliteSystemTraits<ngpt::SATELLITE_SYSTEM::Galileo>::frequency_map =
{
  { 1, 1575.420e0 }, /// E1
  { 5, 1176.450e0 }, /// E5a
  { 7, 1207.140e0 }, /// E5b
  { 8, 1191.795e0 }, /// E5(E5a+E5b)
  { 6, 1278.750e0 }  /// E6
};

/// Initialize the static frequency map for SBAS.
const idmap
ngpt::SatelliteSystemTraits<ngpt::SATELLITE_SYSTEM::SBAS>::frequency_map =
{
  { 1, 1575.42e0 },
  { 5, 1176.45e0 }
};

/// Initialize the static frequency map for QZSS.
const idmap
ngpt::SatelliteSystemTraits<ngpt::SATELLITE_SYSTEM::QZSS>::frequency_map =
{
  { 1, 1575.42e0 },
  { 2, 1227.60e0 },
  { 5, 1176.45e0 },
  { 6, 1278.75e0 } /// LEX
};

/// Initialize the static frequency map for BDS.
const idmap
ngpt::SatelliteSystemTraits<ngpt::SATELLITE_SYSTEM::BeiDou>::frequency_map =
{
  { 1, 1561.098e0 },
  { 2, 1207.140e0 },
  { 3, 1268.520e0 }
};

/// Initialize the static frequency map for IRNSS.
const idmap
ngpt::SatelliteSystemTraits<ngpt::SATELLITE_SYSTEM::IRNSS>::frequency_map =
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
char ngpt::SatSysIdentifier( ngpt::SATELLITE_SYSTEM s )
{
  using ngpt::SATELLITE_SYSTEM;

  switch ( s )
  {
    case SATELLITE_SYSTEM::GPS :
      return ngpt::SatelliteSystemTraits<SATELLITE_SYSTEM::GPS>::identifier;
    case SATELLITE_SYSTEM::GLONASS :
      return ngpt::SatelliteSystemTraits<SATELLITE_SYSTEM::GLONASS>::identifier;
    case SATELLITE_SYSTEM::SBAS :
      return ngpt::SatelliteSystemTraits<SATELLITE_SYSTEM::SBAS>::identifier;
    case SATELLITE_SYSTEM::Galileo :
      return ngpt::SatelliteSystemTraits<SATELLITE_SYSTEM::Galileo>::identifier;
    case SATELLITE_SYSTEM::BeiDou :
      return ngpt::SatelliteSystemTraits<SATELLITE_SYSTEM::BeiDou>::identifier;
    case SATELLITE_SYSTEM::QZSS :
      return ngpt::SatelliteSystemTraits<SATELLITE_SYSTEM::QZSS>::identifier;
    case SATELLITE_SYSTEM::IRNSS :
      return ngpt::SatelliteSystemTraits<SATELLITE_SYSTEM::IRNSS>::identifier;
    case SATELLITE_SYSTEM::MIXED :
      return ngpt::SatelliteSystemTraits<SATELLITE_SYSTEM::MIXED>::identifier;
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
ngpt::SATELLITE_SYSTEM ngpt::charToSatSys( char c )
{
  using ngpt::SATELLITE_SYSTEM;

  switch ( c )
  {
    case (ngpt::SatelliteSystemTraits<SATELLITE_SYSTEM::GPS>::identifier) :
      return SATELLITE_SYSTEM::GPS;
    case (ngpt::SatelliteSystemTraits<SATELLITE_SYSTEM::GLONASS>::identifier) :
      return SATELLITE_SYSTEM::GLONASS;
    case (ngpt::SatelliteSystemTraits<SATELLITE_SYSTEM::Galileo>::identifier) :
      return SATELLITE_SYSTEM::Galileo;
    case (ngpt::SatelliteSystemTraits<SATELLITE_SYSTEM::SBAS>::identifier) :
      return SATELLITE_SYSTEM::SBAS;
    case (ngpt::SatelliteSystemTraits<SATELLITE_SYSTEM::QZSS>::identifier) :
      return SATELLITE_SYSTEM::QZSS;
    case (ngpt::SatelliteSystemTraits<SATELLITE_SYSTEM::BeiDou>::identifier) :
      return SATELLITE_SYSTEM::BeiDou;
    case (ngpt::SatelliteSystemTraits<SATELLITE_SYSTEM::IRNSS>::identifier) :
      return SATELLITE_SYSTEM::IRNSS;
    case (ngpt::SatelliteSystemTraits<SATELLITE_SYSTEM::MIXED>::identifier) :
      return SATELLITE_SYSTEM::MIXED;
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
double ngpt::nominal_frequency(int band, ngpt::SATELLITE_SYSTEM s)
{
  using ngpt::SATELLITE_SYSTEM;
  
  try { // std::map.at() might throw !
    switch ( s )
    {
      case SATELLITE_SYSTEM::GPS :
        return ngpt::SatelliteSystemTraits<SATELLITE_SYSTEM::GPS>
          ::frequency_map.at(band);
      case SATELLITE_SYSTEM::GLONASS :
        return ngpt::SatelliteSystemTraits<SATELLITE_SYSTEM::GLONASS>
          ::frequency_map.at(band);
      case SATELLITE_SYSTEM::SBAS :
        return ngpt::SatelliteSystemTraits<SATELLITE_SYSTEM::SBAS>
          ::frequency_map.at(band);
      case SATELLITE_SYSTEM::Galileo :
        return ngpt::SatelliteSystemTraits<SATELLITE_SYSTEM::Galileo>
          ::frequency_map.at(band);
      case SATELLITE_SYSTEM::BeiDou :
        return ngpt::SatelliteSystemTraits<SATELLITE_SYSTEM::BeiDou>
          ::frequency_map.at(band);
      case SATELLITE_SYSTEM::QZSS :
        return ngpt::SatelliteSystemTraits<SATELLITE_SYSTEM::QZSS>
          ::frequency_map.at(band);
      case SATELLITE_SYSTEM::IRNSS :
        return ngpt::SatelliteSystemTraits<SATELLITE_SYSTEM::IRNSS>
          ::frequency_map.at(band);
      default:
        // std::map.at() will throw an std::out_of_range; let's do the
        // same so we won;t have to catch more than one exception types.
        throw std::out_of_range
        ("ngpt::nominal_frequency -> Invalid Satellite System!!");
    }
  } catch (std::out_of_range& e) {
    throw std::runtime_error
    ("ngpt::nominal_frequency -> Invalid Satellite System / Grequency Band pair");
  }
}
