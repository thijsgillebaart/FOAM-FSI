
/*
 * Author
 *   David Blom, TU Delft. All rights reserved.
 */

#include "WendlandC6Function.H"

namespace rbf
{
    WendlandC6Function::WendlandC6Function( double radius )
        :
        radius( radius )
    {
        assert( radius > 0 );
    }

    WendlandC6Function::~WendlandC6Function()
    {}

    double WendlandC6Function::evaluate( double value )
    {
        value /= radius;

        if ( 1 - value < 0 )
            return 0;

        return std::pow( 1 - value, 8 ) * (32 * std::pow( value, 3 ) + 25 * std::pow( value, 2 ) + 8 * value + 1);
    }
}
