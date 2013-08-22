//
//  Journey.cpp
//  Prototype02
//
//  Created by Jeffrey Crouse on 8/16/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "Journey.h"

long iso8601toTimestamp(string d) {
    int tzd;
    Poco::DateTime dt;
    Poco::DateTimeParser::parse(Poco::DateTimeFormat::ISO8601_FRAC_FORMAT, d, dt, tzd);
    //dt.makeLocal(-4);
    time_t t = dt.timestamp().epochTime();
    return (long)t;
}
