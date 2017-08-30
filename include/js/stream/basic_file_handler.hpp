/*
CppUtility library
Copyright (C) 2015  Jan Schmidt

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#pragma once

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <sys/stat.h>

namespace js {
namespace detail {
/**\ingroup stream
 * \brief Basic file out handling class
 * Basic class for writing to data files. Class contains only the
 * opening routine. If a file with the same name already exists, numbers
 * will be attached to the filename in the form:
 * filename.extension
 * filename-1.extension
 * filename-2.extension
 *
 * If number of tries excedes the previous set number of tries, an
 * exception of class std::runtime_error is thrown. The number of
 * apptemts can be set.
 */
class BasicFileHandler {
  protected:
    std::string _filename;
    std::string _file_extension;
    std::ofstream _file;
    bool _file_open;
    int _writing_attempts;

  public:
    /**@name Constructors
     */
    ///@{
    /// Default constructor. Sets infinite writing attempts as default.
    BasicFileHandler() : BasicFileHandler(-42){};
    /// Constructor. Opens no file, sets writing attempts
    BasicFileHandler(int writing_attempts)
        : _file_open(false), _writing_attempts(writing_attempts){};
    /// Opens file with filname
    BasicFileHandler(std::string);
    /// Opens file with filename and sets writing attempts
    BasicFileHandler(std::string, int);
    /// Open file with filename.extension
    BasicFileHandler(std::string, std::string);
    /// Open file with filename.extension and sets writing attempts
    BasicFileHandler(std::string, std::string, int);
    ///@}
    ~BasicFileHandler();

    /**@name open and close
     */
    ///@{
    /**\brief Opens file with given name
     *
     * Note, if _writing_attempts < 0, then there will be an infinte amount
     * of attempts to write the file.
     */
    void open(std::string);
    /// Opens file with given name and extension
    void open(std::string, std::string);
    /// Closes file
    void close() noexcept;
    ///@}
    /**@name settings
     */
    ///@{
    /// Set how many writing attempts will be made
    void writingAttempts(int) noexcept;
    /// Get how many writing attempts will be made
    int writingAttempts() noexcept;
    /// Get Filename
    std::string getFilename() noexcept;
    ///@}

    template<class T>
    friend BasicFileHandler& operator<<(BasicFileHandler&, const T&);
};

template<class T>
BasicFileHandler& operator<<(BasicFileHandler& in, const T& t) {
    in._file << t;
    return in;
}

/*
 * Functions implementations
 */

inline BasicFileHandler::BasicFileHandler(std::string filename)
    : BasicFileHandler(filename, -42) {}

inline BasicFileHandler::BasicFileHandler(std::string filename, int writing_att)
    : BasicFileHandler(writing_att) {
    try {
        BasicFileHandler::open(filename);
    } catch (std::runtime_error& err) {
        throw err;
    }
}

inline BasicFileHandler::BasicFileHandler(std::string filename,
                                          std::string file_extension)
    : BasicFileHandler(filename, file_extension, -42) {}

inline BasicFileHandler::BasicFileHandler(std::string filename,
                                          std::string file_extension,
                                          int writing_att)
    : BasicFileHandler(writing_att) {
    try {
        BasicFileHandler::open(filename, file_extension);
    } catch (std::runtime_error& e) {
        throw e;
    }
}

inline void BasicFileHandler::open(std::string filename) {
    try {
        return open(filename, std::string());
    } catch (std::runtime_error& e) {
        throw e;
    }
}

inline void BasicFileHandler::open(std::string filename,
                                   std::string file_extension) {
    std::stringstream filename_new;
    _file_extension = file_extension;
    if (!file_extension.empty()) {  // If file_extension is not empty, insert
                                    // '.' at the beginning.
        file_extension.insert(0, 1, '.');
    }
    filename_new << filename;

    struct stat buffer;
    int count = 0;
    while (stat(filename_new.str().append(file_extension).c_str(), &buffer) !=
           -1) {
        count++;
        if (count == _writing_attempts) {
            std::string errorstring(std::string("Could not open file after ") +
                                    std::to_string(_writing_attempts) +
                                    std::string(" attempts.\n"));
            throw std::runtime_error(errorstring);
        }
        filename_new.str(std::string());
        filename_new << filename << "-" << count;
    }
    _filename = filename_new.str();
    _file.open(_filename.append(file_extension).c_str());
    _file_open = true;
}

inline BasicFileHandler::~BasicFileHandler() { _file.close(); }

inline void BasicFileHandler::close() noexcept {
    _file.close();
    _file_open = false;
    _filename = std::string();
    _filename.clear();
    _file_extension = std::string();
    _file_extension.clear();
}

inline void BasicFileHandler::writingAttempts(int writing) noexcept {
    _writing_attempts = writing;
}

inline int BasicFileHandler::writingAttempts() noexcept {
    return _writing_attempts;
}

inline std::string BasicFileHandler::getFilename() noexcept {
    return _filename;
}
}
}  // END namespace js
