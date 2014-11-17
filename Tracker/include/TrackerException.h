#include <string>

class TrackerException: public std::exception
{
public:
    /** Constructor (C strings).
     *  @param message C-style string error message.
     *                 The string contents are copied upon construction.
     *                 Hence, responsibility for deleting the \c char* lies
     *                 with the caller. 
     */
    explicit TrackerException(const char* message):
      msg(message), file(__FILE__), line(__LINE__)
      {
      }

    /** Constructor (C++ STL strings).
     *  @param message The error message.
     */
    explicit TrackerException(const std::string& message):
      msg(message), file(__FILE__), line(__LINE__)
      {}

    /** Destructor.
     * Virtual to allow for subclassing.
     */
    virtual ~TrackerException() throw (){}

    /** Returns a pointer to the (constant) error description.
     *  @return A pointer to a \c const \c char*. The underlying memory
     *          is in posession of the \c Exception object. Callers \a must
     *          not attempt to free the memory.
     */
    virtual const char* what() const throw ()
    {
       std::string message = "File " + file;
       message.append(" line " + line);
       message.append(" : " + msg);
       return message.c_str();
    }

protected:
    /** Error message.
     */
    std::string msg;
    std::string file;
    int line;
};

class TrackerInitializationException: public TrackerException
{
public:
    explicit TrackerInitializationException(const char* resource) : TrackerException(resource) { }
    explicit TrackerInitializationException(const std::string& resource): TrackerException(resource) { }
};

class TrackerRuntimeException: public TrackerException
{
public:
    explicit TrackerRuntimeException(const char* resource) : TrackerException(resource) { }
    explicit TrackerRuntimeException(const std::string& resource): TrackerException(resource) { }
};