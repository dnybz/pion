// ---------------------------------------------------------------------
// pion:  a Boost C++ framework for building lightweight HTTP interfaces
// ---------------------------------------------------------------------
// Copyright (C) 2021 Wang Qiang  (https://github.com/dnybz/pion)
// Copyright (C) 2007-2014 Splunk Inc.  (https://github.com/splunk/pion)
//
// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt
//

#ifndef __PION_FILESERVICE_HEADER__
#define __PION_FILESERVICE_HEADER__

#include <memory>
#include <pion/config.hpp>
#include <pion/logger.hpp>
#include <pion/hash_map.hpp>
#include <pion/noncopyable.hpp>
#include <pion/http/plugin_service.hpp>
#include <pion/http/request.hpp>
#include <pion/http/response_writer.hpp>
#include <pion/http/server.hpp>
#include <string>
#include <vector>
#include <map>


namespace pion {        // begin namespace pion
namespace plugins {     // begin namespace plugins
///
/// DiskFile: class used to represent files stored on disk
/// 
class DiskFile {
public:
    /// default constructor
    DiskFile(void)
        : m_file_size(0), m_last_modified(0) {}

    /// used to construct new disk file objects
    DiskFile(const fs::path& path,
             char *content, unsigned long size,
             std::time_t modified, const std::string& mime)
        : m_file_path(path), m_file_content(content), m_file_size(size),
        m_last_modified(modified), m_mime_type(mime)
    {}

    /// copy constructor
    DiskFile(const DiskFile& f)
        : m_file_path(f.m_file_path), m_file_content(f.m_file_content),
        m_file_size(f.m_file_size), m_last_modified(f.m_last_modified),
        m_last_modified_string(f.m_last_modified_string), m_mime_type(f.m_mime_type)
    {}

    /// updates the file_size and last_modified timestamp to disk
    void update(void);

    /// reads content from disk into file_content buffer (may throw)
    void read(void);

    /**
     * checks if the file has been updated and updates vars if it has (may throw)
     *
     * @return true if the file was updated
     */
    bool checkUpdated(void);

    /// return path to the cached file
    inline const fs::path& getFilePath(void) const { return m_file_path; }

    /// returns content of the cached file
    inline char *getFileContent(void) { return (char*)m_file_content.get(); }

    /// returns true if there is cached file content
	inline bool hasFileContent(void) const { return static_cast<bool>(m_file_content.get()); }

    /// returns size of the file's content
    inline uint64_t getFileSize(void) const { return static_cast<uint64_t>(m_file_size); }

    /// returns timestamp that the cached file was last modified (0 = cache disabled)
    inline std::time_t getLastModified(void) const { return m_last_modified; }

    /// returns timestamp that the cached file was last modified (string format)
    inline const std::string& getLastModifiedString(void) const { return m_last_modified_string; }

    /// returns mime type for the cached file
    inline const std::string& getMimeType(void) const { return m_mime_type; }

    /// sets the path to the cached file
    inline void setFilePath(const fs::path& p) { m_file_path = p; }

    /// appends to the path of the cached file
    inline void appendFilePath(const std::string& p) { m_file_path /= p; }

    /// sets the mime type for the cached file
    inline void setMimeType(const std::string& t) { m_mime_type = t; }

    /// resets the size of the file content buffer
    inline void resetFileContent(unsigned long n = 0) {
		if (n == 0) {
			m_file_content.reset();
		}
		else {
			m_file_content.reset(new char[n]);
		}
    }


protected:

    /// path to the cached file
    fs::path     m_file_path;

    /// content of the cached file
	std::shared_ptr<char>   m_file_content;

    /// size of the file's content
    std::streamsize             m_file_size;

    /// timestamp that the cached file was last modified (0 = cache disabled)
    std::time_t                 m_last_modified;

    /// timestamp that the cached file was last modified (string format)
    std::string                 m_last_modified_string;

    /// mime type for the cached file
    std::string                 m_mime_type;
};


///
/// DiskFileSender: class used to send files to clients using HTTP responses
/// 
class DiskFileSender : 
    public std::enable_shared_from_this<DiskFileSender>,
    private pion::noncopyable
{
public:
    /**
     * creates new DiskFileSender objects
     *
     * @param file disk file object that should be sent
     * @param http_request_ptr HTTP request that we are responding to
     * @param tcp_conn TCP connection used to send the file
     * @param max_chunk_size sets the maximum chunk size (default=0, unlimited)
	 * @param max_cache_size sets the maximum chunk size (default=0, unlimited)
     */
    static inline std::shared_ptr<DiskFileSender>
        create(DiskFile& file,
               const pion::http::request_ptr& http_request_ptr,
               const pion::tcp::connection_ptr& tcp_conn,
               unsigned long max_chunk_size = 0,
			   unsigned long max_cache_size = 0)
    {
        return std::shared_ptr<DiskFileSender>(new DiskFileSender(file, http_request_ptr,
                                                                    tcp_conn, max_chunk_size));
    }

    /// default virtual destructor 
    virtual ~DiskFileSender() {}

    /// Begins sending the file to the client.  Following a call to this
    /// function, it is not thread safe to use your reference to the
    /// DiskFileSender object.
    void send(void);

    /// sets the logger to be used
    inline void set_logger(logger log_ptr) { m_logger = log_ptr; }

    /// returns the logger currently in use
    inline logger get_logger(void) { return m_logger; }


protected:

    /**
     * protected constructor restricts creation of objects (use create())
     * 
     * @param file disk file object that should be sent
     * @param http_request_ptr HTTP request that we are responding to
     * @param tcp_conn TCP connection used to send the file
     * @param max_chunk_size sets the maximum chunk size
     */
    DiskFileSender(DiskFile& file,
                   const pion::http::request_ptr& http_request_ptr,
                   const pion::tcp::connection_ptr& tcp_conn,
                   unsigned long max_chunk_size);

    /**
     * handler called after a send operation has completed
     *
     * @param write_error error status from the last write operation
     * @param bytes_written number of bytes sent by the last write operation
     */
    void handle_write(const asio::error_code& write_error,
                     std::size_t bytes_written);


    /// primary logging interface used by this class
    logger                              m_logger;


private:

    /// the disk file we are sending
    DiskFile                                m_disk_file;

    /// the HTTP response we are sending
    pion::http::response_writer_ptr        m_writer;

    /// used to read the file from disk if it is not already cached in memory
	std::ifstream             m_file_stream;

    /// buffer used to send file content
	std::shared_ptr<char>               m_content_buf;

    /**
     * maximum chunk size (in bytes): files larger than this size will be
     * delivered to clients using HTTP chunked responses.  A value of
     * zero means that the size is unlimited (chunking is disabled).
     */
	unsigned long                          m_max_chunk_size;

    /// the number of file bytes send in the last operation
	uint64_t                          m_file_bytes_to_send;

    /// the number of bytes we have sent so far
    uint64_t                         m_bytes_sent;
};

/// data type for a DiskFileSender pointer
typedef std::shared_ptr<DiskFileSender>       DiskFileSenderPtr;


///
/// FileService: web service that serves regular files
/// 
class FileService :
    public pion::http::plugin_service
{
public:

    // default constructor and destructor
    FileService(void);
    virtual ~FileService() {}

    /**
     * configuration options supported by FileService:
     *
     * directory: all files within the directory will be made available
     * file:
     * cache:
     * scan:
     * max_chunk_size:
     * writable:
     */
    virtual void set_option(const std::string& name, const std::string& value);

    /// handles requests for FileService
    virtual void operator()(const pion::http::request_ptr& http_request_ptr,
                            const pion::tcp::connection_ptr& tcp_conn);

    /// called when the web service's server is starting
    virtual void start(void);

    /// called when the web service's server is stopping
    virtual void stop(void);

    /// sets the logger to be used
    inline void set_logger(logger log_ptr) { m_logger = log_ptr; }

    /// returns the logger currently in use
    inline logger get_logger(void) { return m_logger; }


protected:

    /// data type for map of file names to cache entries
    typedef PION_HASH_MAP<std::string, DiskFile/*, PION_HASH_STRING*/ >     CacheMap;

    /// data type for map of file extensions to MIME types
    typedef PION_HASH_MAP<std::string, std::string/*, PION_HASH_STRING*/ >  MIMETypeMap;

    /**
     * adds all files within a directory to the cache
     *
     * @param dir_path the directory to scan (sub-directories are included)
     */
    void scanDirectory(const fs::path& dir_path);

    /**
     * adds a single file to the cache
     *
     * @param relative_path path for the file relative to the root directory
     * @param file_path actual path to the file on disk
     * @param placeholder if true, the file's contents are not cached
     *
     * @return std::pair<CacheMap::iterator, bool> if an entry is added to the
     *         cache, second will be true and first will point to the new entry
     */
    std::pair<CacheMap::iterator, bool>
        addCacheEntry(const std::string& relative_path,
                      const fs::path& file_path,
                      const bool placeholder);

    /**
     * searches for a MIME type that matches a file
     *
     * @param file_name name of the file to search for
     * @return MIME type corresponding with the file, or DEFAULT_MIME_TYPE if none found
     */
    static std::string findMIMEType(const std::string& file_name);

    void sendNotFoundResponse(const pion::http::request_ptr& http_request_ptr,
                              const pion::tcp::connection_ptr& tcp_conn);

    /// primary logging interface used by this class
    logger                  m_logger;


private:

    /// function called once to initialize the map of MIME types
    static void createMIMETypes(void);


    /// mime type used if no others are found for the file's extension
    static const std::string    DEFAULT_MIME_TYPE;

    /// default setting for cache configuration option
    static const unsigned int   DEFAULT_CACHE_SETTING;

    /// default setting for scan configuration option
    static const unsigned int   DEFAULT_SCAN_SETTING;

    /// default setting for the maximum cache size option
    static const unsigned long  DEFAULT_MAX_CACHE_SIZE;

    /// default setting for the maximum chunk size option
    static const unsigned long  DEFAULT_MAX_CHUNK_SIZE;

    /// flag used to make sure that createMIMETypes() is called only once
    static std::once_flag     m_mime_types_init_flag;

    /// map of file extensions to MIME types
    static MIMETypeMap *        m_mime_types_ptr;


    /// directory containing files that will be made available
    fs::path     m_directory;

    /// single file served by the web service
    fs::path     m_file;

    /// used to cache file contents and metadata in memory
    CacheMap                    m_cache_map;

    /// mutex used to make the file cache thread-safe
    std::mutex                m_cache_mutex;

    /**
     * cache configuration setting:
     * 0 = do not cache files in memory
     * 1 = cache files in memory when requested, check for any updates
     * 2 = cache files in memory when requested, ignore any updates
     */
    unsigned int                m_cache_setting;

    /**
     * scan configuration setting (only applies to directories):
     * 0 = do not scan the directory; allow files to be added at any time
     * 1 = scan directory when started, and do not allow files to be added
     * 2 = scan directory and pre-populate cache; allow new files
     * 3 = scan directory and pre-populate cache; ignore new files
     */
    unsigned int                m_scan_setting;

    /**
     * maximum cache size (in bytes): files larger than this size will never be
     * cached in memory.  A value of zero means that the size is unlimited.
     */
    unsigned long               m_max_cache_size;

    /**
     * maximum chunk size (in bytes): files larger than this size will be
     * delivered to clients using HTTP chunked responses.  A value of
     * zero means that the size is unlimited (chunking is disabled).
     */
    unsigned long               m_max_chunk_size;

    /**
     * Whether the file and/or directory served are writable.
     */
    bool                        m_writable;
};


}   // end namespace plugins
}   // end namespace pion

#endif
