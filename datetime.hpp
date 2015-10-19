#ifndef __GNSS_DATETIME__
#define __GNSS_DATETIME__

namespace ngpt {

// Time Scales.
enum class Time_Scale : char {
  TAI,   ///< International Atomic Time
  UTC,   ///< Coordinated Universal Time
  TT,    ///< Terrestrial Time
  UT1,   ///< Universal Time
  GpsT,  ///< GPS Time
  QzsT,  ///< QZSS Time
  GloT,  ///< Glonass Time
  GalT,  ///< Galileo Time
  BdtT,  ///< BeiDou Time
  IrnT   ///< IRNSS Time
};

class DateTime {
public:

private:
  long   mjd_;
  double sec_;
};

} // end namespace

#endif
