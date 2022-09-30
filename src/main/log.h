//
// Created by Jake M O'Neill on 10/09/2022.
//

#ifndef MRONEILLSCUBES_LOG_H
#define MRONEILLSCUBES_LOG_H

/*! configure the spd context to use both rotating log files and the stdout
 *
 * @param loggerName the name used to fetch references to the logger
 */

#include <string>

namespace logging {

    void configure(const std::string& loggerName);

}

#endif //MRONEILLSCUBES_LOG_H
