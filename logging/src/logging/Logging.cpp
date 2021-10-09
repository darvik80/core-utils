//
// Created by Kishchenko, Ivan on 5/6/21.
//

#include "Logging.h"

#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>

#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/formatting_ostream.hpp>

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/stacktrace.hpp>

BOOST_LOG_GLOBAL_LOGGER_INIT(g_logger, src::severity_logger_mt<trivial::severity_level>) {
    boost::log::sources::severity_logger_mt<boost::log::trivial::severity_level> logger;

    return logger;
}

BOOST_LOG_ATTRIBUTE_KEYWORD(channel,
                            "Channel", std::string)

BOOST_LOG_ATTRIBUTE_KEYWORD(timestamp,
                            "TimeStamp", boost::posix_time::ptime)
BOOST_LOG_ATTRIBUTE_KEYWORD(threadId,
                            "ThreadID", boost::log::attributes::current_thread_id::value_type)
auto static date_time_formatter = boost::log::expressions::stream
        << boost::log::expressions::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S.%f");

void consoleFormatter(boost::log::record_view const &rec, boost::log::formatting_ostream &strm) {

    strm << "\033[38;5;15m[";
    date_time_formatter(rec, strm);
    strm << "] [";

    auto severity = rec[boost::log::trivial::severity];
    if (severity) {
        // Set the color
        switch (severity.get()) {
            case boost::log::trivial::severity_level::debug:
                strm << "\033[1;37m";
                break;

            case boost::log::trivial::severity_level::info:
                strm << "\033[1;32m";
                break;
            case boost::log::trivial::severity_level::warning:
                strm << "\033[1;33m";
                break;
            case boost::log::trivial::severity_level::error:
            case boost::log::trivial::severity_level::fatal:
                strm << "\033[1;31m";
                break;
            default:
                break;
        }
    }

    strm << std::setw(7) << std::right << severity << "\033[38;5;15m] ";

    auto ch = rec[channel];
    if (ch) {
        strm << "[ \033[34m" << ch << "\033[38;5;15m ] ";
    }

    strm << "[ \033[36m" << rec[threadId] << "\033[38;5;15m ] : ";

    strm << "\033[38;5;250m" << rec[boost::log::expressions::smessage] << "\033[38;5;1m";

    switch (severity.get()) {
        case boost::log::trivial::severity_level::error:
        case boost::log::trivial::severity_level::fatal:
            strm << std::endl << "\033[31m" << boost::stacktrace::stacktrace() << "\033[0m";
        default:
            break;
    }
    strm << "\033[0m";
}

void fileFormatter(boost::log::record_view const &rec, boost::log::formatting_ostream &strm) {
    strm << "[";
    date_time_formatter(rec, strm);
    strm << "] [";

    strm << std::setw(7) << std::right << rec[boost::log::trivial::severity] << "] ";
    strm << "[ " << rec[threadId] << " ] : " << rec[boost::log::expressions::smessage];
}

void logger::setup(logger::LoggingProperties &props) {
    boost::log::register_simple_formatter_factory<boost::log::trivial::severity_level, char>("Severity");

    if (props.file) {
        boost::log::add_file_log(
                boost::log::keywords::file_name = "./application.log",
                boost::log::keywords::rotation_size = 10 * 1024 * 1024,
                boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0, 0, 0),
                boost::log::keywords::auto_flush = true
        )->set_formatter(&fileFormatter);
    }


    if (props.console) {
        boost::log::add_console_log(std::clog)->set_formatter(&consoleFormatter);
    }

    auto logLevelValue = boost::log::trivial::trace;

    if (props.level == "trace") {
        logLevelValue = boost::log::trivial::trace;
    } else if (props.level == "debug") {
        logLevelValue = boost::log::trivial::debug;
    } else if (props.level == "info") {
        logLevelValue = boost::log::trivial::info;
    } else if (props.level == "warning") {
        logLevelValue = boost::log::trivial::warning;
    } else if (props.level == "error") {
        logLevelValue = boost::log::trivial::error;
    } else if (props.level == "fatal") {
        logLevelValue = boost::log::trivial::fatal;
    }

    boost::log::core::get()->set_filter(boost::log::trivial::severity >= logLevelValue);

    boost::log::add_common_attributes();
}
